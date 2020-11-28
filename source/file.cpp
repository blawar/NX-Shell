#include <switch.h>
#include "file.h"
#include <memory>
#include <fcntl.h>

namespace FS
{
	namespace file
	{
		Mode::Mode() : m_flags(0)
		{
		}

		Mode::Mode(u8 flags) : m_flags(flags)
		{
			m_flags = flags;
		}

		Mode::operator const char*() const
		{
			switch(m_flags)
			{
				case Mode::R:
					return "rb";
				case Mode::W:
					return "wb";
				case Mode::X:
					return "rb";
				case Mode::RW:
					return "rb+";
				case Mode::RWC:
					return "wb+";
				case Mode::RX:
					return "rb";
				case Mode::WX:
					return "rb+";
				case Mode::RWX:
					return "rb+";
			}
			return "rb";
		}

		int Mode::flags() const
		{
			int f = 0;

			if(m_flags & Mode::C)
			{
				f |= O_CREAT;
			}

			if((m_flags & Mode::W) && (m_flags & Mode::R))
			{
				f |= O_RDWR;
			}
			else if(m_flags & Mode::W)
			{
				f |= O_WRONLY | O_CREAT;
			}

			return f;
		}

		std::shared_ptr<Context> open(const Uri& uri, const Mode mode)
		{
			auto f = factory(uri);

			if(f && f->open(uri, mode))
			{
				return f;
			}

			return nullptr;
		}

		std::shared_ptr<Context> factory(const Uri& uri)
		{
			auto f = std::make_shared<Context>();
			f->uri() = uri;
			return f;
		}

		u64 copy(const Uri& src, const Uri& dst)
		{
			auto f1 = file::open(src, Mode::R);

			if(!f1 || !f1->isOpen())
			{
				return 0;
			}

			auto f2 = file::open(dst, Mode::W);

			if(!f2 || !f2->isOpen())
			{
				return 0;
			}

			f1->rewind();
			f2->rewind();

			std::vector<u8> buffer;
			u64 bytesRead = 0;

			while(!f1->eof())
			{
				bytesRead += f1->read(buffer, 0x10000);
				f2->write(&buffer.front(), buffer.size());
			}

			f2->flush();
			return bytesRead;
		}

		bool exists(const Uri& uri)
		{
			auto f = FS::file::open(uri, Mode::R);

			if(!f || !f->isOpen())
			{
				return false;
			}

			return true;
		}

		bool rename(const Uri& from, const Uri& to)
		{
			auto f = file::factory(from);

			if(f)
			{
				f->rename(to);
			}

			return false;
		}

		bool unlink(const Uri& uri)
		{
			auto f = file::factory(uri);

			if(f)
			{
				return f->unlink(uri);
			}

			return false;
		}

		Context::Context() : m_size(0), m_f(nullptr)
		{
		}

		Context::~Context()
		{
			if(isOpen())
			{
				close();
			}
		}

		bool Context::open(const Uri& uri, const Mode& mode, const u64 initialSize)
		{
			if(isOpen())
			{
				close();
			}

#ifdef __PREMIUM__
			if(initialSize >= 0xFFFF0000 && (mode.flags() & (O_WRONLY | O_CREAT)))
			{
				if(!exists(path))
				{
#ifdef __SWITCH__
					auto r = fsdevCreateFile(path.decoded().c_str(), initialSize, FsCreateOption_BigFile);
#endif
				}
			}
#endif

			m_f = fopen(uri.c_str(), mode);

			if(!m_f)
			{
				return false;
			}

			size();

			this->uri() = uri;

			return true;
		}

		bool Context::resize(u64 size)
		{
			if(!size)
			{
				return false;
			}

			u64 pos = tell();

			if(!seek(size - 1))
			{
				return false;
			}

#if __SWITCH__
			ftruncate(fileno(m_f), size);
#else
			_chsize(fileno(m_f), size);
#endif

			if(write((const u8*)"", 1))
			{
				seek(pos);
				m_size = size;
				return true;
			}

			return false;
		}

		bool Context::flush()
		{
#ifdef __SWITCH__
			fsdevCommitDevice(uri().scheme().c_str());
#else
			fflush(this->m_f);
#endif
			return true;
		}

		bool Context::unlink(const Uri& uri)
		{
			return ::unlink(uri.c_str()) == 0;
		}


		bool Context::rename(const Uri& uri)
		{
			return ::rename(this->uri().c_str(), uri.c_str()) == 0;
		}

		bool Context::close()
		{
			if(!isOpen())
			{
				return false;
			}

			fclose(m_f);
			m_f = NULL;
			return true;
		}

		bool Context::seek(u64 offset, int whence)
		{
			if(!isOpen())
			{
				return false;
			}

#ifndef __SWITCH__
			return _fseeki64(m_f, offset, whence) == 0;
#else
			return fseek(m_f, (long)offset, whence) == 0;
#endif
		}

		bool Context::rewind()
		{
			return seek(0);
		}

		u64 Context::tell()
		{
			if(!isOpen())
			{
				return 0;
			}

#ifndef __SWITCH__
			u64 pos = _ftelli64(m_f);
#else
			u64 pos = ftell(m_f);
#endif

			return pos;
		}

		bool Context::eof()
		{
			return tell() >= size();
		}

		u64 Context::size()
		{
			if(m_size)
			{
				return m_size;
			}

			u64 currentPosition = tell();

			if(!seek(0, SEEK_END))
			{
				return 0;
			}

			m_size = tell();

			seek(currentPosition, SEEK_SET);

			return m_size;
		}

		u64 Context::read(std::vector<u8>& buffer, u64 sz)
		{
			if(!isOpen())
			{
				return 0;
			}

			if(tell() > size())
			{
				buffer.resize(0);
				return 0;
			}

			if(!sz)
			{
				sz = size() - tell();
			}

			if(tell() + sz > size())
			{
				sz = size() - tell();
			}

			buffer.resize(sz); // uncaught exception


			u64 bytesRead = (u64)fread(&buffer.front(), 1, (size_t)sz, m_f);

			buffer.resize(bytesRead); // uncaught exception

			return bytesRead;

		}

		u64 Context::write(const u8* buffer, u64 sz)
		{
			if(!isOpen())
			{
				return 0;
			}

			if(!sz)
			{
				return 0;
			}

			u64 bytes = (u64)fwrite(buffer, 1, sz, m_f);

			u64 pos = tell();

			if(pos > size())
			{
				m_size = pos;
			}

			return bytes;
		}

		bool Context::isOpen()
		{
			return m_f != NULL;
		}

		time_t Context::timestamp()
		{
			return time_t();
		}

		bool Context::setArchiveBit(bool value)
		{
			if(!value)
			{
				return false;
			}

#ifdef __SWITCH__
			if(R_FAILED(fsFsSetConcatenationFileAttribute(fsdevGetDeviceFileSystem(uri().scheme().c_str()), uri().c_str())))
			{
				return false;
			}
#endif
			return true;
		}
	}	
}

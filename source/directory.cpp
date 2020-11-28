#include "directory.h"
#include <sys/stat.h>

namespace FS
{
	namespace directory
	{
		u64 copy(const Uri& src, const Uri& dst)
		{
			return 0;
		}

		std::shared_ptr<Context> open(const Uri& uri)
		{
			auto dh = factory(uri);

			if(dh && dh->open(uri))
			{
				return dh;
			}

			return nullptr;
		}

		std::shared_ptr<Context> factory(const Uri& uri)
		{
			auto dh = std::make_shared<Context>();
			dh->uri() = uri;
			return dh;
		}

		bool exists(const Uri& uri)
		{
			auto dh = open(uri);

			if(!dh)
			{
				return false;
			}

			return true;
		}

		bool rename(const Uri& from, const Uri& to)
		{
			auto d = directory::factory(from);

			if(d)
			{
				d->rename(to);
			}

			return false;
		}

		bool create(const Uri& uri)
		{
			auto d = directory::factory(uri);

			if(d)
			{
				return d->create(uri);
			}

			return false;
		}

		bool unlink(const Uri& uri)
		{
			auto f = directory::factory(uri);

			if(f)
			{
				return f->unlink(uri);
			}

			return false;
		}

		bool Context::open(const Uri& uri)
		{
			m_files.resize(0);
			m_directories.resize(0);
			return true;
		}

		bool Context::rename(const Uri& uri)
		{
			return ::rename(this->uri().c_str(), uri.c_str()) == 0;
		}

		bool Context::unlink(const Uri& uri)
		{
			return false;
		}

		bool Context::create(const Uri& uri)
		{
			mkdir(uri.c_str(), 777);
			return true;
		}

		std::vector<FsDirectoryEntry> Context::entries() const
		{
			std::vector<FsDirectoryEntry> r;

			for(const auto& entry : directories())
			{
				r.push_back(entry);
			}

			for(const auto& entry : files())
			{
				r.push_back(entry);
			}

			return r;
		}
	}
}

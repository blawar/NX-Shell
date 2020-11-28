#pragma once
#include <switch.h>
#include <memory>
#include <vector>
#include "uri.h"

namespace FS
{
	namespace file
	{
		class Mode
		{
		public:
			static const u8 R = 0x01;
			static const u8 W = 0x02;
			static const u8 X = 0x04;
			static const u8 A = 0x08;
			static const u8 C = 0x10;

			static const u8 RW = 3;
			static const u8 RX = 5;
			static const u8 WX = 6;
			//static const u8 WC = 0x12;
			static const u8 RWX = 7;
			static const u8 RWC = 0x13;

			Mode();

			Mode(u8 flags);

			bool isExecutable() const
			{
				return m_flags & Mode::X;
			}
			bool isReadable() const
			{
				return m_flags & Mode::R;
			}
			bool isWritable() const
			{
				return m_flags & Mode::W;
			}
			bool isCreate() const
			{
				return m_flags & Mode::C;
			}

			operator const char*() const;
			int flags() const;
		protected:
			u8 m_flags;
		};

		class Context
		{
		public:
			Context();
			virtual ~Context();

			virtual bool open(const Uri& uri, const Mode& mode, const u64 initialSize = 0);

			virtual bool resize(u64 size);
			virtual bool flush();

			virtual bool unlink(const Uri& uri);


			virtual bool rename(const Uri& uri);

			virtual bool close();
			virtual bool seek(u64 offset, int whence = 0);
			virtual bool rewind();
			virtual u64 tell();
			bool eof();
			virtual u64 size();
			virtual u64 read(std::vector<u8>& buffer, u64 sz = 0);
			virtual u64 write(const u8* buffer, u64 sz);

			virtual time_t timestamp();

			virtual bool setArchiveBit(bool value);

			virtual bool isOpen();

			const Uri& uri() const
			{
				return m_uri;
			}

			Uri& uri()
			{
				return m_uri;
			}

			void setMode(const Mode& mode)
			{
				m_mode = mode;
			}
		protected:
			Uri m_uri;
			Mode m_mode;
			u64 m_size;
			FILE* m_f;
		};

		u64 copy(const Uri& src, const Uri& dst);
		std::shared_ptr<Context> open(const Uri& uri, const Mode mode);
		std::shared_ptr<Context> factory(const Uri& uri);
		bool exists(const Uri& uri);
		bool rename(const Uri& from, const Uri& to);
		bool unlink(const Uri& uri);
	}
}
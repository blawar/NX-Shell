#pragma once
#include <switch.h>
#include <memory>
#include "uri.h"
#include <vector>
#include <cstring>

namespace FS
{
	namespace directory
	{
		struct Entry
		{
			Uri uri;
		};

		struct FileEntry : public Entry
		{
			u64 size;

			operator FsDirectoryEntry() const
			{
				FsDirectoryEntry entry;
				memset(&entry, 0, sizeof(entry));

				auto name = uri.basename();

				std::strcpy(entry.name, name.c_str());

				entry.type = FsDirEntryType_File;
				entry.file_size = size;
				return entry;
			}
		};

		struct DirEntry : public Entry
		{
			operator FsDirectoryEntry() const
			{
				FsDirectoryEntry entry;
				memset(&entry, 0, sizeof(entry));

				auto name = uri.basename();

				std::strcpy(entry.name, name.c_str());

				entry.type = FsDirEntryType_Dir;
				return entry;
			}
		};

		class Context
		{
		public:

			virtual bool open(const Uri& uri);

			const Uri& uri() const
			{
				return m_uri;
			}

			Uri& uri()
			{
				return m_uri;
			}

			bool create(const Uri& uri);
			bool rename(const Uri& uri);
			bool unlink(const Uri& uri);

			const std::vector<FileEntry>& files() const { return m_files; }
			const std::vector<DirEntry>& directories() const { return m_directories; }
			std::vector<FsDirectoryEntry> entries() const;
		protected:
			Uri m_uri;
			std::vector<FileEntry> m_files;
			std::vector<DirEntry> m_directories;
		};

		u64 copy(const Uri& src, const Uri& dst);
		std::shared_ptr<Context> open(const Uri& uri);
		std::shared_ptr<Context> factory(const Uri& uri);
		bool exists(const Uri& uri);
		bool rename(const Uri& from, const Uri& to);
		bool unlink(const Uri& uri);
		bool create(const Uri& uri);
	}
}

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>

#include "config.h"
#include "fs.h"
#include "language.h"
#include "log.h"
#include "popups.h"
#include "file.h"
#include "directory.h"
#include "uri.h"

namespace FS {
	typedef struct {
		Uri from;
		Uri to;
		bool is_dir = false;

		void reset()
		{
			from.reset();
			to.reset();
			is_dir = false;
		}
	} FS_Copy_Struct;
	
	FS_Copy_Struct fs_copy_struct;

	std::string GetFileExt(const std::string &filename) {
		std::string ext = std::filesystem::path(filename).extension();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
		return ext;
	}

	FileType GetFileType(const std::string &filename) {
		std::string ext = FS::GetFileExt(filename);
		
		if ((!ext.compare(".ZIP")) || (!ext.compare(".RAR")) || (!ext.compare(".7Z")))
			return FileTypeArchive;
		else if ((!ext.compare(".FLAC")) || (!ext.compare(".IT")) || (!ext.compare(".MOD")) || (!ext.compare(".MP3")) || (!ext.compare(".OGG"))
			|| (!ext.compare(".OPUS")) || (!ext.compare(".S3M")) || (!ext.compare(".WAV")) || (!ext.compare(".XM")))
			return FileTypeAudio;
		else if ((!ext.compare(".BMP")) || (!ext.compare(".GIF")) || (!ext.compare(".JPG")) || (!ext.compare(".JPEG")) || (!ext.compare(".PGM"))
			|| (!ext.compare(".PPM")) || (!ext.compare(".PNG")) || (!ext.compare(".PSD")) || (!ext.compare(".TGA")) || (!ext.compare(".WEBP")))
			return FileTypeImage;
		else if ((!ext.compare(".JSON")) || (!ext.compare(".LOG")) || (!ext.compare(".TXT")) || (!ext.compare(".CFG")) || (!ext.compare(".INI")))
			return FileTypeText;
		
		return FileTypeNone;
	}
	
	static bool Sort(const FsDirectoryEntry &entryA, const FsDirectoryEntry &entryB) {
		if ((entryA.type == FsDirEntryType_Dir) && !(entryB.type == FsDirEntryType_Dir))
			return true;
		else if (!(entryA.type == FsDirEntryType_Dir) && (entryB.type == FsDirEntryType_Dir))
			return false;
		else {
			switch(cfg.sort) {
				case 0: // Sort alphabetically (ascending - A to Z)
					if (strcasecmp(entryA.name, entryB.name) < 0)
						return true;
					
					break;
				
				case 1: // Sort alphabetically (descending - Z to A)
					if (strcasecmp(entryB.name, entryA.name) < 0)
						return true;
					
					break;
				
				case 2: // Sort by file size (largest first)
					if (entryB.file_size < entryA.file_size)
						return true;
					
					break;
					
				case 3: // Sort by file size (smallest first)
					if (entryA.file_size < entryB.file_size)
						return true;
					
					break;
			}
		}
		
		return false;
	}

	Result GetTimeStamp(FsDirectoryEntry *entry, FsTimeStampRaw *timestamp)
	{
		/*
		TODO
		*/

		return 0;
	}

	Result SetArchiveBit(FsDirectoryEntry *entry)
	{
		return 0;
	}

	Result Rename(FsDirectoryEntry *entry, const Uri& uri)
	{
		if(entry->type == FsDirEntryType_Dir)
		{
			if(!FS::directory::rename(cfg.cwd.join(entry->name), cfg.cwd.join(uri)))
				return -1;
		}
		else
		{
			if(!FS::file::rename(cfg.cwd.join(entry->name), cfg.cwd.join(uri)))
				return -1;
		}
		return 0;
	}

	Result GetDirList(const Uri& uri, std::vector<FsDirectoryEntry>& entries)
	{
		auto d = FS::directory::open(uri);

		if(d)
		{
			entries = d->entries();
			return 0;
		}
		else
		{
			entries.resize(0);
			return -1;
		}
	}

	void Copy(FsDirectoryEntry *entry, const Uri& uri)
	{
		fs_copy_struct.from = uri.join(entry->name);
		if(entry->type == FsDirEntryType_Dir)
			fs_copy_struct.is_dir = true;
	}

	Result Paste(void) {
		bool ret = false;
		
		if(fs_copy_struct.is_dir) // Copy folder recursively
		{
			ret = FS::directory::copy(fs_copy_struct.from, fs_copy_struct.to);
		}
		else // Copy file
		{
			ret = FS::file::copy(fs_copy_struct.from, fs_copy_struct.to);
		}
			
		fs_copy_struct.reset();
		return ret ? 0 : -1;
	}

	Result Move(void) {
		if (fs_copy_struct.is_dir) {
			if(!FS::directory::rename(fs_copy_struct.from, fs_copy_struct.to))
			{
				return -1;
			}
		}
		else {
			if (!FS::file::rename(fs_copy_struct.from, fs_copy_struct.to)) {
				return -1;
			}
		}
		fs_copy_struct.reset();
		return 0;
	}
	
	Result GetFreeStorageSpace(s64 *size) {
		*size = 0;		
		return 0;
	}
	
	Result GetTotalStorageSpace(s64 *size) {
		*size = 0;	
		return 0;
	}
	
	Result GetUsedStorageSpace(s64 *size) {
		Result ret = 0;
		s64 free_size = 0, total_size = 0;
		
		if (R_FAILED(ret = FS::GetFreeStorageSpace(&free_size)))
			return ret;
		
		if (R_FAILED(ret = FS::GetTotalStorageSpace(&total_size)))
			return ret;
			
		*size = (total_size - free_size);
		return 0;
	}
}

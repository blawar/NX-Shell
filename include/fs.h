#ifndef NX_SHELL_FS_H
#define NX_SHELL_FS_H

#include <string>
#include <vector>
#include <switch.h>
#include "uri.h"

typedef enum FileType {
    FileTypeNone,
    FileTypeArchive,
    FileTypeAudio,
    FileTypeImage,
    FileTypeText
} FileType;

namespace FS {
    std::string GetFileExt(const std::string &filename);
    FileType GetFileType(const std::string &filename);
	void Copy(FsDirectoryEntry *entry, const Uri& uri);
	Result SetArchiveBit(FsDirectoryEntry *entry);
	Result GetTimeStamp(FsDirectoryEntry *entry, FsTimeStampRaw *timestamp);
	Result Rename(FsDirectoryEntry *entry, const Uri& uri);
	Result GetDirList(const Uri& uri, std::vector<FsDirectoryEntry>& entries);
    Result Paste(void);
    Result Move(void);
    Result GetFreeStorageSpace(s64 *size);
    Result GetTotalStorageSpace(s64 *size);
    Result GetUsedStorageSpace(s64 *size);
}

#endif

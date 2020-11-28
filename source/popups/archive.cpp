#include <cstring>
#include <filesystem>
#include <zzip/zzip.h>

#include "config.h"
#include "fs.h"
#include "gui.h"
#include "imgui.h"
#include "language.h"
#include "log.h"
#include "popups.h"

namespace ArchiveHelper {
    Uri ConstructPath(const Uri& uri) {
		return cfg.cwd.join(uri);
    }
    
    Uri ConstructDirname(const Uri& uri, char *dirname) {
        return cfg.cwd.join(uri).join(std::filesystem::path(dirname).parent_path().c_str());
    }
    
    Uri ConstructFilename(const Uri& uri, char *filename) {
        return  cfg.cwd.join(uri).join(filename);
    }
    
    Result RecursiveMakeDir(const std::string &path) {
        Result ret = 0;
        /* TODO
		char buf[FS_MAX_PATH + 1];
        char *p = nullptr;
        
        int length = std::snprintf(buf, sizeof(buf), path.c_str());
        if (buf[length - 1] == '/')
            buf[length - 1] = 0;
            
        for (p = buf + 1; *p; p++) {
            if (*p == '/') {
                *p = 0;
                ret = fsFsCreateDirectory(fs, buf);
                *p = '/';
            }
            
            ret = fsFsCreateDirectory(fs, buf);
        }*/
        
        return ret;
    }
    
    Result ExtractFile(ZZIP_DIR *dir, const ZZIP_DIRENT &entry, const std::string &path) {
        /* TODO
		Result ret = 0;
        ZZIP_FILE *src_handle = zzip_file_open(dir, entry.d_name, O_RDONLY);
        if (!src_handle) {
            Log::Error("zzip_file_open(%s) failed\n", path.c_str());
            return -1;
        }
            
        char dest_path[FS_MAX_PATH + 1];
        std::snprintf(dest_path, FS_MAX_PATH, path.c_str());
        
        if (!FS::FileExists(dest_path))
            fsFsCreateFile(fs, dest_path, entry.st_size, 0);
            
        FsFile dest_handle;
        if (R_FAILED(ret = fsFsOpenFile(fs, dest_path, FsOpenMode_Write, &dest_handle))) {
            Log::Error("fsFsOpenFile(%s) failed: 0x%x\n", path.c_str(), ret);
            zzip_file_close(src_handle);
            return ret;
        }
        
        const u64 buf_size = 0x10000;
        s64 offset = 0;
        unsigned char *buf = new unsigned char[buf_size];
        
        zzip_ssize_t bytes_read = 0;
        std::string filename = std::filesystem::path(entry.d_name).filename();
        while (0 < (bytes_read = zzip_read(src_handle, buf, buf_size - 1))) {
            if (R_FAILED(ret = fsFileWrite(&dest_handle, offset, buf, bytes_read, FsWriteOption_Flush))) {
                Log::Error("fsFileWrite(%s) failed: 0x%x\n", path.c_str(), ret);
                delete[] buf;
                zzip_file_close(src_handle);
                fsFileClose(&dest_handle);
                return ret;
            }
            
            offset += bytes_read;
            std::memset(buf, 0, buf_size);
            Popups::ProgressPopup(static_cast<float>(offset), static_cast<float>(entry.st_size), strings[cfg.lang][Lang::ArchiveExtracting], filename.c_str());
        }
        
        delete[] buf;
        fsFileClose(&dest_handle);
        zzip_file_close(src_handle);*/
        return 0; 
    }
    
    void Extract(const char path[FS_MAX_PATH]) {
		/* TODO
        ZZIP_DIR *dir;
        ZZIP_DIRENT entry;
        zzip_error_t error;
        
        dir = zzip_dir_open(path, &error);
        if (!dir) {
            Log::Error("zzip_dir_open(%s) failed: 0x%x\n", path, error);
            return;
        }
            
        while (zzip_dir_read(dir, &entry)) {
            std::string pathname = ArchiveHelper::ConstructDirname(path, entry.d_name);
            ArchiveHelper::RecursiveMakeDir(pathname);
            
            std::string filename = ArchiveHelper::ConstructFilename(path, entry.d_name);
            ArchiveHelper::ExtractFile(dir, entry, filename);
        }
        
        zzip_dir_close(dir);
		*/
    }
}

namespace Popups {
    void ArchivePopup(void) {
        Popups::SetupPopup(strings[cfg.lang][Lang::ArchiveTitle]);
        
        if (ImGui::BeginPopupModal(strings[cfg.lang][Lang::ArchiveTitle], nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text(strings[cfg.lang][Lang::ArchiveMessage]);
            std::string text = strings[cfg.lang][Lang::ArchivePrompt] + std::string(item.entries[item.selected].name) + "?";
            ImGui::Text(text.c_str());
            
            ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
            
            if (ImGui::Button(strings[cfg.lang][Lang::ButtonOK], ImVec2(120, 0))) {
                ImGui::EndPopup();
                ImGui::PopStyleVar();
                ImGui::Render();
                
                char path[FS_MAX_PATH + 1];
                if ((std::snprintf(path, FS_MAX_PATH, "%s/%s", cfg.cwd, item.entries[item.selected].name)) > 0) {
                    ArchiveHelper::Extract(path);
                    FS::GetDirList(cfg.cwd, item.entries);
                    GUI::ResetCheckbox();
                }
                
                ImGui::CloseCurrentPopup();
                item.state = MENU_STATE_FILEBROWSER;
                return;
            }
            
            ImGui::SameLine(0.0f, 15.0f);
            
            if (ImGui::Button(strings[cfg.lang][Lang::ButtonCancel], ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
                item.state = MENU_STATE_FILEBROWSER;
            }
        }
        
        Popups::ExitPopup();
    }
}

#include "config.h"
#include "fs.h"
#include "gui.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "log.h"
#include "windows.h"
#include "file.h"
#include "uri.h"

namespace Windows {
    void FileBrowserWindow(bool *focus, bool *first_item) {
        Windows::SetupWindow();
        
        if (ImGui::Begin("NX-Shell", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
            // Initially set default focus to next window (FS::DirList)
            if (!*focus) {
                ImGui::SetNextWindowFocus();
                *focus = true;
            }
            
            // Display current working directory
            ImGui::TextColored(ImVec4(1.00f, 1.00f, 1.00f, 1.00f), cfg.cwd.c_str());
            
            // Draw storage bar
            ImGui::Dummy(ImVec2(0.0f, 1.0f)); // Spacing
            ImGui::ProgressBar(static_cast<float>(item.used_storage) / static_cast<float>(item.total_storage), ImVec2(1265.0f, 6.0f), "");
            ImGui::Dummy(ImVec2(0.0f, 2.0f)); // Spacing
            
            ImGui::BeginChild("##FS::DirList");
            if (item.entries.size() != 0) {
                for (long unsigned int i = 0; i < item.entries.size(); i++) {
                    std::string filename = item.entries[i].name;
                    
                    if ((item.checked.at(i)) && (item.checked_cwd == cfg.cwd))
                        ImGui::Image(reinterpret_cast<ImTextureID>(check_icon.id), ImVec2(check_icon.width, check_icon.height));
                    else
                        ImGui::Image(reinterpret_cast<ImTextureID>(uncheck_icon.id), ImVec2(uncheck_icon.width, uncheck_icon.height));
                    
                    ImGui::SameLine();
                    
                    FileType file_type = FS::GetFileType(filename);
                    if (item.entries[i].type == FsDirEntryType_Dir)
                        ImGui::Image(reinterpret_cast<ImTextureID>(folder_icon.id), ImVec2(folder_icon.width, folder_icon.height));
                    else
                        ImGui::Image(reinterpret_cast<ImTextureID>(file_icons[file_type].id), ImVec2(file_icons[0].width, file_icons[0].height));
                    
                    ImGui::SameLine();
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4);
                    if (ImGui::Selectable(filename.c_str())) {
                        char path[FS_MAX_PATH + 1];

                        switch(file_type) {
                            case FileTypeArchive:
                                item.state = MENU_STATE_ARCHIVEEXTRACT;
                                break;
                                
                            case FileTypeImage:
                                {
                                    Textures::LoadImageFile(cfg.cwd.join(item.entries[item.selected].name), item.textures);
                                    item.state = MENU_STATE_IMAGEVIEWER;
                                }
                                break;

                            case FileTypeText:
							{
								auto uri = Uri(cfg.cwd).join(item.entries[item.selected].name);

								Log::Exit();

								text_reader.buffer.resize(0);

								auto file = FS::file::open(path, FS::file::Mode::R);

								if(!file)
									Log::Error("fsFsOpenFile(%s) failed\n", path);

								file->read(text_reader.buffer);
								file->close();

								Log::Init();
								item.state = MENU_STATE_TEXTREADER;

								break;
							}
                            default:
                                break;
                        }
                    }
                    
                    if (*first_item) {
                        ImGui::SetFocusID(ImGui::GetID((item.entries[0].name)), ImGui::GetCurrentWindow());
                        ImGuiContext& g = *ImGui::GetCurrentContext();
                        g.NavDisableHighlight = false;
                        *first_item = false;
                    }
                    
                    if (!ImGui::IsAnyItemFocused() && item.state == MENU_STATE_FILEBROWSER)
                        GImGui->NavId = GImGui->CurrentWindow->DC.LastItemId;
                        
                    if (ImGui::IsItemHovered())
                        item.selected = i;
                }
            }
            else
                ImGui::Text("No file entries");
            
            ImGui::EndChild();
        }
        
        Windows::ExitWindow();
    }
}

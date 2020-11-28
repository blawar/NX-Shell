#include "config.h"
#include "fs.h"
#include "gui.h"
#include "imgui.h"
#include "language.h"
#include "log.h"
#include "popups.h"
#include "file.h"
#include "directory.h"

namespace Popups {
    void DeletePopup(void) {
		Popups::SetupPopup(strings[cfg.lang][Lang::OptionsDelete]);
		
		if (ImGui::BeginPopupModal(strings[cfg.lang][Lang::OptionsDelete], nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text(strings[cfg.lang][Lang::DeleteMessage]);
			if ((item.checked_count > 1) && (item.checked_cwd == cfg.cwd)) {
				ImGui::Text(strings[cfg.lang][Lang::DeleteMultiplePrompt]);
				ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
				ImGui::BeginChild("Scrolling", ImVec2(0, 100));
				for (long unsigned int i = 0; i < item.checked.size(); i++) {
					if (item.checked.at(i))
						ImGui::Text(item.entries[i].name);
				}
				ImGui::EndChild();
			}
			else {
				std::string text = strings[cfg.lang][Lang::DeletePrompt] + std::string(item.entries[item.selected].name) + "?";
				ImGui::Text(text.c_str());
			}
			
			ImGui::Dummy(ImVec2(0.0f, 5.0f)); // Spacing
			
			if (ImGui::Button(strings[cfg.lang][Lang::ButtonOK], ImVec2(120, 0))) {
				Result ret = 0;
				Log::Exit();

				if ((item.checked_count > 1) && (item.checked_cwd == cfg.cwd)) {
					for (long unsigned int i = 0; i < item.checked.size(); i++) {
						if (item.checked.at(i)) {
							if(item.entries[i].type == FsDirEntryType_Dir)
							{
								FS::directory::unlink(cfg.cwd.join(item.entries[i].name));
							}
							else
							{
								FS::file::unlink(cfg.cwd.join(item.entries[i].name));
							}

							if (true) {
								auto d = FS::directory::open(cfg.cwd);

								if(d)
								{
									item.entries = d->entries();
								}
								GUI::ResetCheckbox();
								break;
							}
						}
					}
				}
				else
				{
					if(item.entries[item.selected].type == FsDirEntryType_Dir)
					{
						FS::directory::unlink(cfg.cwd.join(item.entries[item.selected].name));
					}
					else
					{
						FS::file::unlink(cfg.cwd.join(item.entries[item.selected].name));
					}
					ret = 0;
				}
				
				if (R_SUCCEEDED(ret)) {
					auto d = FS::directory::open(cfg.cwd);

					if(d)
					{
						item.entries = d->entries();
					}
					GUI::ResetCheckbox();
				}

				Log::Exit();
				ImGui::CloseCurrentPopup();
				item.state = MENU_STATE_FILEBROWSER;
			}
			
			ImGui::SameLine(0.0f, 15.0f);
			
			if (ImGui::Button(strings[cfg.lang][Lang::ButtonCancel], ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
				item.state = MENU_STATE_OPTIONS;
			}
		}
		
		Popups::ExitPopup();
	}
}

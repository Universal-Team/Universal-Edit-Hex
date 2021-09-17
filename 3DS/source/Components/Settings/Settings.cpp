/*
*   This file is part of Universal-Edit
*   Copyright (C) 2019-2021 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "Common.hpp"
#include "DirSelector.hpp"
#include "ListSelection.hpp"
#include "Settings.hpp"
#include "ThemeSelector.hpp"

Settings::SubMode Settings::Mode = Settings::SubMode::Main;


void Settings::Draw() {
	switch(Settings::Mode) {
		case Settings::SubMode::Main:
			this->DrawMain();
			break;

		case Settings::SubMode::Credits:
			this->CE->Draw();
			break;

		case Settings::SubMode::Settings:
			this->DrawSettings();
			break;
	}
};


void Settings::Handler() {
	switch(Settings::Mode) {
		case Settings::SubMode::Main:
			this->HandleMain();
			break;

		case Settings::SubMode::Credits:
			this->CE->Handler();
			break;

		case Settings::SubMode::Settings:
			this->HandleSettings();
			break;
	}
};


/* Draws the Main Mode. */
void Settings::DrawMain() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SETTINGS_MENU"), 310);

	for (uint8_t Idx = 0; Idx < 4; Idx++) {
		Gui::Draw_Rect(this->Menu[Idx].x - 2, this->Menu[Idx].y - 2, this->Menu[Idx].w + 4, this->Menu[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
		Gui::Draw_Rect(this->Menu[Idx].x, this->Menu[Idx].y, this->Menu[Idx].w, this->Menu[Idx].h, UniversalEdit::UE->TData->ButtonColor());
		Gui::DrawStringCentered(24, this->Menu[Idx].y + 9, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr(this->MenuOptions[Idx]));
	}
};


/* Handle Main Mode. */
void Settings::HandleMain() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 4; Idx++) {
			if (Common::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
				this->Funcs[Idx]();
				break;
			}
		}
	}
};


/* Draws the Settings Mode. */
void Settings::DrawSettings() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SETTINGS_MENU"), 310);

	/* Backup stuff. */
	Gui::DrawStringCentered(24, 30, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SETTINGS_SCRIPT_BACKUP"), 280, 20, nullptr, C2D_WordWrap);
	for (uint8_t Idx = 0; Idx < 3; Idx++) {
		if (Idx == UniversalEdit::UE->CData->Backup()) Gui::Draw_Rect(this->SMenu[Idx + 1].x - 2, this->SMenu[Idx + 1].y - 2, this->SMenu[Idx + 1].w + 4, this->SMenu[Idx + 1].h + 4, UniversalEdit::UE->TData->ButtonSelected());
		Gui::Draw_Rect(this->SMenu[Idx + 1].x, this->SMenu[Idx + 1].y, this->SMenu[Idx + 1].w, this->SMenu[Idx + 1].h, UniversalEdit::UE->TData->ButtonColor());
	}

	Gui::DrawStringCentered(-56, this->SMenu[1].y + 4, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("ALWAYS"));
	Gui::DrawStringCentered(24, this->SMenu[2].y + 4, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("NEVER"));
	Gui::DrawStringCentered(104, this->SMenu[3].y + 4, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("ASK"));

	/* Backup Path. */
	Gui::DrawStringCentered(24, 110, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SETTINGS_BACKUP_PATH"), 230, 20, nullptr, C2D_WordWrap);
	Gui::DrawStringCentered(24, 125, 0.38f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CData->BackupPath(), 280);
};


/* Handle Settings Mode. */
void Settings::HandleSettings() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 5; Idx++) {
			if (Common::Touching(UniversalEdit::UE->T, this->SMenu[Idx])) {
				this->SFuncs[Idx]();
				break;
			}
		}
	}

	if (UniversalEdit::UE->Down & KEY_X) this->SetBackupPath();
};


void Settings::LanguageHandler() {
	const std::vector<std::string> Langs = {
		"English"
	};

	std::unique_ptr<ListSelection> LangSelector = std::make_unique<ListSelection>();
	const int Selection = LangSelector->Handler(Common::GetStr("SELECT_LANG"), Langs);

	switch(Selection) {
		case 0:
			UniversalEdit::UE->CData->Lang("en"); Common::LoadLanguage();
			break;

		case 1:
			//UniversalEdit::UE->CData->Lang("de"); Common::LoadLanguage(); // and so on.
			//break;

		default:
			break;
	}
};


void Settings::ThemeHandler() {
	std::unique_ptr<ThemeSelector> TSelector = std::make_unique<ThemeSelector>();
	TSelector->Handler();
};


void Settings::AccessCredits() { Settings::Mode = Settings::SubMode::Credits; };
void Settings::GotoSettings() { Settings::Mode = Settings::SubMode::Settings; };


/*
	Set the Backup State.
	
	0: Do Backup.
	1: Do not.
	2+: Do ask.
*/
void Settings::SetBackup(const uint8_t V) { UniversalEdit::UE->CData->Backup(V); };


/* Set the Backup path. */
void Settings::SetBackupPath() {
	std::unique_ptr<DirSelector> DS = std::make_unique<DirSelector>();
	const std::string Dir = DS->Handler(UniversalEdit::UE->CData->BackupPath(), Common::GetStr("SELECT_BACKUP_PATH"));

	if (Dir != "") UniversalEdit::UE->CData->BackupPath(Dir);
};


void Settings::BackToMain() { Settings::Mode = Settings::SubMode::Main; };
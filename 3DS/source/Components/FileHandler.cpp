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

#include "Actions.hpp"
#include "Common.hpp"
#include "DirSelector.hpp"
#include "FileCompare.hpp"
#include "FileHandler.hpp"
#include "FileBrowser.hpp"
#include "PromptMessage.hpp"
#include "StatusMessage.hpp"

bool FileHandler::Loaded = false;


void FileHandler::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("FILE_HANDLER_MENU"), 310);

	for (uint8_t Idx = 0; Idx < 3; Idx++) { // Load, New File and Compare Files are always visible.
		Gui::Draw_Rect(this->Menu[Idx].x - 2, this->Menu[Idx].y - 2, this->Menu[Idx].w + 4, this->Menu[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
		Gui::Draw_Rect(this->Menu[Idx].x, this->Menu[Idx].y, this->Menu[Idx].w, this->Menu[Idx].h, UniversalEdit::UE->TData->ButtonColor());

		Gui::DrawStringCentered(24, this->Menu[Idx].y + 9, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr(this->MenuOptions[Idx]));
	}

	if (FileHandler::Loaded) { // Save file as.. however is only visible if loaded.
		Gui::Draw_Rect(this->Menu[3].x - 2, this->Menu[3].y - 2, this->Menu[3].w + 4, this->Menu[3].h + 4, UniversalEdit::UE->TData->ButtonSelected());
		Gui::Draw_Rect(this->Menu[3].x, this->Menu[3].y, this->Menu[3].w, this->Menu[3].h, UniversalEdit::UE->TData->ButtonColor());

		Gui::DrawStringCentered(24, this->Menu[3].y + 9, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr(this->MenuOptions[3]));
	}
};


void FileHandler::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 4; Idx++) {
			if (Common::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
				this->Funcs[Idx]();
				break;
			}
		}
	}
};


void FileHandler::LoadFile() {
	if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Scroll) {
		std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
		const std::string EditFile = FB->Handler("sdmc:/", false, Common::GetStr("SELECT_FILE"), { });

		if (EditFile != "") {
			Common::ProgressMessage(Common::GetStr("LOADING_FILE"));

			/* If nullptr, initialize the unique_ptr. */
			if (!UniversalEdit::UE->CurrentFile) UniversalEdit::UE->CurrentFile = std::make_unique<HexData>();
			UniversalEdit::UE->CurrentFile->Load(EditFile, 0xD, 0x2000);
			FileHandler::Loaded = true;
		}

	} else {
		std::unique_ptr<StatusMessage> Msg = std::make_unique<StatusMessage>();
		Msg->Handler(Common::GetStr("ONLY_ACCESS_IN_SCROLLMODE"), -1);
	}
};


void FileHandler::NewFile() {
	if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Scroll) {
		if (Actions::NewFile()) FileHandler::Loaded = true;

	} else {
		std::unique_ptr<StatusMessage> Msg = std::make_unique<StatusMessage>();
		Msg->Handler(Common::GetStr("ONLY_ACCESS_IN_SCROLLMODE"), -1);
	}
};


void FileHandler::SaveFileAs() {
	if (FileHandler::Loaded) {
		if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Scroll) {
			Actions::SaveFileAs();

		} else {
			std::unique_ptr<StatusMessage> Msg = std::make_unique<StatusMessage>();
			Msg->Handler(Common::GetStr("ONLY_ACCESS_IN_SCROLLMODE"), -1);
		}
	}
};


void FileHandler::CompareFiles() {
	std::unique_ptr<FileCompare> FC = std::make_unique<FileCompare>();
	FC->Handler();
};
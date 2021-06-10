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
#include "FileBrowser.hpp"
#include "LabelSelector.hpp"
#include "LUAHelper.hpp"
#include "PromptMessage.hpp"
#include "Utils.hpp"

#define BYTES_PER_LIST 0xD0
#define BYTES_PER_OFFS 0x10

void Utils::Draw() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("UTILS_MENU"), 310);

	if (FileHandler::Loaded) {
		for (uint8_t Idx = 0; Idx < 3; Idx++) {
			Gui::Draw_Rect(this->Menu[Idx].x - 2, this->Menu[Idx].y - 2, this->Menu[Idx].w + 4, this->Menu[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->Menu[Idx].x, this->Menu[Idx].y, this->Menu[Idx].w, this->Menu[Idx].h, UniversalEdit::UE->TData->ButtonColor());

			Gui::DrawString(this->Menu[Idx].x + 5, this->Menu[Idx].y + 5, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr(this->MenuOptions[Idx]));
		};
	};
};

void Utils::Handler() {
	if (UniversalEdit::UE->Down & KEY_TOUCH) {
		for (uint8_t Idx = 0; Idx < 3; Idx++) {
			if (Common::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
				this->Funcs[Idx]();
				break;
			};
		};
	};
};

void Utils::Labels() {
	if (FileHandler::Loaded) {
		std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
		const std::string LBFile = FB->Handler("sdmc:/3ds/Universal-Edit/Hex-Editor/Labels/", true, Common::GetStr("SELECT_LABEL"), { "json" });

		if (LBFile != "") {
			std::unique_ptr<LabelSelector> Label = std::make_unique<LabelSelector>();
			const int Offs = Label->Handler(LBFile);

			if (Offs != -1 && Offs < (int)UniversalEdit::UE->CurrentFile->GetSize()) {
				if (Offs < BYTES_PER_OFFS) {
					HexEditor::OffsIdx = 0;
					HexEditor::CursorIdx = Offs;

				} else {
					HexEditor::OffsIdx = 1 + ((Offs - BYTES_PER_LIST) / BYTES_PER_OFFS);
					HexEditor::CursorIdx = (BYTES_PER_LIST - BYTES_PER_OFFS) + (Offs % BYTES_PER_OFFS);
				};
			};
		};
	};
};


void Utils::Scripts() {
	if (FileHandler::Loaded) {
		std::unique_ptr<LUAHelper> LH = std::make_unique<LUAHelper>();
		LH->RunScript();
	};
};

void Utils::Encoding() {
	if (FileHandler::Loaded) {
		std::unique_ptr<PromptMessage> PMessage = std::make_unique<PromptMessage>();
		const bool Res = PMessage->Handler(Common::GetStr("ENCODING_LOAD"));

		std::unique_ptr<FileBrowser> FB = std::make_unique<FileBrowser>();
		const std::string EncodingFile = FB->Handler((Res ? "sdmc:/3ds/Universal-Edit/Hex-Editor/Encodings/" : "romfs:/encodings/"), true, "Select the Encoding you like to use.", { "json" });

		if (EncodingFile != "") UniversalEdit::UE->CurrentFile->LoadEncoding(EncodingFile);
	};
};
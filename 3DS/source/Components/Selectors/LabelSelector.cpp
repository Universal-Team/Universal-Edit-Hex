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
#include "LabelSelector.hpp"
#include <unistd.h>

#define ENTRIES_ON_LIST 4

void LabelSelector::Draw() {
	C2D_TargetClear(Top, C2D_Color32(0, 0, 0, 0));
	C2D_TargetClear(Bottom, C2D_Color32(0, 0, 0, 0));
	Gui::clearTextBufs();
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	UniversalEdit::UE->DrawTop(); // Keep the top screen for sure.
	UniversalEdit::UE->GData->DrawBottom();
	Gui::Draw_Rect(0, 0, 320, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(0, 20, 320, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 0, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(0, 1, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SELECT_LABEL"), 310);

	/* Now begin to draw the Labels. */
	for (int Idx = 0; Idx < ENTRIES_ON_LIST && Idx < (int)this->Labels.size(); Idx++) {
		if (this->SPos + Idx == this->Selection) Gui::Draw_Rect(this->LPos[Idx + 1].x - 2, this->LPos[Idx + 1].y - 2, this->LPos[Idx + 1].w + 4, this->LPos[Idx + 1].h + 4, UniversalEdit::UE->TData->ButtonSelected());
		Gui::Draw_Rect(this->LPos[Idx + 1].x, this->LPos[Idx + 1].y, this->LPos[Idx + 1].w, this->LPos[Idx + 1].h, UniversalEdit::UE->TData->ButtonColor());

		Gui::DrawStringCentered(0, this->LPos[Idx + 1].y + 4, 0.5f, UniversalEdit::UE->TData->TextColor(), this->Labels[this->SPos + Idx].Title, 240);
		Gui::DrawStringCentered(0, this->LPos[Idx + 1].y + 20, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("OFFSET") + this->Labels[this->SPos + Idx].Offset, 240);
		Gui::DrawStringCentered(0, this->LPos[Idx + 1].y + 30, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SIZE") + std::to_string(this->Labels[this->SPos + Idx].Size), 240);
	};

	C3D_FrameEnd(0);
};


int LabelSelector::Handler(const std::string &LabelJSON) {
	this->SelectionMode = true;

	if (access(LabelJSON.c_str(), F_OK) != 0) return -1;

	/* Open Handle. */
	nlohmann::ordered_json LData = nullptr;
	FILE *File = fopen(LabelJSON.c_str(), "r");
	if (File) {
		Common::ProgressMessage(Common::GetStr("LOADING_LABELS"));

		LData = nlohmann::ordered_json::parse(File, nullptr, false);
		fclose(File);

	} else {
		return -1;
	};

	if (LData.is_discarded()) return -1; // Bad Label data.

	this->Labels.clear();
	int Offset = -1;

	/* Read Data. */
	for (auto Data = LData.begin(); Data != LData.end(); ++Data) {
		this->Labels.push_back(LabelSelector::Label());
		this->Labels[this->Labels.size() - 1].Title = Data.key();

		if (LData[Data.key()].contains("ofs") && LData[Data.key()]["ofs"].is_string()) this->Labels[this->Labels.size() - 1].Offset = LData[Data.key()]["ofs"];
		if (LData[Data.key()].contains("size") && LData[Data.key()]["size"].is_number()) this->Labels[this->Labels.size() - 1].Size = LData[Data.key()]["size"];
	};

	if (!this->Labels.empty()) { // Only do action if not empty.
		while(aptMainLoop() && this->SelectionMode) {
			this->Draw();

			touchPosition T;
			hidScanInput();
			const uint32_t Down = hidKeysDown();
			const uint32_t Repeat = hidKeysDownRepeat();
			hidTouchRead(&T);

			if (Repeat & KEY_DOWN) {
				if (this->Selection < (int)this->Labels.size() - 1) this->Selection++;
				else this->Selection = 0;
			};

			if (Repeat & KEY_UP) {
				if (this->Selection > 0) this->Selection--;
				else this->Selection = this->Labels.size() - 1;
			};

			if (Repeat & KEY_LEFT) {
				if (this->Selection - ENTRIES_ON_LIST >= 0) this->Selection -= ENTRIES_ON_LIST;
				else this->Selection = 0;
			};

			if (Repeat & KEY_RIGHT) {
				if (this->Selection + ENTRIES_ON_LIST < (int)this->Labels.size()) this->Selection += ENTRIES_ON_LIST;
				else this->Selection = this->Labels.size() - 1;
			};

			if (Down & KEY_A) this->SelectionMode = false;
			if (Down & KEY_B) return -1;

			if (Down & KEY_TOUCH) {
				if (Common::Touching(T, this->LPos[0])) return -1;

				for (uint8_t Idx = 0; Idx < ENTRIES_ON_LIST; Idx++) {
					if (this->SPos + Idx < (int)this->Labels.size()) {
						if (Common::Touching(T, this->LPos[Idx + 1])) {
							this->Selection = this->SPos + Idx;
							this->SelectionMode = false;
							break;
						};
					};
				};
			};

			if (this->Selection < this->SPos) this->SPos = this->Selection;
			else if (this->Selection > this->SPos + ENTRIES_ON_LIST - 1) this->SPos = this->Selection - ENTRIES_ON_LIST + 1;
		};
	};

	if (this->Labels[this->Selection].Offset != "" && this->Labels[this->Selection].Offset.size() > 2) {
		if (Labels[this->Selection].Offset.substr(0, 2) == "0x") {
			Offset = std::strtoll(this->Labels[this->Selection].Offset.c_str(), nullptr, 16);
		};
	};

	return Offset;
};
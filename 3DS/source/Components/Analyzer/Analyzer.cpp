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

#include "Analyzer.hpp"
#include "Common.hpp"

bool Analyzer::Endian = false; // Little Endian.
Analyzer::SubMode Analyzer::Mode = Analyzer::SubMode::Main;

void Analyzer::Draw() {
	switch(Analyzer::Mode) {
		case Analyzer::SubMode::Main:
			Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
			Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
			Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("ANALYZER"), 310);

			if (FileHandler::Loaded) {
				for (uint8_t Idx = 0; Idx < 2; Idx++) {
					Gui::Draw_Rect(this->Menu[Idx].x - 2, this->Menu[Idx].y - 2, this->Menu[Idx].w + 4, this->Menu[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
					Gui::Draw_Rect(this->Menu[Idx].x, this->Menu[Idx].y, this->Menu[Idx].w, this->Menu[Idx].h, UniversalEdit::UE->TData->ButtonColor());

					Gui::DrawStringCentered(24, this->Menu[Idx].y + 9, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr(this->MenuOptions[Idx]));
				};
			};
			break;

		case Analyzer::SubMode::Analyze:
			this->_Analyze->Draw();
			break;

		case Analyzer::SubMode::Edit:
			this->EB->Draw();
			break;
	};
};

void Analyzer::AccessAnalyze() { Analyzer::Mode = Analyzer::SubMode::Analyze; };
void Analyzer::AccessEdit() { Analyzer::Mode = Analyzer::SubMode::Edit; };


void Analyzer::Handler() {
	switch(Analyzer::Mode) {
		case Analyzer::SubMode::Main:
			if (FileHandler::Loaded) {
				if (UniversalEdit::UE->Down & KEY_TOUCH) {
					for (uint8_t Idx = 0; Idx < 2; Idx++) {
						if (Common::Touching(UniversalEdit::UE->T, this->Menu[Idx])) {
							this->Funcs[Idx]();
							break;
						};
					};
				};
			};
			break;

		case Analyzer::SubMode::Analyze:
			this->_Analyze->Handler();
			break;

		case Analyzer::SubMode::Edit:
			this->EB->Handler();
			break;
	};
};
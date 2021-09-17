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
#include "FileHandler.hpp"
#include "HexEditor.hpp"

#define BYTES_PER_LIST 0xA0
#define BYTES_PER_OFFS 0x10
#define LINES 0xA

#define ByteGroupSize UniversalEdit::UE->CData->ByteGroup()


void HexEditor::DrawHexOnly() {

};


void HexEditor::DrawTop() {
	if (UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood()) {
		this->DrawHexOnly();
	}
};


void HexEditor::Handler() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood() && UniversalEdit::UE->CurrentFile->GetSize() > 0) {
		if (UniversalEdit::UE->Repeat & KEY_UP) UniversalEdit::UE->CurrentFile->UP();
		if (UniversalEdit::UE->Repeat & KEY_DOWN) UniversalEdit::UE->CurrentFile->DOWN();
		if (UniversalEdit::UE->Repeat & KEY_LEFT) UniversalEdit::UE->CurrentFile->LEFT();
		if (UniversalEdit::UE->Repeat & KEY_RIGHT) UniversalEdit::UE->CurrentFile->RIGHT();
		if (UniversalEdit::UE->Down & KEY_B) UniversalEdit::UE->CurrentFile->BPress();
		if (UniversalEdit::UE->Down & KEY_A) UniversalEdit::UE->CurrentFile->APress();
	}
		
	if (UniversalEdit::UE->Down & KEY_R) {
		if (UniversalEdit::UE->CData->DefaultHexView() < 2) UniversalEdit::UE->CData->DefaultHexView(UniversalEdit::UE->CData->DefaultHexView() + 1);
	}

	if (UniversalEdit::UE->Down & KEY_L) {
		if (UniversalEdit::UE->CData->DefaultHexView() > 0) UniversalEdit::UE->CData->DefaultHexView(UniversalEdit::UE->CData->DefaultHexView() - 1);
	}

	if (UniversalEdit::UE->Down & KEY_SELECT) {
		if (ByteGroupSize < 4) UniversalEdit::UE->CData->ByteGroup(ByteGroupSize + 1);
		else UniversalEdit::UE->CData->ByteGroup(0);
	}
};
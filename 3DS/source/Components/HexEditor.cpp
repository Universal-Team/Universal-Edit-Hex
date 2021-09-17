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
#include "ChangesPrompt.hpp"
#include "HexEditor.hpp"
#include "StatusMessage.hpp"

#define BYTES_PER_OFFS 0x10
#define ByteGroupSize UniversalEdit::UE->CData->ByteGroup()

#define SelectionSize UniversalEdit::UE->CurrentFile->GetSelectionSize()
#define BYTES_PER_LIST UniversalEdit::UE->CurrentFile->GetBytePerList()
#define LINES UniversalEdit::UE->CurrentFile->GetLines()


void HexEditor::DrawHexOnly() {
	/* Display the top bytes '00, 01 02 03 04 ... 0F. */
	for (uint8_t Idx = 0; Idx < this->GetNums(ByteGroupSize); Idx++) {
		/* Highlight the proper section with the selected color, else unselected. */
		Gui::DrawString(this->XPositions[ByteGroupSize][this->GetTopRow(ByteGroupSize, Idx)], 27, 0.4f,
			(UniversalEdit::UE->CurrentFile->GetCursor() % BYTES_PER_OFFS >= this->GetTopRow(ByteGroupSize, Idx) && UniversalEdit::UE->CurrentFile->GetCursor() % BYTES_PER_OFFS < this->GetTopRow(ByteGroupSize, Idx) + this->BytesPerGroup(ByteGroupSize))
			? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint8_t>(this->GetTopRow(ByteGroupSize, Idx)));
	}

	/* Draw Offset list. */
	for (uint8_t Idx = 0; Idx < LINES; Idx++) {
		Gui::DrawString(5, this->YPositions[Idx], 0.4f, UniversalEdit::UE->CurrentFile->GetCursor() / BYTES_PER_OFFS == Idx ? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint32_t>((UniversalEdit::UE->CurrentFile->GetOffs() + Idx) * 0x10));
	}

	/* Draw Data. */
	for (size_t Idx = 0; Idx < (UniversalEdit::UE->CurrentFile->GetBytePerList()) && Idx < UniversalEdit::UE->CurrentFile->GetDisplaySize(); Idx++) {
		uint32_t Color = UniversalEdit::UE->TData->HexRowColor(Idx / 0x10);

		if (Idx >= UniversalEdit::UE->CurrentFile->GetCursor() && Idx < UniversalEdit::UE->CurrentFile->GetCursor() + UniversalEdit::UE->CurrentFile->GetSelectionSize()) {
			if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Change) {
				if (Idx == UniversalEdit::UE->CurrentFile->GetCursor()) Color = UniversalEdit::UE->TData->SelectedByte();

			} else if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Edit) {
				if (Idx == UniversalEdit::UE->CurrentFile->GetCursor()) Color = UniversalEdit::UE->TData->UnselectedByte();
			}
		}

		Gui::DrawString(this->XPositions[ByteGroupSize][Idx % 0x10], this->YPositions[Idx / 0x10], 0.4f, Color, UniversalEdit::UE->CurrentFile->ByteToString(Idx));
	}
};


void HexEditor::DrawTextOnly() {
	/* Display the top bytes '00, 01 02 03 04 ... 0F. */
	for (uint8_t Idx = 0; Idx < this->GetNums(ByteGroupSize); Idx++) {
		/* Highlight the proper section with the selected color, else unselected. */
		Gui::DrawString(this->XPositions[ByteGroupSize][this->GetTopRow(ByteGroupSize, Idx)], 27, 0.4f,
			(UniversalEdit::UE->CurrentFile->GetCursor() % BYTES_PER_OFFS >= this->GetTopRow(ByteGroupSize, Idx) && UniversalEdit::UE->CurrentFile->GetCursor() % BYTES_PER_OFFS < this->GetTopRow(ByteGroupSize, Idx) + this->BytesPerGroup(ByteGroupSize))
			? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint8_t>(this->GetTopRow(ByteGroupSize, Idx)));
	}

	/* Draw Offset list. */
	for (uint8_t Idx = 0; Idx < LINES; Idx++) {
		Gui::DrawString(5, this->YPositions[Idx], 0.4f, UniversalEdit::UE->CurrentFile->GetCursor() / BYTES_PER_OFFS == Idx ? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint32_t>((UniversalEdit::UE->CurrentFile->GetOffs() + Idx) * 0x10));
	}

	/* Draw Data. */
	for (size_t Idx = 0; Idx < (UniversalEdit::UE->CurrentFile->GetBytePerList()) && Idx < UniversalEdit::UE->CurrentFile->GetDisplaySize(); Idx++) {
		uint32_t Color = UniversalEdit::UE->TData->HexRowColor(Idx / 0x10);

		if (Idx >= UniversalEdit::UE->CurrentFile->GetCursor() && Idx < UniversalEdit::UE->CurrentFile->GetCursor() + UniversalEdit::UE->CurrentFile->GetSelectionSize()) {
			if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Change) {
				if (Idx == UniversalEdit::UE->CurrentFile->GetCursor()) Color = UniversalEdit::UE->TData->SelectedByte();

			} else if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Edit) {
				if (Idx == UniversalEdit::UE->CurrentFile->GetCursor()) Color = UniversalEdit::UE->TData->UnselectedByte();
			}
		}

		Gui::DrawString(this->XPositions[ByteGroupSize][Idx % 0x10], this->YPositions[Idx / 0x10], 0.4f, Color, UniversalEdit::UE->CurrentFile->GetChar(Idx));
	}
};


void HexEditor::DrawTextAndHex() {
	/* Display the top bytes '00, 04, 08, 0C. */
	for (uint8_t Idx = 0; Idx < 4; Idx++) { // 32 bit sections.
		Gui::DrawString(this->XPositionsAlt[Idx * 4], 27, 0.38f, UniversalEdit::UE->CurrentFile->GetCursor() % BYTES_PER_OFFS / 4 == Idx ? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint8_t>(Idx * 0x4));
	}

	/* Draw Offset list. */
	for (uint8_t Idx = 0; Idx < LINES; Idx++) {
		Gui::DrawString(5, this->YPositions[Idx], 0.4f, UniversalEdit::UE->CurrentFile->GetCursor() / BYTES_PER_OFFS == Idx ? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint32_t>((UniversalEdit::UE->CurrentFile->GetOffs() + Idx) * 0x10));
	}

	/* Draw Data. */
	for (size_t Idx = 0; Idx < (UniversalEdit::UE->CurrentFile->GetBytePerList()) && Idx < UniversalEdit::UE->CurrentFile->GetDisplaySize(); Idx++) {
		uint32_t Color = UniversalEdit::UE->TData->HexRowColor(Idx / 0x10);

		if (Idx >= UniversalEdit::UE->CurrentFile->GetCursor() && Idx < UniversalEdit::UE->CurrentFile->GetCursor() + UniversalEdit::UE->CurrentFile->GetSelectionSize()) {
			if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Change) {
				if (Idx == UniversalEdit::UE->CurrentFile->GetCursor()) Color = UniversalEdit::UE->TData->SelectedByte();

			} else if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Edit) {
				if (Idx == UniversalEdit::UE->CurrentFile->GetCursor()) Color = UniversalEdit::UE->TData->UnselectedByte();
			}
		}

		Gui::DrawString(this->XPositionsAlt[Idx % 0x10], this->YPositions[Idx / 0x10], 0.38f, Color, UniversalEdit::UE->CurrentFile->ByteToString(Idx));
		Gui::DrawString(this->DecodedPos[Idx % 0x10], this->YPositions[Idx / 0x10], 0.38f, Color, UniversalEdit::UE->CurrentFile->GetChar(Idx));
	}
};


void HexEditor::DrawTop() {
	if (UniversalEdit::UE->CurrentFile) {
		Gui::DrawStringCentered(0, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), UniversalEdit::UE->CurrentFile->EditFile(), 390);

		switch(UniversalEdit::UE->CData->DefaultHexView()) {
			case 0:
			default:
				this->DrawHexOnly();
				break;

			case 1:
				this->DrawTextOnly();
				break;

			case 2:
				this->DrawTextAndHex();
				break;
		}

		Gui::DrawString(5, 27, 0.4f, UniversalEdit::UE->TData->HexOffsetColor(), Common::GetStr("OFFSET_H"), 390);
	}
};


void HexEditor::Handler() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood() && UniversalEdit::UE->CurrentFile->GetSize() > 0) {
		if (UniversalEdit::UE->Repeat & KEY_UP) UniversalEdit::UE->CurrentFile->UP();
		if (UniversalEdit::UE->Repeat & KEY_DOWN) UniversalEdit::UE->CurrentFile->DOWN();
		if (UniversalEdit::UE->Repeat & KEY_LEFT) UniversalEdit::UE->CurrentFile->LEFT();
		if (UniversalEdit::UE->Repeat & KEY_RIGHT) UniversalEdit::UE->CurrentFile->RIGHT();

		/* B: If exit, show change prompt if changes made. */
		if (UniversalEdit::UE->Down & KEY_B) {
			if (UniversalEdit::UE->CurrentFile->GetCurMode() == HexData::EditMode::Edit) {
				if (UniversalEdit::UE->CurrentFile->GetChanges().size() > 0) {
					const bool Res = Actions::Backup();

					if (Res) {
						std::unique_ptr<ChangesPrompt> ChangePrompt = std::make_unique<ChangesPrompt>();
						ChangePrompt->Handler();
					}
				}
			}

			UniversalEdit::UE->CurrentFile->BPress();
		}


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
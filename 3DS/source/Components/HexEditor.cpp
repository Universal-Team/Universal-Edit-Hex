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
#include "HexEditor.hpp"
#include "StatusMessage.hpp"

#define BYTES_PER_LIST 0xD0
#define BYTES_PER_OFFS 0x10
#define LINES 0xD

size_t HexEditor::CursorIdx = 0, HexEditor::OffsIdx = 0;
uint8_t HexEditor::SelectionSize = 1;
#define ByteGroupSize UniversalEdit::UE->CData->ByteGroup()

void HexEditor::DrawHexOnly() {
	/* Display the top bytes '00, 01 02 03 04 ... 0F. */
	for (uint8_t Idx = 0; Idx < this->GetNums(ByteGroupSize); Idx++) {
		/* Highlight the proper section with the selected color, else unselected. */
		Gui::DrawString(this->XPositions[ByteGroupSize][this->GetTopRow(ByteGroupSize, Idx)], 27, 0.4f,
			(HexEditor::CursorIdx % BYTES_PER_OFFS >= this->GetTopRow(ByteGroupSize, Idx) && HexEditor::CursorIdx % BYTES_PER_OFFS < this->GetTopRow(ByteGroupSize, Idx) + this->BytesPerGroup(ByteGroupSize))
			? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint8_t>(this->GetTopRow(ByteGroupSize, Idx)));
	};

	/* Draw Offset list. */
	for (uint8_t Idx = 0; Idx < LINES; Idx++) {
		Gui::DrawString(5, this->YPositions[Idx], 0.4f, HexEditor::CursorIdx / BYTES_PER_OFFS == Idx ? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint32_t>((HexEditor::OffsIdx + Idx) * 0x10));
	};

	for (size_t Idx = HexEditor::OffsIdx * BYTES_PER_OFFS, Idx2 = 0; Idx < (HexEditor::OffsIdx * BYTES_PER_OFFS) + BYTES_PER_LIST && Idx < UniversalEdit::UE->CurrentFile->GetSize(); Idx++, Idx2++) {
		uint32_t Color;
		if (Idx2 >= HexEditor::CursorIdx && Idx2 < HexEditor::CursorIdx + SelectionSize) {
			if (this->IsEditMode() && Idx2 == HexEditor::CursorIdx) {
				Color = UniversalEdit::UE->TData->SelectedByte();

			} else {
				Color = UniversalEdit::UE->TData->UnselectedByte();
			};

		} else {
			Color = UniversalEdit::UE->TData->HexRowColor(Idx2 / 0x10);
		};

		Gui::DrawString(this->XPositions[ByteGroupSize][Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.4f, Color, UniversalEdit::UE->CurrentFile->ByteToString(Idx));
	};
};

void HexEditor::DrawTextOnly() {
	/* Display the top bytes '00, 01 02 03 04 ... 0F. */
	for (uint8_t Idx = 0; Idx < this->GetNums(ByteGroupSize); Idx++) {
		/* Highlight the proper section with the selected color, else unselected. */
		Gui::DrawString(this->XPositions[ByteGroupSize][this->GetTopRow(ByteGroupSize, Idx)], 27, 0.4f,
			(HexEditor::CursorIdx % BYTES_PER_OFFS >= this->GetTopRow(ByteGroupSize, Idx) && HexEditor::CursorIdx % BYTES_PER_OFFS < this->GetTopRow(ByteGroupSize, Idx) + this->BytesPerGroup(ByteGroupSize))
			? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint8_t>(this->GetTopRow(ByteGroupSize, Idx)));
	};

	/* Draw Offset list. */
	for (uint8_t Idx = 0; Idx < LINES; Idx++) {
		Gui::DrawString(5, this->YPositions[Idx], 0.4f, HexEditor::CursorIdx / BYTES_PER_OFFS == Idx ? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint32_t>((HexEditor::OffsIdx + Idx) * 0x10));
	};

	for (size_t Idx = HexEditor::OffsIdx * BYTES_PER_OFFS, Idx2 = 0; Idx < (HexEditor::OffsIdx * BYTES_PER_OFFS) + BYTES_PER_LIST && Idx < UniversalEdit::UE->CurrentFile->GetSize(); Idx++, Idx2++) {
		uint32_t Color;
		if (Idx2 >= HexEditor::CursorIdx && Idx2 < HexEditor::CursorIdx + SelectionSize) {
			if (this->IsEditMode() && Idx2 == HexEditor::CursorIdx) {
				Color = UniversalEdit::UE->TData->SelectedByte();

			} else {
				Color = UniversalEdit::UE->TData->UnselectedByte();
			};

		} else {
			Color = UniversalEdit::UE->TData->HexRowColor(Idx2 / 0x10);
		};

		Gui::DrawString(this->XPositions[ByteGroupSize][Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.4f, Color, UniversalEdit::UE->CurrentFile->GetChar(Idx));
	};
};

void HexEditor::DrawTextAndHex() {
	/* Display the top bytes '00, 04, 08, 0C. */
	for (uint8_t Idx = 0; Idx < 4; Idx++) { // 32 bit sections.
		Gui::DrawString(this->XPositionsAlt[Idx * 4], 27, 0.38f, HexEditor::CursorIdx % BYTES_PER_OFFS / 4 == Idx ? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint8_t>(Idx * 0x4));
	};

	/* Draw Offset list. */
	for (uint8_t Idx = 0; Idx < LINES; Idx++) {
		Gui::DrawString(5, this->YPositions[Idx], 0.4f, HexEditor::CursorIdx / BYTES_PER_OFFS == Idx ? UniversalEdit::UE->TData->HexOffsetHighlight() : UniversalEdit::UE->TData->HexOffsetColor(), Common::ToHex<uint32_t>((HexEditor::OffsIdx + Idx) * 0x10));
	};

	for (size_t Idx = HexEditor::OffsIdx * BYTES_PER_OFFS, Idx2 = 0; Idx < (HexEditor::OffsIdx * BYTES_PER_OFFS) + BYTES_PER_LIST && Idx < UniversalEdit::UE->CurrentFile->GetSize(); Idx++, Idx2++) {
		uint32_t Color;
		if (Idx2 >= HexEditor::CursorIdx && Idx2 < HexEditor::CursorIdx + SelectionSize) {
			if (this->IsEditMode() && Idx2 == HexEditor::CursorIdx) {
				Color = UniversalEdit::UE->TData->SelectedByte();

			} else {
				Color = UniversalEdit::UE->TData->UnselectedByte();
			};

		} else {
			Color = UniversalEdit::UE->TData->HexRowColor(Idx2 / 0x10);
		};

		Gui::DrawString(this->XPositionsAlt[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.38f, Color, UniversalEdit::UE->CurrentFile->ByteToString(Idx));
		Gui::DrawString(this->DecodedPos[Idx2 % 0x10], this->YPositions[Idx2 / 0x10], 0.38f, Color, UniversalEdit::UE->CurrentFile->GetChar(Idx));
	};
};

void HexEditor::DrawTop() {
	if (UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood()) {
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
		};

		Gui::DrawString(5, 27, 0.4f, UniversalEdit::UE->TData->HexOffsetColor(), Common::GetStr("OFFSET_H"), 390);
	};
};


void HexEditor::Handler() {
	if (FileHandler::Loaded && UniversalEdit::UE->CurrentFile && UniversalEdit::UE->CurrentFile->IsGood() && UniversalEdit::UE->CurrentFile->GetSize() > 0) {
		if (this->IsEditMode()) { // Edit the selected byte.
			if (UniversalEdit::UE->Repeat & KEY_UP) {
				if (UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] < 0xFF) {
					UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)]++;
					UniversalEdit::UE->CurrentFile->SetChanges(true);
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_DOWN) {
				if (UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] > 0x0) {
					UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)]--;
					UniversalEdit::UE->CurrentFile->SetChanges(true);
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_RIGHT) {
				if (UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] < 0xF0) {
					UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] += 0x10;
					UniversalEdit::UE->CurrentFile->SetChanges(true);
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_LEFT) {
				if (UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] > 0xF) {
					UniversalEdit::UE->CurrentFile->GetData()[(HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx)] -= 0x10;
					UniversalEdit::UE->CurrentFile->SetChanges(true);
				};
			};

			if (UniversalEdit::UE->Down & KEY_B) {
				this->EditMode = false;
			};

		} else { // Change the Offset.
			if (UniversalEdit::UE->Repeat & KEY_RIGHT) {
				if ((HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx) < UniversalEdit::UE->CurrentFile->GetSize() - 1) {
					if (HexEditor::CursorIdx < BYTES_PER_LIST - 1) HexEditor::CursorIdx++;
					else {
						HexEditor::CursorIdx = BYTES_PER_LIST - BYTES_PER_OFFS;
						HexEditor::OffsIdx++; // One offset row down.
					};
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_LEFT) {
				if ((HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx) > 0) {
					if (HexEditor::CursorIdx > 0) HexEditor::CursorIdx--;
					else {
						HexEditor::CursorIdx = 0xF; // 0xF.
						HexEditor::OffsIdx--; // One offset row down.
					};
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_DOWN) {
				if ((HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx) < UniversalEdit::UE->CurrentFile->GetSize() - BYTES_PER_OFFS) {
					if (HexEditor::CursorIdx >= BYTES_PER_LIST - BYTES_PER_OFFS) HexEditor::OffsIdx++;
					else HexEditor::CursorIdx += BYTES_PER_OFFS;

					if ((HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx) >= UniversalEdit::UE->CurrentFile->GetSize()) {
						if (UniversalEdit::UE->CurrentFile->GetSize() < BYTES_PER_LIST) {
							HexEditor::CursorIdx = UniversalEdit::UE->CurrentFile->GetSize() - 1;

						} else {
							HexEditor::CursorIdx += BYTES_PER_LIST - BYTES_PER_OFFS; // TODO: Set to max.
						};
					};
				};
			};

			if (UniversalEdit::UE->Repeat & KEY_UP) {
				if ((HexEditor::OffsIdx * BYTES_PER_OFFS + HexEditor::CursorIdx) > BYTES_PER_OFFS - 1) {
					if (HexEditor::CursorIdx > 0xF) HexEditor::CursorIdx -= BYTES_PER_OFFS;
					else {
						HexEditor::OffsIdx--; // One offset row up.
					};
				};
			};

			if (UniversalEdit::UE->Down & KEY_A) {
				this->EditMode = true;
			};
		};
	};
		
	if (UniversalEdit::UE->Down & KEY_R) {
		if (UniversalEdit::UE->CData->DefaultHexView() < 2) UniversalEdit::UE->CData->DefaultHexView(UniversalEdit::UE->CData->DefaultHexView() + 1);
	};

	if (UniversalEdit::UE->Down & KEY_L) {
		if (UniversalEdit::UE->CData->DefaultHexView() > 0) UniversalEdit::UE->CData->DefaultHexView(UniversalEdit::UE->CData->DefaultHexView() - 1);
	};

	if (UniversalEdit::UE->Down & KEY_SELECT) {
		if (ByteGroupSize < 4) UniversalEdit::UE->CData->ByteGroup(ByteGroupSize + 1);
		else UniversalEdit::UE->CData->ByteGroup(0);
	};

	if (UniversalEdit::UE->Down & KEY_X) {
		if (FileHandler::Loaded) {
			if ((HexEditor::OffsIdx * BYTES_PER_OFFS) + HexEditor::CursorIdx + HexEditor::SelectionSize <= UniversalEdit::UE->CurrentFile->GetSize()) {
				const int Res = UniversalEdit::UE->CurrentFile->EraseBytes((HexEditor::OffsIdx * BYTES_PER_OFFS) + HexEditor::CursorIdx, HexEditor::SelectionSize);

				if (Res == -1) { // Bad.
					std::unique_ptr<StatusMessage> SMsg = std::make_unique<StatusMessage>();
					SMsg->Handler(Common::GetStr("ERROR_IN_FILE_ERASE"), Res);
					return;
				};

				if (Res == 0) { // Good.
					if (UniversalEdit::UE->CurrentFile->GetSize() == 0) {
						HexEditor::OffsIdx = 0;
						HexEditor::CursorIdx = 0;
						return;
					};
			
					/* Now properly check for cursor index and set it to not go out of screen. */
					if (((HexEditor::OffsIdx * BYTES_PER_OFFS) + HexEditor::CursorIdx) >= UniversalEdit::UE->CurrentFile->GetSize()) {
						if (UniversalEdit::UE->CurrentFile->GetSize() < BYTES_PER_LIST) {
							HexEditor::OffsIdx = 0;
							HexEditor::CursorIdx = UniversalEdit::UE->CurrentFile->GetSize() - 1;

						} else {
							/* Larger than one screen, so set the row & cursor idx. */
							HexEditor::OffsIdx = 1 + (((UniversalEdit::UE->CurrentFile->GetSize() - 0x1) - BYTES_PER_LIST) / BYTES_PER_OFFS);
							HexEditor::CursorIdx = (BYTES_PER_LIST - BYTES_PER_OFFS) + ((UniversalEdit::UE->CurrentFile->GetSize() - 1) % BYTES_PER_OFFS);
						};
					};
				};
			};
		};
	};

	if (UniversalEdit::UE->Down & KEY_Y) {
		if (FileHandler::Loaded) {
			const int Res = UniversalEdit::UE->CurrentFile->InsertBytes((HexEditor::OffsIdx * BYTES_PER_OFFS) + HexEditor::CursorIdx, { 0x0 });

			if (Res == -1) {
				std::unique_ptr<StatusMessage> SMsg = std::make_unique<StatusMessage>();
				SMsg->Handler(Common::GetStr("ERROR_IN_FILE_INSERT"), Res);
			};
		};
	};
};
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
#include "Search.hpp"
#include "StatusMessage.hpp"

#define RESULTS_PER_LIST 6 // 6 Results per list.
#define SEQUENCE_PER_LIST 5 // 5 Sequences per list.

void Search::Draw() {
	switch(this->Mode) {
		case Search::DisplayMode::Sequence:
			this->DrawSequenceList();
			break;

		case Search::DisplayMode::Results:
			this->DrawResultList();
			break;
	};
};

void Search::Handler() {
	switch(this->Mode) {
		case Search::DisplayMode::Sequence:
			this->SequenceHandler();
			break;

		case Search::DisplayMode::Results:
			this->ResultHandler();
			break;
	};
};


/* Displays the Sequence List. */
void Search::DrawSequenceList() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SEARCH_MENU"), 310);

	if (FileHandler::Loaded) {
		for (uint8_t Idx = 0; Idx < SEQUENCE_PER_LIST && Idx < this->Sequences.size(); Idx++) {
			/* Display Sequence List buttons. */
			if (this->SPos + Idx == this->Selection) Gui::Draw_Rect(this->SeqMenu[Idx + 1].x - 2, this->SeqMenu[Idx + 1].y - 2, this->SeqMenu[Idx + 1].w + 4, this->SeqMenu[Idx + 1].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->SeqMenu[Idx + 1].x, this->SeqMenu[Idx + 1].y, this->SeqMenu[Idx + 1].w, this->SeqMenu[Idx + 1].h, UniversalEdit::UE->TData->ButtonColor());

			/* Draw Sequence. */
			Gui::DrawString(this->SeqMenu[Idx + 1].x + 5, this->SeqMenu[Idx + 1].y + 3, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::ToHex<uint8_t>(this->Sequences[this->SPos + Idx]));
			
			/* Display Remove button next to sequence. */
			Gui::Draw_Rect(this->SeqMenu[Idx + 6].x, this->SeqMenu[Idx + 6].y, this->SeqMenu[Idx + 6].w, this->SeqMenu[Idx + 6].h, UniversalEdit::UE->TData->ButtonColor());
		};

		/* Draw add, search and clear buttons. */
		Gui::Draw_Rect(this->SeqMenu[11].x, this->SeqMenu[11].y, this->SeqMenu[11].w, this->SeqMenu[11].h, UniversalEdit::UE->TData->ButtonColor());
		Gui::DrawString(this->SeqMenu[11].x + 3, this->SeqMenu[11].y + 3, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("ADD"));

		Gui::Draw_Rect(this->SeqMenu[12].x, this->SeqMenu[12].y, this->SeqMenu[12].w, this->SeqMenu[12].h, UniversalEdit::UE->TData->ButtonColor());
		Gui::DrawString(this->SeqMenu[12].x + 3, this->SeqMenu[12].y + 3, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("SEARCH"));

		Gui::Draw_Rect(this->SeqMenu[13].x, this->SeqMenu[13].y, this->SeqMenu[13].w, this->SeqMenu[13].h, UniversalEdit::UE->TData->ButtonColor());
		Gui::DrawString(this->SeqMenu[13].x + 3, this->SeqMenu[13].y + 3, 0.4f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("CLEAR"));
	};
};

/* Edit Sequence. */
void Search::EditSequence(const size_t Idx) {
	if (FileHandler::Loaded) {
		if (Idx < this->Sequences.size()) {
			this->Sequences[Idx] = (uint8_t)Common::HexPad(Common::GetStr("ENTER_VALUE_IN_HEX"), this->Sequences[Idx], 0x0, 0xFF, 4);
		};
	};
};

/* Remove Sequence. */
void Search::RemoveSequence(const size_t Idx) {
	if (FileHandler::Loaded) {
		if (Idx < this->Sequences.size()) {
			this->Sequences.erase(this->Sequences.begin() + Idx);

			if (this->Selection > this->Sequences.size() - 1) {
				this->Selection = this->Sequences.size() - 1;
			};
		};
	};
};

/* Add Sequence. */
void Search::AddSequence() {
	if (FileHandler::Loaded) {
		this->Sequences.push_back((uint8_t)Common::HexPad(Common::GetStr("ENTER_VALUE_IN_HEX"), 0x0, 0x0, 0xFF, 4));
	};
};


/* Clear Sequence. */
void Search::ClearSequence() {
	if (FileHandler::Loaded) {
		this->Sequences.clear();
		this->Selection = 0;
	};
};


/* Handle of the Sequence List. */
void Search::SequenceHandler() {
	if (FileHandler::Loaded) {
		if (this->Sequences.size() > 0) {
			if (UniversalEdit::UE->Repeat & KEY_UP) {
				if (this->Selection > 0) this->Selection--;
			};

			if (UniversalEdit::UE->Repeat & KEY_DOWN) {
				if (this->Selection < this->Sequences.size() - 1) this->Selection++;
			};

			if (UniversalEdit::UE->Repeat & KEY_LEFT) {
				if (this->Selection > SEQUENCE_PER_LIST) this->Selection -= SEQUENCE_PER_LIST;
				else this->Selection = 0;
			};

			if (UniversalEdit::UE->Repeat & KEY_RIGHT) {
				if (this->Selection + SEQUENCE_PER_LIST < this->Sequences.size() - 1) this->Selection += SEQUENCE_PER_LIST;
				else this->Selection = this->Sequences.size() - 1;
			};
		};

		if (UniversalEdit::UE->Down & KEY_A) this->EditSequence(this->Selection); // A: Edit.
		if (UniversalEdit::UE->Down & KEY_X) this->RemoveSequence(this->Selection); // X: Remove.
		if (UniversalEdit::UE->Down & KEY_Y) this->AddSequence(); // Y: Add.
		if (UniversalEdit::UE->Down & KEY_SELECT) this->SearchAction(); // SELECT: Search.
		if (UniversalEdit::UE->Down & KEY_B) this->Back(); // B: Back.

		if (UniversalEdit::UE->Down & KEY_TOUCH) {
			if (Common::Touching(UniversalEdit::UE->T, this->SeqMenu[0])) {
				this->Back();
				return;
			};

			bool Touched = false;

			for (uint8_t Idx = 0; Idx < SEQUENCE_PER_LIST; Idx++) { // Edit click.
				if (this->SPos + Idx < this->Sequences.size()) {
					if (Common::Touching(UniversalEdit::UE->T, this->SeqMenu[Idx + 1])) {
						this->EditSequence(this->SPos + Idx);
						Touched = true;
						break;
					};
				};
			};

			if (!Touched) {
				for (uint8_t Idx = 0; Idx < SEQUENCE_PER_LIST; Idx++) { // Remove click.
					if (this->SPos + Idx < this->Sequences.size()) {
						if (Common::Touching(UniversalEdit::UE->T, this->SeqMenu[Idx + 6])) {
							this->RemoveSequence(this->SPos + Idx);
							Touched = true;
							break;
						};
					};
				};
			};

			if (!Touched) {
				for (uint8_t Idx = 0; Idx < 3; Idx++) { // Add, Search and Clear.
					if (Common::Touching(UniversalEdit::UE->T, this->SeqMenu[Idx + 11])) {
						this->Funcs[Idx]();
						break;
					};
				};
			};
		};
	};


	/* Scroll. */
	if (this->Selection < this->SPos) this->SPos = this->Selection;
	else if (this->Selection > this->SPos + SEQUENCE_PER_LIST - 1) this->SPos = this->Selection - SEQUENCE_PER_LIST + 1;
};


void Search::DrawResultList() {
	Gui::Draw_Rect(49, 0, 271, 20, UniversalEdit::UE->TData->BarColor());
	Gui::Draw_Rect(49, 20, 271, 1, UniversalEdit::UE->TData->BarOutline());
	UniversalEdit::UE->GData->SpriteBlend(sprites_arrow_idx, 50, 0, UniversalEdit::UE->TData->BackArrowColor(), 1.0f);
	Gui::DrawStringCentered(24, 2, 0.5f, UniversalEdit::UE->TData->TextColor(), Common::GetStr("FOUND_RESULTS") + std::to_string(this->FoundResults.size()), 310);

	if (FileHandler::Loaded) {
		/* Now begin to draw the contents. */
		for (uint32_t Idx = 0; Idx < RESULTS_PER_LIST && Idx < this->FoundResults.size(); Idx++) {
			if (this->SPos + Idx == this->Selection) Gui::Draw_Rect(this->ResMenu[Idx].x - 2, this->ResMenu[Idx].y - 2, this->ResMenu[Idx].w + 4, this->ResMenu[Idx].h + 4, UniversalEdit::UE->TData->ButtonSelected());
			Gui::Draw_Rect(this->ResMenu[Idx].x, this->ResMenu[Idx].y, this->ResMenu[Idx].w, this->ResMenu[Idx].h, UniversalEdit::UE->TData->ButtonColor());
		
			Gui::DrawStringCentered(24, this->ResMenu[Idx].y + 7, 0.5f, UniversalEdit::UE->TData->TextColor(), "0x" + Common::ToHex<uint32_t>(this->FoundResults[this->SPos + Idx]), 240);
		};
	};
};


/* Search Handler. */
void Search::SearchAction() {
	if (FileHandler::Loaded && this->Sequences.size() > 0) {
		Common::ProgressMessage(Common::GetStr("SEARCH_MATCHES"));
		bool Matched = true;

		for (size_t Idx = 0; Idx < UniversalEdit::UE->CurrentFile->GetSize(); Idx++) {
			Matched = true; // Always set to true first.

			for (size_t Idx2 = 0; Idx2 < this->Sequences.size(); Idx2++) {
				if (Idx + Idx2 < UniversalEdit::UE->CurrentFile->GetSize()) {
					if (UniversalEdit::UE->CurrentFile->GetData()[Idx + Idx2] != this->Sequences[Idx2]) {
						Matched = false;
						break;
					};

				} else { // Index too large.
					Matched = false;
					break;
				};
			};

			if (Matched) this->FoundResults.push_back(Idx);
		};

		if (this->FoundResults.empty()) {
			std::unique_ptr<StatusMessage> Msg = std::make_unique<StatusMessage>();
			Msg->Handler(Common::GetStr("NO_RESULTS_FOUND"), -1);
			return;
		};

		this->Mode = Search::DisplayMode::Results;
		this->SPos = 0, this->Selection = 0;
	};
};


void Search::JumpToSelected(const uint32_t Selected) {
	if (Selected < this->FoundResults.size()) {
		const uint32_t Offs = this->FoundResults[Selected];

		/* Jump to the selected offset. */
		if (Offs < UniversalEdit::UE->CurrentFile->GetSize()) {
			if (Offs < 0xD0) {
				HexEditor::OffsIdx = 0;
				HexEditor::CursorIdx = Offs;

			} else {
				HexEditor::OffsIdx = 1 + ((Offs - 0xD0) / 0x10);
				HexEditor::CursorIdx = (0xD0 - 0x10) + (Offs % 0x10);
			};
		};
	};
};

void Search::ResultHandler() {
	if (FileHandler::Loaded) {
		if (UniversalEdit::UE->Repeat & KEY_UP) {
			if (this->Selection > 0) this->Selection--;
		};

		if (UniversalEdit::UE->Repeat & KEY_DOWN) {
			if (this->Selection < this->FoundResults.size() - 1) this->Selection++;
		};

		if (UniversalEdit::UE->Repeat & KEY_LEFT) {
			if (this->Selection > RESULTS_PER_LIST) this->Selection -= RESULTS_PER_LIST;
			else this->Selection = 0;
		};

		if (UniversalEdit::UE->Repeat & KEY_RIGHT) {
			if (this->Selection + RESULTS_PER_LIST < this->FoundResults.size() - 1) this->Selection += RESULTS_PER_LIST;
			else this->Selection = this->FoundResults.size() - 1;
		};


		if (UniversalEdit::UE->Down & KEY_A) this->JumpToSelected(this->Selection);
		if (UniversalEdit::UE->Down & KEY_B) this->Back();

		if (UniversalEdit::UE->Down & KEY_TOUCH) {
			if (Common::Touching(UniversalEdit::UE->T, this->SeqMenu[0])) {
				this->Back();
				return;
			};

			for (uint8_t Idx = 0; Idx < RESULTS_PER_LIST; Idx++) {
				if (this->SPos + Idx < this->FoundResults.size()) {
					if (Common::Touching(UniversalEdit::UE->T, this->ResMenu[Idx])) {
						this->JumpToSelected(this->SPos + Idx);
						break;
					};
				};
			};
		};
	};

	/* Scroll. */
	if (this->Selection < this->SPos) this->SPos = this->Selection;
	else if (this->Selection > this->SPos + RESULTS_PER_LIST - 1) this->SPos = this->Selection - RESULTS_PER_LIST + 1;
};


void Search::Back() {
	/* Sequence Mode -> Go back to Navigator. */
	if (this->Mode == Search::DisplayMode::Sequence) Navigation::Mode = Navigation::SubMode::Main;
	else { // Result Mode -> Go back to Sequence.
		this->FoundResults.clear();
		this->SPos = 0, this->Selection = 0;
		this->Mode = Search::DisplayMode::Sequence;
	};
};
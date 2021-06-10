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

bool FileHandler::Loaded = false;

void FileHandler::Draw() {

};

void FileHandler::Handler() {
	if (UniversalEdit::UE->Down & KEY_SELECT) {
		UniversalEdit::UE->CurrentFile = std::make_unique<HexData>();
	
		HexEditor::CursorIdx = 0; // After sucessful loading, also reset the Hex Editor cursor.
		HexEditor::OffsIdx = 0;
		FileHandler::Loaded = true;
	};
};

void FileHandler::LoadFile() {

};

void FileHandler::NewFile() {
	UniversalEdit::UE->CurrentFile = std::make_unique<HexData>();
	
	HexEditor::CursorIdx = 0; // After sucessful loading, also reset the Hex Editor cursor.
	HexEditor::OffsIdx = 0;
	FileHandler::Loaded = true;
};

void FileHandler::SaveFile() {
	if (FileHandler::Loaded) {
		if (UniversalEdit::UE->CurrentFile->Changes()) { // Only write if changes have been made.
			const bool Success = UniversalEdit::UE->CurrentFile->WriteBack(UniversalEdit::UE->CurrentFile->EditFile());
			UniversalEdit::UE->CurrentFile->SetChanges(false); // Since we saved, no changes have been made.
		};
	};
};

void FileHandler::SaveFileAs() { };
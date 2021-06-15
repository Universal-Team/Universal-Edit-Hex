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
#include "UniversalEdit.hpp"
#include <dirent.h>
#include <fat.h>
#include <nds.h>

std::unique_ptr<UniversalEdit> UniversalEdit::UE = nullptr;

UniversalEdit::UniversalEdit() {
	keysSetRepeat(25, 2);
	defaultExceptionHandler();

	this->CData = std::make_unique<ConfigData>();
	this->GData = std::make_unique<GFXData>();

	this->FH = std::make_unique<FileHandler>();
	this->HE = std::make_unique<HexEditor>();
	this->_Tab = std::make_unique<Tab>();
};

void UniversalEdit::DrawTop() {
	this->GData->DrawTop();

	if (FileHandler::Loaded) this->HE->DrawTop();
	else printf("Universal-Edit");
};

void UniversalEdit::DrawBottom() {
	this->GData->DrawBottom();
	this->_Tab->Draw();

	switch(this->ActiveTab) {
		case UniversalEdit::Tabs::FileHandler:
			this->FH->Draw();
			break;

		case UniversalEdit::Tabs::Navigator:
		case UniversalEdit::Tabs::Analyzer:
		case UniversalEdit::Tabs::Utils:
		case UniversalEdit::Tabs::Settings:
			break;
	};
};

int UniversalEdit::Handler() {
	while(!this->Exiting) {
		this->DrawTop();
		this->DrawBottom();

		do {
			scanKeys();
			this->Down = keysDown();
			this->Repeat = keysDownRepeat();
			touchRead(&this->T);
			swiWaitForVBlank();
		} while (!this->Down);

		this->_Tab->Handler();
		this->HE->Handler();
		if (this->Down & KEY_START) this->Exiting = true;
		
		switch(this->ActiveTab) {
			case UniversalEdit::Tabs::FileHandler:
				this->FH->Handler();
				break;

			case UniversalEdit::Tabs::Navigator:
			case UniversalEdit::Tabs::Analyzer:
			case UniversalEdit::Tabs::Utils:
			case UniversalEdit::Tabs::Settings:
				break;
		};
	};

	return 0;
};


int main(int argc, char **argv) {
	if (!fatInitDefault()) {
		consoleDemoInit();
		printf("fatInitDefault() failed...");
		printf("\nPress START to exit.");

		while(1) {
			scanKeys();
			if (keysDown() & KEY_START) return 0;
			swiWaitForVBlank();
		};
	};

	/* Create base folders if missing. */
	mkdir("sd:/_nds", 0777);
	mkdir("sd:/_nds/Universal-Edit", 0777);

	/* Hex Editor related. */
	mkdir("sd:/_nds/Universal-Edit/Hex-Editor", 0777);
	mkdir("sd:/_nds/Universal-Edit/Hex-Editor/Backups", 0777);
	mkdir("sd:/_nds/Universal-Edit/Hex-Editor/Labels", 0777);
	mkdir("sd:/_nds/Universal-Edit/Hex-Editor/Scripts", 0777);
	mkdir("sd:/_nds/Universal-Edit/Hex-Editor/Encodings", 0777);

	consoleDemoInit();
	UniversalEdit::UE = std::make_unique<UniversalEdit>();
	return UniversalEdit::UE->Handler();
};
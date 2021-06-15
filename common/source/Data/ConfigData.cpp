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
#include "ConfigData.hpp"
#include <unistd.h>

#ifdef _3DS
	#define CONFIG_PATH "sdmc:/3ds/Universal-Edit/Config.json"
	#define BACKUP_PATH "sdmc:/3ds/Univeral-Edit/Hex-Editor/Backups/"
#elif ARM9
	#define CONFIG_PATH "sd:/_nds/Universal-Edit/Config.json"
	#define BACKUP_PATH "sd:/_nds/Univeral-Edit/Hex-Editor/Backups/"
#endif

/* Detects system language and is used later to set app language to system language. */
std::string ConfigData::SysLang(void) {
	uint8_t Language = 1;
	#ifdef _3DS // 3DS only, does set english on NDS.
		CFGU_GetSystemLanguage(&Language);
	#endif

	switch(Language) {
		case 0:
			return "ja"; // Japanese.

		case 1:
		default:
			return "en"; // English.

		case 2:
			return "fr"; // French.

		case 3:
			return "de"; // German.

		case 4:
			return "it"; // Italian.

		case 5:
			return "es"; // Spanish.

		case 6:
			return "zh-CN"; // Chinese (Simplified).

		case 7:
			return "ko"; // Korean.

		case 8:
			return "nl"; // Dutch.

		case 9:
			return "pt"; // Portuguese.

		case 10:
			return "ru"; // Russian.

		case 11:
			return "zh-TW"; // Chinese (Traditional).
	};
};

/* Loads the Configuration file. */
void ConfigData::Load() {
	if (access(CONFIG_PATH, F_OK) != 0) this->Initialize();

	FILE *File = fopen(CONFIG_PATH, "r");
	this->CFG = nlohmann::json::parse(File, nullptr, false);
	fclose(File);

	this->VBackupPath = BACKUP_PATH; // Still set to default here.

	if (!this->CFG.is_discarded()) {
		this->Backup(this->Get<nlohmann::json::number_integer_t>("Backup", this->Backup()));
		this->BackupPath(this->Get<std::string>("BackupPath", BACKUP_PATH));
		this->ByteGroup(this->Get<nlohmann::json::number_integer_t>("ByteGroup", this->ByteGroup()));
		this->DefaultHexView(this->Get<nlohmann::json::number_integer_t>("DefaultHexView", this->DefaultHexView()));
		this->Lang(this->Get<std::string>("Lang", this->Lang()));
		this->Theme(this->Get<std::string>("Theme", this->Theme()));
	};
};

/* Initializes the Configuration file properly as a JSON. */
void ConfigData::Initialize() {
	FILE *Temp = fopen(CONFIG_PATH, "w");

	const nlohmann::json OBJ = {
		{ "Backup", this->Backup() },
		{ "BackupPath", BACKUP_PATH },
		{ "ByteGroup", this->ByteGroup() },
		{ "DefaultHexView", this->DefaultHexView() },
		{ "Lang", this->SysLang() },
		{ "Theme", this->Theme() }
	};

	const std::string Dump = OBJ.dump(1, '\t');
	fwrite(Dump.c_str(), 1, Dump.size(), Temp);
	fclose(Temp);
};

/* SAV changes to the Configuration, if changes made. */
void ConfigData::Sav() {
	if (this->ChangesMade) {
		this->Set<nlohmann::json::number_integer_t>("Backup", this->Backup());
		this->Set<std::string>("BackupPath", this->BackupPath());
		this->Set<nlohmann::json::number_integer_t>("ByteGroup", this->ByteGroup());
		this->Set<nlohmann::json::number_integer_t>("DefaultHexView", this->DefaultHexView());
		this->Set<std::string>("Lang", this->Lang());
		this->Set<std::string>("Theme", this->Theme());

		FILE *Out = fopen(CONFIG_PATH, "w");

		/* Write changes to file. */
		const std::string Dump = this->CFG.dump(1, '\t');
		fwrite(Dump.c_str(), 1, Dump.size(), Out);
		fclose(Out);
	};
};
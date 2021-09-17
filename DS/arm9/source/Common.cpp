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
#include <unistd.h>


static nlohmann::json AppJSON = nullptr;
static std::string IfNotFound = "";


/*
	Gets a translated string from the JSON.

	const std::string &Key: The string to get from the translation.
*/
const std::string &Common::GetStr(const std::string &Key) {
	if (!AppJSON || !AppJSON.contains(Key)) return IfNotFound; // Since we'd return a reference there, we need to have it like this.

	return AppJSON.at(Key).get_ref<const std::string &>();
};


/* Loads the Language Strings. */
void Common::LoadLanguage() {
	FILE *In = nullptr;
	bool Good = true;

	if (UniversalEdit::UE->CData->Lang() != "") { // Ensure it isn't ''.
		for (size_t Idx = 0; Idx < UniversalEdit::UE->CData->Lang().size(); Idx++) {
			if (UniversalEdit::UE->CData->Lang()[Idx] == '/') { // Contains a '/' and hence breaks.
				Good = false;
				break;
			}
		}
	}

	if (Good) {
		if (access(std::string(std::string("nitro:/lang/") + UniversalEdit::UE->CData->Lang()).c_str(), F_OK) == 0) { // Ensure access is ok.
			if (access((std::string(std::string("nitro:/lang/") + UniversalEdit::UE->CData->Lang() + std::string("/app.json")).c_str()), F_OK) == 0) { // Ensure access is ok.
				In = fopen(std::string(std::string("nitro:/lang/") + UniversalEdit::UE->CData->Lang() + std::string("/app.json")).c_str(), "r");
				if (In)	AppJSON = nlohmann::json::parse(In, nullptr, false);
				fclose(In);
				return;
			}
		}
	}

	Good = false;

	if (!Good) {
		/* Load English. */
		In = fopen("nitro:/lang/en/app.json", "r");
		if (In)	AppJSON = nlohmann::json::parse(In, nullptr, false);
		fclose(In);
		UniversalEdit::UE->CData->Lang("en"); // Set back to english too.
	}
};
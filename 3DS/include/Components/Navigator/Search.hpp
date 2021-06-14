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

#ifndef _UNIVERSAL_EDIT_NAVIGATOR_SEARCH_HPP
#define _UNIVERSAL_EDIT_NAVIGATOR_SEARCH_HPP

#include "structs.hpp"
#include <string>
#include <vector>

class Search {
public:
	void Draw();
	void Handler();
private:
	enum class DisplayMode : uint8_t { Main = 0, Sequence = 1, Results = 2 };
	DisplayMode Mode = DisplayMode::Main;
	uint32_t SPos = 0, Selection = 0, Offs = 0, Size = 0;
	std::vector<uint8_t> Sequences; // All the sequences.
	std::vector<uint32_t> FoundResults; // Found results.

	void EnterOffs();
	void EnterSize();
	void SyncSettings();
	void AccessSequence();

	/* Sequence Stuff. */
	void DrawSequenceList();
	void EditSequence(const size_t Idx);
	void RemoveSequence(const size_t Idx);
	void AddSequence();
	void ClearSequence();
	void SequenceHandler();

	/* Some other Actions. */
	void Back();
	void DrawResultList();
	void SearchAction();
	void ResultHandler();
	void JumpToSelected(const uint32_t Selected);

	const std::vector<Structs::ButtonPos> Menu = {
		{ 50, 0, 20, 20 }, // Back.

		{ 114, 40, 140, 30 }, // Offset.
		{ 114, 90, 140, 30 }, // Size.
		{ 114, 140, 140, 30 }, // Sequence.
		{ 114, 190, 140, 30 } // Search.
	};

	const std::vector<Structs::ButtonPos> SeqMenu = {
		{ 50, 0, 20, 20 }, // Back.

		{ 150, 28, 50, 30 }, // Edit Value.
		{ 150, 63, 50, 30 },
		{ 150, 98, 50, 30 },
		{ 150, 133, 50, 30 },
		{ 150, 168, 50, 30 },

		{ 250, 28, 30, 30 }, // Remove Value.
		{ 250, 63, 30, 30 },
		{ 250, 98, 30, 30 },
		{ 250, 133, 30, 30 },
		{ 250, 168, 30, 30 },

		{ 70, 210, 50, 20 }, // Add.
		{ 240, 210, 50, 20 } // Clear.
	};

	const std::vector<Structs::ButtonPos> ResMenu = {
		{ 114, 28, 141, 30 },
		{ 114, 63, 141, 30 },
		{ 114, 98, 141, 30 },
		{ 114, 133, 141, 30 },
		{ 114, 168, 141, 30 },
		{ 114, 203, 141, 30 }
	};

	/* Sequence Menu Actions. */
	const std::vector<std::function<void()>> Funcs = {
		{ [this]() { this->AddSequence(); } },
		{ [this]() { this->ClearSequence(); } }
	};

	const std::vector<std::function<void()>> FuncsMain = {
		{ [this]() { this->Back(); } },
		{ [this]() { this->EnterOffs(); } },
		{ [this]() { this->EnterSize(); } },
		{ [this]() { this->AccessSequence(); } },
		{ [this]() { this->SearchAction(); } }
	};
};

#endif
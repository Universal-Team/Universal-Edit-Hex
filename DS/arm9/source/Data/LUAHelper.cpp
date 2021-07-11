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
#include "lua.hpp"
#include "LUAHelper.hpp"
#include <dirent.h> // mkdir.
#include <unistd.h> // access().


/*
	Read a value from the currently open file.

	Usage:
	local Res = UniversalEdit.Read(0x0, 0x1000, "uint8_t", false);

	First: Offset to read from.
	Second: Amount of elements from specified type to read.
	Third (Optional): The type to read (uint8_t by default).
	Fourth (Optional): The Endian to read (Little Endian (false) by default).
*/
static int Read(lua_State *LState) {
	if (lua_gettop(LState) != 2 && lua_gettop(LState) != 3 && lua_gettop(LState) != 4) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	bool IsBigEndian = false; // Little Endian.
	std::string Type = "uint8_t"; // Default type.
	const uint32_t Offs = luaL_checkinteger(LState, 1); // Offset to read.
	const uint32_t Elm = luaL_checkinteger(LState, 2); // Elements to read.
	if (lua_gettop(LState) >= 3) Type = (std::string)(luaL_checkstring(LState, 3)); // Get the string of the type.
	if (lua_gettop(LState) == 4) IsBigEndian = lua_toboolean(LState, 4);


	if (Type == "uint8_t" || Type == "u8") {
		if ((Offs + Elm) >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());

		std::vector<uint8_t> Res = UniversalEdit::UE->CurrentFile->ReadScript<uint8_t>(Offs, Elm, IsBigEndian);

		if (lua_checkstack(LState, Res.size())) { // Ensure enough STACK is available.
			lua_createtable(LState, 0, Res.size());
			const int StackTop = lua_gettop(LState);

			for (size_t Idx = 0; Idx < Res.size(); Idx++) { // Set vector into the table.
				lua_pushinteger(LState, Idx);
				lua_pushinteger(LState, Res[Idx]);
				lua_settable(LState, StackTop);
			};

		} else {
			return luaL_error(LState, Common::GetStr("OUT_OF_SPACE").c_str());
		};

	} else if (Type == "uint16_t" || Type == "u16") {
		if (Offs + (Elm * 0x2) - 1 >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());
		
		std::vector<uint16_t> Res = UniversalEdit::UE->CurrentFile->ReadScript<uint16_t>(Offs, Elm, IsBigEndian);

		if (lua_checkstack(LState, Res.size() * 2)) { // Ensure enough STACK is available.
			lua_createtable(LState, 0, Res.size());
			const int StackTop = lua_gettop(LState);

			for (size_t Idx = 0; Idx < Res.size(); Idx++) { // Set vector into the table.
				lua_pushinteger(LState, Idx);
				lua_pushinteger(LState, Res[Idx]);
				lua_settable(LState, StackTop);
			};

		} else {
			return luaL_error(LState, Common::GetStr("OUT_OF_SPACE").c_str());
		};

	} else if (Type == "uint32_t" || Type == "u32") {
		if (Offs + (Elm * 0x4) - 1 >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());

		std::vector<uint32_t> Res = UniversalEdit::UE->CurrentFile->ReadScript<uint32_t>(Offs, Elm, IsBigEndian);

		if (lua_checkstack(LState, Res.size() * 4)) { // Ensure enough STACK is available.
			lua_createtable(LState, 0, Res.size());
			const int StackTop = lua_gettop(LState);

			for (size_t Idx = 0; Idx < Res.size(); Idx++) { // Set vector into the table.
				lua_pushinteger(LState, Idx);
				lua_pushinteger(LState, Res[Idx]);
				lua_settable(LState, StackTop);
			};

		} else {
			return luaL_error(LState, Common::GetStr("OUT_OF_SPACE").c_str());
		};

	} else return luaL_error(LState, Common::GetStr("NOT_A_VALID_TYPE").c_str());

	return 1;
};


/*
	Read bit indexes from the currently open file.

	Usage:
	local Res = UniversalEdit.ReadBit(0x40, 0x1);

	First: Offset to read from.
	Second: Bit index to read from ( 0 - 7 ).
*/
static int ReadBit(lua_State *LState) {
	if (lua_gettop(LState) != 2) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const uint32_t Offs = luaL_checkinteger(LState, 1);
	if (Offs >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());
	const uint8_t BitIndex = luaL_checkinteger(LState, 2);
	if (BitIndex > 7) return luaL_error(LState, Common::GetStr("BIT_INDEX_VALID").c_str());
	
	lua_pushboolean(LState, UniversalEdit::UE->CurrentFile->ReadBit(Offs, BitIndex));
	return 1;
};


/*
	Read lower / upper bits from the currently open file.

	Usage:
	local Res = UniversalEdit.ReadBits(0x40, true);

	First: Offset to read from.
	Second: If reading from the first bits ( true, 0 - 3 ) or the second ( false, 4 - 7 ).
*/
static int ReadBits(lua_State *LState) {
	if (lua_gettop(LState) != 2) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const uint32_t Offs = luaL_checkinteger(LState, 1);
	if (Offs >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());
	const bool First = lua_toboolean(LState, 2);
	lua_pushinteger(LState, UniversalEdit::UE->CurrentFile->ReadBits(Offs, First));
	return 1;
};


/*
	Write a value to the currently open file.

	Usage:
	UniversalEdit.Write(0x0, { 0x0, 0x5, 0x10}, "uint8_t", false);

	First: Offset to write.
	Second: Table of data to insert.
	Third (Optional): The type to write (uint8_t by default).
	Fourth (optional): If writing a big endian (true) or little endian (false, default).
*/
static int Write(lua_State *LState) {
	if (lua_gettop(LState) != 2 && lua_gettop(LState) != 3 && lua_gettop(LState) != 4) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	bool IsBigEndian = false; // Little Endian.
	std::string Type = "uint8_t"; // Default type.
	const uint32_t Offs = luaL_checkinteger(LState, 1);

	if (lua_gettop(LState) >= 3) Type = (std::string)(luaL_checkstring(LState, 3)); // Get the string of the type.
	if (lua_gettop(LState) == 4) IsBigEndian = lua_toboolean(LState, 4);


	if (Type == "uint8_t" || Type == "u8") {
		std::vector<uint8_t> DT;

		if (lua_istable(LState, 2)) { // Get content from TABLE.
			lua_pushnil(LState);

			while (lua_next(LState, 2)) {
				try {
					DT.push_back(lua_tointeger(LState, -1));

				} catch(...) {
					/* Cannot allocate anymore, it's better to have this check in place or it will cause bad behaviour. */
					return luaL_error(LState, Common::GetStr("CANNOT_ALLOCATE_SPACE").c_str());
				};

				lua_pop(LState, 1);
			};
		};

		DT.shrink_to_fit(); // Shrink the vector size to only the size we need.
		if (Offs + DT.size() >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());
		UniversalEdit::UE->CurrentFile->WriteScript<uint8_t>(Offs, DT);

	} else if (Type == "uint16_t" || Type == "u16") {
		std::vector<uint16_t> DT;

		if (lua_istable(LState, 2)) { // Get content from TABLE.
			lua_pushnil(LState);

			while (lua_next(LState, 2)) {
				try {
					DT.push_back(lua_tointeger(LState, -1));
					if (IsBigEndian) DT[DT.size() - 1] = (DT[DT.size() - 1] >> 8) | (DT[DT.size() - 1] << 8);

				} catch(...) {
					/* Cannot allocate anymore, it's better to have this check in place or it will cause bad behaviour. */
					return luaL_error(LState, Common::GetStr("CANNOT_ALLOCATE_SPACE").c_str());
				};

				lua_pop(LState, 1);
			};
		};

		DT.shrink_to_fit(); // Shrink the vector size to only the size we need.
		if (Offs + (DT.size() * 2) - 1 >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());
		UniversalEdit::UE->CurrentFile->WriteScript<uint16_t>(Offs, DT);

	} else if (Type == "uint32_t" || Type == "u32") {
		std::vector<uint32_t> DT;

		if (lua_istable(LState, 2)) { // Get content from TABLE.
			lua_pushnil(LState);

			while (lua_next(LState, 2)) {
				try {
					DT.push_back(lua_tointeger(LState, -1));
					if (IsBigEndian)  DT[DT.size() - 1] = (DT[DT.size() - 1] >> 24) | ((DT[DT.size() - 1] << 8) & 0x00FF0000) | ((DT[DT.size() - 1] >> 8) & 0x0000FF00) | (DT[DT.size() - 1] << 24);

				} catch(...) {
					/* Cannot allocate anymore, it's better to have this check in place or it will cause bad behaviour. */
					return luaL_error(LState, Common::GetStr("CANNOT_ALLOCATE_SPACE").c_str());
				};

				lua_pop(LState, 1);
			};
		};

		DT.shrink_to_fit(); // Shrink the vector size to only the size we need.
		if (Offs + (DT.size() * 4) - 1 >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());
		UniversalEdit::UE->CurrentFile->WriteScript<uint32_t>(Offs, DT);

	} else return luaL_error(LState, Common::GetStr("NOT_A_VALID_TYPE").c_str());

	return 0;
};


/*
	Write a bit index to the currently open file.

	Usage:
	UniversalEdit.WriteBit(0x40, 0, true);

	First: Offset to write to.
	Second: Bit index to write to. ( 0 - 7 )
	Third: If bit is set (true) or not (false).
*/
static int WriteBit(lua_State *LState) {
	if (lua_gettop(LState) != 3) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const uint32_t Offs = luaL_checkinteger(LState, 1);
	if (Offs >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());
	const uint8_t BitIndex = luaL_checkinteger(LState, 2);
	if (BitIndex > 7) return luaL_error(LState, Common::GetStr("BIT_INDEX_VALID").c_str());
	const bool Set = lua_toboolean(LState, 3);

	UniversalEdit::UE->CurrentFile->WriteBit(Offs, BitIndex, Set, false);
	return 0;
};


/*
	Write lower / upper bits to curently open file.

	Usage:
	UniversalEdit.WriteBits(0x40, true, 0x0);

	First: Offset to write to.
	Second: If writing to the first bits ( true, 0 - 3 ) or the second ( false, 4 - 7 ).
	Third: The value to write.
*/
static int WriteBits(lua_State *LState) {
	if (lua_gettop(LState) != 3) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const uint32_t Offs = luaL_checkinteger(LState, 1);
	if (Offs >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());
	const bool First = lua_toboolean(LState, 2);
	const uint8_t Val = luaL_checkinteger(LState, 3);

	UniversalEdit::UE->CurrentFile->WriteBits(Offs, First, Val, false);
	return 0;
};


/*
	Just displays a status message cause why not. :P

	Usage:
	UniversalEdit.StatusMSG("Some Status Message.", 0);

	First: The status message to display.
	Second: The status code to display.
*/
static int StatusMSG(lua_State *LState) {
	return 0;
};


/*
	Just a prompt message that returns true or false, depending on what the user selected.
	
	Usage:
	local Confirmed = UniversalEdit::Prompt("Some question for prompt.");

	First: The message to display for the prompt.
*/
static int Prompt(lua_State *LState) {
	return 0;
};


/*
	Select something from a list and return the selected index as an integer.

	Usage:
	local SelectedIdx = UniversalEdit.SelectList("Select Something from the list.", { "Option 1", "Option 2" });

	First: Message to display of what should be selected or so.
	Second: Table of Strings.
*/
static int SelectList(lua_State *LState) {
	return 0;
};


/*
	Select something from a JSON File list and return the selected index as an integer.

	Usage:
	local SelectedIdx = UniversalEdit.SelectJSONList("Select Something from the list.", "sd:/_nds/Universal-Edit-Hex/Scripts/Test.json");

	First: Message to display of what should be selected or so.
	Second: Path to the JSON File.

	The JSON SHOULD look like this:
	{
		"Option 1": 4,
		"Option 2": 100,
		"Option 3": 1
	}
*/
static int SelectJSONList(lua_State *LState) {
	return 0;
};


/*
	Opens the system Numpad to enter a value.

	Usage:
		local Str = UniversalEdit.Numpad("What should be entered?", 0, 0, 100, 3);

	First: Display what the user should input.
	Second: What should be returned, when the user does cancel.
	Third: The min value.
	Fourth: The max value.
	Fifth: The length of numbers.
*/
static int Numpad(lua_State *LState) {
	return 0;
};


/*
	Opens the system keyboard to enter a hex value.

	Usage:
		local Str = UniversalEdit.Hexpad("What should be entered in hex?", 0x0, 0x0, 0x64, 4);

	First: Display what the user should input.
	Second: What should be returned, when the user does cancel.
	Third: The min value.
	Fourth: The max value.
	Fifth: The length of numbers, including the '0x' identifier
*/
static int HexPad(lua_State *LState) {
	return 0;
};


/*
	Opens the system keyboard to enter a string.

	Usage:
		local Str = UniversalEdit.Keyboard("What should be entered?", "If cancelled, return this.", 6);

	First: Display what the user should input.
	Second: What should be returned, when the user does cancel.
	Third: The max length of allowed signs.
*/
static int Kbd(lua_State *LState) {
	return 0;
};


/*
	Dump the current file's data to a file.

	Usage:
		UniversalEdit.DumpBytes(0x100, 0x50, "sd:/Test.txt");

	First: The offset from where to dump the data.
	Second: The size in bytes to dump.
	Third: Where to write the data to.
*/
static int DumpBytes(lua_State *LState) { // TODO.
	if (lua_gettop(LState) != 3) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());
	const uint32_t Offs = luaL_checkinteger(LState, 1);
	const uint32_t Size = luaL_checkinteger(LState, 2);

	if (Offs + Size >= UniversalEdit::UE->CurrentFile->GetSize()) return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());
	const std::string File = (std::string)(luaL_checkstring(LState, 3));


	FILE *Out = fopen(File.c_str(), "wb");
	if (Out) {
		try {
			std::vector<uint8_t> DT = UniversalEdit::UE->CurrentFile->ReadScript<uint8_t>(Offs, Size);
			fwrite(DT.data(), 1, Size, Out);

		} catch(...) {
			fclose(Out);
			return luaL_error(LState, Common::GetStr("CANNOT_ALLOCATE_SPACE").c_str());
		};
		
		fclose(Out);
	};

	return 0;
};


/*
	Inject a file into the current file's data.

	Usage:
		UniversalEdit.InjectFile(0x100, "sd:/Test.txt");

	First: The offset where to inject the data.
	Second: The file to inject.
*/
static int InjectFile(lua_State *LState) {
	if (lua_gettop(LState) != 2) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());
	const uint32_t Offs = luaL_checkinteger(LState, 1);
	const std::string File = (std::string)(luaL_checkstring(LState, 2));

	if (access(File.c_str(), F_OK) != 0) {
		char Buffer[200] = { 0 };
		snprintf(Buffer, sizeof(Buffer), Common::GetStr("DOES_NOT_EXIST").c_str(), File.c_str());
		return luaL_error(LState, Buffer);
	};

	/* Do the Injection. */
	FILE *F = fopen(File.c_str(), "r");
	if (F) {
		fseek(F, 0, SEEK_END);
		const uint32_t Size = ftell(F);
		fseek(F, 0, SEEK_SET);

		if (Offs + Size >= UniversalEdit::UE->CurrentFile->GetSize()) {
			fclose(F);
			return luaL_error(LState, Common::GetStr("OUT_OF_BOUNDS").c_str());
		};

		
		try { // Ensure it works fine.
			std::vector<uint8_t> Data; Data.resize(Size);
			fread(Data.data(), 1, Size, F);
			UniversalEdit::UE->CurrentFile->WriteScript<uint8_t>(Offs, Data);

		} catch(...) {
			fclose(F);
			return luaL_error(LState, Common::GetStr("CANNOT_ALLOCATE_SPACE").c_str());
		};


		fclose(F);
	};

	return 0;
};


/*
	Select a file from the SD Card and return the selected filepath.

	Usage:
	local SelectedFilePath = UniversalEdit.SelectFile("Select a file", "sd:/_nds/Universal-Edit-Hex/", true, { "lua" });

	First: Message to display of what should be selected or so.
	Second: Default Path.
	Third: If limiting the go back to the specified directory (true) or nah (false).
	Fourth: The table of extensions.
*/
static int SelectFile(lua_State *LState) {
	return 0;
};


/*
	Return the size of the current open file.

	Usage:
		local Size = UniversalEdit.FileSize();

	Could optionally have a string passed in to check for the specified file.
*/
static int FileSize(lua_State *LState) {
	if (lua_gettop(LState) != 0 && lua_gettop(LState) != 1) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	if (lua_gettop(LState) == 1) {
		const std::string F = (std::string)(luaL_checkstring(LState, 1));

		FILE *In = fopen(F.c_str(), "rb");
		if (In) {
			fseek(In, 0, SEEK_END);
			lua_pushinteger(LState, ftell(In));
			fclose(In);
			return 1;

		} else { // If not good, return -1 as the size.
			lua_pushinteger(LState, -1);
			return 1;
		};
	};


	lua_pushinteger(LState, UniversalEdit::UE->CurrentFile->GetSize());
	return 1;
};


/*
	The basic mkdir function.

	Usage:
		UniversalEdit.Mkdir("sd:/Test");
*/
static int Mkdir(lua_State *LState) {
	if (lua_gettop(LState) != 1) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	const std::string Dir = (std::string)(luaL_checkstring(LState, 1));
	mkdir(Dir.c_str(), 0x777);
	return 0;
};


/*
	Return the base path of the app.

	Usage:
		local Path = UniversalEdit.BasePath();
*/
static int BasePath(lua_State *LState) {
	if (lua_gettop(LState) != 0) return luaL_error(LState, Common::GetStr("WRONG_NUMBER_OF_ARGUMENTS").c_str());

	lua_pushstring(LState, "/_nds/Universal-Edit-Hex/");
	return 1;
};


/*
	Displays a frame / progress message.

	Usage:
		UniversalEdit.ProgressMessage("I'm doing this right now.");

	First: The message to display as the progress.
*/
static int ProgressMessage(lua_State *LState) {
	return 0;
};


/*
	Select a Destination for a file or so.

	Usage:
	local SelectedPath = UniversalEdit.SelectDir("Select a destination for file x.", "sd:/_nds/Universal-Edit-Hex/");

	First: Message to display of what should be selected or so.
	Second: Default Path.
*/
static int SelectDir(lua_State *LState) {
	return 0;
};


/* Register our Universal-Edit functions here. */
static constexpr luaL_Reg UniversalEditFunctions[] = {
	{ "Read", Read },
	{ "ReadBit", ReadBit },
	{ "ReadBits", ReadBits },
	{ "Write", Write },
	{ "WriteBit", WriteBit },
	{ "WriteBits", WriteBits },
	{ "StatusMSG", StatusMSG },
	{ "Prompt", Prompt },
	{ "SelectList", SelectList },
	{ "SelectJSONList", SelectJSONList },
	{ "Numpad", Numpad },
	{ "HexPad", HexPad },
	{ "Keyboard", Kbd },
	{ "DumpBytes", DumpBytes },
	{ "InjectFile", InjectFile },
	{ "SelectFile", SelectFile },
	{ "FileSize", FileSize },
	{ "Mkdir", Mkdir },
	{ "BasePath", BasePath },
	{ "ProgressMessage", ProgressMessage },
	{ "SelectDir", SelectDir },
	{ 0, 0 }
};


static void InitLibraries(lua_State *LState) {
	luaL_openlibs(LState); // Standard Libraries.
	
	/* Init UniversalEdit related modules. */
	lua_newtable(LState);
	luaL_setfuncs(LState, UniversalEditFunctions, 0);
	lua_setglobal(LState, "UniversalEdit");
};


void LUAHelper::RunScript() {
	const std::string LUAFile = "sd:/Test.lua"; // TODO: FileSelector handle.

	std::pair<int, std::string> Status = std::make_pair(0, "");
	lua_State *LUAScript = luaL_newstate();
	InitLibraries(LUAScript); // Universal-Edit related modules, such as Read, Write and standard libraries.

	Status.first = luaL_loadfile(LUAScript, LUAFile.c_str());
	if (Status.first == 0) Status.first = lua_pcall(LUAScript, 0, LUA_MULTRET, 0);

	if (Status.first) { // 1+, an error occured.
		Status.second = lua_tostring(LUAScript, -1); // Return error message.
		lua_pop(LUAScript, 1); // Remove error message from LUA Script.
	};

	lua_close(LUAScript);
	UniversalEdit::UE->CurrentFile->UpdateDisplay(); // Refresh, cause of new changes.
};
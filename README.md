# Universal-Edit-Hex
A work in progress Hex Editor for Nintendo 3DS. The name is also still WIP.

Features including:
- [x] Custom Themes
- [x] Labels
- [x] Basic Hex Editing (such as insert, remove, value change, Jump to...)
- [x] LUA Scripts
- [x] Custom Encoding
- [x] Multi Language

... And more!

Check out the wiki in the future to read more about the features.

## Known things
- [x] The 3DS version seems to cause a `std::bad_alloc` when loading a file around ~ 32+ MB. That is likely caused due to 3DS memory limits or so. Unsure about the NDS version size yet. If you want to edit larger files, please use a PC Hex Editor, because a PC has more memory than a 3DS or DS(i). Alternatively, you could try GodMode9 on 3DS or GodMode9i on DS(i).

## Compilation

### Setting up your environment

To build Universal-Edit-Hex from source, you will need to install devkitARM, libctru citro2d and citro3d. Follow devkitPro's [Getting Started](https://devkitpro.org/wiki/Getting_Started) page to install pacman, then run the following command to install everything you need:
```
(sudo dkp-)pacman -S 3ds-dev
```
(The `sudo dkp-` may not be needed depending on your OS).

You will also need [bannertool](https://github.com/Steveice10/bannertool/releases/latest) and [makerom](https://github.com/profi200/Project_CTR/releases/latest) in your PATH.

### Cloning the repository

To download the source you will need to clone the repository with submodules, this can be done by running:
```
git clone --recursive https://github.com/Universal-Team/Universal-Edit-Hex.git
```

If you've already cloned it, you can use the following command to update all of the submodules:
```
git submodule update --init --recursive
```

### Building

Once you've cloned the repository (with submodules), simply run `make` in the root of the repository.

## Screenshots

<details><summary>Screenshots</summary>

### File Handler
![Main](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/main.png)

### Top Screen
![Hex Only](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/hexonly.png)
![Text Only](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/textonly.png)
![Text and Hex](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/textandhex.png)

![Byte Group 1](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/bytegroup1.png)
![Byte Group 2](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/bytegroup2.png)
![Byte Group 3](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/bytegroup3.png)
![Byte Group 4](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/bytegroup4.png)

### Navigator
![Navigator](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/navigator.png)
![Search](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/search.png)
![Search Result](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/searchresults.png)
![Remove Insert](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/reminsert.png)

### Analyzer
![Analyze](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/analyze.png)
![Analyzer](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/analyzer.png)
![Edit Bytes](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/editbytes.png)

### Utils
![Utils](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/utils.png)
![Labels](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/labels.png)
![Scripts](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/scripts.png)
![Encoding](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/encoding.png)

### Settings
![Settings](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/settings.png)
![Language](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/language.png)
![Themes](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/themes.png)
![Credits](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/credits.png)

### Others
![File Selector](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/fileselector.png)
![Directory Selector](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/directoryselector.png)
![Status Message](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/statusmsg.png)
![Prompt Message](https://github.com/Universal-Team/Universal-Edit-Hex/blob/main/resources/screenshots/prompt.png)

</details>

## Credits
- [SuperSaiyajinStackZ](https://github.com/SuperSaiyajinStackZ) - Main developer of Universal-Edit-Hex.
- [Pk11](https://github.com/Epicpkmn11) - Main developer of Universal-Edit-Hex as well.
- [Icons8](https://icons8.com/) - Icon Designer.
- [LUA Team](https://www.lua.org/) - The LUA code from the `common/lua` directory.
- [devkitPro](https://github.com/devkitpro) - devkitARM, Libctru, citro2d, citro3d.
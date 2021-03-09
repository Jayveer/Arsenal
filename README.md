
# Arsenal


Arsenal is a free open source tool designed to be used with the PS2 version of the game Metal Gear Solid 2. It allows the user to extract Face and Stage archive files packaged with the game. The dictionary provided is thanks to [GirianSeed](https://github.com/GirianSeed) you can check out his mgs dictionary project [here](https://github.com/Joy-Division/JoyDict)

This project uses [Zlib](https://github.com/madler/zlib) by [Mark Adler](https://github.com/madler)

### To Do
 - Add multithreaded extract
 - Add error handling
 - Create GUI variant
 - Clean up the code

##  Usage

Currently only a CLI version of the application exists. An optional output directory can also be added. it is also possible to just drag the file you wish to extract on the executable. A dictionary file is also included in order to resolve a lot of the hashes.

If you wish to use the dictionary to resolve filenames place it alonside Arsenal.exe, please note this program is currently only designed to work with the PS2 version of MGS2.

```
Arsenal.exe "path\to\STAGE.DAT"
```
The above instruction will extract all files from STAGE.DAT to the current directory.

```
Arsenal.exe -sub "path\to\STAGE.DAT"
```
The above instruction will extract all files from STAGE.DAT for the subsistence version of the game.

```
Arsenal.exe "path\to\FACE.DAT" "path\to\output"
```
The same can be used for Face files. An optional output path can be added at the end, if it is not included then it will extract to the directory of the file being extracted.

## License
[MIT](LICENSE.md)
This project falls under the MIT license.
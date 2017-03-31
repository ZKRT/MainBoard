@echo off
md ..\Project\Release

echo turn executable files from axf to bin by tool names fromelf in filefordler of D:\Keil_v5\ARM\ARMCC\bin\ .
D:\Keil_v5\ARM\ARMCC\bin\fromelf.exe --bin -o ..\Project\Objects\MainBoard.bin ..\Project\Objects\MainBoard.axf

echo copy release files to release folder.
copy ..\Project\Objects\MainBoard.bin ..\Project\Release\MainBoard-V01-00-170331.bin
copy ..\Project\Objects\MainBoard.hex ..\Project\Release\MainBoard-V01-00-170331.hex

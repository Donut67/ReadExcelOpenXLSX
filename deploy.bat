@echo off
REM Create deployment directory if it doesn't exist
if not exist deploy mkdir deploy

REM Copy executable file
copy /Y "build\output\ReadExcel.exe" deploy\

REM Copy library file (.a file)
copy /Y "build\output\libOpenXLSX.a" deploy\

REM Copy data files
copy /Y config.json deploy\

REM Copy config directory recursively
xcopy /Y /E config deploy\config\

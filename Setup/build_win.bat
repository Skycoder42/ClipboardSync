:: %1: $$PWD
:: %2: path to Qt-Installer-Framwork

@echo off

:: copy elements

xcopy "%~1\config\" .\Setup\base\* /y /s /e
xcopy "%~1\packages\" .\Setup\base\* /y /s /e

::"%~2\bin\repogen.exe" --update-new-components -p ./packages ./online_repos/win_x64
::"%~2\bin\binarycreator.exe" -n -c ./config/config.xml -p ./packages ./QtIFW-Advanced_Sample_setup_win_x64_online.exe


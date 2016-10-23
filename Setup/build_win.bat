:: %1: $$PWD
:: %2: path to Qt-Installer-Framwork
:: %3: path to Qt-Kit
@echo off
cd Setup

rmdir .\deploy /s /q
rmdir .\config /s /q
rmdir .\packages /s /q

:: make deploy
mkdir deploy
xcopy ..\ClipboardSyncCore\release\ClipboardSyncCore.dll .\deploy\* /y
xcopy ..\ClipboardSyncServer\release\ClipboardSyncServer.exe .\deploy\* /y
xcopy ..\ClipboardSyncClient\release\ClipboardSyncClient.exe .\deploy\* /y
xcopy ..\ClipboardSyncWizard\release\ClipboardSyncWizard.exe .\deploy\* /y
cd deploy
"%~3\bin\windeployqt.exe" --release ./ClipboardSyncServer.exe
"%~3\bin\windeployqt.exe" --release ./ClipboardSyncClient.exe
"%~3\bin\windeployqt.exe" --release ./ClipboardSyncWizard.exe
cd ..

:: copy elements
xcopy "%~1\config" .\config\ /y /s /e
xcopy "%~1\packages" .\packages\ /y /s /e

:: move apps and libraries
mkdir .\packages\com.SkyCoder42.ClipboardSyncServer\data
move /y .\deploy\ClipboardSyncServer.exe .\packages\com.SkyCoder42.ClipboardSyncServer\data\ClipboardSyncServer.exe

mkdir .\packages\com.SkyCoder42.ClipboardSyncClient\data
move /y .\deploy\ClipboardSyncClient.exe .\packages\com.SkyCoder42.ClipboardSyncClient\data\ClipboardSyncClient.exe

mkdir .\packages\com.SkyCoder42.ClipboardSyncWizard\data
move /y .\deploy\ClipboardSyncWizard.exe .\packages\com.SkyCoder42.ClipboardSyncWizard\data\ClipboardSyncWizard.exe

del .\deploy\vcredist_x64.exe
move /y .\deploy .\packages\data
move /y .\packages\data .\packages\com.SkyCoder42.ClipboardSyncCore

"%~2\bin\repogen.exe" --update-new-components -p ./packages ./repository
"%~2\bin\binarycreator.exe" -n -c ./config/config.xml -p ./packages ./Clipboard_Sync_Setup_x64.exe


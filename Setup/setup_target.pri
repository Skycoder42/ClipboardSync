win32 {
	wintarget.target = installer
	wintarget.commands = "$$shell_path($$PWD/build_win.bat)" "$$shell_path($$PWD)" "C:\\Qt\\Tools\\QtInstallerFramework\\2.0" "C:\\Qt\\5.7\\msvc2015_64"

	QMAKE_EXTRA_TARGETS += wintarget
}

win32 {
	wintarget.target = setup
	wintarget.commands = "$$shell_path($$PWD/build_win.bat)" "$$shell_path($$PWD)" "C:\\Qt\\Tools\\QtInstallerFramework\\2.0"

	QMAKE_EXTRA_TARGETS += wintarget
}

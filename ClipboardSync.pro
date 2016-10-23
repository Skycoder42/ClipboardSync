TEMPLATE = subdirs

SUBDIRS += ClipboardSyncCore \
	ClipboardSyncServer \
	ClipboardSyncClient \
	ClipboardSyncWizard \
    Setup

ClipboardSyncServer.depends += ClipboardSyncCore
ClipboardSyncClient.depends += ClipboardSyncCore
ClipboardSyncWizard.depends += ClipboardSyncServer ClipboardSyncClient
Setup.depends += ClipboardSyncCore ClipboardSyncServer ClipboardSyncClient ClipboardSyncWizard

include(./Setup/setup_target.pri)

#For Windows:
#libeay32.dll	OpenSSL Shared Library	The OpenSSL Project, http://www.openssl.org/	C:\Windows\System32\libeay32.dll
#ssleay32.dll	OpenSSL Shared Library	The OpenSSL Project, http://www.openssl.org/	C:\Windows\System32\ssleay32.dll

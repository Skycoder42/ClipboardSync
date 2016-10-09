TEMPLATE = subdirs

SUBDIRS += ClipboardSyncCore \
	ClipboardSyncServer \
	ClipboardSyncClient \
	ClipboardSyncWizard

ClipboardSyncServer.depends += ClipboardSyncCore
ClipboardSyncClient.depends += ClipboardSyncCore
ClipboardSyncWizard.depends += ClipboardSyncServer ClipboardSyncClient

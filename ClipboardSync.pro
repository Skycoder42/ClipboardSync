TEMPLATE = subdirs

SUBDIRS += ClipboardSyncWizard \
    ClipboardSyncServer \
    ClipboardSyncCore \
    ClipboardSyncClient

ClipboardSyncServer.depends += ClipboardSyncCore

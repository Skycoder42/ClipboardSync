TEMPLATE = subdirs

SUBDIRS += ClipboardSyncWizard \
    ClipboardSyncServer \
    ClipboardSyncCore

ClipboardSyncServer.depends += ClipboardSyncCore

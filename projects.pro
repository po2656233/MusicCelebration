TEMPLATE = subdirs
CONFIG -= ordered
SUBDIRS = libqtmdk musiccelebration
qtHaveModule(quick): {
    SUBDIRS += musiccelebration
}

libqtmdk.file = $$PWD/qmdk/libqtmdk.pro

musiccelebration.file = MusicCelebration.pro
musiccelebration.depends = libqtmdk



TEMPLATE    = subdirs
SUBDIRS	    =\
    library\
    elokab-terminal

CONFIG += qt \
          release


INCLUDEPATH +=common\
              usr\
              etc

#---------------------------------------------
#                   INSTALL
#---------------------------------------------

applicationsData.files=usr/share/applications/*
applicationsData.path=/usr/share/applications/

elokabData.files=usr/share/elokab/*
elokabData.path=/usr/share/elokab

INSTALLS +=     applicationsData \
                elokabData


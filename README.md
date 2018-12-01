# طرفية العقاب
طرفية خفيفة تدعم اللعة العربية
هذا البرنامج جزء من سطح المكتب العقاب

# Elokab-terminal
léger tarminal qui prend en charge la langue arabe
ce programme fait partie de elokab desktop

# Elokab-terminal 
lightweight terminal program that supports the Arabic language......
this program is part of the elokab desktop

![Screenshots](https://github.com/zakariakov/screenshots/blob/master/elokab-terminal.png)

# Install

git clone https://github.com/zakariakov/elokab-terminal.git

cd elokab-terminal

qmake

make

sudo make install

#  Nautilus script - terminal-here

This script will open a elokab-Terminal in the current directory.

Save this script under $HOME/.local/nautilus/scripts/terminal-here. Make sure that

you give this file executable permission. { chmod +x terminal-here }

        #!/bin/bash

        if [ "$NAUTILUS_SCRIPT_CURRENT_URI" == "x-nautilus-desktop:///" ]; then
                DIR=$HOME
        else
                        DIR=`echo $NAUTILUS_SCRIPT_CURRENT_URI | sed 's/^file:\/\///' | sed 's/%20/ /g'`
        fi

        elokab-terminal --working-directory "$DIR"

        exit 0


# AppImage v:02

x86_64

https://sourceforge.net/projects/elokab/files/elokab-terminal/Elokab_Terminal-x86_64.AppImage/download



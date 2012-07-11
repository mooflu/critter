#!/bin/csh
if( "$1" == "")then
    echo Please specify a relase version
    exit
endif

echo Start release $1 build at `date`

#----------------------- Source package

#update version
sed "s/__NO_VERSION__/$1/" < configure.in > tmpfile
mv -f tmpfile configure.in

sed "s/__NO_VERSION__/$1/" < critter.spec > tmpfile
mv -f tmpfile critter.spec

./autogen.sh

tar cvf CriticalMass-$1.tar.bz2 --dereference --bzip2 --exclude=.git CriticalMass-$1
7z a -xr\!\.git\* CriticalMass-$1.7z CriticalMass-$1



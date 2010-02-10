#!/bin/bash

opwd=`pwd`
cd /tmp

echo Checking out...
git clone $opwd
cd Martta

if [ "$1" = "" ]; then
	archdir="Martta-$(date +%Y%m%d)"
else
	archdir="Martta-$1"
fi
archfile="$archdir.tar.bz2"

echo Cleaning backup files...
find . | grep \~ | xargs rm -f

echo Cleaning others...
rm release.sh link.sh

echo Cleaning versioning...
rm -rf .git .gitignore

echo Renaming directory...
cd ..
mv Martta $archdir

echo Creating archive...
tar c $archdir | bzip2 -- > $archfile

echo Packaging...
cd "$archdir"
./package.sh

echo Cleaning up...
rm -rf $archdir
mv /tmp/$archfile ~

echo Done.

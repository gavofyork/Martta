#!/bin/bash

cd /tmp
svn co file:///Volumes/DATA/quidprocode.repo/Martta Martta
cd Martta
find . | grep \\.svn\$ | xargs rm -rf
cd /tmp
if [ "$1" = "" ]
then
	name="Martta-$(date +%Y%m%d)"
else
	name="Martta-$1"
fi
rm -f Martta/makeRelease.sh
rm -f Martta/src/updateProject.sh
mv Martta $name
tar c $name | bzip2 -- > "$name.tar.bz2"
rm -rf $name
mv "$name.tar.bz2" ~

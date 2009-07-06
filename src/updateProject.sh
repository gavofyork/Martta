#!/bin/bash

#rm Composed.cpp

DIRS=". $(find [A-Z]* -type d | grep -v \\. | xargs echo)"
COMPOSED=""
for i in $DIRS; do
	rm "$i/Composed.cpp"
	echo "// Composed source file for directory $i" > /tmp/Composed.cpp
	for j in $i/*.cpp; do
		echo "#include \"$j\"" >> /tmp/Composed.cpp
	done
	mv /tmp/Composed.cpp "$i"
	COMPOSED="$COMPOSED $i/Composed.cpp"
done

SUPPORT="$(find ../support | grep \\.inl\$ | xargs echo) $(find ../support | grep \\.h\$ | xargs echo)"
#SOURCES="$(find *.cpp [A-Z]* | grep \\.cpp\$ | grep -v _ | grep -v \\/\\. | perl -i -p -e "s:\\./::gc" | xargs echo)"
HEADERS="$(find *.h [A-Z]* | grep \\.h\$ | grep -v _ | grep -v \\/\\. | perl -i -p -e "s:\\./::gc" | xargs echo)"

#for i in $SOURCES; do echo "#include \"$i\"" >> Composed.cpp; done
#echo >> Composed.cpp

SOURCES="$COMPOSED"
perl -i -p -e "s:^ALL_SOURCES \\=.*\$:ALL_SOURCES = $SOURCES:gc" src.pro

perl -i -p -e "s:^HEADERS \\=.*\$:HEADERS = $HEADERS:gc" src.pro

perl -i -p -e "s:^SUPPORT\\.files \\=.*\$:SUPPORT.files = $SUPPORT:gc" src.pro

if (( $(echo $OSTYPE | grep darwin | wc -l) )) ; then
	qmake -spec macx-xcode
else
	cd .. 
	qmake
	cd src
fi


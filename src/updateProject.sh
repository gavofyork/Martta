#!/bin/bash

#rm Composed.cpp

DIRS=". $(find [A-Z]* -type d | grep -v \\. | xargs echo)"
MARTTA_COMPOSED=""
for i in $DIRS; do
	rm "$i/Composed.cpp"
	echo "// Composed source file for directory $i" > /tmp/Composed.cpp
	for j in $i/*.cpp; do
		echo "#include \"$j\"" >> /tmp/Composed.cpp
	done
	mv /tmp/Composed.cpp "$i"
	MARTTA_COMPOSED="$MARTTA_COMPOSED $i/Composed.cpp"
done

SUPPORT_SOURCES="$(find ../support | grep \\.cpp\$ | xargs echo)"
SUPPORT_HEADERS="$(find ../support | grep \\.h\$ | xargs echo)"
SUPPORT_INLINES="$(find ../support | grep \\.inl\$ | xargs echo)"
MARTTA_SOURCES="$(find *.cpp [A-Z]* | grep \\.cpp\$ | grep -v _ | grep -v Composed.cpp | grep -v \\/\\. | perl -i -p -e "s:\\./::gc" | xargs echo)"
MARTTA_HEADERS="$(find [A-Z]* | grep \\.h\$ | grep -v _ | grep -v \\/\\. | perl -i -p -e "s:\\./::gc" | xargs echo)"
MARTTA_INLINES="$(find [A-Z]* | grep \\.inl\$ | grep -v _ | grep -v \\/\\. | perl -i -p -e "s:\\./::gc" | xargs echo)"

echo DIRS = $DIRS > files.pro
echo MARTTA_COMPOSED = $MARTTA_COMPOSED >> files.pro
echo MARTTA_SOURCES = $MARTTA_SOURCES >> files.pro
echo MARTTA_HEADERS = $MARTTA_HEADERS >> files.pro
echo MARTTA_INLINES = $MARTTA_INLINES >> files.pro
echo SUPPORT_SOURCES = $SUPPORT_SOURCES >> files.pro
echo SUPPORT_HEADERS = $SUPPORT_HEADERS >> files.pro
echo SUPPORT_INLINES = $SUPPORT_INLINES >> files.pro

if (( $(echo $OSTYPE | grep darwin | wc -l) )) ; then
	qmake -spec macx-xcode
else
	cd .. 
	qmake
	cd src
fi


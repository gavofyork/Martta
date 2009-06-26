#!/bin/bash

stem=$1
super=$2
name=${stem/[a-zA-Z\/]*\/}

cp ../templates/object.h "$stem.h"
cp ../templates/object.cpp "$stem.cpp"
perl -i -p -e "s/_NAME_/$name/gc" $stem.h $stem.cpp
perl -i -p -e "s/_SUPER_/$super/gc" $stem.h $stem.cpp


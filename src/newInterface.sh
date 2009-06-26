#!/bin/bash

stem=$1
name=${stem/[a-zA-Z\/]*\/}

cp ../templates/interface.h "$stem.h"
cp ../templates/interface.cpp "$stem.cpp"
perl -i -p -e "s/_NAME_/$name/gc" $stem.h $stem.cpp


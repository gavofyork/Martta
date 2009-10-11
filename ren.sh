#!/bin/bash

from="$1"
to="$2"
perl -i -p -e "s/$from/$to/gc" `find . -type f` ${0/ren.sh/link.sh}
for i in `find . -type d | grep $from`; do j="${i/$from/$to}"; mv "$i" "$j"; git add $j; done
for i in `find . -type f | grep $from`; do j="${i/$from/$to}"; mv "$i" "$j"; git add $j; done

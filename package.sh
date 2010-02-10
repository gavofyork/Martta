#!/bin/bash

set -e
rm -f ../martta_*
debuild -S -sa
cd ..
dput -f ppa:r-launchpad-gavofyork-fastmail-fm/martta martta_*_source.changes


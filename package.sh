#!/bin/bash

set -e
rm -f ../Martta_*
debuild -S -sa
cd ..
dput -f ppa:r-launchpad-gavofyork-fastmail-fm/martta Martta_*_source.changes


system(echo $$CONFIG)
CONFIG(debug): system(echo debug)
CONFIG(release): system(echo release)

TEMPLATE = subdirs
SUBDIRS = support Language Editor

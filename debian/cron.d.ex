#
# Regular cron jobs for the exscalibar package
#
0 4	* * *	root	[ -x /usr/bin/exscalibar_maintenance ] && /usr/bin/exscalibar_maintenance

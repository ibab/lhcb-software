#!/usr/bin/env python

from CondDBUI.PyCoolDiff import diff

from sys import argv

(lhs,lhs_tag,rhs,rhs_tag,dest) = argv[1:]

# diff(originalDB, modifiedDB, diffDB,
#      nodeName='/',
#      since=0L, until=9223372036854775807L, channels=<ROOT.cool::ChannelSelection object>,
#      originalTAG='', modifiedTAG='')

diff(lhs,rhs,dest,originalTAG=lhs_tag, modifiedTAG=rhs_tag)

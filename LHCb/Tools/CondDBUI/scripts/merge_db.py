#!/usr/bin/env python

from CondDBUI import merge

from sys import argv

(source,target) = argv[1:]

merge(source,target)

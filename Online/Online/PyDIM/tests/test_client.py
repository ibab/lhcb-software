#!/usr/bin/env python
# encoding: utf-8
"""
test_client.py

Created by Albert Puig on 2008-02-04.
Copyright (c) 2008 . All rights reserved.
"""

import sys
import os
import time
from dimcpp import *
import dimc

def dic_command_callback(*args):
	print '     Dic callback called. Args are',args
	

if __name__ == '__main__':
#	d=DimRpcInfo('Test','I','I',5,-1)
#	value=0
#	while(1):
		try:
			dimc.dic_info_service('Test', 'I:1', dic_command_callback,4,0,0)
#		 	d.setData(value)
#			value=d.getInt()
#			print "Return value="+str(value)
			time.sleep(5)
		except KeyboardInterrupt:
			print("Exiting...")
			sys.exit()		


#!/usr/bin/env python
# encoding: utf-8
"""
Prova.py

Created by Albert Puig on 2008-01-31.
Copyright (c) 2008 . All rights reserved.
"""

import sys
import os
import time
from dimcpp import *
import dimc

def service1(tag):
	global counter
	counter=counter+1	
#	counter=34
 	return counter

if __name__ == '__main__':
	global counter
	counter=1
	svc1=dimc.dis_add_service('Test','I:1',service1,0)
	dimc.dis_start_serving("Test server")
	while(1):
		try:
			dimc.dis_update_service(svc1)
			time.sleep(10)
		except KeyboardInterrupt:
			print("Exiting...")
			sys.exit()
			

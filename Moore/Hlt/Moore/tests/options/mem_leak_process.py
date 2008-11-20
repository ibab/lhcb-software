#!/usr/bin/env gaudirun.py

# this option file is for a qmtest
# uses the standard option file "options/Moore.py" 
# but makes sure:
#          EventSelector().PrintFreq = 200
#          Moore().EvtMax = 50000
#
# Should be used in combination
# with mem_leak_monitor.py
#
# Author: Stephan Nies    

import os, re

# load the Moore.py executable option script
main_moore_option_file = os.environ['MOOREROOT'] + '/options/Moore.py'
f = file(main_moore_option_file)
main_moore_content = f.read()

# execute all the configuration steps in Moore.py
# but remove the lines that would start Moore 
my_moore = re.sub('Moore\(\)\.applyConf\(\)','', main_moore_content)
my_moore = re.sub('print Moore\(\)','', my_moore)
exec my_moore 

# now reconfigure as needed
EventSelector().PrintFreq = 200
Moore().EvtMax = 50000 

# finally run Moore
Moore().applyConf()
print Moore()


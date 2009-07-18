#!/usr/bin/env python
from shutil  import copy2
from os import environ,makedirs
from os.path import exists, dirname
import  subprocess
import string
import sys

cmt = subprocess.Popen(['cmt','show','version'],stdout=subprocess.PIPE)
version = string.strip(cmt.communicate()[0],'\n')

### use TCKUtils to generate the right TCK manifest, and copy it 
### directly into InstallArea...
manifest = environ['HLTTCKROOT'] + '/manifest/MOORE_' + version
target_dir = dirname( sys.argv[1] )
if exists( manifest)        : 
    print 'copying ' + manifest + ' to ' + sys.argv[1]
    if not exists( target_dir ) : makedirs( target_dir )
    copy2( manifest,  sys.argv[1] )
else:
    print 'manifest  ' + manifest + ' does not exist'

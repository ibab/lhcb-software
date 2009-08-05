#!/usr/bin/env python
from shutil  import copy2
from os import environ,makedirs
from os.path import exists, dirname
import  subprocess
import string
import sys

### use the TCKUtils generated manifest for this version, and copy it 
### directly into InstallArea... (if it exists)
manifest = sys.argv[1]
target_dir = dirname( sys.argv[2] )
if exists( manifest)        : 
    print 'copying ' + manifest + ' to ' + sys.argv[2]
    if not exists( target_dir ) : makedirs( target_dir )
    copy2( manifest,  sys.argv[1] )
else:
    print 'manifest  ' + manifest + ' does not exist'

#!/usr/bin/env python
from shutil  import copy2
from os import makedirs
from os.path import exists, dirname
from sys import argv

### use the TCKUtils generated manifest for this version, and copy it 
### directly into InstallArea... (if it exists)
manifest = argv[1]
target_dir = dirname( argv[2] )
if exists( manifest)        : 
    print 'copying ' + manifest + ' to ' + argv[2]
    if not exists( target_dir ) : makedirs( target_dir )
    copy2( manifest,  argv[2] )
else:
    print 'manifest  ' + manifest + ' does not exist'

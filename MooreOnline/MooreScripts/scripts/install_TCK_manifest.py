#!/usr/bin/env python
from shutil  import copy2
from os import makedirs
from os.path import exists, dirname
from sys import argv

def cpManifest(source,target) :
    target_dir = dirname( target )
    if not exists( source) : return False
    print 'copying %s to %s'  %(source,target_dir)
    if not exists( target_dir ) : makedirs( target_dir )
    copy2( manifest,  target )
    return True


### use the TCKUtils generated manifest for this version, and copy it 
### directly into InstallArea... (if it exists)
manifest = argv[1]
target = argv[2]
if not cpManifest(manifest,target) :
    import re
    # in case of dev, try without the 'dev' postfix
    manifest = re.sub('dev$','',manifest)
    if not cpManifest(manifest,target) :
        #  try again without the 'pX' suffix
        manifest = re.sub('p\d+$','',manifest)
        if not cpManifest(manifest,target) :
               print 'WARNING: could not locate TCK manifest %s for %s'

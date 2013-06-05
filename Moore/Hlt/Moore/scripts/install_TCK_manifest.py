#!/usr/bin/env python
from shutil  import copy2
from os import makedirs
from os.path import exists, dirname
from sys import argv

def cpManifest(source,target) :
	target_dir = dirname( target )
	if exists( source)        : 
	    print 'copying %s to %s'  %(source,target_dir)
	    if not exists( target_dir ) : makedirs( target_dir )
	    copy2( manifest,  target )
	    return True
	else:
	    print 'manifest  ' + manifest + ' does not exist'
            return False


### use the TCKUtils generated manifest for this version, and copy it 
### directly into InstallArea... (if it exists)
manifest = argv[1]
target = argv[2]
if not cpManifest(manifest,target) :
    # in case of dev, try without the 'dev' postfix
    if manifest.endswith('dev') :
        manifest = manifest.rstrip('dev')
        cpManifest(manifest,target)
    for suffix in [ 'p1','p2','p3','p4','p5' ] :
        if manifest.endswith(suffix) :
	    manifest = manifest[0:len(manifest)-2]
            cpManifest(manifest,target)
            continue

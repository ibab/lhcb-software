'''
Created on May 7, 2013

@author: Ben Couturier
'''
#!/usr/bin/env python

import subprocess
from LbRelease.SoftConfDB.AppImporter import AppImporter

appnames = [ "Brunel", "Moore", "Bender", "DaVinci", "Panoramix" ]
allapps = {}
for a in appnames:
    allapps[a.upper()] = 'svn+ssh://svn.cern.ch/reps/lhcb/%s/tags/%s' % (a, a.upper())

s = AppImporter()

for k in allapps.keys():
    app = k
    cmd = [ 'svn', 'ls', allapps[k] ]
    #print cmd
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    out, err = p.communicate()

    for line in out.splitlines():
        line2 = line.replace("/", "")
        if line.startswith(app) and "_v" in line:
            version = line2.split("_")[1]
            print "====> Importing  %s %s" % (app, version)
            s.recurseDependencies(app, version)

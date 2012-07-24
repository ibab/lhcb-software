import os, sys, time
import subprocess

os.system('echo $ROOTSYS')

print "ROOT environment configured"

fileout = 'out_'+sys.argv[1]+'.txt'

print fileout

print sys.argv[2]

os.system('./ampFit ' + sys.argv[1] + ' < '+ sys.argv[2] +' > ' + fileout)

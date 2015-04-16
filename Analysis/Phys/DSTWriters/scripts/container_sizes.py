from ROOT import *
# script modified for new Root persistency
dst    = True
sdst   = False
nmax   = 10000

import os
if len(os.sys.argv) > 1 : 
 inputfile = os.sys.argv[1]
else:
 inputfile =  '$PANORAMIXDATA/Bu2JpsiK_00011917_00000048_1.dst'

# get the basic configuration from here
#from LHCbConfig import *
#lhcbApp.DataType = "2011"
from Gaudi.Configuration import *

appConf = ApplicationMgr( OutputLevel = INFO, AppName = 'dst_sizes' )
  
import GaudiPython

appMgr = GaudiPython.AppMgr()
sel    = appMgr.evtsel()
sel.open(['PFN:'+inputfile])

evt = appMgr.evtsvc()

appMgr.run(1)
appMgr.executeEvent()

bsizes = {}
zsizes = {}

f  = TFile(inputfile)
fx = open('out.txt','w')


def bytes(br) :
 daughters = br.GetListOfBranches()
 b = br.GetZipBytes(),br.GetTotBytes()
 if daughters:
   for j in range(daughters.GetEntries()):
     temp=bytes(daughters.At(j))
     b = b[0]+temp[0],b[1]+temp[1]
 return b

tree = f.Get('Event')
refs = f.Get('Refs')
frec = f.Get('FileRecords')
nevents  = tree.GetEntries()
allnodes = tree.GetListOfBranches()
for j in range( allnodes.GetEntries() ):
    b = allnodes.At(j)
    bnam = b.GetName()
    if bnam.find("_R.") == len(bnam)-3 : continue
    refnam = bnam[:len(bnam)-1]+"_R."
    r = tree.GetBranch(refnam)
    d_zip,d_tot = bytes(b)
    r_zip,r_tot = bytes(r)
    print bnam,d_zip,d_tot,d_zip,d_tot
    bsizes[bnam] = d_tot+r_tot
    zsizes[bnam] = d_zip+r_zip
    
out = 'new root format  total bytes   zipped'  
print out 
fx.write(out)
out =  '%80s zipped: %4.2F  unzipped: %4.2F'%('Event Total', tree.GetZipBytes() / 1000.,tree.GetTotBytes()/ 1000.) 
print out 
fx.write(out)
out =  '%80s zipped: %4.2F  unzipped: %4.2F'%('Refs Total', refs.GetZipBytes() / 1000.,refs.GetTotBytes()/ 1000.) 
print out 
fx.write(out)
try:
    out =  '%80s zipped: %4.2F  unzipped: %4.2F'%('Filerecords: Total',frec.GetZipBytes() / 1000.,frec.GetTotBytes()/ 1000.) 
    print out 
    fx.write(out)
except:
    # do nothing
    print ''

# printout
print ' container name ',  'size in kb'

items = zsizes.values()
items.sort()

totmeansize = 0
totbsize    = 0

for i in items:
 for x in zsizes:
  if i == zsizes[x] : 
     temp1 = x.replace('_','/')
     temp2 = temp1.replace('.','')
     xx = temp2.replace('/Event/','')
     meansize = zsizes[x] / float(nevents) 
     bsize    = bsizes[x] / float(nevents) 
     totmeansize += meansize
     totbsize    += bsize 
     out =  ' %80s zipped: %7.3F  unzipped: %7.3F'%(xx,meansize / 1000.,bsize / 1000.) 
     print out 
     fx.write(out+'\n')

out =  ' %80s zipped: %7.3F  unzipped: %7.3F'%('Total',totmeansize / 1000.,totbsize / 1000.) 
print out 
fx.write(out+'\n')

print 'check:',totmeansize / 1000.*float(nevents), totbsize / 1000. * float(nevents)
fx.close()

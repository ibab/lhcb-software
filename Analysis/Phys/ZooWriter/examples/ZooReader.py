#!/usr/bin/python
#
# @file ZooReader.py
#
# read an example tuple and make a few plots (very simple example)
#
# see ../options/standalone-simple.py for an example of an option file
# to go with this example
#
# @author Manuel Schiller <manuel.schiller@nikhef.nl>
# @date 2011-09-27

#############################################################################
# BEGIN setup phase: make sure ROOT/Zoo components are accessible
#############################################################################
# get Zoo libs into path
import os, sys, re
if not 'ZOOWRITERROOT' in os.environ:
    # ZOOWRITERROOT environment variable unset, try to guess correct location
    # of libZooROOT.so from location of python script
    if os.path.exists(re.sub('(/|\\\\)[^/\\\\]+$', '/Zoo/libZooROOT.so', \
	    sys.argv[0])):
	os.environ['ZOOWRITERROOT'] = re.sub('(/|\\\\)[^/\\\\]+$', '', \
		sys.argv[0])
    else:
	# try to ask user by reading from stdin
        print 'Please enter location to be used for $ZOOWRITERROOT'
        os.environ['ZOOWRITERROOT'] = re.sub('(\n|\r)+$', '', \
		sys.stdin.readline())
if not 'LD_LIBRARY_PATH' in os.environ or '' == os.environ['LD_LIBRARY_PATH']:
    os.environ['LD_LIBRARY_PATH'] = os.environ['ZOOWRITERROOT'] + '/Zoo'
else:
    os.environ['LD_LIBRARY_PATH'] = \
        os.environ['ZOOWRITERROOT'] + '/Zoo:' + os.environ['LD_LIBRARY_PATH']
# ok, use root-config to get ROOT library path and set up python search paths
# accordingly (on most installations, python does not know about ROOT)
tmp = os.popen('root-config --libdir')
sys.path.append(re.sub('(\n|\r)+$', '', tmp.readline()))
tmp.close()
#############################################################################
# END setup phase
#############################################################################

# get ROOT stuff
from ROOT import *
# load Zoo library (including needed dictionaries)
gSystem.Load('libZooROOT.so')

# create a TChain and add files to it
c = TChain('Forest')
c.Add('MC.root')

# set up the branches we want to read
zev = ZooEv()
myBs = ZooObjRefArray(ZooP)()
myDs = ZooObjRefArray(ZooP)()
myKaons = ZooObjRefArray(ZooP)()
c.SetBranchAddress('Event', AddressOf(zev))
c.SetBranchAddress('MyKaons', AddressOf(myKaons))
c.SetBranchAddress('MyBs', AddressOf(myBs))
c.SetBranchAddress('MyDs', AddressOf(myDs))

TH1.AddDirectory(kFALSE)

# book histograms
mDs = TH1D("mDs", "D_{s} mass;m [MeV];candidates", 300, 1800., 2100.)
pDs = TH1D("pDs", "p of D_{s};p [GeV];candidates", 100, 0., 300.)
ptDs = TH1D("ptDs", "p_{T} of D_{s};p_{T} [GeV];candidates", 100, 0., 50.)
mBs = TH1D("mBs", "B_{s} mass;m [MeV];candidates", 300, 5000., 5600.)
pBs = TH1D("pBs", "p of B_{s};p [GeV];candidates", 100, 0., 300.)
ptBs = TH1D("ptBs", "p_{T} of B_{s};p_{T} [GeV];candidates", 100, 0., 50.)
KDLLKPi = TH1D("KDLLKPi", "Kaon DLL(K-#pi);DLL(K-#pi);entries", 100, -100., 100.)

nevents = c.GetEntries()
nTrueBs = 0
for i in range(0, nevents):
    # read next event
    c.GetEntry(i)
    if 0 == (i % 1000):
	print 'Event ' + str(i) + '/' + str(nevents)
    # loop over MCPs and count true Bs
    for j in range(0, zev.NMCP()):
	zmcp = zev.MCP(j)
	if 531 == abs(zmcp.pid()):
	    nTrueBs = nTrueBs + 1
    # loop over Bs
    for j in range(0, myBs.size()):
	bs = myBs[j]
	mBs.Fill(bs.measuredMass())
	p = bs.momentum()
	pBs.Fill(p.P() / 1e3)
	ptBs.Fill(p.Pt() / 1e3)
    # loop over Ds
    for j in range(0, myDs.size()):
	ds = myDs[j]
	mDs.Fill(ds.measuredMass())
	p = ds.momentum()
	pDs.Fill(p.P() / 1e3)
	ptDs.Fill(p.Pt() / 1e3)
    # loop over Kaons for DLL plots
    for j in range(0, myKaons.size()):
	k = myKaons[j]
	dll = k.DLL()
	if None == dll:
	    continue
	KDLLKPi.Fill(dll.DLLk())

print str(nTrueBs) + ' true Bs found in MC truth.'

c = TCanvas()
mBs.Draw('E1')
c.Print('mBs-python.eps')
pBs.Draw('E1')
c.Print('pBs-python.eps')
ptBs.Draw('E1')
c.Print('ptBs-python.eps')
mDs.Draw('E1')
c.Print('mDs-python.eps')
pDs.Draw('E1')
c.Print('pDs-python.eps')
ptDs.Draw('E1')
c.Print('ptDs-python.eps')
KDLLKPi.Draw('E1')
c.Print('KDLLKPi-python.eps')

#!/usr/bin/env python

import gaudimodule
import PyCintex

import ROOT

#
# setup some ROOT things
#
rootCanvas = ROOT.TCanvas('rootCanvas', 'Canvas', 200, 10, 700, 500 )


#
# load the dictionaries which hold information
# about the LHC classes, etc.
#
gaudimodule.loaddict('PhysEventDict')
gaudimodule.loaddict('PhysDict')

#
# get namespaces
#
std  = gaudimodule.gbl.std
LHCb = gaudimodule.gbl.LHCb


#
# initialise Gaudi
#
# N.B. the above DLLs are needed for the following:
# ParticleMaker       : for 'ParticleStuffer'
# DaVinciTransporter  : for 'ParticleMaker'
# DaVinciTools        : for 'Particle2State'
# DaVinciKernel       : for 'PhysDesktop'
# Tracktools          : for 'TrackSelector'
# ProtoParticleFilter : for 'ProtoParticleCALOFilter'
#
#
# N.B.. the output levels are:
# Fatal (6), Error (5), Warning(4), Infomative (3), Debug (2), Verbose (1)
appMgr = gaudimodule.AppMgr(outputlevel=3)
appMgr.config( files   = ['$STDOPTS/LHCbApplication.opts'
                          ,'$STDOPTS/DstDicts.opts'
                          ],
               options = ['EventSelector.PrintFreq        =   100 '
                          ,'ApplicationMgr.ExtSvc        += { "ParticlePropertySvc"}'
                          ,'ApplicationMgr.DLLs          += { "VertexFit" }'
                          ,'ApplicationMgr.DLLs          += { "DaVinciKernel" }'
                          ,'ApplicationMgr.DLLs          += { "ParticleMaker" }'
                          ,'ApplicationMgr.DLLs          += { "DaVinciTransporter" }'
                          ,'ApplicationMgr.DLLs          += { "TrackExtrapolators" }'
                          ,'ApplicationMgr.DLLs          += { "DaVinciTools" }'
                          ,'ApplicationMgr.DLLs          += { "TrackTools" }'
                          ,'ApplicationMgr.DLLs          += { "ProtoParticleFilter" }'
                          ,'MagneticFieldSvc.FieldMapFile = "$FIELDMAPROOT/cdf/field047.cdf"'
                          ,'DetectorDataSvc.DetDbLocation = "$XMLDDDBROOT/DDDB/lhcb-200601.xml"'
                          ,'ApplicationMgr.TopAlg        += {"PreLoadParticles/PreLoadMuons"}'
                          ,'PreLoadMuons.PhysDesktop.ParticleMakerType                              = "CombinedParticleMaker"'
                          ,'PreLoadMuons.PhysDesktop.CombinedParticleMaker.ExclusiveSelection       = false'
                          ,'PreLoadMuons.PhysDesktop.CombinedParticleMaker.Particles                = {"muon"}'
                          ,'PreLoadMuons.PhysDesktop.CombinedParticleMaker.TrackSelector.TrackTypes = {"Long"}'
                          ,'PreLoadMuons.PhysDesktop.CombinedParticleMaker.Muon.Selection           = {"RequiresDet=\'MUON\' "}'
                          ,'PreLoadMuons.PhysDesktop.OutputLevel                                    = 4'
                          ] )

#
# N.B. for 'tight muon', add: CombDLL(mu-pi)>\'0.0\'
#

#
# define some standard vectors
#
particleVector = std.vector('LHCb::Particle *')
intVector      = std.vector('int')
doubleVector   = std.vector('double')


#
# open file and get access to event
#
sel  = appMgr.evtsel()
# use a local copy of a DST
# sel.open(['PFN:/afs/cern.ch/lhcb/scratch/week/kerzel/00001438_00000001_5.dst'])

# use a (pre-staged) file from CASTOR
sel.open(['PFN:/castor/cern.ch/grid/lhcb/production/DC06/v1-lumi2/00001438/DST/0000/00001438_00000001_5.dst'])
evt  = appMgr.evtsvc()


#
# create some tools
#

# vertex fitter
OfflineVertexFitter = appMgr.toolsvc().create('OfflineVertexFitter', interface='IVertexFit')



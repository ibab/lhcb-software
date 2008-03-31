#!/usr/bin/env python

import GaudiPython
import PyCintex

import ROOT


#
# from Pere Mato
# (to loop over C++ containers)
#
class irange(object) :
  def __init__(self, b, e ):
    self.begin, self.end  = b, e
  def __iter__(self):
    it = self.begin
    while it != self.end :
      yield it.__deref__()
      it.__postinc__(1)



##
## ROOT init
##
rootFile   = ROOT.TFile("ntuple.root", "RECREATE")

#rootCanvas = ROOT.TCanvas('rootCanvas', 'Canvas', 200, 10, 700, 500 )


##
## get namespaces: read C++ namespaces into Python
##
std  = GaudiPython.gbl.std
LHCb = GaudiPython.gbl.LHCb

##
## define some standard vectors
##
particleVector      = std.vector('LHCb::Particle *')
particleConstVector = std.vector('const LHCb::Particle *')
intVector           = std.vector('int')
doubleVector        = std.vector('double')


##
## initialise Gaudi
##
## - read in standard option files (include DaVinci)
## - add special options, override standards settings, etc
##
appMgr = GaudiPython.AppMgr(outputlevel=3)
appMgr.config( files   = ['$STDOPTS/LHCbApplication.opts'
                          ,'$STDOPTS/DstDicts.opts'
                          ,'$DAVINCIROOT/options/DaVinciCommon.opts'
                          ,'$CERN_DATA'
                          ],
                options = ['EventSelector.PrintFreq   =  10 '
                           ,'EventSelector.OutputLevel = 1'
                           ,'ApplicationMgr.TopAlg.OutputLevel = 1 '
                           #,'ApplicationMgr.TopAlg.RootInTES = "/Event/microDST" '
                           #,'DaVinciMainSeq.RootInTES = "/Event/microDST" '
                           #,'ApplicationMgr.TopAlg                += { "StoreExplorerAlg" }'
                           #,'StoreExplorerAlg.Load                = 1'
                           #,'StoreExplorerAlg.PrintFreq           = 1.0'
                           #,'StoreExplorerAlg.Outputlevel         = 1'
                           ,'ApplicationMgr.ExtSvc += {"ParticlePropertySvc"}'
                           ,'ApplicationMgr.ExtSvc += { "DataOnDemandSvc" }'
                           ,'DataOnDemandSvc.Nodes += { "DATA=\'/Event/MC\' TYPE=\'DataObject\'" }'
                           ,'DataOnDemandSvc.Algorithms += {"DATA=\'/Event/MC/Particles\'  TYPE=\'UnpackMCParticle\'",  "DATA=\'/Event/MC/Vertices\' TYPE=\'UnpackMCVertex\'"}'
                           ] )

GaudiPython.loaddict('PhysEventDict') 


##
## access to the EventService, EventSelector 
##
sel  = appMgr.evtsel()
evt  = appMgr.evtsvc()

##
## access to tools, etc
##
OfflineVertexFitter = appMgr.toolsvc().create('OfflineVertexFitter',interface='IVertexFit')


##
## open microDST
##
#sel.open(['PFN:microDST.dst'])

##
## define a small ROOT tree to write as a plain ntuple
##
ROOT.gROOT.ProcessLine(
    "struct EventStruct { \
    Double_t  runNr;              \
    Double_t  eventNr;            \
    };" );               


ROOT.gROOT.ProcessLine(
    "struct ParticleStruct {           \
    Double_t  mass;              \
    Double_t  chi2;              \
    };" );

from ROOT import EventStruct
from ROOT import ParticleStruct

EventInfo = EventStruct()
JPsiInfo  = ParticleStruct()

rootTree = ROOT.TTree("rootTree","ROOT tree")
rootTree.Branch( 'Event' , EventInfo , 'runNr/D:eventNr')
rootTree.Branch( 'JPsi'  , JPsiInfo  , 'mass/D:chi2')
                            

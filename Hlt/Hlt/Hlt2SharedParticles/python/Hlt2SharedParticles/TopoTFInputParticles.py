### @file
#
#  Standard good Particles with IP and Pt.
#
#  @author P. Koppenburg Patrick.Koppenburg@cern.ch
#  @date 2008-07-15
#
##
from Gaudi.Configuration import *
from HltLine.HltLine import bindMembers
from HltLine.HltPVs import PV3D
from Configurables import FilterDesktop

__all__ = ( 'TopoTFInputParticles' )

##########################################################################
# Based on fit tracks
##########################################################################
importOptions("$HLT2LINESROOT/options/Hlt2TrackFitForTopo.py")

Hlt2TopoTFInputParticles = FilterDesktop("Hlt2TopoTFInputParticles"
                                        , Code = "(TRCHI2DOF<10) & (PT>300*MeV) & (P>2*GeV) & (MIPCHI2DV(PRIMARY)>9)"
                                        )
Hlt2TopoTFInputParticles.InputLocations  = [ "Hlt2TFPionsForTopo","Hlt2TFKaonsForTopo" ]


TopoTFInputParticles = bindMembers( None, [ GaudiSequencer('SeqHlt2TFParticlesForTopo'),
                                            PV3D(), Hlt2TopoTFInputParticles ] )


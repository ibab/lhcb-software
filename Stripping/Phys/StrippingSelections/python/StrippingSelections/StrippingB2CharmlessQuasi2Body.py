'''
Module for construction of B->(pipi)(pipi) to use for quasi-two body
B decays involving two light resonances, such as eta, etaprime, omega, Kstar, phi
'''

__author__ = ['Fred Blanc']
__date__ = '25/10/2010'
__version__ = '$Revision: 1.0 $'


# Need __all__ for ???
__all__ = ('Lines')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig
from GaudiKernel.SystemOfUnits import MeV

##############################################################################
## This is the common Jpsi that is used in all of the subsequent selections ##
##############################################################################
# a few functions that we will use. are there alternatives for this in Juan's framework?
B2Quasi2Body4piLines = []

# create a selection using a FilterDesktop
def createSubSel( OutputList, InputList, Cuts ) :
    # create a unique filterdesktop
    filterName = "FilterDesktopFor" + OutputList
    if allConfigurables.get( filterName ) :
        raise ValueError, 'FilterDesktop instance with name '+filterName+' already exists'
    filter = FilterDesktop(filterName, Code = Cuts)
    return Selection( OutputList,
                      Algorithm = filter,
                      RequiredSelections = [ InputList ] )

# create a selection using a ParticleCombiner
def createCombinationSel( OutputList,
                          DecayDescriptor,
                          DaughterLists,
                          DaughterCuts = {} ,
                          PreVertexCuts = "ALL",
                          PostVertexCuts = "ALL" ) :
    combinerName = "CombinerParticlesFor" + OutputList
    if allConfigurables.get( combinerName ) :
        raise ValueError, 'CombineParticles instance with name '+ combinerName+' already exists'
    combiner = CombineParticles( combinerName,
                                 DecayDescriptor = DecayDescriptor,
                                 DaughtersCuts = DaughterCuts,
                                 MotherCut = PostVertexCuts,
                                 CombinationCut = PreVertexCuts,
                                 ReFitPVs = True)
    return Selection ( OutputList,
                       Algorithm = combiner,
                       RequiredSelections = DaughterLists)


# define input daughter lists
TrackList = createSubSel( OutputList = "TracksForCharmlessB",
                          InputList = DataOnDemand(Location = "Phys/StdNoPIDsPions"),
                          Cuts = "(TRCHI2DOF < 4) & (MIPCHI2DV(PRIMARY) > 16) & (PT > 250*MeV)" )

DiTrackList = createCombinationSel( OutputList = "DiTracksForCharmlessB",
                                    DaughterLists = [ TrackList ],
                                    DaughterCuts = { },
                                    DecayDescriptor = "rho(770)0 -> pi+ pi-",
                                    PreVertexCuts = "(APT>1000*MeV) & (AP>10*GeV) & (AM<1000*MeV)",
                                    PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 4)" )


#######################
### B->(pipi)(pipi) ###
#######################
B2Q2B = createCombinationSel( OutputList = "B2Q2B",
                              DecayDescriptor = "B0 -> rho(770)0 rho(770)0",
                              DaughterLists = [ DiTrackList ],
                              DaughterCuts  = {},
                              PreVertexCuts = "(APT>1000*MeV) & in_range(2500,AM,5700)",
                              PostVertexCuts = "(VFASPF(VCHI2/VDOF) < 4)" )

B2Quasi2Body4piLine = StrippingLine("B2Quasi2Body4piLine",
                                    algos = [ createSubSel( InputList = B2Q2B,
                                                            OutputList = B2Q2B.name() + "Prescaled",
                                                            Cuts = "(MAXTREE('pi+'==ABSID, PT) > 1500*MeV)" ) ] ,
                                    prescale = 0.2)

B2Quasi2Body4piLines += [ B2Quasi2Body4piLine ]

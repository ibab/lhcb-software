'''
Module for construction of Bs-->MuMu stripping selections and lines

Exported symbols (use python help!):
   - Bs2MuMuNoMuIDConf
   - makeDefault
   - makeLoose
'''

__author__ = ['Diego Martinez Santos','Johannes Albrecht']
__date__ = '19/07/2010'
__version__ = '$Revision: 1.2 $'

__all__ = ('Bs2MuMuNoMuIDConf',
           'makeDefault',
           'makeLoose')

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig

class Bs2MuMuNoMuIDConf(object) :
    """
    Builder of Bs-> mumu stripping lines: default and loose

    Usage:
    >>> config = { .... }
    >>> bsConf = Bs2MuMuNoMuIDConf('PrescaledBs2MuMuTest',config)
    >>> bsLines = bsConf.lines
    >>> for line in line :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.


    Exports as instance data members:
    selDefault     : nominal Bs2mumu stripping line
    selLoose       : loose Bs2MuMu stripping line to understand systematics
    defaultLine    : Stripping line made from selDefault
    looseLine      : Stripping line made from selLoose
    lines          : lsit of lines:  [ defaultLine, looseLine ]
    
    Exports as class data member:
    Bs2MuMuNoMuIDConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('MuIPChi2_loose',
                              'MuTrChi2_loose',
                              'BIPChi2_loose',
                              'BFDChi2_loose',
                              'DefaultLinePrescale',
                              'DefaultLinePostscale',
                              'LooseLinePrescale',
                              'LooseLinePostscale'
                              )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'DefaultLinePrescale'   : 1,
        'DefaultLinePostscale'  : 1,
        'LooseLinePrescale'     : 1,
        'LooseLinePostscale'    : 1,
        'MuIPChi2_loose'        : 9,
        'MuTrChi2_loose'        : 100,
        'BIPChi2_loose'         : 49,
        'BFDChi2_loose'         : 100
        }                
    
    


    def __init__(self, 
                 name = 'Bs2MuMu',
                 config = None) :

        checkConfig(Bs2MuMuNoMuIDConf.__configuration_keys__,
                    config)

        default_name='Bs2MuMuNoMuID'
        loose_name='Bs2MuMuNoMuIDLoose'

        self.selDefault = makeDefault(default_name)

        self.selLoose = makeLoose(loose_name,
                                  MuIPChi2=config['MuIPChi2_loose'],
                                  MuTrChi2=config['MuTrChi2_loose'],
                                  BIPChi2=config['BIPChi2_loose'],
                                  BFDChi2=config['BFDChi2_loose']
                                  )
        
             
        self.defaultLine = StrippingLine(default_name+"Line",
                                            prescale = config['DefaultLinePrescale'],
                                            postscale = config['DefaultLinePostscale'],
                                            algos = [ self.selDefault ]
                                            )
        self.looseLine = StrippingLine(loose_name+"Line",
                                            prescale = config['LooseLinePrescale'],
                                            postscale = config['LooseLinePostscale'],
                                            algos = [ self.selLoose ]
                                            )

        self.lines = [ self.defaultLine, self.looseLine ]


def makeDefault(name) :
    """
    default Bs2mumu selection object (tighter selection a la roadmap)
    starts from Phys/StdNoPIDsMuons
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    Bs2MuMuNoMuID = CombineParticles("StripBs2MuMuNoMuID")
    Bs2MuMuNoMuID.DecayDescriptor = "B_s0 -> mu+ mu-"
    Bs2MuMuNoMuID.InputLocations = ["Phys/StdNoPIDsMuons"]
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    Bs2MuMuNoMuID.addTool( OfflineVertexFitter() )
    Bs2MuMuNoMuID.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2MuMuNoMuID.OfflineVertexFitter.useResonanceVertex = False
    Bs2MuMuNoMuID.ReFitPVs = True
    Bs2MuMuNoMuID.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> 25.)" }
    Bs2MuMuNoMuID.CombinationCut = "(ADAMASS('B_s0')<600*MeV)";
    Bs2MuMuNoMuID.MotherCut = "(VFASPF(VCHI2/VDOF)<9) "\
                              "& (MIPCHI2DV(PRIMARY)<25) "\
                              "& (BPVVDCHI2>225) & (BPVVDSIGN > 0 )"
    
    _stdNoPIDsMuons = DataOnDemand(Location = "Phys/StdNoPIDsMuons")

    return Selection (name,
                      Algorithm = Bs2MuMuNoMuID,
                      RequiredSelections = [ _stdNoPIDsMuons])



def makeLoose(name, MuIPChi2, MuTrChi2, BIPChi2, BFDChi2 ) :
    """      
    loose Bs2mumu selection object to study selection, etc.
    starts from Phys/StdNoPIDsMuons
    Arguments:
    name        : name of the Selection.
    MuIPChi2    : muon MIPCHI2DV(PRIMARY)
    MuTrChi2    : muon TRCHI2
    BIPChi2     : Bs MIPCHI2DV(PRIMARY)
    BFDChi2     : Bs BPVVDCHI2> %(BFDChi2)
    """
    from Configurables import OfflineVertexFitter
    Bs2MuMuNoMuIDLoose = CombineParticles("StripBs2MuMuNoMuIDLoose")
    Bs2MuMuNoMuIDLoose.DecayDescriptor = "B_s0 -> mu+ mu-"
    Bs2MuMuNoMuIDLoose.InputLocations = ["Phys/StdNoPIDsMuons"]
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    Bs2MuMuNoMuIDLoose.addTool( OfflineVertexFitter() )
    Bs2MuMuNoMuIDLoose.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2MuMuNoMuIDLoose.OfflineVertexFitter.useResonanceVertex = False
    Bs2MuMuNoMuIDLoose.ReFitPVs = True
    Bs2MuMuNoMuIDLoose.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> %(MuIPChi2)s ) "% locals() }#\
    #TODO J.A. 2010-07-20
    #a chi2 cut of 100 makes a huge difference in the test data.
    #needs to be understood before put in.  
#                                         "& (TRCHI2< %(MuTrChi2)s)"% locals() }

    Bs2MuMuNoMuIDLoose.CombinationCut = "(ADAMASS('B_s0')<600*MeV)";
  
    Bs2MuMuNoMuIDLoose.MotherCut = "(VFASPF(VCHI2/VDOF)<25) "\
                                   "& (MIPCHI2DV(PRIMARY)< %(BIPChi2)s) "\
                                   "& (BPVVDCHI2> %(BFDChi2)s) & (BPVVDSIGN > 0 )"% locals()

    _stdNoPIDsMuons = DataOnDemand(Location = "Phys/StdNoPIDsMuons")

    return Selection (name,
                      Algorithm = Bs2MuMuNoMuIDLoose,
                      RequiredSelections = [ _stdNoPIDsMuons])
    
    
    


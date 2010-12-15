'''
Module for construction of Bs-->MuMu stripping selections and lines

Exported symbols (use python help!):
   - Bs2MuMuLinesConf
   - makeDefault
   - makeBs2mmWide
   - makeLoose
   - makeDetachedJPsi
   - makeDetachedJPsiLoose
   - makePromptJPsi
'''

__author__ = ['Diego Martinez Santos','Johannes Albrecht']
__date__ = '19/07/2010'
__version__ = '$Revision: 1.2 $'

__all__ = ('Bs2MuMuLinesConf',
           'makeDefault',
           'makeBs2mmWide',
           'makeLoose',
           'makeDetachedJPsi',
           'makeDetachedJPsiLoose',
           'makePromptJPsi'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig

class Bs2MuMuLinesConf(object) :
    """
    Builder of:
     - Bs-> mumu stripping lines: default and loose,
     - detached JPsi lines: default, loose and prescaled prompt
    

    Usage:
    >>> config = { .... }
    >>> bsConf = Bs2MuMuLinesConf('PrescaledBs2MuMuTest',config)
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
    Bs2MuMuLinesConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ('MuIPChi2_loose',
                              'MuTrChi2_loose',
                              'BIPChi2_loose',
                              'BFDChi2_loose',
                              'DefaultLinePrescale',
                              'DefaultLinePostscale',
                              'Bs2mmWideLinePrescale',
                              'Bs2mmWideLinePostscale',
                              'LooseLinePrescale',
                              'LooseLinePostscale',
                              'JPsiLinePrescale',
                              'JPsiLinePostscale',
                              'JPsiLooseLinePrescale',
                              'JPsiLooseLinePostscale',
                              'JPsiPromptLinePrescale',
                              'JPsiPromptLinePostscale'
                              )
    
    #### This is the dictionary of all tunable cuts ########
    config_default={
        'DefaultLinePrescale'    : 1,
        'DefaultLinePostscale'   : 1,
        'Bs2mmWideLinePrescale'  : 1,
        'Bs2mmWideLinePostscale'  : 1,
        'LooseLinePrescale'      : 0.001,
        'LooseLinePostscale'     : 1,
        'JPsiLinePrescale'       : 1,
        'JPsiLinePostscale'      : 1,
        'JPsiLooseLinePrescale'  : 0.1,
        'JPsiLooseLinePostscale' : 1,
        'JPsiPromptLinePrescale' : 0.001,
        'JPsiPromptLinePostscale': 1,
        
        'MuIPChi2_loose'        :  9,
        'MuTrChi2_loose'        : 10,
        'BIPChi2_loose'         : 64,
        'BFDChi2_loose'         : 100
        }                
    
    


    def __init__(self, 
                 name = 'Bs2MuMu',
                 config = None) :

        checkConfig(Bs2MuMuLinesConf.__configuration_keys__,
                    config)

        default_name='Bs2MuMuNoMuID'
        wide_name = 'Bs2MuMuWideMass'
        loose_name='Bs2MuMuNoMuIDLoose'
        jPsi_name='DetachedJPsi'
        jPsiLoose_name='DetachedJPsiLoose'
        jPsiPrompt_name='DetachedJPsi_noDetached'

        self.selDefault = makeDefault(default_name)

        self.selWide = makeBs2mmWide(wide_name)

        self.selLoose = makeLoose(loose_name,
                                  MuIPChi2=config['MuIPChi2_loose'],
                                  MuTrChi2=config['MuTrChi2_loose'],
                                  BIPChi2=config['BIPChi2_loose'],
                                  BFDChi2=config['BFDChi2_loose']
                                  )
        
        self.selJPsi = makeDetachedJPsi(jPsi_name)

        self.selJPsiLoose = makeDetachedJPsiLoose(jPsiLoose_name)

        self.selJPsiPrompt = makePromptJPsi( jPsiPrompt_name )
        
        self.defaultLine = StrippingLine(default_name+"Line",
                                            prescale = config['DefaultLinePrescale'],
                                            postscale = config['DefaultLinePostscale'],
                                            algos = [ self.selDefault ]
                                            )
        
        self.wideLine = StrippingLine(wide_name+"Line",
                                      prescale = config['Bs2mmWideLinePrescale'],
                                      postscale = config['Bs2mmWideLinePostscale'],
                                      algos = [ self.selWide ]
                                      )
        
        self.looseLine = StrippingLine(loose_name+"Line",
                                            prescale = config['LooseLinePrescale'],
                                            postscale = config['LooseLinePostscale'],
                                            algos = [ self.selLoose ]
                                            )

        self.jPsiLine = StrippingLine( jPsi_name+"Line",
                                       prescale = config['JPsiLinePrescale'],
                                       postscale = config['JPsiLinePostscale'],
                                       algos = [ self.selJPsi ]
                                       )

        self.jPsiLooseLine = StrippingLine( jPsiLoose_name+"Line",
                                       prescale = config['JPsiLooseLinePrescale'],
                                       postscale = config['JPsiLooseLinePostscale'],
                                       algos = [ self.selJPsiLoose ]
                                       )

        self.jPsiPromptLine = StrippingLine( jPsiPrompt_name+"Line",
                                             prescale = config['JPsiPromptLinePrescale'],
                                             postscale = config['JPsiPromptLinePostscale'],
                                             algos = [ self.selJPsiPrompt ]
                                             )



        self.lines = [ self.defaultLine, self.wideLine, self.looseLine,
                       self.jPsiLine, self.jPsiLooseLine,  self.jPsiPromptLine ]


def makeDefault(name) :
    """
    default Bs2mumu selection object (tighter selection a la roadmap)
    starts from Phys/StdNoPIDsMuons

    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    Bs2MuMuNoMuID = CombineParticles("StripBs2MuMuNoMuID")
    Bs2MuMuNoMuID.DecayDescriptor = "B_s0 -> mu+ mu-"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    Bs2MuMuNoMuID.addTool( OfflineVertexFitter() )
    Bs2MuMuNoMuID.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2MuMuNoMuID.OfflineVertexFitter.useResonanceVertex = False
    Bs2MuMuNoMuID.ReFitPVs = True
    Bs2MuMuNoMuID.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> 25.)&(TRCHI2DOF < 5 )" }
    Bs2MuMuNoMuID.CombinationCut = "(ADAMASS('B_s0')<600*MeV)"\
                                   "& (AMAXDOCA('')<0.3*mm)"

    Bs2MuMuNoMuID.MotherCut = "(VFASPF(VCHI2/VDOF)<15) "\
                              "& (ADMASS('B_s0') < 600*MeV )"\
                              "& (BPVDIRA > 0) "\
                              "& (BPVVDCHI2> 225)"\
                              "& (BPVIPCHI2()< 25) "
                             
    _stdNoPIDsMuons = DataOnDemand(Location = "Phys/StdNoPIDsMuons")

    return Selection (name,
                      Algorithm = Bs2MuMuNoMuID,
                      RequiredSelections = [ _stdNoPIDsMuons])


def makeBs2mmWide(name) :
    """
    Bs2mumu selection object (tighter selection a la roadmap)
    with muon Id and wide mass window (1.2GeV)
    starts from Phys/StdLooseMuons

    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    Bs2MuMuWideMass = CombineParticles("StripBs2MuMuWideMass")
    Bs2MuMuWideMass.DecayDescriptor = "B_s0 -> mu+ mu-"
    Bs2MuMuWideMass.addTool( OfflineVertexFitter() )
    Bs2MuMuWideMass.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2MuMuWideMass.OfflineVertexFitter.useResonanceVertex = False
    Bs2MuMuWideMass.ReFitPVs = True
    Bs2MuMuWideMass.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> 25.)&(TRCHI2DOF < 5 )" }
    Bs2MuMuWideMass.CombinationCut = "(ADAMASS('B_s0')<1200*MeV)"\
                                     "& (AMAXDOCA('')<0.3*mm)"

    Bs2MuMuWideMass.MotherCut = "(VFASPF(VCHI2/VDOF)<15) "\
                                "& (ADMASS('B_s0') < 1200*MeV )"\
                                "& (BPVDIRA > 0) "\
                                "& (BPVVDCHI2> 225)"\
                                "& (BPVIPCHI2()< 25) "
    
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons")

    return Selection (name,
                      Algorithm = Bs2MuMuWideMass,
                      RequiredSelections = [ _stdLooseMuons])



def makeLoose(name, MuIPChi2, MuTrChi2, BIPChi2, BFDChi2 ) :
    """      
    loose Bs2mumu selection object to monitor selection,
    systematics from cuts, etc.
    starts from Phys/StdNoPIDsMuons

    prescaled

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
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    Bs2MuMuNoMuIDLoose.addTool( OfflineVertexFitter() )
    Bs2MuMuNoMuIDLoose.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2MuMuNoMuIDLoose.OfflineVertexFitter.useResonanceVertex = False
    Bs2MuMuNoMuIDLoose.ReFitPVs = True
    Bs2MuMuNoMuIDLoose.DaughtersCuts = { "mu+" : "(TRCHI2DOF < %(MuTrChi2)s ) "\
                                         "& (MIPCHI2DV(PRIMARY)> %(MuIPChi2)s ) "% locals() }
  
    Bs2MuMuNoMuIDLoose.CombinationCut = "(ADAMASS('B_s0')<600*MeV)"\
                                        "& (AMAXDOCA('')<0.5*mm)"

    Bs2MuMuNoMuIDLoose.MotherCut = "(VFASPF(VCHI2/VDOF)<25) "\
                                   "& (ADMASS('B_s0') < 600*MeV )"\
                                   "& (BPVDIRA > 0) "\
                                   "& (BPVVDCHI2> %(BFDChi2)s)"\
                                   "& (BPVIPCHI2()< %(BIPChi2)s) "% locals()

    _stdNoPIDsMuons = DataOnDemand(Location = "Phys/StdNoPIDsMuons")

    return Selection (name,
                      Algorithm = Bs2MuMuNoMuIDLoose,
                      RequiredSelections = [ _stdNoPIDsMuons])
    
    

def makeDetachedJPsi(name) :
    """
    detached JPsi selection for B--> JPsi X calibration and
    normalization channels. Selection is aligned to the Bs2MuMu
    selection.

    Please contact Johannes Albrecht if you think of prescaling this line!

    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    DetachedJPsi = CombineParticles("StripDetachedJPsi")
    DetachedJPsi.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    DetachedJPsi.addTool( OfflineVertexFitter() )
    DetachedJPsi.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    DetachedJPsi.OfflineVertexFitter.useResonanceVertex = False
    DetachedJPsi.ReFitPVs = True
    DetachedJPsi.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 5 ) "\
                                    "& (MIPCHI2DV(PRIMARY)> 25.)"}
                                 
    DetachedJPsi.CombinationCut = "(ADAMASS('J/psi(1S)')<100*MeV) "\
                                   "& (AMAXDOCA('')<0.3*mm)"

    DetachedJPsi.MotherCut = "(VFASPF(VCHI2)<15) "\
                             "& (ADMASS('J/psi(1S)') < 100*MeV )"\
                             "& (BPVDIRA > 0) "\
                             "& (BPVVDCHI2>169)"

    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons")

    return Selection (name,
                      Algorithm = DetachedJPsi,
                      RequiredSelections = [ _stdLooseMuons ])


def makeDetachedJPsiLoose(name) :
    """
    loose detached JPsi selection to monitor selection,
    systematics from cuts, etc.
    
    prescaled
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    DetachedJPsiLoose = CombineParticles("StripDetachedJPsiLoose")
    DetachedJPsiLoose.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    DetachedJPsiLoose.addTool( OfflineVertexFitter() )
    DetachedJPsiLoose.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    DetachedJPsiLoose.OfflineVertexFitter.useResonanceVertex = False
    DetachedJPsiLoose.ReFitPVs = True
    DetachedJPsiLoose.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 10 ) "\
                                    "& (MIPCHI2DV(PRIMARY)> 9.)"}
                                 
    DetachedJPsiLoose.CombinationCut = "(ADAMASS('J/psi(1S)')<120*MeV) "\
                                   "& (AMAXDOCA('')<0.5*mm)"

    DetachedJPsiLoose.MotherCut = "(VFASPF(VCHI2)<25) "\
                                  "& (ADMASS('J/psi(1S)') < 120*MeV )"\
                                  "& (BPVDIRA > 0) "\
                                  "& (BPVVDCHI2>100)"

    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons")

    return Selection (name,
                      Algorithm = DetachedJPsiLoose,
                      RequiredSelections = [ _stdLooseMuons ])


def makePromptJPsi(name) :
    """
    Prompt variant of the detached JPsi selection for
    B--> JPsi X calibration and normalization channels.
    Selection is aligned to the Bs2MuMu selection.
    heavily prescaled
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    PromptJPsi = CombineParticles("StripPromptJPsi")
    PromptJPsi.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    PromptJPsi.addTool( OfflineVertexFitter() )
    PromptJPsi.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    PromptJPsi.OfflineVertexFitter.useResonanceVertex = False
    PromptJPsi.ReFitPVs = True
    PromptJPsi.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 5 ) "}
    
    PromptJPsi.CombinationCut = "(ADAMASS('J/psi(1S)')<100*MeV) "\
                                "& (AMAXDOCA('')<0.3*mm)"
    
    PromptJPsi.MotherCut = "(VFASPF(VCHI2)<15) "\
                           "& (ADMASS('J/psi(1S)') < 100*MeV )"
    
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons")

    return Selection (name,
                      Algorithm = PromptJPsi,
                      RequiredSelections = [ _stdLooseMuons ])

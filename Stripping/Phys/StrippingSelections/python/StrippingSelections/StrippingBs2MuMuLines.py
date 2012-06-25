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

__author__ = ['Johannes Albrecht']
__date__ = '19/07/2010'
__version__ = '$Revision: 1.2 $'

__all__ = ('Bs2MuMuLinesConf',
           'config_default',
           'makeDefault',
           'makeBs2mmWide',
           'makeLoose',
           'makeBu',
           'makeBs',
           'makeBd'
#           'makeDetachedJPsi',
#           'makeDetachedJPsiLoose',
#           'makePromptJPsi'
           )

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
#from StrippingSelections.Utils import checkConfig

class Bs2MuMuLinesConf(LineBuilder) :
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
                              'BuPrescale',
                              'BuPostscale',
                              'BsPrescale',
                              'BsPostscale',
                              'BdPrescale',
                              'BdPostscale',
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
        'LooseLinePrescale'      : 0.02,
        'LooseLinePostscale'     : 1,
        'BuPrescale'    : 1,
        'BuPostscale'   : 1,
        'BsPrescale'    : 1,
        'BsPostscale'   : 1,
        'BdPrescale'    : 1,
        'BdPostscale'   : 1,
        'JPsiLinePrescale'       : 1,
        'JPsiLinePostscale'      : 1,
        'JPsiLooseLinePrescale'  : 0.1,
        'JPsiLooseLinePostscale' : 1,
        'JPsiPromptLinePrescale' : 0.005,
        'JPsiPromptLinePostscale': 1,
        
        'MuIPChi2_loose'        :  9,
        'MuTrChi2_loose'        : 10,
        'BIPChi2_loose'         : 64,
        'BFDChi2_loose'         : 100
        }                
    
    


    def __init__(self, 
                 name = 'Bs2MuMu',
                 config = None) :

        LineBuilder.__init__(self, name, config)
        #checkConfig(Bs2MuMuLinesConf.__configuration_keys__,config)

        default_name=name+'NoMuID'
        wide_name = name+'WideMass'
        loose_name=name+'NoMuIDLoose'
        bu_name=name+'Bu2JPsiK'
        bs_name=name+'Bs2JPsiPhi'
        bd_name=name+'Bd2JPsiKst'

        self.selDefault = makeDefault(default_name)

        self.selWide = makeBs2mmWide(wide_name)

        self.selLoose = makeLoose(loose_name,
                                  MuIPChi2=config['MuIPChi2_loose'],
                                  MuTrChi2=config['MuTrChi2_loose'],
                                  BIPChi2=config['BIPChi2_loose'],
                                  BFDChi2=config['BFDChi2_loose']
                                  )
        
        self.selBu = makeBu(bu_name)

        self.selBs = makeBs(bs_name)

        self.selBd = makeBd(bd_name)

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

        self.buLine = StrippingLine(bu_name+"Line",
                                    prescale = config['BuPrescale'],
                                    postscale = config['BuPostscale'],
                                    algos = [ self.selBu ]
                                    )

        self.bsLine = StrippingLine(bs_name+"Line",
                                    prescale = config['BsPrescale'],
                                    postscale = config['BsPostscale'],
                                    algos = [ self.selBs ]
                                    )

        self.bdLine = StrippingLine(bd_name+"Line",
                                    prescale = config['BdPrescale'],
                                    postscale = config['BdPostscale'],
                                    algos = [ self.selBd ]
                                    )


        self.registerLine(self.defaultLine)
        self.registerLine(self.wideLine)
        self.registerLine(self.looseLine)
        self.registerLine(self.buLine)
        self.registerLine(self.bsLine)
        self.registerLine(self.bdLine)


def makeDefault(name) :
    """
    default Bs2mumu selection object (tighter selection a la roadmap)
    starts from Phys/StdNoPIDsMuons

    Please contact Johannes Albrecht if you think of prescaling this line!
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    Bs2MuMuNoMuID = CombineParticles("Comine"+name)
    Bs2MuMuNoMuID.DecayDescriptor = "B_s0 -> mu+ mu-"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    Bs2MuMuNoMuID.addTool( OfflineVertexFitter() )
    Bs2MuMuNoMuID.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2MuMuNoMuID.OfflineVertexFitter.useResonanceVertex = False
    Bs2MuMuNoMuID.ReFitPVs = True
    Bs2MuMuNoMuID.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> 25.)&(TRCHI2DOF < 4 )"\
                                    " & (0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1))"\
                                    " & (PT < 40*GeV)"\
                                    " & (P < 500*GeV)"}
    
    Bs2MuMuNoMuID.CombinationCut = "(ADAMASS('B_s0')<500*MeV)"\
                                   "& (AMAXDOCA('')<0.3*mm)"

    Bs2MuMuNoMuID.MotherCut = "(VFASPF(VCHI2/VDOF)<9) "\
                              "& (ADMASS('B_s0') < 500*MeV )"\
                              "& (BPVDIRA > 0) "\
                              "& (BPVVDCHI2> 225)"\
                              "& (BPVIPCHI2()< 25) "\
                              "& (BPVLTIME()<13.248*ps)"\
                              "& (PT > 350*MeV)"
                             
    _stdNoPIDsMuons = DataOnDemand(Location = "Phys/StdNoPIDsMuons/Particles")

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
    Bs2MuMuWideMass = CombineParticles("Combine"+name)
    Bs2MuMuWideMass.DecayDescriptor = "B_s0 -> mu+ mu-"
    Bs2MuMuWideMass.addTool( OfflineVertexFitter() )
    Bs2MuMuWideMass.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    Bs2MuMuWideMass.OfflineVertexFitter.useResonanceVertex = False
    Bs2MuMuWideMass.ReFitPVs = True
    Bs2MuMuWideMass.DaughtersCuts = { "mu+" : "(MIPCHI2DV(PRIMARY)> 25.)&(TRCHI2DOF < 4 )" }
    Bs2MuMuWideMass.CombinationCut = "(ADAMASS('B_s0')<1200*MeV)"\
                                     "& (AMAXDOCA('')<0.3*mm)"

    Bs2MuMuWideMass.MotherCut = "(VFASPF(VCHI2/VDOF)<9) "\
                                "& (ADMASS('B_s0') < 1200*MeV )"\
                                "& (BPVDIRA > 0) "\
                                "& (BPVVDCHI2> 225)"\
                                "& (BPVIPCHI2()< 25) "
    
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

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
    Bs2MuMuNoMuIDLoose = CombineParticles("Combine"+name)
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

    _stdNoPIDsMuons = DataOnDemand(Location = "Phys/StdNoPIDsMuons/Particles")

    return Selection (name,
                      Algorithm = Bs2MuMuNoMuIDLoose,
                      RequiredSelections = [ _stdNoPIDsMuons])
    
    


def makeBu(name) :
    """
    detached Bu-->JPsiK selection. Selection is aligned to the Bs2MuMu
    selection.

    Please contact Johannes Albrecht if you think of prescaling this line!

    Arguments:
    name        : name of the Selection.
    """

    
    from Configurables import OfflineVertexFitter
   
    SelDJPsi = makeDetachedJPsi(name)

    PreselBu2JPsiKCommon = CombineParticles("PreselBu2JPsiKCommon")
    PreselBu2JPsiKCommon.DecayDescriptor =  " [B+ -> J/psi(1S) K+]cc ";
    PreselBu2JPsiKCommon.addTool( OfflineVertexFitter() )
    PreselBu2JPsiKCommon.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    PreselBu2JPsiKCommon.OfflineVertexFitter.useResonanceVertex = False
    PreselBu2JPsiKCommon.ReFitPVs = True
    PreselBu2JPsiKCommon.DaughtersCuts = { "K+" : "(ISLONG) & (TRCHI2DOF < 5 ) &(MIPCHI2DV(PRIMARY)>25)& (PT>250*MeV) "}
    PreselBu2JPsiKCommon.CombinationCut = "(ADAMASS('B+') < 500*MeV)"
    PreselBu2JPsiKCommon.MotherCut = "(BPVIPCHI2()< 25)& (VFASPF(VCHI2)<45) "

    _kaons = DataOnDemand(Location='Phys/StdNoPIDsKaons/Particles')

    return Selection( "SelBu2JPsiK",
                         Algorithm = PreselBu2JPsiKCommon,
                         RequiredSelections=[SelDJPsi,_kaons] )


def makeBs(name) :
    """
    detached Bs-->JPsiPhi selection. Selection is aligned to the Bs2MuMu
    selection.

    Please contact Johannes Albrecht if you think of prescaling this line!

    Arguments:
    name        : name of the Selection.
    """

    
    from Configurables import OfflineVertexFitter
   
    SelDJPsi = makeDetachedJPsi(name)

    makePhi = CombineParticles("makePhi")
    makePhi.DecayDescriptor =  "phi(1020) -> K+ K-"
    makePhi.DaughtersCuts = {"K+": "(ISLONG) & (TRCHI2DOF < 5 ) & (MIPCHI2DV(PRIMARY)> 4.) & (PT>250*MeV)"}
    
    _kaons = DataOnDemand(Location='Phys/StdNoPIDsKaons/Particles')

    makePhi.CombinationCut =  "(ADAMASS('phi(1020)')<20*MeV)"
    makePhi.MotherCut = " (MIPCHI2DV(PRIMARY)> 25.)"
    SelPhi = Selection( "SelPhi",                       Algorithm= makePhi,
                        RequiredSelections=[_kaons] )

    PreselBs2JPsiPhiCommon = CombineParticles("PreselBs2JPsiPhiCommon")
    PreselBs2JPsiPhiCommon.DecayDescriptor = "B_s0 -> J/psi(1S) phi(1020)"
    PreselBs2JPsiPhiCommon.addTool( OfflineVertexFitter() )
    PreselBs2JPsiPhiCommon.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    PreselBs2JPsiPhiCommon.OfflineVertexFitter.useResonanceVertex = False
    PreselBs2JPsiPhiCommon.ReFitPVs = True
    PreselBs2JPsiPhiCommon.DaughtersCuts = {}
    PreselBs2JPsiPhiCommon.CombinationCut = "(ADAMASS('B_s0') < 500*MeV)"
    PreselBs2JPsiPhiCommon.MotherCut = "(BPVIPCHI2()< 25)& (VFASPF(VCHI2)<75)"
    
    return  Selection( "SelBs2JPsiPhi",
                       Algorithm = PreselBs2JPsiPhiCommon,
                       RequiredSelections=[SelDJPsi,SelPhi] )



def makeBd(name) :
    """
    detached Bd-->JPsiK* selection. Selection is aligned to the Bs2MuMu
    selection.

    Please contact Johannes Albrecht if you think of prescaling this line!

    Arguments:
    name        : name of the Selection.
    """

    
    from Configurables import OfflineVertexFitter
   
    SelDJPsi = makeDetachedJPsi(name)


    ## make Kstar

    makeKstar = CombineParticles("makeKstar")

    makeKstar.DecayDescriptor =  "[K*(892)0 -> K+ pi-]cc"
    makeKstar.DaughtersCuts = {"K+": "(ISLONG) & (TRCHI2DOF < 5 ) & (MIPCHI2DV(PRIMARY)> 4.)& (PT>250*MeV)",
                               "pi-":"(ISLONG) & (TRCHI2DOF < 5 ) & (MIPCHI2DV(PRIMARY)> 4.)& (PT>250*MeV)"}
    makeKstar.CombinationCut =  "(ADAMASS('K*(892)0')<2000*MeV)"#huge, to allow to study JPsi K1 etc
    makeKstar.MotherCut = " (MIPCHI2DV(PRIMARY)> 25.)"

    _pions = DataOnDemand(Location='Phys/StdNoPIDsPions/Particles')
    _kaons = DataOnDemand(Location='Phys/StdNoPIDsKaons/Particles')

    SelKst = Selection( "SelKst",
                        Algorithm= makeKstar,
                        RequiredSelections=[_pions,_kaons] )
    

    ## make BtoJPsiKstar
    PreselBd2JPsiKstCommon = CombineParticles("PreselBd2JPsiKstCommon")
    PreselBd2JPsiKstCommon.DecayDescriptor = "[B0 -> J/psi(1S) K*(892)0]cc"
    PreselBd2JPsiKstCommon.addTool( OfflineVertexFitter() )
    PreselBd2JPsiKstCommon.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    PreselBd2JPsiKstCommon.OfflineVertexFitter.useResonanceVertex = False
    PreselBd2JPsiKstCommon.ReFitPVs = True
    PreselBd2JPsiKstCommon.DaughtersCuts = {}
    PreselBd2JPsiKstCommon.CombinationCut = "(ADAMASS('B0') < 500*MeV)"
    PreselBd2JPsiKstCommon.MotherCut = "(BPVIPCHI2()< 25) & (VFASPF(VCHI2)<75)"

    return Selection( "SelBd2JPsiKstar",
                      Algorithm = PreselBd2JPsiKstCommon,
                      RequiredSelections=[SelDJPsi,SelKst] )




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
    DetachedJPsi = CombineParticles("Combine"+name)
    DetachedJPsi.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    DetachedJPsi.addTool( OfflineVertexFitter() )
    DetachedJPsi.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    DetachedJPsi.OfflineVertexFitter.useResonanceVertex = False
    DetachedJPsi.ReFitPVs = True
    DetachedJPsi.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 4 ) "\
                                    "& (MIPCHI2DV(PRIMARY)> 25.)"}
                                 
    DetachedJPsi.CombinationCut = "(ADAMASS('J/psi(1S)')<100*MeV) "\
                                   "& (AMAXDOCA('')<0.3*mm)"

    DetachedJPsi.MotherCut = "(VFASPF(VCHI2)<9) "\
                             "& (ADMASS('J/psi(1S)') < 100*MeV )"\
                             "& (BPVDIRA > 0) "\
                             "& (BPVVDCHI2>169)"

    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = DetachedJPsi,
                      RequiredSelections = [ _stdLooseMuons ])

'''
def makeDetachedJPsiLoose(name) :
    """
    loose detached JPsi selection to monitor selection,
    systematics from cuts, etc.
    
    prescaled
    
    Arguments:
    name        : name of the Selection.
    """
    from Configurables import OfflineVertexFitter
    DetachedJPsiLoose = CombineParticles("Combine"+name)
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

    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

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
    PromptJPsi = CombineParticles("Combine"+name)
    PromptJPsi.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
    # Set the OfflineVertexFitter to keep the 4 tracks and not the J/Psi Kstar:
    PromptJPsi.addTool( OfflineVertexFitter() )
    PromptJPsi.VertexFitters.update( { "" : "OfflineVertexFitter"} )
    PromptJPsi.OfflineVertexFitter.useResonanceVertex = False
    PromptJPsi.ReFitPVs = True
    PromptJPsi.DaughtersCuts = { "mu+" : "(TRCHI2DOF < 4 ) "}
    
    PromptJPsi.CombinationCut = "(ADAMASS('J/psi(1S)')<100*MeV) "\
                                "& (AMAXDOCA('')<0.3*mm)"
    
    PromptJPsi.MotherCut = "(VFASPF(VCHI2)<15) "\
                           "& (ADMASS('J/psi(1S)') < 100*MeV )"
    
    _stdLooseMuons = DataOnDemand(Location = "Phys/StdLooseMuons/Particles")

    return Selection (name,
                      Algorithm = PromptJPsi,
                      RequiredSelections = [ _stdLooseMuons ])
'''

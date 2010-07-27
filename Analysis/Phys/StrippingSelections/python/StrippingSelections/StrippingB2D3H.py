# $Id: StrippingB2D3H.py,v 1.0 2010/07/23 sblusk 
"""
Module for construction of B->D+3H Stripping Selections and StrippingLines.
Provides functions to build 3H Final states in PiPiPi and KPiPi.
D candidates are filtered from StdLooseD02HH and StdLooseDplus2XXX.
Provides class StrippingB2D3HConf, which constructs the Selections and 
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - StrippngB2D3HConf
   - makeD0Meson
   - makeDmeson
   - makeDStarmeson
   - makeA12PiPiPi
   - makeK12KPiPi
   - makePions
   - makeKaons
   - StrippingB2D3HLoose
   - StrippingB2D3HNominal
"""


__author__ = ['Steven Blusk']
__date__ = '26/07/2010'
__version__ = '$Revision: 1.1 $'


__all__ = ('StrippingB2D3HConf',
           'makeB2D3H',
           'makeD0Meson',
           'makeDMeson',
           'makeDStarMeson',
           'makeA12PiPiPi',
           'makeK12KPiPi',
           'makePions',
           'makeKaons',
           'StrippingB2D3HLoose',
           'StrippingB2D3HNominal'
           )

"""
B->D+3H channels 
"""
from StrippingConf.StrippingStream import StrippingStream
from Configurables import FilterDesktop, CombineParticles
from StrippingConf.StrippingLine import StrippingLine
from StrippingSelections.Utils import checkConfig
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from copy import copy


class StrippingB2D3HConf( object ):

    """
    Builder of B-->D+3H stripping Selection and StrippingLine.
    Constructs B-->D+3H Selections and StrippingLines from a configuration dictionary.
    Usage:
    >>> config = { .... }
    >>> b2d3h = StrippingB2D3HConf('NameOfSelection',config)
    >>> bLines = b2d3h.lines
    >>> for line in bLines :
    >>>  print line.name(), line.outputLocation()
    The lines can be used directly to build a StrippingStream object.

    Exports as instance data members:
    selPions               : selected Pions for the 3H bachelor
    selKaons               : selected Kaons for the 3H bachelor
    selPiPiPi              : selected PiPiPi bachelor
    selKPiPi               : selected KPiPi bachelor
    selDch                 : selected D+ -->K-Pi+Pi+, K+K-Pi+, Pi+Pi-Pi+ and Ds+ -->K+K-Pi+, K+Pi-Pi+, Pi+Pi-Pi+ 
    selD0                  : selected D0 --> KK, KPi, KPi(DCS) and PiPi
    B2D0PiPiPi             : Selected B+ -->D0+PiPiPi
    B2D0KPiPi              : Selected B+ -->D0+KPiPi
    B2DPiPiPi              : Selected B0/Bs -->D+PiPiPi, Ds+PiPiPi
    B2DKPiPi               : Selected B0/Bs -->Ds+KPiPi, Ds+KPiPi
    lines                  : List of lines [B2D0PiPiPi, B2D0KPiPi, B2DPiPiPi, B2DKPiPi]

    Exports as class data member:
    StrippingB2D3HConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ("PionMinP",
                              "PionMaxP", 
                              "PionMinPT",
                              "KaonMinP" ,
                              "KaonMaxP" ,
                              "KaonMinPT",
                              "PionMinIPChisq",
                              "KaonMinIPChisq",
                              "TrkChisq",
                              "Bach3HMassWindow",    
                              "Bach3HDocaMax",
                              "Bach3HVtxChisq",      
                              "Bach3HMinPT",   
                              "Bach3HIP2PV",   
                              "Bach3HIPChisq2PV",  
                              "Bach3HVtxSepChisq",   
                              "Bach3HDiraPV",        
                              "Bach3HZVtxSep",       
                              "DMinPT",            
                              "DVtxChisq",        
                              "DMassWindow",         
                              "DIP2PV",              
                              "DIPChisq2PV",         
                              "DVtxSepChisq",         
                              "DDiraPV",              
                              "DZVtxSep",             
                              "BMinPT",               
                              "BVtxChisq",            
                              "BMassWindow",   
                              "BIP2PV",        
                              "BIPChisq2PV",    
                              "BVtxSepChisq",    
                              "BDiraPV",         
                              "BZVtxSep",
                              "BDZVtxSep"        
                              )


    def __init__(self, 
                 name = 'Loose',
                 config = None) :

        checkConfig(StrippingB2D3HConf.__configuration_keys__, config)

        # First filter pions
        self.selPions = makePions( 'PionsForB2D3H'+name,
                                    PionMinP = config['PionMinP'],
                                    PionMaxP = config['PionMaxP'],
                                    PionMinPT = config['PionMinPT'],
                                    PionMinIPChisq = config['PionMinIPChisq'],
                                    TrkChisq = config['TrkChisq'])
        # First filter kaons
        self.selKaons = makeKaons( 'KaonsForB2D3H'+name,
                                    KaonMinP = config['KaonMinP'],
                                    KaonMaxP = config['KaonMaxP'],
                                    KaonMinPT = config['KaonMinPT'],
                                    KaonMinIPChisq = config['KaonMinIPChisq'],
                                    TrkChisq = config['TrkChisq'])

        # Now make the PiPiPi bachelor
        self.selPiPiPi = makePiPiPi( 'PiPiPiForB2D3H'+name,
                                     pionSel = self.selPions,
                                     Bach3HMassWindow = config['Bach3HMassWindow'],
                                     Bach3HDocaMax = config['Bach3HDocaMax'],
                                     Bach3HMinPT = config['Bach3HMinPT'],
                                     Bach3HIP2PV = config['Bach3HIP2PV'],
                                     Bach3HIPChisq2PV = config['Bach3HIPChisq2PV'],
                                     Bach3HVtxChisq = config['Bach3HVtxChisq'],
                                     Bach3HVtxSepChisq = config['Bach3HVtxSepChisq'],
                                     Bach3HDiraPV = config['Bach3HDiraPV'],
                                     Bach3HZVtxSep = config['Bach3HZVtxSep']
                                     )
        # Make the KPiPi bachelor
        self.selKPiPi = makeKPiPi( 'KPiPiForB2D3H'+name,
                                   pionSel = self.selPions,
                                   kaonSel = self.selKaons,
                                   Bach3HMassWindow = config['Bach3HMassWindow'],                                 
                                   Bach3HDocaMax = config['Bach3HDocaMax'],
                                   Bach3HMinPT = config['Bach3HMinPT'],
                                   Bach3HIP2PV = config['Bach3HIP2PV'],
                                   Bach3HIPChisq2PV = config['Bach3HIPChisq2PV'],
                                   Bach3HVtxChisq = config['Bach3HVtxChisq'],
                                   Bach3HVtxSepChisq = config['Bach3HVtxSepChisq'],
                                   Bach3HDiraPV = config['Bach3HDiraPV'],
                                   Bach3HZVtxSep = config['Bach3HZVtxSep']
                                   )

        # Filter D+
        self.selDch = makeDMeson( 'DchForB2D3H'+name,
                                   DMassWindow = config['DMassWindow'],
                                   DMinPT = config['DMinPT'],
                                   DIP2PV = config['DIP2PV'],
                                   DIPChisq2PV = config['DIPChisq2PV'],
                                   DVtxChisq = config['DVtxChisq'],
                                   DVtxSepChisq = config['DVtxSepChisq'],
                                   DDiraPV = config['DDiraPV'],
                                   DZVtxSep = config['DZVtxSep']
                                   )
        # Filter D0
        self.selD0 = makeD0Meson( 'D0ForB2D3H'+name,
                                 DMassWindow = config['DMassWindow'],
                                 DMinPT = config['DMinPT'],
                                 DIP2PV = config['DIP2PV'],
                                 DIPChisq2PV = config['DIPChisq2PV'],
                                 DVtxChisq = config['DVtxChisq'],
                                 DVtxSepChisq = config['DVtxSepChisq'],
                                 DDiraPV = config['DDiraPV'],
                                 DZVtxSep = config['DZVtxSep']
                                 )

        # Filter D0
        self.selDStar = makeDStarMeson( 'DStarForB2D3H'+name,
                                        DMassWindow = config['DMassWindow'],
                                        DMinPT = config['DMinPT'],
                                        DIP2PV = config['DIP2PV'],
                                        DIPChisq2PV = config['DIPChisq2PV'],
                                        DVtxChisq = config['DVtxChisq'],
                                        DVtxSepChisq = config['DVtxSepChisq'],
                                        DDiraPV = config['DDiraPV'],
                                        DZVtxSep = config['DZVtxSep']
                                        )

        # Make B- --> D~0 (pipipi)+
        name1 = 'B2D0PiPiPi' + name
        self.B2D0PiPiPi = makeB2D3H(name1,
                                    dSel = self.selD0,
                                    hhhSel = self.selPiPiPi,
                                    BMassWindow = config['BMassWindow'],
                                    BMinPT = config['BMinPT'],
                                    BIP2PV = config['BIP2PV'],
                                    BIPChisq2PV = config['BIPChisq2PV'],
                                    BVtxChisq = config['BVtxChisq'],
                                    BVtxSepChisq = config['BVtxSepChisq'],
                                    BDiraPV = config['BDiraPV'],
                                    BZVtxSep = config['BZVtxSep'],
                                    BDZVtxSep = config['BDZVtxSep'],
                                    decayDesc = "[B+ -> D~0 a_1(1260)+]cc",
                                    parentB = "B+"
                                  )
        
        # Make B- --> D~0 (KPiPi)+
        name2 = 'B2D0KPiPi' + name
        self.B2D0KPiPi = makeB2D3H(name2,
                                   dSel = self.selD0,
                                   hhhSel = self.selKPiPi,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   decayDesc = "[B+ -> D~0 K_1(1270)+]cc",
                                   parentB = "B+"
                                  )

        # Make B- --> D- (pipipi)+
        name3 = 'B2DPiPiPi' + name
        self.B2DPiPiPi = makeB2D3H(name3,
                                   dSel = self.selDch,
                                   hhhSel = self.selPiPiPi,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   decayDesc = "[B0 -> D- a_1(1260)+]cc",
                                   parentB = "B0")

        # Make B0 --> D- (Kpipi)+
        name4 = 'B2DKPiPi' + name
        self.B2DKPiPi = makeB2D3H(name4,
                                  dSel = self.selDch,
                                  hhhSel = self.selKPiPi,
                                  BMassWindow = config['BMassWindow'],
                                  BMinPT = config['BMinPT'],
                                  BIP2PV = config['BIP2PV'],
                                  BIPChisq2PV = config['BIPChisq2PV'],
                                  BVtxChisq = config['BVtxChisq'],
                                  BVtxSepChisq = config['BVtxSepChisq'],
                                  BDiraPV = config['BDiraPV'],
                                  BZVtxSep = config['BZVtxSep'],
                                  BDZVtxSep = config['BDZVtxSep'],
                                  decayDesc = "[B0 -> D- K_1(1270)+]cc",
                                  parentB = "B0")


        # Make B- --> D*- (pipipi)+
        name5 = 'B2DStarPiPiPi' + name
        self.B2DStarPiPiPi = makeB2D3H(name5,
                                   dSel = self.selDStar,
                                   hhhSel = self.selPiPiPi,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   decayDesc = "[B0 -> D*(2010)- a_1(1260)+]cc",
                                   parentB = "B0")

        # Make B0 --> D*- (Kpipi)+
        name6 = 'B2DStarKPiPi' + name
        self.B2DStarKPiPi = makeB2D3H(name6,
                                  dSel = self.selDStar,
                                  hhhSel = self.selKPiPi,
                                  BMassWindow = config['BMassWindow'],
                                  BMinPT = config['BMinPT'],
                                  BIP2PV = config['BIP2PV'],
                                  BIPChisq2PV = config['BIPChisq2PV'],
                                  BVtxChisq = config['BVtxChisq'],
                                  BVtxSepChisq = config['BVtxSepChisq'],
                                  BDiraPV = config['BDiraPV'],
                                  BZVtxSep = config['BZVtxSep'],
                                  BDZVtxSep = config['BDZVtxSep'],
                                  decayDesc = "[B0 -> D*(2010)- K_1(1270)+]cc",
                                  parentB = "B0")



        self.StrippingB2D0PiPiPiLine = StrippingLine('B2D0PiPiPiLine'+name,
                                                     algos = [ self.B2D0PiPiPi]
                                                     )

        self.StrippingB2D0KPiPiLine = StrippingLine('B2D0KPiPiLine'+name,
                                                     algos = [ self.B2D0KPiPi]
                                                     )

        self.StrippingB2DPiPiPiLine = StrippingLine('B2DPiPiPiLine'+name,
                                                      algos = [ self.B2DPiPiPi]
                                                      )

        self.StrippingB2DKPiPiLine = StrippingLine('B2DKPiPiLine'+name,
                                                   algos = [ self.B2DKPiPi]
                                                   )
        
        self.StrippingB2DStarPiPiPiLine = StrippingLine('B2DStarPiPiPiLine'+name,
                                                        algos = [ self.B2DStarPiPiPi]
                                                        )

        self.StrippingB2DStarKPiPiLine = StrippingLine('B2DStarKPiPiLine'+name,
                                                     algos = [ self.B2DStarKPiPi]
                                                     )
        
        self.lines = [self.StrippingB2D0PiPiPiLine,
                      self.StrippingB2D0KPiPiLine,
                      self.StrippingB2DPiPiPiLine,
                      self.StrippingB2DKPiPiLine,
                      self.StrippingB2DStarPiPiPiLine,
                      self.StrippingB2DStarKPiPiLine
                      ]
                      

def makePions(name, PionMinP, PionMaxP, PionMinPT, PionMinIPChisq, TrkChisq):

    """
    Create and return Pion Selection object by Filterinf StdLoosePions
    #
    Arguments:
    name                  : name of the Selection.
    PionMinP              : Pion minimum momentum
    PionMaxP              : Pion maximum momentum
    PionMinPT             : Pion minimum pT
    PionMinIPChisq        : Pion minimum IP Chisq
    TrkChisq              : Cut on Track Chisq/DOF
    """

    _stdLoosePions = DataOnDemand(Location = "Phys/StdLoosePions")
    _pionFilter = FilterDesktop("_filterFor"+name)
    _pionFilter.Code = "(P > %(PionMinP)s *MeV) & (P < %(PionMaxP)s *MeV) & (PT> %(PionMinPT)s *MeV)" \
                       "& (MIPCHI2DV(PRIMARY)> %(PionMinIPChisq)s ) & (TRCHI2DOF < %(TrkChisq)s ) " %locals()
    return Selection (name,
                      Algorithm = _pionFilter,
                      RequiredSelections = [_stdLoosePions])

def makeKaons(name, KaonMinP, KaonMaxP, KaonMinPT, KaonMinIPChisq, TrkChisq):

    """
    Create and return Kaon Selection object by Filtering StdLooseKaons
    #
    Arguments:
    name                  : name of the Selection.
    KaonMinP              : Kaon minimum momentum
    KaonMaxP              : Kaon maximum momentum
    KaonMinPT             : Kaon minimum pT
    KaonMinIPChisq        : Kaon minimum IP Chisq
    TrkChisq              : Cut on Track Chisq/DOF
    """

    _stdLooseKaons = DataOnDemand(Location = "Phys/StdLooseKaons")
    _kaonFilter = FilterDesktop("_filterFor"+name)
    _kaonFilter.Code = "(P > %(KaonMinP)s *MeV) & (P < %(KaonMaxP)s *MeV) & (PT> %(KaonMinPT)s *MeV)" \
                       "& (MIPCHI2DV(PRIMARY)> %(KaonMinIPChisq)s ) & (TRCHI2DOF < %(TrkChisq)s ) " %locals()
    return Selection (name,
                      Algorithm = _kaonFilter,
                      RequiredSelections = [_stdLooseKaons])


def makePiPiPi( name,
                pionSel,
                Bach3HMassWindow,
                Bach3HDocaMax, 
                Bach3HMinPT, 
                Bach3HIP2PV, 
                Bach3HIPChisq2PV, 
                Bach3HVtxChisq, 
                Bach3HVtxSepChisq, 
                Bach3HDiraPV, 
                Bach3HZVtxSep
                ):

    """
    Create and return PiPiPi Selection object.
    #
    Arguments:
    name                  : name of the Selection.
    pionSel               : Pion selection object
    Bach3HMassWindow      : Mass Window
    Bach3HDocaMax         : Maximum value for DocaMax
    Bach3HMinPT           : Minimum pT of Candidate
    Bach3HIP2PV           : Minimum IP to PV 
    Bach3HIPChisq2PV      : Minimum IP Chisq to PC
    Bach3HVtxChisq        : Maximum Vertex Chisquare
    Bach3HVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    Bach3HDiraPV          : Minimum direction angle value
    Bach3HZVtxSep         : Minimum vertex separation from PC
    """

    _a1Alg = CombineParticles(name)
    _a1Alg.DecayDescriptor = "[a_1(1260)+ -> pi+ pi- pi+]cc" 
    _a1Alg.CombinationCut = "(ADAMASS('a_1(1260)-') < %(Bach3HMassWindow)s *MeV) & (APT > %(Bach3HMinPT)s *MeV)" \
                           " & (AMAXDOCA('') < %(Bach3HDocaMax)s *mm) " %locals()
    _a1Alg.MotherCut = "( (PT > %(Bach3HMinPT)s *MeV) & (VFASPF(VCHI2/VDOF)< %(Bach3HVtxChisq)s )" \
                       " & (BPVVDCHI2 > %(Bach3HVtxSepChisq)s ) & (MIPCHI2DV(PRIMARY) > %(Bach3HIPChisq2PV)s )" \
                       " & (BPVDIRA > %(Bach3HDiraPV)s ) & (BPVVDZ> %(Bach3HZVtxSep)s *mm) " \
                       " & (MIPDV(PRIMARY)> %(Bach3HIP2PV)s *mm))" % locals()

    selName = 'Sel'+name    
    PiPiPiSelection = Selection(selName,
                                Algorithm = _a1Alg,
                                RequiredSelections = [pionSel])
    return PiPiPiSelection

def makeKPiPi( name,
               pionSel,
               kaonSel,
               Bach3HMassWindow, 
               Bach3HDocaMax, 
               Bach3HMinPT, 
               Bach3HIP2PV, 
               Bach3HIPChisq2PV, 
               Bach3HVtxChisq, 
               Bach3HVtxSepChisq, 
               Bach3HDiraPV, 
               Bach3HZVtxSep
               ):

    """
    Create and return KPiPi Selection object.
    #
    Arguments:
    name                  : name of the Selection.
    pionSel               : Pion selection object
    kaonSel               : Kaon selection object
    Bach3HMassWindow      : Mass Window
    Bach3HDocaMax         : Maximum value for DocaMax
    Bach3HMinPT           : Minimum pT of Candidate
    Bach3HIP2PV           : Minimum IP to PV 
    Bach3HIPChisq2PV      : Minimum IP Chisq to PC
    Bach3HVtxChisq        : Maximum Vertex Chisquare
    Bach3HVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    Bach3HDiraPV          : Minimum direction angle value
    Bach3HZVtxSep         : Minimum vertex separation from PC
    """

    _k1Alg = CombineParticles(name)
    _k1Alg.DecayDescriptor = "[K_1(1270)+ -> K+ pi- pi+]cc" 
    _k1Alg.CombinationCut = "(ADAMASS('K_1(1270)-') < %(Bach3HMassWindow)s *MeV) & (APT > %(Bach3HMinPT)s  *MeV)" \
                           " & (AMAXDOCA('') < %(Bach3HDocaMax)s *mm) " %locals()
    _k1Alg.MotherCut = "( (PT > %(Bach3HMinPT)s *MeV) & (VFASPF(VCHI2/VDOF)< %(Bach3HVtxChisq)s )  " \
                       " & (BPVVDCHI2 > %(Bach3HVtxSepChisq)s ) & (MIPCHI2DV(PRIMARY) > %(Bach3HIPChisq2PV)s )" \
                       " & (BPVDIRA > %(Bach3HDiraPV)s ) & (BPVVDZ> %(Bach3HZVtxSep)s *mm) " \
                       " & (MIPDV(PRIMARY)> %(Bach3HIP2PV)s *mm))" % locals()

    selName = 'Sel'+name
    KPiPiSelection = Selection(selName,
                               Algorithm = _k1Alg,
                               RequiredSelections = [pionSel, kaonSel]) 
    return KPiPiSelection



def makeD0Meson(name,
                DMassWindow, 
                DMinPT, 
                DIP2PV, 
                DIPChisq2PV, 
                DVtxChisq, 
                DVtxSepChisq, 
                DDiraPV, 
                DZVtxSep
                ):
    """
    Create and return D0 Selection object.
    Select the follwing charged D decays:
    D0  --> K-Pi, K+Pi-(DCS), K+K- and Pi+Pi-, filtered from StdLooseD02XX
    #
    Arguments:
    name             : name of the Selection.
    DMassWindow      : D/Ds Mass Window
    DMinPT           : Minimum pT of Candidate
    DIP2PV           : Minimum IP to PV 
    DIPChisq2PV      : Minimum IP Chisq to PC
    DVtxChisq        : Maximum Vertex Chisquare
    DVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    DDiraPV          : Minimum direction angle value
    DZVtxSep         : Minimum vertex separation from PC
    """

    _d02pipi = DataOnDemand(Location = "Phys/StdLooseD02PiPi")
    _d02kk = DataOnDemand(Location = "Phys/StdLooseD02KK")
    _d02kpi = DataOnDemand(Location = "Phys/StdLooseD02KPi")
    _d02kpiDCS = DataOnDemand(Location = "Phys/StdLooseD02KPiDCS")

    name2 = "_filterFor" + name
    _d0Filter = FilterDesktop(name2)
    _d0Filter.Code = "((VFASPF(VCHI2/VDOF)< %(DVtxChisq)s ) & (PT > %(DMinPT)s *MeV) &  (ADMASS('D0') < %(DMassWindow)s *MeV) " \
                    " & (BPVVDZ> %(DZVtxSep)s *mm) & (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s )" \
                    "& (BPVVDCHI2 > %(DVtxSepChisq)s ) & (BPVDIRA > %(DDiraPV)s )"   \
                    "& (MIPDV(PRIMARY)> %(DIP2PV)s *mm)) "  %locals()


    selName = "MergedD0Sel" + name
    _d0 = MergedSelection(selName, RequiredSelections = [_d02kpi, _d02kpiDCS, _d02kk, _d02pipi])    
    selName = "FilterD0Sel" + name

    return Selection(selName, Algorithm = _d0Filter, RequiredSelections = [ _d0 ])


def makeDStarMeson(name,
                DMassWindow, 
                DMinPT, 
                DIP2PV, 
                DIPChisq2PV, 
                DVtxChisq, 
                DVtxSepChisq, 
                DDiraPV, 
                DZVtxSep
                ):
    """
    Create and return D0 Selection object.
    Select the follwing charged D decays:
    D0  --> K-Pi, K+Pi-(DCS), K+K- and Pi+Pi-, filtered from StdLooseD02XX
    #
    Arguments:
    name             : name of the Selection.
    DMassWindow      : Mass Window (not currently used)
    DMinPT           : Minimum pT of Candidate
    DIP2PV           : Minimum IP to PV 
    DIPChisq2PV      : Minimum IP Chisq to PC
    DVtxChisq        : Maximum Vertex Chisquare
    DVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    DDiraPV          : Minimum direction angle value
    DZVtxSep         : Minimum vertex separation from PC
    """

    _dstar2kpi = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPi")
    _dstar2pipi = DataOnDemand(Location = "Phys/StdLooseDstarWithD02PiPi")
    _dstar2kk = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KK")
    _dstar2kpidcs = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPiDCS")
    
    name2 = "_filterFor" + name
    _dstarFilter = FilterDesktop(name2)
    _dstarFilter.Code = "((VFASPF(VCHI2/VDOF)< %(DVtxChisq)s ) & (PT > %(DMinPT)s *MeV) " \
                    " & (BPVVDZ> %(DZVtxSep)s *mm) & (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s )" \
                    "& (BPVVDCHI2 > %(DVtxSepChisq)s ) & (BPVDIRA > %(DDiraPV)s )"   \
                    "& (MIPDV(PRIMARY)> %(DIP2PV)s *mm)) "  %locals()

    selName = "MergedDStarSel" + name
    _dstar = MergedSelection(selName, RequiredSelections = [_dstar2kpi, _dstar2kpidcs, _dstar2kk, _dstar2pipi])    

    selName = "FilterDStarSel" + name
    return Selection(selName, Algorithm = _dstarFilter, RequiredSelections = [ _dstar] )


def makeDMeson(name,
               DMassWindow, 
               DMinPT, 
               DIP2PV, 
               DIPChisq2PV, 
               DVtxChisq, 
               DVtxSepChisq, 
               DDiraPV, 
               DZVtxSep
                ):
    """
    Create and return D+, Ds+ Selection object.
    Select the follwing charged D decays:
    D+   --> K-Pi+Pi+, K+ K- Pi+, Pi+Pi-Pi+
    D_s+ --> K+Pi-Pi+, K+ K- Pi+, Pi+Pi-Pi+
    Note that the K Pi Pi final state has different charges for the D+ and D_s_
    #
    Arguments:
    name             : name of the Selection.
    DMassWindow      : D/Ds Mass Window
    DMinPT           : Minimum pT of Candidate
    DIP2PV           : Minimum IP to PV 
    DIPChisq2PV      : Minimum IP Chisq to PC
    DVtxChisq        : Maximum Vertex Chisquare
    DVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    DDiraPV          : Minimum direction angle value
    DZVtxSep         : Minimum vertex separation from PC
    """


    _d2kpipi = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi")
    _d2kkpi = DataOnDemand(Location = "Phys/StdLooseDplus2KKPi")
    _d2pipipi = DataOnDemand(Location = "Phys/StdLooseDplus2PiPiPi")
    _d2kpipiOppSignPi = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPiOppSignPi")

    name2 = "_DFilterFor" + name
    _dFilter = FilterDesktop(name2)
    _dFilter.Code = "((VFASPF(VCHI2/VDOF)< %(DVtxChisq)s ) & (PT > %(DMinPT)s *MeV) & (ADMASS('D-') < %(DMassWindow)s *MeV ) " \
                    " & (BPVVDZ> %(DZVtxSep)s *mm) & (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s )" \
                    "& (BPVVDCHI2 > %(DVtxSepChisq)s ) & (BPVDIRA > %(DDiraPV)s )"   \
                    "& (MIPDV(PRIMARY)> %(DIP2PV)s *mm)) "  %locals()
    name2 = "_DsFilterFor" + name
    _dsFilter = FilterDesktop(name2)
    _dsFilter.Code = "((VFASPF(VCHI2/VDOF)< %(DVtxChisq)s ) & (PT > %(DMinPT)s *MeV) & (ADMASS('D_s+') < %(DMassWindow)s *MeV ) " \
                    " & (BPVVDZ> %(DZVtxSep)s *mm) & (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s )" \
                    "& (BPVVDCHI2 > %(DVtxSepChisq)s ) & (BPVDIRA > %(DDiraPV)s )"   \
                    "& (MIPDV(PRIMARY)> %(DIP2PV)s *mm)) "  %locals()
    name2 = "_DandDsFilterFor" + name
    _danddsFilter = FilterDesktop(name2)
    _danddsFilter.Code = "((VFASPF(VCHI2/VDOF)< %(DVtxChisq)s ) & (PT > %(DMinPT)s *MeV) " \
                         " & ((ADMASS('D+') < %(DMassWindow)s *MeV) | (ADMASS('D_s+') < %(DMassWindow)s *MeV))" \
                         " & (BPVVDZ> %(DZVtxSep)s *mm) & (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s )" \
                         "& (BPVVDCHI2 > %(DVtxSepChisq)s ) & (BPVDIRA > %(DDiraPV)s )"   \
                         "& (MIPDV(PRIMARY)> %(DIP2PV)s *mm)) "  %locals()



    selName = 'FilterKPiPiSel_'+name
    _selKPiPi =  Selection(selName, Algorithm = _dFilter, RequiredSelections = [_d2kpipi] )
    selName = 'FilterKKPiSel_'+name
    _selKKPi =  Selection(selName, Algorithm = _danddsFilter, RequiredSelections = [_d2kkpi] )
    selName = 'FilterPiPiPiSel_'+name
    _selPiPiPi =  Selection(selName, Algorithm = _danddsFilter, RequiredSelections = [_d2pipipi] )
    selName = 'FilterKPiPiOppSignSel_'+name
    _selKPiPiOppSign =  Selection(selName, Algorithm = _dsFilter, RequiredSelections = [_d2kpipiOppSignPi])


    selName = 'MergedSel'+name
    _d = MergedSelection(selName, RequiredSelections = [_selKPiPi, _selKKPi, _selPiPiPi, _selKPiPiOppSign] )
    return _d


def makeB2D3H( name,
               dSel,
               hhhSel,
               BMassWindow, 
               BMinPT, 
               BIP2PV, 
               BIPChisq2PV, 
               BVtxChisq, 
               BVtxSepChisq, 
               BDiraPV, 
               BZVtxSep,
               BDZVtxSep,
               decayDesc,
               parentB):

    """
    Create and return a B-->D+3H Selection object.
    Requires the D and 3h Selection objects.
    Arguments:
    name             : name of the Selection.
    dSel             : D Selection object
    hhhSel           : 3H Selection object
    BMassWindow      : Mass Window
    BMinPT           : Minimum pT of Candidate
    BIP2PV           : Maximum IP to PV 
    BIPChisq2PV      : Maximum IP Chisq to PC
    BVtxChisq        : Maximum Vertex Chisquare
    BVtxSepChisq,    : Minimum separation chisq. between candidate and assoc. PV
    BDiraPV          : Minimum direction angle value
    BZVtxSep         : Minimum vertex separation from PC
    BDZVtxSep         : Minimum vertex separation from PC
    decayDesc        : Decay descriptor for CombineParticles
    parentB          : parent B (use either B0 or B+)
    """
    
    _b2d3h = CombineParticles(name)
    _b2d3h.DecayDescriptor = decayDesc

    combCut = "(ADAMASS('" + parentB + "') < %(BMassWindow)s *MeV) & (APT > %(BMinPT)s *MeV)"
    if parentB == "B0":
        combCut = "((ADAMASS('B0') < %(BMassWindow)s *MeV) | (ADAMASS('B_s0') < %(BMassWindow)s *MeV))" \
                  "& (APT > %(BMinPT)s *MeV)"    
    _b2d3h.CombinationCut = combCut  % locals()
    _b2d3h.Preambulo = [ "Z  = VFASPF(VZ)"   ,  
                        "DZ = CHILD(Z,1)-Z" ]
    _b2d3h.MotherCut = "( " \
                       " (VFASPF(VCHI2/VDOF)< %(BVtxChisq)s ) & (PT > %(BMinPT)s *MeV) " \
                       " & (DZ > %(BDZVtxSep)s *mm) " \
                       " & (BPVVDZ> %(BZVtxSep)s *mm) & (MIPCHI2DV(PRIMARY)< %(BIPChisq2PV)s )" \
                       " & (BPVVDCHI2 > %(BVtxSepChisq)s ) & (BPVDIRA > %(BDiraPV)s )"   \
                       " & (MIPDV(PRIMARY)< %(BIP2PV)s *mm) " \
                       ")" % locals()

    selName = 'Sel'+name
    return Selection(name, Algorithm  = _b2d3h, RequiredSelections = [dSel, hhhSel])



def StrippingB2D3HLoose(name='Loose') :
    from StrippingSelections.StrippingB2D3H import StrippingB2D3HConf as conf
    config_params =  {
        "PionMinP"             : 2000.,
        "PionMaxP"             : 200000.,
        "PionMinPT"            : 250.,
        "KaonMinP"             : 2000.,
        "KaonMaxP"             : 100000.,
        "KaonMinPT"            : 250.,
        "PionMinIPChisq"       : 7.0,
        "KaonMinIPChisq"       : 7.0,
        "TrkChisq"             : 10.,
        "Bach3HMassWindow"     : 400,
        "Bach3HDocaMax"        : 0.3,
        "Bach3HVtxChisq"       : 10.0,
        "Bach3HMinPT"          : 900.0, 
        "Bach3HIP2PV"          : 0.07,
        "Bach3HIPChisq2PV"     : 6.25,
        "Bach3HVtxSepChisq"    : 16.0,
        "Bach3HDiraPV"         : 0.98,
        "Bach3HZVtxSep"        : 1.5,
        "DMinPT"               : 1000,
        "DVtxChisq"            : 10.0,
        "DMassWindow"          : 100,
        "DIP2PV"               : 0.07,
        "DIPChisq2PV"          : 6.25,
        "DVtxSepChisq"         : 16.0,
        "DDiraPV"              : 0.99,
        "DZVtxSep"             : 2.0,
        "BMinPT"               : 0.0, 
        "BVtxChisq"            : 10.0,
        "BMassWindow"          : 350, 
        "BIP2PV"               : 0.15,
        "BIPChisq2PV"          : 49.0,
        "BVtxSepChisq"         : 16.0,
        "BDiraPV"              : 0.9995,
        "BZVtxSep"             : 1.5,
        "BDZVtxSep"            : -1.0  
        }


    b2d3h = conf(name,config_params)

    return b2d3h

def StrippingB2D3HNominal(name="Def") :
    from StrippingSelections.StrippingB2D3H import StrippingB2D3HConf as conf
    config_params =  {
        "PionMinP"             : 2000.,
        "PionMaxP"             : 200000.,
        "PionMinPT"            : 250.,
        "KaonMinP"             : 2000.,
        "KaonMaxP"             : 100000.,
        "KaonMinPT"            : 250.,
        "PionMinIPChisq"       : 9.0,
        "KaonMinIPChisq"       : 9.0,
        "TrkChisq"             : 10.,
        "Bach3HMassWindow"     : 400,
        "Bach3HDocaMax"        : 0.3,
        "Bach3HVtxChisq"       : 10.0,
        "Bach3HMinPT"          : 1000.0, 
        "Bach3HIP2PV"          : 0.075,
        "Bach3HIPChisq2PV"     : 9.0,
        "Bach3HVtxSepChisq"    : 25.0,
        "Bach3HDiraPV"         : 0.985,
        "Bach3HZVtxSep"        : 1.5,
        "DMinPT"               : 1000,
        "DVtxChisq"            : 10.0,
        "DMassWindow"          : 70,
        "DIP2PV"               : 0.070,
        "DIPChisq2PV"          : 9.0,
        "DVtxSepChisq"         : 25.0,
        "DDiraPV"              : 0.992,
        "DZVtxSep"             : 2.0,
        "BMinPT"               : 0.0, 
        "BVtxChisq"            : 10.0,
        "BMassWindow"          : 200, 
        "BIP2PV"               : 0.10,
        "BIPChisq2PV"          : 36.0,
        "BVtxSepChisq"         : 25.0,
        "BDiraPV"              : 0.9999,
        "BZVtxSep"             : 2.0,  
        "BDZVtxSep"            : -1.0  
        }

    
    b2d3h = conf(name,config_params)

    return b2d3h

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
   - makeDMeson
   - makeLambdaC
   - makeDStarMeson
   - makeA12PiPiPi
   - makeK12KPiPi
   - makePions
   - makeKaons
   - StrippingB2D3HLoose
   - StrippingB2D3HNominal
"""


__author__ = ['Steven Blusk']
__date__ = '26/07/2010'
__version__ = '$Revision: 1.4 $'


__all__ = ('StrippingB2D3HConf',
           'makeB2D3H',
           'makeD0Meson',
           'makeDMeson',
           'makeLambdaC',
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
    selD0                  : selected D0 --> KK, KPi, and PiPi
    selDStar               : selected D*+, with D0 --> KK, KPi, KPiPi
    B2D0PiPiPi             : Selected B+ -->D0+PiPiPi
    B2D0KPiPi              : Selected B+ -->D0+KPiPi
    B2DPiPiPi              : Selected B0/Bs -->D+PiPiPi, Ds+PiPiPi
    B2DKPiPi               : Selected B0/Bs -->Ds+KPiPi, Ds+KPiPi
    lines                  : List of lines [B2D0PiPiPi, B2D0KPiPi, B2DPiPiPi, B2DKPiPi]

    A number of prescale & postscale factors are included for maximum flexibility [# prescale/postscales]
    -----------------------------------------------------------------------------
    B2D0hhhAll_Prescale, Postscale = Prescale/postscale for B+ -->D0 hhh, hhh=PiPiPi or KPiPi, full mass region. [4]
    B2DhhhAll_Prescale, Postscale = Prescale/postscale for B 0/Bs-->D+/Ds hhh, hhh=PiPiPi or KPiPi, full mass region. [8]
    B2DStarhhhAll_Prescale, Postscale = Prescale/postscale for B0 -->D*+ hhh, hhh=PiPiPi or KPiPi, full mass region. [4]
    B2D0hhhASignal Prescale, Postscale = Prescale/postscale for B+ -->D0 hhh, hhh=PiPiPi or KPiPi, signal box region. [4]
    B2DhhhSignal_Prescale, Postscale = Prescale/postscale for B 0/Bs-->D+/Ds hhh, hhh=PiPiPi or KPiPi, signal box region. [8]
    B2DStarhhhSignal_Prescale, Postscale = Prescale/postscale for B0 -->D*+ hhh, hhh=PiPiPi or KPiPi, signal region. [4]

    --> Ordered preference for reducing timing/retentions
    [1] If needed one should prescale the "B2DxxxxAll" prescale/postscale factors, say to 0.2 or 0.1 (needed for SideBand subtraction, background studies, etc)
    [2] Reduce MaxTracks, keep value at least at 100 Long tracks (S/B will degrade at high #Long tracks)
    [3] Reduce DMassWindow to 30 MeV (this will selecton only B-->D+ (hhh) in the D signal region (don't reduce D0MassWindow or DsMassWindow)
    [4] hhh/D/B Vertex separation chisquare can be increased if absolutely necessary to 49.
    [5] Set DDocaMax to 0.3 mm
    [6] Set BMinPT to 2 GeV

    Exports as class data member:
    StrippingB2D3HConf.__configuration_keys__ : List of required configuration parameters.
    """

    __configuration_keys__ = ("PionMinP",
                              "PionMaxP", 
                              "PionMinPT",
                              "PionMinPTtight",
                              "PionPiDDLL",
                              "KaonMinP" ,
                              "PionMinIPChisq",
                              "KaonMaxP" ,
                              "KaonMinPT",
                              "KaonMinPTtight",
                              "KaonMinIPChisq",
                              "ProtonMinP" ,
                              "ProtonMaxP" ,
                              "ProtonMinPT",
                              "ProtonMinIPChisq",
                              "MinPT",
                              "TrkChisq",
                              "TrkChisqtight",
                              "Bach3HMassWindow",    
                              "Bach3HDocaMax",
                              "Bach3HVtxChisq",      
                              "Bach3HMinPT",   
                              "Bach3HIP2PV",   
                              "Bach3HIPChisq2PV",  
                              "Bach3HVtxSepChisq",   
                              "Bach3HDiraPV",        
                              "Bach3HZVtxSep",
                              "Bach3HDRPV",       
                              "DMinPT",            
                              "DVtxChisq",        
                              "DMassWindow",
                              "D0MassWindow",
                              "DsMassWindow",
                              "tightDMassWindow",
                              "DDocaMax",         
                              "DIP2PV",              
                              "DIPChisq2PV",         
                              "DVtxSepChisq",         
                              "DDiraPV",              
                              "DZVtxSep",
                              "DDRPV",       
                              "DStarMassWindow",
                              "DStarMinPT",            
                              "BMinPT",               
                              "BVtxChisq",            
                              "BMassWindow",   
                              "tightBMassWindow",
                              "BIP2PV",        
                              "BIPChisq2PV",    
                              "BVtxSepChisq",    
                              "BDiraPV",         
                              "BZVtxSep",
                              "BDZVtxSep",
                              "BDRPV",       
                              "MaxTracks",
                              "B2D0PiPiPiAll_Prescale",
                              "B2D0PiPiPiAll_Postscale",
                              "B2DPiPiPiAll_Prescale",
                              "B2DPiPiPiAll_Postscale",
                              "B2DStarPiPiPiAll_Prescale",
                              "B2DStarPiPiPiAll_Postscale",
                              "B2D0PiPiPiSignal_Prescale",
                              "B2D0PiPiPiSignal_Postscale",
                              "B2DPiPiPiSignal_Prescale",
                              "B2DPiPiPiSignal_Postscale",
                              "B2DStarPiPiPiSignal_Prescale",
                              "B2DStarPiPiPiSignal_Postscale",
                              "B2D0KPiPiAll_Prescale",
                              "B2D0KPiPiAll_Postscale",
                              "B2DKPiPiAll_Prescale",
                              "B2DKPiPiAll_Postscale",
                              "B2DStarKPiPiAll_Prescale",
                              "B2DStarKPiPiAll_Postscale",
                              "B2D0KPiPiSignal_Prescale",
                              "B2D0KPiPiSignal_Postscale",
                              "B2DKPiPiSignal_Prescale",
                              "B2DKPiPiSignal_Postscale",
                              "B2DStarKPiPiSignal_Prescale",
                              "B2DStarKPiPiSignal_Postscale",
                              "B2DDAll_Prescale",
                              "B2DDAll_Postscale",
                              "B2DDSignal_Prescale",
                              "B2DDSignal_Postscale",
                              "B2DStarDAll_Prescale",
                              "B2DStarDAll_Postscale",
                              "B2DStarDSignal_Prescale",
                              "B2DStarDSignal_Postscale",
                              "UnbiasedB2DPiPiPiAll_Prescale",
                              "UnbiasedB2DPiPiPiAll_Postscale",
                              "WSB2D3H_Prescale",
                              "WSB2D3H_Postscale"
                              )


    def __init__(self, 
                 name = 'Loose',
                 config = None) :

        checkConfig(StrippingB2D3HConf.__configuration_keys__, config)


        self.EventFilter = MyEventFilter('B2D3H'+name,
                                         MaxTracks = config['MaxTracks'])
        # First filter pions
        self.selPions = makePions( 'PionsForB2D3H'+name,
                                   PionMinP = config['PionMinP'],
                                   PionMaxP = config['PionMaxP'],
                                   PionMinPT = config['PionMinPT'],
                                   PionMinIPChisq = config['PionMinIPChisq'],
                                   PionPiDDLL = config['PionPiDDLL'],
                                   TrkChisq = config['TrkChisq'])
        # First filter kaons
        self.selKaons = makeKaons( 'KaonsForB2D3H'+name,
                                   KaonMinP = config['KaonMinP'],
                                   KaonMaxP = config['KaonMaxP'],
                                   KaonMinPT = config['KaonMinPT'],
                                   KaonMinIPChisq = config['KaonMinIPChisq'],
                                   TrkChisq = config['TrkChisq'])

        self.selProtons = makeProtons( 'ProtonsForB2D3H'+name,
                                   ProtonMinP = config['ProtonMinP'],
                                   ProtonMaxP = config['ProtonMaxP'],
                                   ProtonMinPT = config['ProtonMinPT'],
                                   ProtonMinIPChisq = config['ProtonMinIPChisq'],
                                   TrkChisq = config['TrkChisq'])

	#Filter Pions and Kaons with tighter PID cuts
	# First filter pions
        self.selTightPions = makeTightPions( 'TightPionsForB2D3H'+name,
                                    PionMinP = config['PionMinP'],
                                    PionMaxP = config['PionMaxP'],
                                    PionMinPT = config['PionMinPT'],
                                    PionMinIPChisq = config['PionMinIPChisq'],
                                    TrkChisq = config['TrkChisq'])
        # First filter kaons
        self.selTightKaons = makeTightKaons( 'TightKaonsForB2D3H'+name,
                                   KaonMinP = config['KaonMinP'],
                                   KaonMaxP = config['KaonMaxP'],
                                   KaonMinPT = config['KaonMinPT'],
                                   KaonMinIPChisq = config['KaonMinIPChisq'],
                                   TrkChisq = config['TrkChisq'])
        
	# Filter Pions and Kaons with higher PT/Chi2 cut and without IP
        # First filter pions
        self.selUnbiasedPions = makePions( 'UnbiasedPionsForB2D3H'+name,
                                           PionMinP = config['PionMinP'],
                                           PionMaxP = config['PionMaxP'],
                                           PionMinPT = config['PionMinPTtight'],
                                           PionMinIPChisq = 0, #should never change
                                           PionPiDDLL = 100000.0,
                                           TrkChisq = config['TrkChisqtight'])
        # First filter kaons
        self.selUnbiasedKaons = makeKaons( 'UnbiasedKaonsForB2D3H'+name,
                                   KaonMinP = config['KaonMinP'],
                                   KaonMaxP = config['KaonMaxP'],
                                   KaonMinPT = config['KaonMinPTtight'],
                                   KaonMinIPChisq = 0, #should never change  
                                   TrkChisq = config['TrkChisqtight'])

        # Now make the LT unbiased PiPiPi bachelor
        self.selUnbiasedPiPiPi = makePiPiPi( 'UnbiasedPiPiPiForB2D3H'+name,
                                     pionSel = self.selUnbiasedPions,
                                     MinPT = config['MinPT'],
                                     Bach3HMassWindow = 1650.0,
                                     Bach3HDocaMax = config['Bach3HDocaMax'],
                                     Bach3HMinPT = config['Bach3HMinPT'],
                                     Bach3HIP2PV = 0, #should never change  
                                     Bach3HIPChisq2PV = 0, #should never change 
                                     Bach3HVtxChisq = config['Bach3HVtxChisq'],
                                     Bach3HVtxSepChisq = config['Bach3HVtxSepChisq'],
                                     Bach3HDiraPV = config['Bach3HDiraPV'],
                                     Bach3HZVtxSep = config['Bach3HZVtxSep'],
                                     Bach3HDRPV = 0  #should never change      
                                     )
        
	# Now make the PiPiPi bachelor
        self.selPiPiPi = makePiPiPi( 'PiPiPiForB2D3H'+name,
                                     pionSel = self.selPions,
                                     MinPT = config['MinPT'],
                                     Bach3HMassWindow = config['Bach3HMassWindow'],
                                     Bach3HDocaMax = config['Bach3HDocaMax'],
                                     Bach3HMinPT = config['Bach3HMinPT'],
                                     Bach3HIP2PV = config['Bach3HIP2PV'],
                                     Bach3HIPChisq2PV = config['Bach3HIPChisq2PV'],
                                     Bach3HVtxChisq = config['Bach3HVtxChisq'],
                                     Bach3HVtxSepChisq = config['Bach3HVtxSepChisq'],
                                     Bach3HDiraPV = config['Bach3HDiraPV'],
                                     Bach3HZVtxSep = config['Bach3HZVtxSep'],
                                     Bach3HDRPV = config['Bach3HDRPV']       
                                     )
        # Make the KPiPi bachelor
        self.selKPiPi = makeKPiPi( 'KPiPiForB2D3H'+name,
                                   pionSel = self.selPions,
                                   kaonSel = self.selKaons,
                                   MinPT = config['MinPT'],
                                   Bach3HMassWindow = config['Bach3HMassWindow'],                                 
                                   Bach3HDocaMax = config['Bach3HDocaMax'],
                                   Bach3HMinPT = config['Bach3HMinPT'],
                                   Bach3HIP2PV = config['Bach3HIP2PV'],
                                   Bach3HIPChisq2PV = config['Bach3HIPChisq2PV'],
                                   Bach3HVtxChisq = config['Bach3HVtxChisq'],
                                   Bach3HVtxSepChisq = config['Bach3HVtxSepChisq'],
                                   Bach3HDiraPV = config['Bach3HDiraPV'],
                                   Bach3HZVtxSep = config['Bach3HZVtxSep'],
                                   Bach3HDRPV = config['Bach3HDRPV']      
                                   )

        # Filter LT Unbiased D+, the children don't suffer from lifetime biases since also prompt D's have a lifetime
        self.selUnbiasedDch = makeDMeson( 'UnbiasedForB2D3H'+name,
                                  pionSel = self.selTightPions,
                                  kaonSel = self.selTightKaons,
                                  DMassWindow = config['DMassWindow'],
                                  DsMassWindow = config['DsMassWindow'],
                                  DDocaMax = config['DDocaMax'],
                                  DMinPT = config['DMinPT'],
                                  MinPT = config['MinPT'],
                                  DIP2PV = 0, #should never change  
                                  DIPChisq2PV = 0, #should never change  
                                  DVtxChisq = config['DVtxChisq'],
                                  DVtxSepChisq = 0, #should never change 
                                  DDiraPV = config['DDiraPV'],
                                  DZVtxSep = 0, #should never change  
                                  DDRPV = 0 #should never change       
                                  )
        # Filter D+
        self.selDch = makeDMeson( 'ForB2D3H'+name,
                                  pionSel = self.selPions,
                                  kaonSel = self.selKaons,
                                  DMassWindow = config['DMassWindow'],
                                  DsMassWindow = config['DsMassWindow'],
                                  DDocaMax = config['DDocaMax'],
                                  DMinPT = config['DMinPT'],
                                  MinPT = config['MinPT'],
                                  DIP2PV = config['DIP2PV'],
                                  DIPChisq2PV = config['DIPChisq2PV'],
                                  DVtxChisq = config['DVtxChisq'],
                                  DVtxSepChisq = config['DVtxSepChisq'],
                                  DDiraPV = config['DDiraPV'],
                                  DZVtxSep = config['DZVtxSep'],
                                  DDRPV = config['DDRPV']      
                                  )

        # Filter Lambda_c+
        self.selLambdaC = makeLambdaC( 'ForB2D3H'+name,
                                      pionSel = self.selPions,
                                      kaonSel = self.selKaons,
                                      protonSel = self.selProtons,
                                      DMassWindow = config['DMassWindow'],
                                      DDocaMax = config['DDocaMax'],
                                      DMinPT = config['DMinPT'],
                                      MinPT = config['MinPT'],
                                      DIP2PV = config['DIP2PV'],
                                      DIPChisq2PV = config['DIPChisq2PV'],
                                      DVtxChisq = config['DVtxChisq'],
                                      DVtxSepChisq = config['DVtxSepChisq'],
                                      DDiraPV = config['DDiraPV'],
                                      DZVtxSep = config['DZVtxSep'],
                                      DDRPV = config['DDRPV']      
                                      ) 


        # Filter D0
        self.selD0 = makeD0Meson( 'ForB2D03H'+name,
                                  pionSel = self.selPions,
                                  kaonSel = self.selKaons,
                                  DMassWindow = config['D0MassWindow'],
                                  DDocaMax = config['DDocaMax'],
                                  DMinPT = config['DMinPT'],
                                  MinPT = config['MinPT'],
                                  DIP2PV = config['DIP2PV'],
                                  DIPChisq2PV = config['DIPChisq2PV'],
                                  DVtxChisq = config['DVtxChisq'],
                                  DVtxSepChisq = config['DVtxSepChisq'],
                                  DDiraPV = config['DDiraPV'],
                                  DZVtxSep = config['DZVtxSep'],
                                  DDRPV = config['DDRPV']      
                                  )

        # Filter Dstar
        self.selDStar = makeDStarMeson( 'ForB2DStar3H'+name,
                                        dSel = self.selD0,
                                        pionSel = self.selPions,
                                        DStarMassWindow = config['DStarMassWindow'],
                                        DDocaMax = config['DDocaMax'],
                                        DStarMinPT = config['DStarMinPT'],
                                        DIP2PV = config['DIP2PV'],
                                        DIPChisq2PV = config['DIPChisq2PV'],
                                        DVtxChisq = config['DVtxChisq'],
                                        DVtxSepChisq = config['DVtxSepChisq'],
                                        DDiraPV = config['DDiraPV'],
                                        DZVtxSep = config['DZVtxSep'],
                                        DDRPV = config['DDRPV']      
                                        )

        # Make B- --> D~0 (pipipi)+
        name1 = 'B2D0PiPiPi' + name
        self.B2D0PiPiPi = makeB2D3H(name1,
                                    dSel = self.selD0,
                                    hhhSel = self.selPiPiPi,
                                    BMassWindow = config['BMassWindow'],
                                    BMinPT = config['BMinPT'],
                                    MinPT = config['MinPT'],
                                    BIP2PV = config['BIP2PV'],
                                    BIPChisq2PV = config['BIPChisq2PV'],
                                    BVtxChisq = config['BVtxChisq'],
                                    BVtxSepChisq = config['BVtxSepChisq'],
                                    BDiraPV = config['BDiraPV'],
                                    BZVtxSep = config['BZVtxSep'],
                                    BDZVtxSep = config['BDZVtxSep'],
                                    BDRPV = config['BDRPV'],      
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
                                   MinPT = config['MinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   BDRPV = config['BDRPV'],      
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
                                   MinPT = config['MinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   BDRPV = config['BDRPV'],      
                                   decayDesc = "[B0 -> D- a_1(1260)+]cc",
                                   parentB = "B0")

        # Make LT unbiased B- --> D- (pipipi)+
        name4 = 'UnbiasedB2DPiPiPi' + name
        self.UnbiasedB2DPiPiPi = makeB2D3H(name4,
                                   dSel = self.selUnbiasedDch,
                                   hhhSel = self.selUnbiasedPiPiPi,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   MinPT = config['MinPT'],
                                   BIP2PV = 100000, #should never change
                                   BIPChisq2PV = 100000, #should never change   
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = 0, #should never change
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = 0, #should never change
                                   BDZVtxSep = 0, #should never change
                                   BDRPV = 0, #should never change
                                   decayDesc = "[B0 -> D- a_1(1260)+]cc",
                                   parentB = "B0")

	#Filter LT unbiased B's for HLT1 and HLT2 TIS
 	self.UnbiasedB2DPiPiPiHLT1TIS = makeTISTOSSel("HLT1TISSelFor" + name + "WithUnbiasedB2DPiPiPi", self.UnbiasedB2DPiPiPi, "Hlt1Global%TIS")
 	self.UnbiasedB2DPiPiPiHLT1HLT2TIS = makeTISTOSSel("HLT2TISSelFor" + name + "WithUnbiasedB2DPiPiPi", self.UnbiasedB2DPiPiPiHLT1TIS, "Hlt2Global%TIS")

        # Make B0 --> D- (Kpipi)+
        name5 = 'B2DKPiPi' + name
        self.B2DKPiPi = makeB2D3H(name5,
                                  dSel = self.selDch,
                                  hhhSel = self.selKPiPi,
                                  BMassWindow = config['BMassWindow'],
                                  BMinPT = config['BMinPT'],
                                  MinPT = config['MinPT'],
                                  BIP2PV = config['BIP2PV'],
                                  BIPChisq2PV = config['BIPChisq2PV'],
                                  BVtxChisq = config['BVtxChisq'],
                                  BVtxSepChisq = config['BVtxSepChisq'],
                                  BDiraPV = config['BDiraPV'],
                                  BZVtxSep = config['BZVtxSep'],
                                  BDZVtxSep = config['BDZVtxSep'],
                                  BDRPV = config['BDRPV'],      
                                  decayDesc = "[B0 -> D- K_1(1270)+]cc",
                                  parentB = "B0")

        # Make B- --> D*- (pipipi)+
        name6 = 'B2DStarPiPiPi' + name
        self.B2DStarPiPiPi = makeB2D3H(name6,
                                       dSel = self.selDStar,
                                       hhhSel = self.selPiPiPi,
                                       BMassWindow = config['BMassWindow'],
                                       BMinPT = config['BMinPT'],
                                       MinPT = config['MinPT'],
                                       BIP2PV = config['BIP2PV'],
                                       BIPChisq2PV = config['BIPChisq2PV'],
                                       BVtxChisq = config['BVtxChisq'],
                                       BVtxSepChisq = config['BVtxSepChisq'],
                                       BDiraPV = config['BDiraPV'],
                                       BZVtxSep = config['BZVtxSep'],
                                       BDZVtxSep = config['BDZVtxSep'],
                                       BDRPV = config['BDRPV'],      
                                       decayDesc = "[B0 -> D*(2010)- a_1(1260)+]cc",
                                       parentB = "B0")
        


        # Make B0 --> D*- (Kpipi)+
        name7 = 'B2DStarKPiPi' + name
        self.B2DStarKPiPi = makeB2D3H(name7,
                                      dSel = self.selDStar,
                                      hhhSel = self.selKPiPi,
                                      BMassWindow = config['BMassWindow'],
                                      BMinPT = config['BMinPT'],
                                      MinPT = config['MinPT'],
                                      BIP2PV = config['BIP2PV'],
                                      BIPChisq2PV = config['BIPChisq2PV'],
                                      BVtxChisq = config['BVtxChisq'],
                                      BVtxSepChisq = config['BVtxSepChisq'],
                                      BDiraPV = config['BDiraPV'],
                                      BZVtxSep = config['BZVtxSep'],
                                      BDZVtxSep = config['BDZVtxSep'],
                                      BDRPV = config['BDRPV'],      
                                      decayDesc = "[B0 -> D*(2010)- K_1(1270)+]cc",
                                      parentB = "B0")



        # Make B- --> D_(s)- D_(s)+
        name8 = 'B2DD' + name
        self.B2DD = makeB2D3H(name8,
                                   dSel = self.selDch,
                                   hhhSel = self.selDch,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   MinPT = config['MinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   BDRPV = config['BDRPV'],      
                                   decayDesc = "B0 -> D- D+",
                                   parentB = "B0")

        # Make B- --> D_(s)- D_(s)+
        name9 = 'B2DStarD' + name
        self.B2DStarD = makeB2D3H(name9,
                                   dSel = self.selDStar,
                                   hhhSel = self.selDch,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   MinPT = config['MinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   BDRPV = config['BDRPV'],      
                                   decayDesc = "[B0 -> D*(2010)- D+]cc",
                                   parentB = "B0")


        # Make LambdaB --> LambdaC+ (pipipi)+
        name9 = 'LambdaB2LambdaCPiPiPi' + name
        self.LambdaB2LambdaCPiPiPi = makeB2D3H(name9,
                                   dSel = self.selLambdaC,
                                   hhhSel = self.selPiPiPi,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   MinPT = config['MinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   BDRPV = config['BDRPV'],      
                                   decayDesc = "[Lambda_b0 -> Lambda_c+ a_1(1260)-]cc",
                                   parentB = "Lambda_b0")


        # Make LambdaB --> LambdaC+ (Kpipi)-
        name10 = 'LambdaB2LambdaCKPiPi' + name
        self.LambdaB2LambdaCKPiPi = makeB2D3H(name10,
                                              dSel = self.selLambdaC,
                                              hhhSel = self.selKPiPi,
                                              BMassWindow = config['BMassWindow'],
                                              BMinPT = config['BMinPT'],
                                              MinPT = config['MinPT'],
                                              BIP2PV = config['BIP2PV'],
                                              BIPChisq2PV = config['BIPChisq2PV'],
                                              BVtxChisq = config['BVtxChisq'],
                                              BVtxSepChisq = config['BVtxSepChisq'],
                                              BDiraPV = config['BDiraPV'],
                                              BZVtxSep = config['BZVtxSep'],
                                              BDZVtxSep = config['BDZVtxSep'],
                                              BDRPV = config['BDRPV'],      
                                              decayDesc = "[Lambda_b0 -> Lambda_c+ K_1(1270)-]cc",
                                              parentB = "Lambda_b0")
        
        # Now, create signal lines, with tight windows around the D mass, and a narrower B mass window.

        # Filter off B+ -->D0(PiPiPi) & D0 KPiPi
        self.SignalB2D0PiPiPi = filterB2D3H('B2D0PiPiPi',
                                            bSel = self.B2D0PiPiPi,
                                            tightDMassWindow = config['tightDMassWindow'],
                                            tightBMassWindow = config['tightBMassWindow'],
                                            parentB = 'B+',
                                            parentD = 1
                                            )
        self.SignalB2D0KPiPi = filterB2D3H('B2D0KPiPi',
                                            bSel = self.B2D0KPiPi,
                                            tightDMassWindow = config['tightDMassWindow'],
                                            tightBMassWindow = config['tightBMassWindow'],
                                            parentB = 'B+',
                                            parentD = 1
                                            )
        #Filter Off the B0-->D+(PiPiPi) (includes Bs -->D_s(PiPiPi))
        self.SignalB2DPiPiPi = filterB2D3H('B2DPiPiPi',
                                            bSel = self.B2DPiPiPi,
                                            tightDMassWindow = config['tightDMassWindow'],
                                            tightBMassWindow = config['tightBMassWindow'],
                                            parentB = 'B0',
                                            parentD = 2
                                            )
        #Filter Off the B0-->D+(KPiPi) (includes Bs-->Ds(KPiPi))
        self.SignalB2DKPiPi = filterB2D3H('B2DKPiPi',
                                            bSel = self.B2DKPiPi,
                                            tightDMassWindow = config['tightDMassWindow'],
                                            tightBMassWindow = config['tightBMassWindow'],
                                            parentB = 'B0',
                                            parentD = 2
                                            )
        #Filter Off the B0-->D*+ (PiPiPi), D*+ -->D0pi+ 
        self.SignalB2DStarPiPiPi = filterB2D3H('B2DStarPiPiPi',
                                            bSel = self.B2DStarPiPiPi,
                                            tightDMassWindow = config['tightDMassWindow'],
                                            tightBMassWindow = config['tightBMassWindow'],
                                            parentB = 'B0',
                                            parentD = 3
                                            )
        #Filter Off the B0-->D*+ (KPiPi), D*+ -->D0pi+ 
        self.SignalB2DStarKPiPi = filterB2D3H('B2DStarKPiPi',
                                            bSel = self.B2DStarKPiPi,
                                            tightDMassWindow = config['tightDMassWindow'],
                                            tightBMassWindow = config['tightBMassWindow'],
                                            parentB = 'B0',
                                            parentD = 3
                                            )
        #Filter Off the B0-->D_(s)+D_(s)- 
        self.SignalB2DD = filterB2D3H('B2DD',
                                      bSel = self.B2DD,
                                      tightDMassWindow = config['tightDMassWindow'],
                                      tightBMassWindow = config['tightBMassWindow'],
                                      parentB = 'B0',
                                      parentD = 4
                                      )
        #Filter Off the B0-->D*+D_(s)- 
        self.SignalB2DStarD = filterB2D3H('B2DStarD',
                                      bSel = self.B2DStarD,
                                      tightDMassWindow = config['tightDMassWindow'],
                                      tightBMassWindow = config['tightBMassWindow'],
                                      parentB = 'B0',
                                      parentD = 5
                                      )

        #Filter Off the LambdaB0-->Lambda+(PiPiPi)
        self.SignalLambdaB2LambdaCPiPiPi = filterB2D3H('LambdaB2LambdaCPiPiPi',
                                            bSel = self.LambdaB2LambdaCPiPiPi,
                                            tightDMassWindow = config['tightDMassWindow'],
                                            tightBMassWindow = config['tightBMassWindow'],
                                            parentB = 'Lambda_b0',
                                            parentD = 6
                                            )

        #Filter Off the LambdaB0-->Lambda+(KPiPi)
        self.SignalLambdaB2LambdaCKPiPi = filterB2D3H('LambdaB2LambdaCKPiPi',
                                            bSel = self.LambdaB2LambdaCKPiPi,
                                            tightDMassWindow = config['tightDMassWindow'],
                                            tightBMassWindow = config['tightBMassWindow'],
                                            parentB = 'Lambda_b0',
                                            parentD = 6
                                            )


        # ----------------------------------
        # Create the Wrong-Sign Combinations
        # ----------------------------------

        # Make B- --> D- (pipipi)+
        name3 = 'WSB2DPiPiPi' + name
        self.WSB2DPiPiPi = makeB2D3H(name3,
                                   dSel = self.selDch,
                                   hhhSel = self.selPiPiPi,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   MinPT = config['MinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   BDRPV = config['BDRPV'],      
                                   decayDesc = "[B0 -> D- a_1(1260)-]cc",
                                   parentB = "B0")
        
        # Make B0 --> D- (Kpipi)-
        name5 = 'WSB2DKPiPi' + name
        self.WSB2DKPiPi = makeB2D3H(name5,
                                  dSel = self.selDch,
                                  hhhSel = self.selKPiPi,
                                  BMassWindow = config['BMassWindow'],
                                  BMinPT = config['BMinPT'],
                                  MinPT = config['MinPT'],
                                  BIP2PV = config['BIP2PV'],
                                  BIPChisq2PV = config['BIPChisq2PV'],
                                  BVtxChisq = config['BVtxChisq'],
                                  BVtxSepChisq = config['BVtxSepChisq'],
                                  BDiraPV = config['BDiraPV'],
                                  BZVtxSep = config['BZVtxSep'],
                                  BDZVtxSep = config['BDZVtxSep'],
                                  BDRPV = config['BDRPV'],      
                                  decayDesc = "[B0 -> D- K_1(1270)-]cc",
                                  parentB = "B0")

        # Make B- --> D*- (pipipi)-
        name6 = 'WSB2DStarPiPiPi' + name
        self.WSB2DStarPiPiPi = makeB2D3H(name6,
                                       dSel = self.selDStar,
                                       hhhSel = self.selPiPiPi,
                                       BMassWindow = config['BMassWindow'],
                                       BMinPT = config['BMinPT'],
                                       MinPT = config['MinPT'],
                                       BIP2PV = config['BIP2PV'],
                                       BIPChisq2PV = config['BIPChisq2PV'],
                                       BVtxChisq = config['BVtxChisq'],
                                       BVtxSepChisq = config['BVtxSepChisq'],
                                       BDiraPV = config['BDiraPV'],
                                       BZVtxSep = config['BZVtxSep'],
                                       BDZVtxSep = config['BDZVtxSep'],
                                       BDRPV = config['BDRPV'],      
                                       decayDesc = "[B0 -> D*(2010)- a_1(1260)-]cc",
                                       parentB = "B0")
        


        # Make B0 --> D*- (Kpipi)-
        name7 = 'WSB2DStarKPiPi' + name
        self.WSB2DStarKPiPi = makeB2D3H(name7,
                                      dSel = self.selDStar,
                                      hhhSel = self.selKPiPi,
                                      BMassWindow = config['BMassWindow'],
                                      BMinPT = config['BMinPT'],
                                      MinPT = config['MinPT'],
                                      BIP2PV = config['BIP2PV'],
                                      BIPChisq2PV = config['BIPChisq2PV'],
                                      BVtxChisq = config['BVtxChisq'],
                                      BVtxSepChisq = config['BVtxSepChisq'],
                                      BDiraPV = config['BDiraPV'],
                                      BZVtxSep = config['BZVtxSep'],
                                      BDZVtxSep = config['BDZVtxSep'],
                                      BDRPV = config['BDRPV'],      
                                      decayDesc = "[B0 -> D*(2010)- K_1(1270)-]cc",
                                      parentB = "B0")



        # Make B- --> D_(s)- D_(s)-
        name8 = 'WSB2DD' + name
        self.WSB2DD = makeB2D3H(name8,
                                   dSel = self.selDch,
                                   hhhSel = self.selDch,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   MinPT = config['MinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   BDRPV = config['BDRPV'],      
                                   decayDesc = "[B0 -> D- D-]cc",
                                   parentB = "B0")

        # Make B- --> D_(s)- D_(s)-
        name9 = 'WSB2DStarD' + name
        self.WSB2DStarD = makeB2D3H(name9,
                                   dSel = self.selDStar,
                                   hhhSel = self.selDch,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   MinPT = config['MinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   BDRPV = config['BDRPV'],      
                                   decayDesc = "[B0 -> D*(2010)- D-]cc",
                                   parentB = "B0")


        # Make LambdaB --> LambdaC+ (pipipi)+
        name9 = 'WSLambdaB2LambdaCPiPiPi' + name
        self.WSLambdaB2LambdaCPiPiPi = makeB2D3H(name9,
                                   dSel = self.selLambdaC,
                                   hhhSel = self.selPiPiPi,
                                   BMassWindow = config['BMassWindow'],
                                   BMinPT = config['BMinPT'],
                                   MinPT = config['MinPT'],
                                   BIP2PV = config['BIP2PV'],
                                   BIPChisq2PV = config['BIPChisq2PV'],
                                   BVtxChisq = config['BVtxChisq'],
                                   BVtxSepChisq = config['BVtxSepChisq'],
                                   BDiraPV = config['BDiraPV'],
                                   BZVtxSep = config['BZVtxSep'],
                                   BDZVtxSep = config['BDZVtxSep'],
                                   BDRPV = config['BDRPV'],      
                                   decayDesc = "[Lambda_b0 -> Lambda_c+ a_1(1260)+]cc",
                                   parentB = "Lambda_b0")


        # Make LambdaB --> LambdaC+ (Kpipi)+
        name10 = 'WSLambdaB2LambdaCKPiPi' + name
        self.WSLambdaB2LambdaCKPiPi = makeB2D3H(name10,
                                              dSel = self.selLambdaC,
                                              hhhSel = self.selKPiPi,
                                              BMassWindow = config['BMassWindow'],
                                              BMinPT = config['BMinPT'],
                                              MinPT = config['MinPT'],
                                              BIP2PV = config['BIP2PV'],
                                              BIPChisq2PV = config['BIPChisq2PV'],
                                              BVtxChisq = config['BVtxChisq'],
                                              BVtxSepChisq = config['BVtxSepChisq'],
                                              BDiraPV = config['BDiraPV'],
                                              BZVtxSep = config['BZVtxSep'],
                                              BDZVtxSep = config['BDZVtxSep'],
                                              BDRPV = config['BDRPV'],      
                                              decayDesc = "[Lambda_b0 -> Lambda_c+ K_1(1270)+]cc",
                                              parentB = "Lambda_b0")
        



        # Now create stripping lines...
        
        # These are for the full mass region with sidebands.
        # These lines should be prescaled forst, if necessary.

        #---------------------------
        # Right-Sign Stripping Lines
        #---------------------------

        self.StrippingAllB2D0PiPiPiLine = StrippingLine('AllB2D0PiPiPiLine'+name,
                                                     prescale = config['B2D0PiPiPiAll_Prescale'],
                                                     postscale = config['B2D0PiPiPiAll_Postscale'],
                                                     algos = [ self.EventFilter, self.B2D0PiPiPi]
                                                     )

        self.StrippingAllB2D0KPiPiLine = StrippingLine('AllB2D0KPiPiLine'+name,
                                                     prescale = config['B2D0KPiPiAll_Prescale'],
                                                     postscale = config['B2D0KPiPiAll_Postscale'],
                                                     algos = [ self.EventFilter, self.B2D0KPiPi]
                                                     )

        self.StrippingAllUnbiasedB2DPiPiPiLine = StrippingLine('AllUnbiasedB2DPiPiPiLine'+name,
                                                    prescale = config['UnbiasedB2DPiPiPiAll_Prescale'],
                                                    postscale = config['UnbiasedB2DPiPiPiAll_Postscale'],
                                                    algos = [ self.EventFilter, self.UnbiasedB2DPiPiPiHLT1HLT2TIS]
                                                    )

        self.StrippingAllB2DPiPiPiLine = StrippingLine('AllB2DPiPiPiLine'+name,
                                                    prescale = config['B2DPiPiPiAll_Prescale'],
                                                    postscale = config['B2DPiPiPiAll_Postscale'],
                                                    algos = [ self.EventFilter, self.B2DPiPiPi]
                                                    )

        self.StrippingAllB2DKPiPiLine = StrippingLine('AllB2DKPiPiLine'+name,
                                                   prescale = config['B2DKPiPiAll_Prescale'],
                                                   postscale = config['B2DKPiPiAll_Postscale'],
                                                   algos = [ self.EventFilter, self.B2DKPiPi]
                                                   )
        
        self.StrippingAllB2DStarPiPiPiLine = StrippingLine('AllB2DStarPiPiPiLine'+name,
                                                        prescale = config['B2DStarPiPiPiAll_Prescale'],
                                                        postscale = config['B2DStarPiPiPiAll_Postscale'],
                                                        algos = [ self.EventFilter, self.B2DStarPiPiPi]
                                                        )

        self.StrippingAllB2DStarKPiPiLine = StrippingLine('AllB2DStarKPiPiLine'+name,
                                                       prescale = config['B2DStarKPiPiAll_Prescale'],
                                                       postscale = config['B2DStarKPiPiAll_Postscale'],
                                                       algos = [ self.EventFilter, self.B2DStarKPiPi]
                                                       )

        self.StrippingAllB2DDLine = StrippingLine('AllB2DDLine'+name,
                                                    prescale = config['B2DDAll_Prescale'],
                                                    postscale = config['B2DDAll_Postscale'],
                                                    algos = [ self.EventFilter, self.B2DD]
                                                    )

        self.StrippingAllB2DStarDLine = StrippingLine('AllB2DStarDLine'+name,
                                                    prescale = config['B2DStarDAll_Prescale'],
                                                    postscale = config['B2DStarDAll_Postscale'],
                                                    algos = [ self.EventFilter, self.B2DStarD]
                                                    )

        self.StrippingAllLambdaB2LambdaCPiPiPiLine = StrippingLine('AllLambdaB2LambdaCPiPiPiLine'+name,
                                                    prescale = config['B2DPiPiPiAll_Prescale'],
                                                    postscale = config['B2DPiPiPiAll_Postscale'],
                                                    algos = [ self.EventFilter, self.LambdaB2LambdaCPiPiPi]
                                                    )

        self.StrippingAllLambdaB2LambdaCKPiPiLine = StrippingLine('AllLambdaB2LambdaCKPiPiLine'+name,
                                                    prescale = config['B2DKPiPiAll_Prescale'],
                                                    postscale = config['B2DKPiPiAll_Postscale'],
                                                    algos = [ self.EventFilter, self.LambdaB2LambdaCKPiPi]
                                                    )

        #---------------------------
        # Wrong-Sign Stripping Lines
        #---------------------------

        
        self.StrippingAllWSB2DPiPiPiLine = StrippingLine('AllWSB2DPiPiPiLine'+name,
                                                         prescale = config['WSB2D3H_Prescale'],
                                                         postscale = config['WSB2D3H_Postscale'],
                                                         algos = [ self.EventFilter, self.WSB2DPiPiPi]
                                                         )
        
        self.StrippingAllWSB2DKPiPiLine = StrippingLine('AllWSB2DKPiPiLine'+name,
                                                        prescale = config['WSB2D3H_Prescale'],
                                                        postscale = config['WSB2D3H_Postscale'],
                                                        algos = [ self.EventFilter, self.WSB2DKPiPi]
                                                        )
        
        self.StrippingAllWSB2DStarPiPiPiLine = StrippingLine('AllWSB2DStarPiPiPiLine'+name,
                                                             prescale = config['WSB2D3H_Prescale'],
                                                             postscale = config['WSB2D3H_Postscale'],
                                                             algos = [ self.EventFilter, self.WSB2DStarPiPiPi]
                                                             )
        
        self.StrippingAllWSB2DStarKPiPiLine = StrippingLine('AllWSB2DStarKPiPiLine'+name,
                                                            prescale = config['WSB2D3H_Prescale'],
                                                            postscale = config['WSB2D3H_Postscale'],
                                                            algos = [ self.EventFilter, self.WSB2DStarKPiPi]
                                                            )
        
        self.StrippingAllWSB2DDLine = StrippingLine('AllWSB2DDLine'+name,
                                                    prescale = config['WSB2D3H_Prescale'],
                                                    postscale = config['WSB2D3H_Postscale'],
                                                    algos = [ self.EventFilter, self.WSB2DD]
                                                    )
        
        self.StrippingAllWSB2DStarDLine = StrippingLine('AllWSB2DStarDLine'+name,
                                                        prescale = config['WSB2D3H_Prescale'],
                                                        postscale = config['WSB2D3H_Postscale'],
                                                        algos = [ self.EventFilter, self.WSB2DStarD]
                                                        )
        
        self.StrippingAllWSLambdaB2LambdaCPiPiPiLine = StrippingLine('AllWSLambdaB2LambdaCPiPiPiLine'+name,
                                                                     prescale = config['WSB2D3H_Prescale'],
                                                                     postscale = config['WSB2D3H_Postscale'],
                                                                     algos = [ self.EventFilter, self.WSLambdaB2LambdaCPiPiPi]
                                                                     )
        
        self.StrippingAllWSLambdaB2LambdaCKPiPiLine = StrippingLine('AllWSLambdaB2LambdaCKPiPiLine'+name,
                                                                    prescale = config['WSB2D3H_Prescale'],
                                                                    postscale = config['WSB2D3H_Postscale'],
                                                                    algos = [ self.EventFilter, self.WSLambdaB2LambdaCKPiPi]
                                                                    )

        
        # These are for the signal mass region with sidebands.
        # These lines should not be prescaled unless absolutely necessary
        # But, if prescales neeeded for future...


        self.StrippingSignalB2D0PiPiPiLine = StrippingLine('SignalB2D0PiPiPiLine'+name,
                                                     prescale = config['B2D0PiPiPiSignal_Prescale'],
                                                     postscale = config['B2D0PiPiPiSignal_Postscale'],
                                                     algos = [  self.EventFilter, self.SignalB2D0PiPiPi]
                                                     )

        self.StrippingSignalB2D0KPiPiLine = StrippingLine('SignalB2D0KPiPiLine'+name,
                                                     prescale = config['B2D0KPiPiSignal_Prescale'],
                                                     postscale = config['B2D0KPiPiSignal_Postscale'],
                                                     algos = [  self.EventFilter, self.SignalB2D0KPiPi]
                                                     )

        self.StrippingSignalB2DPiPiPiLine = StrippingLine('SignalB2DPiPiPiLine'+name,
                                                    prescale = config['B2DPiPiPiSignal_Prescale'],
                                                    postscale = config['B2DPiPiPiSignal_Postscale'],
                                                    algos = [  self.EventFilter, self.SignalB2DPiPiPi]
                                                    )

        self.StrippingSignalB2DKPiPiLine = StrippingLine('SignalB2DKPiPiLine'+name,
                                                   prescale = config['B2DKPiPiSignal_Prescale'],
                                                   postscale = config['B2DKPiPiSignal_Postscale'],
                                                   algos = [  self.EventFilter, self.SignalB2DKPiPi]
                                                   )
        
        self.StrippingSignalB2DStarPiPiPiLine = StrippingLine('SignalB2DStarPiPiPiLine'+name,
                                                        prescale = config['B2DStarPiPiPiSignal_Prescale'],
                                                        postscale = config['B2DStarPiPiPiSignal_Postscale'],
                                                        algos = [  self.EventFilter, self.SignalB2DStarPiPiPi]
                                                        )

        self.StrippingSignalB2DStarKPiPiLine = StrippingLine('SignalB2DStarKPiPiLine'+name,
                                                       prescale = config['B2DStarKPiPiSignal_Prescale'],
                                                       postscale = config['B2DStarKPiPiSignal_Postscale'],
                                                       algos = [  self.EventFilter, self.SignalB2DStarKPiPi]
                                                       )
        self.StrippingSignalB2DDLine = StrippingLine('SignalB2DDLine'+name,
                                                    prescale = config['B2DDAll_Prescale'],
                                                    postscale = config['B2DDAll_Postscale'],
                                                    algos = [ self.EventFilter, self.SignalB2DD]
                                                    )

        self.StrippingSignalB2DStarDLine = StrippingLine('SignalB2DStarDLine'+name,
                                                    prescale = config['B2DStarDSignal_Prescale'],
                                                    postscale = config['B2DStarDSignal_Postscale'],
                                                    algos = [ self.EventFilter, self.SignalB2DStarD]
                                                    )

        self.StrippingSignalLambdaB2LambdaCPiPiPiLine = StrippingLine('SignalLambdaB2LambdaCPiPiPiLine'+name,
                                                    prescale = config['B2DPiPiPiSignal_Prescale'],
                                                    postscale = config['B2DPiPiPiSignal_Postscale'],
                                                    algos = [  self.EventFilter, self.SignalLambdaB2LambdaCPiPiPi]
                                                    )

        self.StrippingSignalLambdaB2LambdaCKPiPiLine = StrippingLine('SignalLambdaB2LambdaCKPiPiLine'+name,
                                                    prescale = config['B2DKPiPiSignal_Prescale'],
                                                    postscale = config['B2DKPiPiSignal_Postscale'],
                                                    algos = [  self.EventFilter, self.SignalLambdaB2LambdaCKPiPi]
                                                    )



        self.lines = [ self.StrippingAllB2D0PiPiPiLine,
                       self.StrippingAllB2D0KPiPiLine,
                       self.StrippingAllUnbiasedB2DPiPiPiLine,
                       self.StrippingAllB2DPiPiPiLine,
                       self.StrippingAllB2DKPiPiLine,
                       self.StrippingAllB2DStarPiPiPiLine,
                       self.StrippingAllB2DStarKPiPiLine,
                       self.StrippingAllB2DDLine,
                       self.StrippingAllB2DStarDLine,
                       self.StrippingAllLambdaB2LambdaCPiPiPiLine,
                       self.StrippingAllLambdaB2LambdaCKPiPiLine,
                       self.StrippingAllWSB2DPiPiPiLine,
                       self.StrippingAllWSB2DKPiPiLine,
                       self.StrippingAllWSB2DStarPiPiPiLine,
                       self.StrippingAllWSB2DStarKPiPiLine,
                       self.StrippingAllWSB2DDLine,
                       self.StrippingAllWSB2DStarDLine,
                       self.StrippingAllWSLambdaB2LambdaCPiPiPiLine,
                       self.StrippingAllWSLambdaB2LambdaCKPiPiLine,                      
                       self.StrippingSignalB2D0PiPiPiLine,
                       self.StrippingSignalB2D0KPiPiLine,
                       self.StrippingSignalB2DPiPiPiLine,
                       self.StrippingSignalB2DKPiPiLine,
                       self.StrippingSignalB2DStarPiPiPiLine,
                       self.StrippingSignalB2DStarKPiPiLine,            
                       self.StrippingSignalB2DDLine,
                       self.StrippingSignalB2DStarDLine,
                       self.StrippingSignalLambdaB2LambdaCPiPiPiLine,
                       self.StrippingSignalLambdaB2LambdaCKPiPiLine
                       ]


def MyEventFilter(name, MaxTracks):
    """
    Create and return EventFilter object by Filtering on Ntracks
    #
    Arguments:
    name                  : name of the Selection.
    PionMinP              : Pion minimum momentum
    """
    from Configurables import LoKi__VoidFilter as VoidFilter
    from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
    modules = CoreFactory('CoreFactory').Modules
    for i in ['LoKiTrigger.decorators']:
      if i not in modules : modules.append(i)


    _eventFilter = VoidFilter("EventfilterFor"+name)
    _eventFilter.Code =  "TrSOURCE('Rec/Track/Best', TrLONG) >> (TrSIZE < %(MaxTracks)s )" %locals()

    return _eventFilter

def makeTISTOSSel(name, sel, trigger ) : 
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name + "Filter")
    tisTosFilter.TisTosSpecs = { trigger : 0}
    # the rest ist only to speed it up... (TISTOSsing only done with tracking system)
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = { 4:0.0, 5:0.0 }

    return Selection(name, Algorithm = tisTosFilter, RequiredSelections = [ sel ] )

def makePions(name, PionMinP, PionMaxP, PionMinPT, PionMinIPChisq, PionPiDDLL, TrkChisq):

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
    _pionFilter.Code = "( (P > %(PionMinP)s *MeV) & (P < %(PionMaxP)s *MeV) & (PT> %(PionMinPT)s *MeV) & (PIDK < %(PionPiDDLL)s )" \
                       "& (MIPCHI2DV(PRIMARY)> %(PionMinIPChisq)s )  ) " %locals()
    return Selection (name,
                      Algorithm = _pionFilter,
                      RequiredSelections = [_stdLoosePions])

def makeTightPions(name, PionMinP, PionMaxP, PionMinPT, PionMinIPChisq, TrkChisq):

    """
    Create and return Pion Selection object by Filterinf StdTightPions
    #
    Arguments:
    name                  : name of the Selection.
    PionMinP              : Pion minimum momentum
    PionMaxP              : Pion maximum momentum
    PionMinPT             : Pion minimum pT
    PionMinIPChisq        : Pion minimum IP Chisq
    TrkChisq              : Cut on Track Chisq/DOF
    """

    _stdLoosePions = DataOnDemand(Location = "Phys/StdTightPions")
    _pionFilter = FilterDesktop("_filterFor"+name)
    _pionFilter.Code = "( (P > %(PionMinP)s *MeV) & (P < %(PionMaxP)s *MeV) & (PT> %(PionMinPT)s *MeV) " \
                       "& (MIPCHI2DV(PRIMARY)> %(PionMinIPChisq)s )  ) " %locals()
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
    _kaonFilter.Code = "( (P > %(KaonMinP)s *MeV) & (P < %(KaonMaxP)s *MeV) & (PT> %(KaonMinPT)s *MeV) " \
                       "& (MIPDV(PRIMARY)>0.045*mm)  & (MIPCHI2DV(PRIMARY)> %(KaonMinIPChisq)s )  ) " %locals()
    return Selection (name,
                      Algorithm = _kaonFilter,
                      RequiredSelections = [_stdLooseKaons])

def makeTightKaons(name, KaonMinP, KaonMaxP, KaonMinPT, KaonMinIPChisq, TrkChisq):

    """
    Create and return Kaon Selection object by Filtering StdTightKaons
    #
    Arguments:
    name                  : name of the Selection.
    KaonMinP              : Kaon minimum momentum
    KaonMaxP              : Kaon maximum momentum
    KaonMinPT             : Kaon minimum pT
    KaonMinIPChisq        : Kaon minimum IP Chisq
    TrkChisq              : Cut on Track Chisq/DOF
    """

    _stdLooseKaons = DataOnDemand(Location = "Phys/StdTightKaons")
    _kaonFilter = FilterDesktop("_filterFor"+name)
    _kaonFilter.Code = "( (P > %(KaonMinP)s *MeV) & (P < %(KaonMaxP)s *MeV) & (PT> %(KaonMinPT)s *MeV) " \
                       "& (MIPDV(PRIMARY)>0.045*mm)  & (MIPCHI2DV(PRIMARY)> %(KaonMinIPChisq)s )  ) " %locals()
    return Selection (name,
                      Algorithm = _kaonFilter,
                      RequiredSelections = [_stdLooseKaons])

def makeProtons(name, ProtonMinP, ProtonMaxP, ProtonMinPT, ProtonMinIPChisq, TrkChisq):

    """
    Create and return Kaon Selection object by Filtering StdLooseKaons
    #
    Arguments:
    name                  : name of the Selection.
    ProtonMinP              : Proton minimum momentum
    ProtonMaxP              : Proton maximum momentum
    ProtonMinPT             : Proton minimum pT
    ProtonMinIPChisq        : Proton minimum IP Chisq
    TrkChisq              : Cut on Track Chisq/DOF
    """

    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons")
    _protonFilter = FilterDesktop("_filterFor"+name)
    _protonFilter.Code = "( (P > %(ProtonMinP)s *MeV) & (P < %(ProtonMaxP)s *MeV) & (PT> %(ProtonMinPT)s *MeV) " \
                       "& (MIPDV(PRIMARY)>0.045*mm)  & (MIPCHI2DV(PRIMARY)> %(ProtonMinIPChisq)s )  ) " %locals()
    return Selection (name,
                      Algorithm = _protonFilter,
                      RequiredSelections = [_stdLooseProtons])


def makePiPiPi( name,
                pionSel,
                MinPT,
                Bach3HMassWindow,
                Bach3HDocaMax, 
                Bach3HMinPT, 
                Bach3HIP2PV, 
                Bach3HIPChisq2PV, 
                Bach3HVtxChisq, 
                Bach3HVtxSepChisq, 
                Bach3HDiraPV, 
                Bach3HZVtxSep,
                Bach3HDRPV
                ):

    """
    Create and return PiPiPi Selection object.
    #
    Arguments:
    name                  : name of the Selection.
    pionSel               : Pion selection object
    MinPT                 : One track allowed to have pT less than this value
    Bach3HMassWindow      : Mass Window
    Bach3HDocaMax         : Maximum value for DocaMax
    Bach3HMinPT           : Minimum pT of Candidate
    Bach3HIP2PV           : Minimum IP to PV 
    Bach3HIPChisq2PV      : Minimum IP Chisq to PC
    Bach3HVtxChisq        : Maximum Vertex Chisquare
    Bach3HVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    Bach3HDiraPV          : Minimum direction angle value
    Bach3HZVtxSep         : Minimum vertex separation from PV
    Bach3HDRPV            : Minimum DR vertex separation from PV
    """

    _a1Alg = CombineParticles(name)
    _a1Alg.DecayDescriptor = "[a_1(1260)+ -> pi+ pi- pi+]cc" 
    _a1Alg.CombinationCut = "( (AM < %(Bach3HMassWindow)s *MeV) & (APT > %(Bach3HMinPT)s *MeV)" \
                           " & (ANUM(PT < %(MinPT)s *MeV) <= 1) & (ACUTDOCA( %(Bach3HDocaMax)s *mm, '')) ) " %locals()
    _a1Alg.MotherCut = "( (PT > %(Bach3HMinPT)s *MeV) & (VFASPF(VCHI2/VDOF)< %(Bach3HVtxChisq)s )" \
                       " & (BPVVDCHI2 > %(Bach3HVtxSepChisq)s ) & (MIPCHI2DV(PRIMARY) > %(Bach3HIPChisq2PV)s )" \
                       " & (BPVDIRA > %(Bach3HDiraPV)s ) & (BPVVDZ> %(Bach3HZVtxSep)s *mm) & (BPVVDRHO > %(Bach3HDRPV)s *mm) " \
                       " & (MIPDV(PRIMARY)> %(Bach3HIP2PV)s *mm))" % locals()

    selName = 'Sel'+name    
    PiPiPiSelection = Selection(selName,
                                Algorithm = _a1Alg,
                                RequiredSelections = [pionSel])
    return PiPiPiSelection

def makeKPiPi( name,
               pionSel,
               kaonSel,
               MinPT,
               Bach3HMassWindow, 
               Bach3HDocaMax, 
               Bach3HMinPT, 
               Bach3HIP2PV, 
               Bach3HIPChisq2PV, 
               Bach3HVtxChisq, 
               Bach3HVtxSepChisq, 
               Bach3HDiraPV, 
               Bach3HZVtxSep,
               Bach3HDRPV
               ):

    """
    Create and return KPiPi Selection object.
    #
    Arguments:
    name                  : name of the Selection.
    pionSel               : Pion selection object
    kaonSel               : Kaon selection object
    MinPT                 : One track allowed to have pT less than this value
    Bach3HMassWindow      : Mass Window
    Bach3HDocaMax         : Maximum value for DocaMax
    Bach3HMinPT           : Minimum pT of Candidate
    Bach3HIP2PV           : Minimum IP to PV 
    Bach3HIPChisq2PV      : Minimum IP Chisq to PC
    Bach3HVtxChisq        : Maximum Vertex Chisquare
    Bach3HVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    Bach3HDiraPV          : Minimum direction angle value
    Bach3HZVtxSep         : Minimum vertex separation from PC
    Bach3HDRPV            : Minimum DR vertex separation from PV
    """

    
    _k1Alg = CombineParticles(name)
    _k1Alg.DecayDescriptor = "[K_1(1270)+ -> K+ pi- pi+]cc" 
    _k1Alg.CombinationCut = "( (AM < %(Bach3HMassWindow)s *MeV) & (APT > %(Bach3HMinPT)s  *MeV)" \
                           " & (ANUM(PT < %(MinPT)s *MeV) <= 1) & (ACUTDOCA( %(Bach3HDocaMax)s *mm, '')) ) " %locals()
    _k1Alg.MotherCut = "( (PT > %(Bach3HMinPT)s *MeV) & (VFASPF(VCHI2/VDOF)< %(Bach3HVtxChisq)s )  " \
                       " & (BPVVDCHI2 > %(Bach3HVtxSepChisq)s ) & (MIPCHI2DV(PRIMARY) > %(Bach3HIPChisq2PV)s )" \
                       " & (BPVDIRA > %(Bach3HDiraPV)s ) & (BPVVDZ> %(Bach3HZVtxSep)s *mm) & (BPVVDRHO > %(Bach3HDRPV)s *mm) " \
                       " & (MIPDV(PRIMARY)> %(Bach3HIP2PV)s *mm))" % locals()

    selName = 'Sel'+name
    KPiPiSelection = Selection(selName,
                               Algorithm = _k1Alg,
                               RequiredSelections = [pionSel, kaonSel]) 
    return KPiPiSelection



def makeD0Meson(name,
                pionSel,
                kaonSel,
                DMassWindow, 
                DDocaMax,   
                DMinPT,
                MinPT, 
                DIP2PV, 
                DIPChisq2PV, 
                DVtxChisq, 
                DVtxSepChisq, 
                DDiraPV, 
                DZVtxSep,
                DDRPV
                ):
    """
    Create and return D0 Selection object.
    Select the follwing charged D decays:
    D0  --> K-Pi, K+Pi-(DCS), K+K- and Pi+Pi-, filtered from StdLooseD02XX
    #
    Arguments:
    name             : name of the Selection.
    pionSel          : pion Selection object
    kaonSel          : kaon Selection object
    DMassWindow      : D/Ds Mass Window
    DDocaMax         : Maximum value for Doca
    DMinPT           : Minimum pT of Candidate
    DMinPT           : Minimum pT of Candidate
    DIP2PV           : Minimum IP to PV 
    DIPChisq2PV      : Minimum IP Chisq to PC
    DVtxChisq        : Maximum Vertex Chisquare
    DVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    DDiraPV          : Minimum direction angle value
    DZVtxSep         : Minimum vertex separation from PC
    DDRPV            : Minimum DR vertex separation from PV
    """


    _dMassWindow = "(ADAMASS('D0') < %(DMassWindow)s *MeV ) " %locals()

    _dFilterCode = "((VFASPF(VCHI2/VDOF)< %(DVtxChisq)s ) & (PT > %(DMinPT)s *MeV) " \
                    " & (BPVVDZ> %(DZVtxSep)s *mm) & (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s )" \
                    "& (BPVVDCHI2 > %(DVtxSepChisq)s ) & (BPVDIRA > %(DDiraPV)s ) & (BPVVDRHO > %(DDRPV)s *mm)"   \
                    "& (MIPDV(PRIMARY)> %(DIP2PV)s *mm)) "  %locals()
    
    # D0 -->Pi- Pi+
    _d2pipi = CombineParticles("PiPi"+name)
    _d2pipi.DecayDescriptors = [ "D0 -> pi- pi+" ]
    dauCut = "PT > %(MinPT)s *MeV" %locals()
    _d2pipi.DaughtersCuts =  { "pi+"      : dauCut,
                               "K+"       : dauCut }
                                  
    
    _d2pipi.CombinationCut = "(" + _dMassWindow + " & (APT > %(DMinPT)s  *MeV)" \
                              " & ACUTDOCA( %(DDocaMax)s *mm, '' )  )" %locals()
    _d2pipi.MotherCut = _dFilterCode %locals()    
    D2PiPiSelection = Selection("D2PiPiSel"+name, Algorithm = _d2pipi, RequiredSelections = [pionSel]) 

    # D0 -->K- Pi+ 
    _d2kpi = _d2pipi.clone("KPi"+name, DecayDescriptors = [ "[D0 -> K- pi+]cc" ] )
    D2KPiSelection = Selection("D2KPiSel"+name, Algorithm = _d2kpi, RequiredSelections = [ pionSel, kaonSel ] )

    # D0 -->K- K+
    _d2kk = _d2pipi.clone("KK"+name, DecayDescriptors = [ "D0 -> K- K+" ] )
    D2KKSelection = Selection("D2KKSel"+name, Algorithm = _d2kk, RequiredSelections = [ kaonSel ] )

    # D0 -->K- Pi+ 
    _d2kpiDCS = _d2pipi.clone("KPiDCS"+name, DecayDescriptors = [ "[D0 -> K+ pi-]cc" ] )
    D2KPiDCSSelection = Selection("D2KPiDCSSel"+name, Algorithm = _d2kpiDCS, RequiredSelections = [ pionSel, kaonSel ] )

    selName = "MergedSel" + name
    _d0m = MergedSelection(selName, RequiredSelections = [D2PiPiSelection, D2KPiSelection, D2KKSelection, D2KPiDCSSelection] )    

    return _d0m
    

def makeDStarMeson(name,
                   dSel,
                   pionSel,
                   DStarMassWindow, 
                   DDocaMax,   
                   DStarMinPT, 
                   DIP2PV, 
                   DIPChisq2PV, 
                   DVtxChisq, 
                   DVtxSepChisq, 
                   DDiraPV, 
                   DZVtxSep,
                   DDRPV
                   ):
    """
    Create and return D0 Selection object.
    Select the follwing charged D decays:
    D0  --> K-Pi, K+Pi-(DCS), K+K- and Pi+Pi-, filtered from StdLooseD02XX
    #
    Arguments:
    name             : name of the Selection.
    dSel             : D0 selection object
    pionSel          : pion Selection object
    DStarMassWindow  : Mass Window 
    DDocaMax         : Maximum value for Doca
    DStarMinPT       : Minimum pT of Candidate
    DIP2PV           : Minimum IP to PV 
    DIPChisq2PV      : Minimum IP Chisq to PC
    DVtxChisq        : Maximum Vertex Chisquare
    DVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    DDiraPV          : Minimum direction angle value
    DZVtxSep         : Minimum vertex separation from PC
    DDRPV            : Minimum DR vertex separation from PV
    """


    
    _dMassWindow = "(ADAMASS('D*(2010)+') < %(DStarMassWindow)s *MeV ) " %locals()
    _dFilterCode = "((VFASPF(VCHI2/VDOF)< %(DVtxChisq)s ) & (PT > %(DStarMinPT)s *MeV) " \
                   " & (BPVVDZ> %(DZVtxSep)s *mm) & (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s )" \
                   "& (BPVVDCHI2 > %(DVtxSepChisq)s ) & (BPVDIRA > %(DDiraPV)s ) & (BPVVDRHO > %(DDRPV)s *mm)"   \
                   "& (MIPDV(PRIMARY)> %(DIP2PV)s *mm)) "  %locals()


    _dstar = CombineParticles ( name )
    _dstar.DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc" 
    _dstar.CombinationCut = "(" + _dMassWindow + " & (APT > %(DStarMinPT)s  *MeV)" \
                              " & ACUTDOCA( %(DDocaMax)s *mm, '' ) )" %locals()
    _dstar.MotherCut = _dFilterCode %locals()    
    DStarSelection = Selection("Sel"+name, Algorithm = _dstar, RequiredSelections = [pionSel, dSel]) 

    return DStarSelection


def makeDMeson(name,
               pionSel,
               kaonSel,
               DMassWindow,
               DsMassWindow, 
               DDocaMax,   
               DMinPT, 
               MinPT,
               DIP2PV, 
               DIPChisq2PV, 
               DVtxChisq, 
               DVtxSepChisq, 
               DDiraPV, 
               DZVtxSep,
               DDRPV
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
    pionSel          : pion Selection object
    kaonSel          : kaon Selection object
    DMassWindow      : D/Ds Mass Window
    DDocaMax         : Maximum value for Doca               
    DMinPT           : Minimum pT of Candidate
    MinPT            : One track allowed to have pT less than this value
    DIP2PV           : Minimum IP to PV 
    DIPChisq2PV      : Minimum IP Chisq to PC
    DVtxChisq        : Maximum Vertex Chisquare
    DVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    DDiraPV          : Minimum direction angle value
    DZVtxSep         : Minimum vertex separation from PC
    DDRPV            : Minimum DR vertex separation from PV
    """

    _dMassWindow = "(ADAMASS('D+') < %(DMassWindow)s *MeV ) " %locals()
    _dsMassWindow = "(ADAMASS('D_s+') < %(DsMassWindow)s *MeV ) "  %locals()
    _danddsMassWindow = "( (ADAMASS('D+') < %(DMassWindow)s *MeV) | (ADAMASS('D_s+') < %(DsMassWindow)s *MeV) )"  %locals()

    _dFilterCode = "((VFASPF(VCHI2/VDOF)< %(DVtxChisq)s ) & (PT > %(DMinPT)s *MeV) " \
                    " & (BPVVDZ> %(DZVtxSep)s *mm) & (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s )" \
                    "& (BPVVDCHI2 > %(DVtxSepChisq)s ) & (BPVDIRA > %(DDiraPV)s ) & (BPVVDRHO > %(DDRPV)s *mm)"   \
                    "& (MIPDV(PRIMARY)> %(DIP2PV)s *mm)) "  %locals()
    
    # D+ -->K- Pi+ Pi+
    name2 = "KPiPi"+name
    _d2kpipi = CombineParticles(name2)
    dauCut = "PT > %(MinPT)s *MeV" %locals()
    _d2kpipi.DaughtersCuts =  { "pi+"      : dauCut,
                               "K+"       : dauCut }
                                  
    _d2kpipi.DecayDescriptor = "[D+ -> K- pi+ pi+]cc" 
    _d2kpipi.CombinationCut = "(" + _dMassWindow + " & (APT > %(DMinPT)s  *MeV) " \
                              " & (ANUM(PT < %(MinPT)s *MeV) <= 1) & ACUTDOCA( %(DDocaMax)s *mm, '') )" %locals()
    _d2kpipi.MotherCut = _dFilterCode %locals()    
    D2KPiPiSelection = Selection("D2KPiPiSel"+name, Algorithm = _d2kpipi, RequiredSelections = [pionSel, kaonSel]) 


    # D+ -->K- K+ Pi+ OR D_s+ -->K- K+ Pi+   
    _d2kkpi = _d2kpipi.clone("KKPi"+name, DecayDescriptors = [ "[D+ -> K- K+ pi+]cc" ])
    _d2kkpi.CombinationCut = "(" + _danddsMassWindow + " & (APT > %(DMinPT)s  *MeV) " \
                              " & (ANUM(PT < %(MinPT)s *MeV) <= 1) & ACUTDOCA( %(DDocaMax)s *mm, '') )" %locals()
    D2KKPiSelection = Selection("D2KKPiSel"+name, Algorithm = _d2kkpi, RequiredSelections = [pionSel, kaonSel])
    

    # D+ -->Pi- Pi+ Pi+ OR D_s+ -->Pi- Pi+ Pi+       
    _d2pipipi = _d2kkpi.clone("PiPiPi"+name, DecayDescriptors = [ "[D+ -> pi- pi+ pi+]cc" ])
    _d2pipipi.CombinationCut = "(" + _danddsMassWindow + " & (APT > %(DMinPT)s  *MeV) " \
                              " & (ANUM(PT < %(MinPT)s *MeV) <= 1) & ACUTDOCA( %(DDocaMax)s *mm, '') )" %locals()
    D2PiPiPiSelection = Selection("D2PiPiPiSel"+name, Algorithm = _d2pipipi, RequiredSelections = [pionSel])

    # D_s+ -->K+ Pi- Pi+   
    _d2kpipiOppSignPi = _d2kpipi.clone("KKPiOppSignPi"+name, DecayDescriptors = [ "[D+ -> K+ pi- pi+]cc" ])
    _d2kpipiOppSignPi.CombinationCut = "(" + _dsMassWindow + " & (APT > %(DMinPT)s  *MeV)" \
                                       " & (ANUM(PT < %(MinPT)s *MeV) <= 1) & ACUTDOCA( %(DDocaMax)s *mm, '' ) )" %locals()

    D2KPiPiOppSignPiSelection = Selection("D2KPiPiOppSignPiSel"+name, Algorithm = _d2kpipiOppSignPi, RequiredSelections = [pionSel, kaonSel])


    selName = 'MergedSel'+name
    _dm = MergedSelection(selName, RequiredSelections = [ D2KPiPiSelection, D2KKPiSelection, D2PiPiPiSelection, D2KPiPiOppSignPiSelection])
    return _dm

def makeLambdaC(name,
                pionSel,
                kaonSel,
                protonSel,
                DMassWindow,
                DDocaMax,   
                DMinPT, 
                MinPT,
                DIP2PV, 
                DIPChisq2PV, 
                DVtxChisq, 
                DVtxSepChisq, 
                DDiraPV, 
                DZVtxSep,
                DDRPV
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
    pionSel          : pion Selection object
    kaonSel          : kaon Selection object
    protonSel          : proton Selection object
    DMassWindow      : D/Ds Mass Window
    DDocaMax         : Maximum value for Doca               
    DMinPT           : Minimum pT of Candidate
    MinPT            : One track allowed to have pT less than this value
    DIP2PV           : Minimum IP to PV 
    DIPChisq2PV      : Minimum IP Chisq to PC
    DVtxChisq        : Maximum Vertex Chisquare
    DVtxSepChisq,    : Minimum separation chisq. between cand and assoc. PV
    DDiraPV          : Minimum direction angle value
    DZVtxSep         : Minimum vertex separation from PC
    DDRPV            : Minimum DR vertex separation from PV
    """

    _lambdaMassWindow = "(ADAMASS('Lambda_c+') < %(DMassWindow)s *MeV ) " %locals()
    _lambdaFilterCode = "((VFASPF(VCHI2/VDOF)< %(DVtxChisq)s ) & (PT > %(DMinPT)s *MeV) " \
                    " & (BPVVDZ> %(DZVtxSep)s *mm) & (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s )" \
                    "& (BPVVDCHI2 > %(DVtxSepChisq)s ) & (BPVDIRA > %(DDiraPV)s ) & (BPVVDRHO > %(DDRPV)s *mm)"   \
                    "& (MIPDV(PRIMARY)> %(DIP2PV)s *mm)) "  %locals()
    
    # Lambda_C+ -->P+ K- Pi+ 
    name2 = "PKPi"+name
    _lambdac2pkpi = CombineParticles(name2)
    dauCut = "PT > %(MinPT)s *MeV" %locals()
    _lambdac2pkpi.DaughtersCuts =  { "pi+"      : dauCut,
                                     "K+"       : dauCut ,
                                     "p+"       : dauCut }
                                  
    _lambdac2pkpi.DecayDescriptor = "[Lambda_c+ -> p+ K- pi+]cc" 
    _lambdac2pkpi.CombinationCut = "(" + _lambdaMassWindow + " & (APT > %(DMinPT)s  *MeV) & (ANUM(PT < %(MinPT)s *MeV) <= 1) & ACUTDOCA( %(DDocaMax)s *mm, '') )" %locals()
    _lambdac2pkpi.MotherCut = _lambdaFilterCode %locals()    
    LambdaC2PKPiSelection = Selection("LambdaC2PKPiSel"+name, Algorithm = _lambdac2pkpi, RequiredSelections = [pionSel, kaonSel, protonSel]) 
    

    return LambdaC2PKPiSelection


def makeB2D3H( name,
               dSel,
               hhhSel,
               BMassWindow, 
               BMinPT,
               MinPT, 
               BIP2PV, 
               BIPChisq2PV, 
               BVtxChisq, 
               BVtxSepChisq, 
               BDiraPV, 
               BZVtxSep,
               BDZVtxSep,
               BDRPV,
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
    MinPT            : One track allowed to have pT less than this value
    BIP2PV           : Maximum IP to PV 
    BIPChisq2PV      : Maximum IP Chisq to PC
    BVtxChisq        : Maximum Vertex Chisquare
    BVtxSepChisq,    : Minimum separation chisq. between candidate and assoc. PV
    BDiraPV          : Minimum direction angle value
    BZVtxSep         : Minimum vertex separation from PC
    BDZVtxSep        : Minimum vertex separation from PC
    DDRPV            : Minimum DR vertex separation from PV
    decayDesc        : Decay descriptor for CombineParticles
    parentB          : parent B (use either B0 or B+)
    """
    
    _b2d3h = CombineParticles('_'+name)
    _b2d3h.DecayDescriptor = decayDesc
    _b2d3h.Preambulo = [ "Z  = VFASPF(VZ)"   ,  
                        "DZ = CHILD(Z,1)-Z" 
                         ]

    combCut = "(ADAMASS('" + parentB + "') < %(BMassWindow)s *MeV) & (APT > %(BMinPT)s *MeV) " 

    if parentB == "B0":
        combCut = "( ((ADAMASS('B0') < %(BMassWindow)s *MeV) | (ADAMASS('B_s0') < %(BMassWindow)s *MeV))" \
                  "& (APT > %(BMinPT)s *MeV) )" 
    _b2d3h.CombinationCut = combCut  % locals()

    _b2d3h.MotherCut = "( (VFASPF(VCHI2/VDOF)< %(BVtxChisq)s ) & (PT > %(BMinPT)s *MeV) " \
                       " & (DZ > %(BDZVtxSep)s *mm) & (NINGENERATION( (PT < %(MinPT)s *MeV) , 2) <= 1)" \
                       " & (BPVVDZ> %(BZVtxSep)s *mm) & (BPVVDRHO > %(BDRPV)s *mm) & (MIPCHI2DV(PRIMARY)< %(BIPChisq2PV)s )" \
                       " & (BPVVDCHI2 > %(BVtxSepChisq)s ) & (BPVDIRA > %(BDiraPV)s )"   \
                       " & (MIPDV(PRIMARY)< %(BIP2PV)s *mm) )" % locals()

    selName = 'Sel'+name
    if dSel == hhhSel:
        return Selection(name, Algorithm  = _b2d3h, RequiredSelections = [dSel])
    else:
        return Selection(name, Algorithm  = _b2d3h, RequiredSelections = [dSel, hhhSel])


def filterB2D3H( name,
                 bSel,
                 tightDMassWindow,
                 tightBMassWindow,
                 parentB,
                 parentD):
    
    """
    Filter the B-->D+3H Selection object for events in a tight signal region
    Arguments:
    name             : name of the Selection.
    bSel             : B Selection object
    tightDMassWindow : Tight D Mass Window
    parentB          : parent B (use either B0 or B+)

    Several final states (FS) considered here for parentD
    [1] FS = B^+ --> D^0 3h
    [2] FS = B0 or Bs --> (D+ or D_s+) 3h
    [3] FS = B0 or Bs --> D*+ 3h
    [4] FS = B0 or Bs --> (D+ or D_s+) + (D- or D_s-)
    [5] FS = B0 or Bs --> D*+ (D- or D_s-)


    """

    _dMassCut = " "
    _bMassCut = " "
    
    if parentD == 1:
        _dMassCut = "(CHILDCUT( (ADMASS('D0') <%(tightDMassWindow)s *MeV ), 1 ))" %locals()
        _bMassCut = "( ADMASS('" + parentB + "') < %(tightBMassWindow)s *MeV )"  %locals()
    if parentD==2:
        _dMassCut1 = "(CHILDCUT( (ABSID=='D+') & (ADMASS('D+')   < %(tightDMassWindow)s *MeV ), 1 ))" %locals()
        _dMassCut2 = "(CHILDCUT( (ABSID=='D+') & (ADMASS('D_s+') < %(tightDMassWindow)s *MeV ), 1 ))" %locals()
        _dMassCut = "(" + _dMassCut1 + " | " + _dMassCut2 + ")"
        _bMassCut = "( (ADMASS('B0') < %(tightBMassWindow)s *MeV) | (ADMASS('B_s0') < %(tightBMassWindow)s *MeV) )"  %locals()        
    if parentD==3:
        _dMassCut = "(CHILDCUT( (ABSID=='D*(2010)+') & (ADMASS('D*(2010)+')   < %(tightDMassWindow)s *MeV ), 1 ))" %locals()
        _bMassCut = "( (ADMASS('B0') < %(tightBMassWindow)s *MeV) | (ADMASS('B_s0') < %(tightBMassWindow)s *MeV) )"  %locals()        
    if parentD==4:
        _dMassCut1 = "(CHILDCUT( (ABSID=='D+') & (ADMASS('D+')   < %(tightDMassWindow)s *MeV ), 1 ))" %locals()
        _dMassCut2 = "(CHILDCUT( (ABSID=='D+') & (ADMASS('D_s+') < %(tightDMassWindow)s *MeV ), 1 ))" %locals()
        _dMassCut3 = "(CHILDCUT( (ABSID=='D+') & (ADMASS('D+')   < %(tightDMassWindow)s *MeV ), 2 ))" %locals()
        _dMassCut4 = "(CHILDCUT( (ABSID=='D+') & (ADMASS('D_s+') < %(tightDMassWindow)s *MeV ), 2 ))" %locals()
        _dMassCut = "( (" + _dMassCut1 + " | " + _dMassCut2 + ") &  (" + _dMassCut3 + " | " + _dMassCut4 + ") )"     
        _bMassCut = "( (ADMASS('B0') < %(tightBMassWindow)s *MeV) | (ADMASS('B_s0') < %(tightBMassWindow)s *MeV) )"  %locals()        
    if parentD==5:
        _dMassCut1 = "(CHILDCUT( (ABSID=='D*(2010)+') & (ADMASS('D*(2010)+')   < %(tightDMassWindow)s *MeV ), 1 ))" %locals()
        _dMassCut3 = "(CHILDCUT( (ABSID=='D+') & (ADMASS('D+')   < %(tightDMassWindow)s *MeV ), 2 ))" %locals()
        _dMassCut4 = "(CHILDCUT( (ABSID=='D+') & (ADMASS('D_s+') < %(tightDMassWindow)s *MeV ), 2 ))" %locals()
        _dMassCut = "( (" + _dMassCut1 + ") &  (" + _dMassCut3 + " | " + _dMassCut4 + ") )"     
        _bMassCut = "( (ADMASS('B0') < %(tightBMassWindow)s *MeV) | (ADMASS('B_s0') < %(tightBMassWindow)s *MeV) )"  %locals()        
    if parentD==6:
        _dMassCut = "(CHILDCUT( (ABSID=='Lambda_c+') & (ADMASS('Lambda_c+') < %(tightDMassWindow)s *MeV ), 1 ))" %locals()
        _bMassCut = "( (ADMASS('Lambda_b0') < %(tightBMassWindow)s *MeV) )"  %locals()            
    

    _bFilter = FilterDesktop('SignalFilter'+name)
    _bFilter.Code = "(" + _dMassCut + " & " + _bMassCut + ")" 

    return Selection ('SignalSel'+name, Algorithm = _bFilter, RequiredSelections = [bSel])    


def StrippingB2D3HNominal(name="Def") :
    from StrippingSelections.StrippingB2D3H import StrippingB2D3HConf as conf
    config_params =  {
        "PionMinP"             : 2000.,
        "PionMaxP"             : 500000.,
        "PionMinPT"            : 200.,
        "PionMinPTtight"       : 500.,
        "PionMinIPChisq"       : 6.5,
        "PionPiDDLL"           : 10,
        "KaonMinP"             : 2000.,
        "KaonMaxP"             : 500000.,
        "KaonMinPT"            : 200.,
        "KaonMinPTtight"       : 500.,
        "KaonMinIPChisq"       : 6.5,
        "ProtonMinP"             : 2000.,
        "ProtonMaxP"             : 500000.,
        "ProtonMinPT"            : 200.,
        "ProtonMinIPChisq"       : 6.5,       
        "MinPT"                : 300.,
        "TrkChisq"             : 6.0,
        "TrkChisqtight"        : 5.0,
        "Bach3HMassWindow"     : 2500,
        "Bach3HDocaMax"        : 0.35,
        "Bach3HVtxChisq"       : 7.0,
        "Bach3HMinPT"          : 1000.0, 
        "Bach3HIP2PV"          : 0.075,
        "Bach3HIPChisq2PV"     : 11.0,
        "Bach3HVtxSepChisq"    : 36.0,
        "Bach3HDiraPV"         : 0.985,
        "Bach3HZVtxSep"        : 2.0,
        "Bach3HDRPV"           : 0.15,
        "DMinPT"               : 1200,
        "DVtxChisq"            : 7.0,
        "DMassWindow"          : 100,
        "D0MassWindow"         : 100,
        "DsMassWindow"         : 100,
        "tightDMassWindow"     : 40,
        "DDocaMax"             : 0.35,
        "DIP2PV"               : 0.070,
        "DIPChisq2PV"          : 10.0,
        "DVtxSepChisq"         : 25.0,
        "DDiraPV"              : 0.992,
        "DZVtxSep"             : 2.0,
        "BDRPV"                : 0.15,
        "DStarMassWindow"      : 40,
        "DStarMinPT"           : 1250,
        "BMinPT"               : 0.0, 
        "BVtxChisq"            : 7.0,
        "BMassWindow"          : 250, 
        "tightBMassWindow"     : 150,
        "BIP2PV"               : 0.10,
        "BIPChisq2PV"          : 36.0,
        "BVtxSepChisq"         : 25.0,
        "BDiraPV"              : 0.99993,
        "BZVtxSep"             : 2.0,  
        "BDZVtxSep"            : -1.0,  
        "BDRPV"                : 0.15,
        "MaxTracks"                     : 300,
        "B2D0PiPiPiAll_Prescale"        : 1.0,
        "B2D0PiPiPiAll_Postscale"       : 1.0,
        "B2DPiPiPiAll_Prescale"         : 1.0,
        "B2DPiPiPiAll_Postscale"        : 1.0,
        "B2DStarPiPiPiAll_Prescale"     : 1.0,
        "B2DStarPiPiPiAll_Postscale"    : 1.0,
        "B2D0PiPiPiSignal_Prescale"     : 1.0,
        "B2D0PiPiPiSignal_Postscale"    : 1.0,
        "B2DPiPiPiSignal_Prescale"      : 1.0,
        "B2DPiPiPiSignal_Postscale"     : 1.0,
        "B2DStarPiPiPiSignal_Prescale"  : 1.0,
        "B2DStarPiPiPiSignal_Postscale" : 1.0,
        "B2D0KPiPiAll_Prescale"         : 1.0,
        "B2D0KPiPiAll_Postscale"        : 1.0,
        "B2DKPiPiAll_Prescale"          : 1.0,
        "B2DKPiPiAll_Postscale"         : 1.0,
        "B2DStarKPiPiAll_Prescale"      : 1.0,
        "B2DStarKPiPiAll_Postscale"     : 1.0,
        "B2D0KPiPiSignal_Prescale"      : 1.0,
        "B2D0KPiPiSignal_Postscale"     : 1.0,
        "B2DKPiPiSignal_Prescale"       : 1.0,
        "B2DKPiPiSignal_Postscale"      : 1.0,
        "B2DStarKPiPiSignal_Prescale"   : 1.0,
        "B2DStarKPiPiSignal_Postscale"  : 1.0,
        "B2DDAll_Prescale"              : 1.0,
        "B2DDAll_Postscale"             : 1.0,
        "B2DDSignal_Prescale"           : 1.0,
        "B2DDSignal_Postscale"          : 1.0,
        "B2DStarDAll_Prescale"          : 1.0,
        "B2DStarDAll_Postscale"         : 1.0,
        "B2DStarDSignal_Prescale"       : 1.0,
        "B2DStarDSignal_Postscale"      : 1.0,
        "UnbiasedB2DPiPiPiAll_Prescale"         : 0.3,
        "UnbiasedB2DPiPiPiAll_Postscale"        : 1.0,
        "WSB2D3H_Prescale"              : 0.1,
        "WSB2D3H_Postscale"             : 1.0
        }

    
    b2d3h = conf(name,config_params)

    return b2d3h

def StrippingB2D3HLoose(name='Loose') :
    from StrippingSelections.StrippingB2D3H import StrippingB2D3HConf as conf
    config_params =  {
        "PionMinP"             : 2000.,
        "PionMaxP"             : 500000.,
        "PionMinPT"            : 200.,
        "PionMinPTtight"       : 500.,      
        "PionMinIPChisq"       : 6.25,
        "PionPiDDLL"           : 12,
        "KaonMinP"             : 2000.,
        "KaonMaxP"             : 500000.,
        "KaonMinPT"            : 200.,
        "KaonMinPTtight"       : 500.,        
        "KaonMinIPChisq"       : 6.25,
        "ProtonMinP"             : 2000.,
        "ProtonMaxP"             : 500000.,
        "ProtonMinPT"            : 200.,
        "ProtonMinIPChisq"       : 6.25,
        "MinPT"                : 300.,
        "TrkChisq"             : 5.0,
        "TrkChisqtight"        : 5.0,        
        "Bach3HMassWindow"     : 3000,
        "Bach3HDocaMax"        : 0.4,
        "Bach3HVtxChisq"       : 8.0,
        "Bach3HMinPT"          : 1000.0, 
        "Bach3HIP2PV"          : 0.07,
        "Bach3HIPChisq2PV"     : 9.0,
        "Bach3HVtxSepChisq"    : 36.0,
        "Bach3HDiraPV"         : 0.985,
        "Bach3HZVtxSep"        : 2.0,
        "Bach3HDRPV"           : 0.1,
        "DMinPT"               : 1100,
        "DVtxChisq"            : 8.0,
        "DMassWindow"          : 100,
        "D0MassWindow"         : 100,
        "DsMassWindow"         : 100,
        "tightDMassWindow"     : 40,
        "DDocaMax"             : 0.4,
        "DIP2PV"               : 0.07,
        "DIPChisq2PV"          : 10.0,
        "DVtxSepChisq"         : 36.0,
        "DDiraPV"              : 0.99,
        "DZVtxSep"             : 2.0,
        "DDRPV"                : 0.1,
        "DStarMassWindow"      : 50,
        "DStarMinPT"           : 1250,
        "BMinPT"               : 0.0, 
        "BVtxChisq"            : 6.0,
        "BMassWindow"          : 300, 
        "tightBMassWindow"     : 300,
        "BIP2PV"               : 0.15,
        "BIPChisq2PV"          : 16.0,
        "BVtxSepChisq"         : 25.0,
        "BDiraPV"              : 0.99994,
        "BZVtxSep"             : 2.0,
        "BDZVtxSep"            : -1.0,  
        "BDRPV"                : 0.1,
        "MaxTracks"                     : 300,
        "B2D0PiPiPiAll_Prescale"        : 1.0,
        "B2D0PiPiPiAll_Postscale"       : 1.0,
        "B2DPiPiPiAll_Prescale"         : 1.0,
        "B2DPiPiPiAll_Postscale"        : 1.0,
        "B2DStarPiPiPiAll_Prescale"     : 1.0,
        "B2DStarPiPiPiAll_Postscale"    : 1.0,
        "B2D0PiPiPiSignal_Prescale"     : 1.0,
        "B2D0PiPiPiSignal_Postscale"    : 1.0,
        "B2DPiPiPiSignal_Prescale"      : 1.0,
        "B2DPiPiPiSignal_Postscale"     : 1.0,
        "B2DStarPiPiPiSignal_Prescale"  : 1.0,
        "B2DStarPiPiPiSignal_Postscale" : 1.0,
        "B2D0KPiPiAll_Prescale"         : 1.0,
        "B2D0KPiPiAll_Postscale"        : 1.0,
        "B2DKPiPiAll_Prescale"          : 1.0,
        "B2DKPiPiAll_Postscale"         : 1.0,
        "B2DStarKPiPiAll_Prescale"      : 1.0,
        "B2DStarKPiPiAll_Postscale"     : 1.0,
        "B2D0KPiPiSignal_Prescale"      : 1.0,
        "B2D0KPiPiSignal_Postscale"     : 1.0,
        "B2DKPiPiSignal_Prescale"       : 1.0,
        "B2DKPiPiSignal_Postscale"      : 1.0,
        "B2DStarKPiPiSignal_Prescale"   : 1.0,
        "B2DStarKPiPiSignal_Postscale"  : 1.0,
        "B2DDAll_Prescale"              : 1.0,
        "B2DDAll_Postscale"             : 1.0,
        "B2DDSignal_Prescale"           : 1.0,
        "B2DDSignal_Postscale"          : 1.0,
        "B2DStarDAll_Prescale"          : 1.0,
        "B2DStarDAll_Postscale"         : 1.0,
        "B2DStarDSignal_Prescale"       : 1.0,
        "B2DStarDSignal_Postscale"      : 1.0,
        "UnbiasedB2DPiPiPiAll_Prescale"         : 0.3,
        "UnbiasedB2DPiPiPiAll_Postscale"        : 1.0,
        "WSB2D3H_Prescale"              : 0.1,
        "WSB2D3H_Postscale"             : 1.0
        }


    b2d3h = conf(name,config_params)

    return b2d3h

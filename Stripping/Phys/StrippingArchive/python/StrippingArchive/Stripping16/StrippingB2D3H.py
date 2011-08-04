# $Id: StrippingB2D3H.py,v 1.6 2011/02/21 sblusk bmaynard
"""
DaVinci v27r1
Module for construction of B->D+3H Stripping Selections and StrippingLines.
Provides functions to build 3H Final states in PiPiPi and KPiPi.
D candidates are filtered from StdLooseD02HH and StdLooseDplus2XXX.
Provides class B2D3HAllLinesConf, which constructs the Selections and
StrippingLines given a configuration dictionary.
Exported symbols (use python help!):
   - StrippngB2D3HConf
   - makeD0Meson
   - makeDMeson
   - makeLambdaC
   - makeDStarMeson
   - makeA12PiPiPi
   - makeK12KPiPi
   - makeK12KPPbarh
   - makePions
   - makeKaons
   - StrippingB2D3HLoose
   - StrippingB2D3HNominal
"""


__author__ = ['Steven Blusk', 'Brian Maynard']
__date__ = '26/07/2010'
__version__ = '$Revision: 1.6 $'


__all__ = ('B2D3HAllLinesConf',
           'makeB2D3H',
           'makeD0Meson',
           'makeDMeson',
           'makeLambdaC',
           'makeDStarMeson',
           'makeA12PiPiPi',
           'makeK12KPiPi',
           'makePPbarh',
           'makePions',
           'makeKaons',
           )

confdict =  {
        "PionMinP"             : 2000.,
        "PionMaxP"             : 500000.,
        "PionMinPT"            : 250.,
        "PionMinPTtight"       : 500.,
        "PionMinIPChisq"       : 6.25,
        "PionPiDDLL"           : 12,
        "KaonMinP"             : 2000.,
        "KaonMaxP"             : 500000.,
        "KaonMinPT"            : 250.,
        "KaonMinPTtight"       : 500.,
        "KaonMinIPChisq"       : 6.25,
        "KaonPiDDLL"           : -5,
        "ProtonMinP"             : 2000.,
        "ProtonMaxP"             : 500000.,
        "ProtonMinPT"            : 250.,
        "ProtonMinIPChisq"       : 6.25,
        "ProtonPiDDLL"           : -5,
        "MinPT"                : 300.,
        "TrkChisq"             : 4.0,
        "TrkChisqtight"        : 4.0,
        "Bach3HMassWindow"     : 3000,
        "Bach3HDocaMax"        : 0.35,
        "Bach3HVtxChisq"       : 8.0,
        "Bach3HMinPT"          : 1000.0,
        "Bach3HIP2PV"          : 0.0,
        "Bach3HIPChisq2PV"     : 9.0,
        "Bach3HVtxSepChisq"    : 36.0,
        "Bach3HDiraPV"         : 0.98,
        "Bach3HZVtxSep"        : 0.0,
        "Bach3HDRPV"           : 0.1,
        "DMinPT"               : 1250,
        "DVtxChisq"            : 8.0,
        "DMassWindow"          : 70,
        "D0MassWindow"         : 70,
        "DsMassWindow"         : 70,
        "tightDMassWindow"     : 40,
        "DDocaMax"             : 0.35,
        "DIP2PV"               : 0.0,
        "DIPChisq2PV"          : 4.0,
        "DVtxSepChisq"         : 49.0,
        "DDiraPV"              : 0.98,
        "DZVtxSep"             : 0.0,
        "DDRPV"                : 0.1,
        "DStarMassWindow"      : 50,
        "DStarMinPT"           : 1000,
        "BMinPT"               : 0.0,
        "BVtxChisq"            : 8.0, #was 6.0
        "BMassWindow"          : 300,
        "tightBMassWindow"     : 300,
        "BIP2PV"               : 0.15,
        "BIPChisq2PV"          : 16.0,
        "BVtxSepChisq"         : 25.0,
        "BDiraPV"              : 0.99994,
        "BZVtxSep"             : 0.0,
        "BDZVtxSep"            : -1.0,
        "BDRPV"                : 0.1,
        "MaxTracks"                     : 300,
        "B2D0PiPiPiAll_Prescale"        : 1.0,
        "B2D0PiPiPiAll_Postscale"       : 1.0,
        "B2DPiPiPiAll_Prescale"         : 1.0,
        "B2DPiPiPiAll_Postscale"        : 1.0,
        "B2DStarPiPiPiAll_Prescale"     : 1.0,
        "B2DStarPiPiPiAll_Postscale"    : 1.0,
        "B2D0KPiPiAll_Prescale"         : 1.0,
        "B2D0KPiPiAll_Postscale"        : 1.0,
        "B2DKPiPiAll_Prescale"          : 1.0,
        "B2DKPiPiAll_Postscale"         : 1.0,
        "B2DStarKPiPiAll_Prescale"      : 1.0,
        "B2DStarKPiPiAll_Postscale"     : 1.0,
        "B2DDAll_Prescale"              : 1.0,
        "B2DDAll_Postscale"             : 1.0,
        "B2DStarDAll_Prescale"          : 1.0,
        "B2DStarDAll_Postscale"         : 1.0,
        "UnbiasedB2DPiPiPiAll_Prescale"         : 0.15,
        "UnbiasedB2DPiPiPiAll_Postscale"        : 1.0,
        "WSB2D3H_Prescale"              : 0.1,
        "WSB2D3H_Postscale"             : 1.0,
        "B2DStarDKAll_Prescale"          : 1.0,
        "B2DStarDKAll_Postscale"         : 1.0
        }

"""
B->D+3H channels
"""
from StrippingConf.StrippingStream import StrippingStream
from Configurables import FilterDesktop, CombineParticles
from StrippingConf.StrippingLine import StrippingLine
#from StrippingSelections.Utils import checkConfig
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from copy import copy
from StrippingUtils.Utils import LineBuilder, checkConfig  #ADDED LINE
from CommonParticles import StdLoosePions, StdLooseKaons, StdLooseProtons, StdLooseDplus


class B2D3HAllLinesConf( LineBuilder ):  #ADDED LINE (CHANGED OBJECT TO LINEBUILDER)

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
                              "KaonPiDDLL",
                              "ProtonMinP" ,
                              "ProtonMaxP" ,
                              "ProtonMinPT",
                              "ProtonMinIPChisq",
                              "ProtonPiDDLL",
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
                              "B2D0KPiPiAll_Prescale",
                              "B2D0KPiPiAll_Postscale",
                              "B2DKPiPiAll_Prescale",
                              "B2DKPiPiAll_Postscale",
                              "B2DStarKPiPiAll_Prescale",
                              "B2DStarKPiPiAll_Postscale",
                              "B2DDAll_Prescale",
                              "B2DDAll_Postscale",
                              "B2DStarDAll_Prescale",
                              "B2DStarDAll_Postscale",
                              "UnbiasedB2DPiPiPiAll_Prescale",
                              "UnbiasedB2DPiPiPiAll_Postscale",
                              "WSB2D3H_Prescale",
                              "WSB2D3H_Postscale",
                              "B2DStarDKAll_Prescale",
                              "B2DStarDKAll_Postscale"
                              )


    def __init__(self,
                 name = 'Loose',
                 config = None) :

	LineBuilder.__init__(self, name, config)  #ADDED LINE

        checkConfig(B2D3HAllLinesConf.__configuration_keys__, config)

#       Use FILTER agrument of StrippingLine instead of VoidFilter - A. Poluektov

#        self.EventFilter = MyEventFilter('B2D3H'+name,
#                                         MaxTracks = config['MaxTracks'])

        from Configurables import LoKi__VoidFilter as VoidFilter
        from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
        #modules = CoreFactory('CoreFactory').Modules
        #for i in ['LoKiTracks.decorators']:
        #  if i not in modules : modules.append(i)
        self.EventFilter = { 'Code': "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %s )" % config['MaxTracks'],
                            'Preambulo' : ["from LoKiTracks.decorators import *"]
                            }

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

        # Make the PPbarPi bachelor
        self.selPPbarPi = makePPbarh( 'PPbarPiForB2D3H'+name,
                                      protonSel = self.selProtons,
                                      hSel = self.selPions,
                                      MinPT = config['MinPT'],
                                      Bach3HMassWindow = 5000.0,
                                      Bach3HDocaMax = config['Bach3HDocaMax'],
                                      Bach3HMinPT = config['Bach3HMinPT'],
                                      Bach3HIP2PV = config['Bach3HIP2PV'],
                                      Bach3HIPChisq2PV = config['Bach3HIPChisq2PV'],
                                      Bach3HVtxChisq = config['Bach3HVtxChisq'],
                                      Bach3HVtxSepChisq = config['Bach3HVtxSepChisq'],
                                      Bach3HDiraPV = config['Bach3HDiraPV'],
                                      Bach3HZVtxSep = config['Bach3HZVtxSep'],
                                      Bach3HDRPV = config['Bach3HDRPV'],
                                      decayDesc = "[Xi_c+ -> p+ p~- pi+]cc"
                                      )


        # Make the PPbarPi bachelor
        self.selPPbarK = makePPbarh( 'PPbarKForB2D3H'+name,
                                      protonSel = self.selProtons,
                                      hSel = self.selKaons,
                                      MinPT = config['MinPT'],
                                      Bach3HMassWindow = 5000.0,
                                      Bach3HDocaMax = config['Bach3HDocaMax'],
                                      Bach3HMinPT = config['Bach3HMinPT'],
                                      Bach3HIP2PV = config['Bach3HIP2PV'],
                                      Bach3HIPChisq2PV = config['Bach3HIPChisq2PV'],
                                      Bach3HVtxChisq = config['Bach3HVtxChisq'],
                                      Bach3HVtxSepChisq = config['Bach3HVtxSepChisq'],
                                      Bach3HDiraPV = config['Bach3HDiraPV'],
                                      Bach3HZVtxSep = config['Bach3HZVtxSep'],
                                      Bach3HDRPV = config['Bach3HDRPV'],
                                      decayDesc = "[Xi_c+ -> p+ p~- K+]cc"
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
                                          DDRPV = 0, #should never change
                                          PionMinIPChisq = config['PionMinIPChisq'],
                                          PionPiDDLL = 10000.0,
                                          KaonPiDDLL = -10000.0
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
                                  DDRPV = config['DDRPV'],
                                  PionMinIPChisq = config['PionMinIPChisq'],
                                  PionPiDDLL = config['PionPiDDLL'],
                                  KaonPiDDLL = config['KaonPiDDLL']
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
                                      DDRPV = config['DDRPV'],
                                       PionMinIPChisq = config['PionMinIPChisq'],
                                       PionPiDDLL = config['PionPiDDLL'],
                                       KaonPiDDLL = config['KaonPiDDLL']
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
                                  DDRPV = config['DDRPV'],
                                  PionMinIPChisq = config['PionMinIPChisq'],
                                  PionPiDDLL = config['PionPiDDLL'],
                                  KaonPiDDLL = config['KaonPiDDLL']
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

        # Make X(cc) -> D*+D-
        self.selXcc = makeXccMeson('ForB2XccK'+name,self.selDStar,self.selDch)

        # Make WS X(cc) -> D*+D-
        self.selXccWS = makeXccWSMeson('ForB2XccK'+name,self.selDStar,self.selDch)

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
                                   BMassWindow = 800.0,
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

        # Make LambdaB --> LambdaC+ (ppbarpi)+
        name11 = 'LambdaB2LambdaCPPbarPi' + name
        self.LambdaB2LambdaCPPbarPi = makeB2D3H(name11,
                                   dSel = self.selLambdaC,
                                   hhhSel = self.selPPbarPi,
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
                                   decayDesc = "[Lambda_b0 -> Lambda_c+ Xi_c~-]cc",
                                   parentB = "Lambda_b0")

        # Make LambdaB --> LambdaC+ (ppbarK)+
        name12 = 'LambdaB2LambdaCPPbarK' + name
        self.LambdaB2LambdaCPPbarK = makeB2D3H(name12,
                                   dSel = self.selLambdaC,
                                   hhhSel = self.selPPbarK,
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
                                   decayDesc = "[Lambda_b0 -> Lambda_c+ Xi_c~-]cc",
                                   parentB = "Lambda_b0")

        # Make B+ --> D*- D+ K+
        theName = 'B2DStarDK' + name
        self.B2DStarDK = makeB2D3H(theName,
                                   dSel = self.selXcc,
                                   hhhSel = self.selKaons,
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
                                   decayDesc = "[B+ -> eta_c(1S) K+]cc",
                                   parentB = "B+")
        name13 = 'B2D0D' + name
        self.B2D0D = makeB2D3H(name13,
                                   dSel = self.selD0,
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
                                   decayDesc = "[B- -> D0 D-]cc",
                                   parentB = "B+")


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
                                   BMassWindow = 600.0,
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

        # Make LambdaB --> LambdaC+ (ppbarpi)+
        name11 = 'WSLambdaB2LambdaCPPbarPi' + name
        self.WSLambdaB2LambdaCPPbarPi = makeB2D3H(name11,
                                   dSel = self.selLambdaC,
                                   hhhSel = self.selPPbarPi,
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
                                   decayDesc = "[Lambda_b0 -> Lambda_c+ Xi_c+]cc",
                                   parentB = "Lambda_b0")

        # Make LambdaB --> LambdaC+ (ppbarK)+
        name12 = 'WSLambdaB2LambdaCPPbarK' + name
        self.WSLambdaB2LambdaCPPbarK = makeB2D3H(name12,
                                   dSel = self.selLambdaC,
                                   hhhSel = self.selPPbarK,
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
                                   decayDesc = "[Lambda_b0 -> Lambda_c+ Xi_c+]cc",
                                   parentB = "Lambda_b0")

        # Make WS B+ --> D*+ D+ K+
        theName = 'WSB2DStarDK' + name
        self.WSB2DStarDK = makeB2D3H(theName,
                                     dSel = self.selXccWS,
                                     hhhSel = self.selKaons,
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
                                     decayDesc = "[B+ -> eta_c(1S) K+]cc",
                                     parentB = "B+")


        # Now create stripping lines...

        # These are for the full mass region with sidebands.
        # These lines should be prescaled first, if necessary.

        #---------------------------
        # Right-Sign Stripping Lines
        #---------------------------

        self.StrippingAllB2D0PiPiPiLine = StrippingLine('AllB2D0PiPiPiLine'+name,
                                                     prescale = config['B2D0PiPiPiAll_Prescale'],
                                                     postscale = config['B2D0PiPiPiAll_Postscale'],
                                                     selection = self.B2D0PiPiPi,
                                                     FILTER = self.EventFilter
						     #FILTER = "TrSOURCE('Rec/Track/Best', TrLONG) >> (TrSIZE < %(MaxTracks)s )" %locals()
                                                     )

        self.StrippingAllB2D0KPiPiLine = StrippingLine('AllB2D0KPiPiLine'+name,
                                                     prescale = config['B2D0KPiPiAll_Prescale'],
                                                     postscale = config['B2D0KPiPiAll_Postscale'],
                                                     #algos = [ self.EventFilter, self.B2D0KPiPi],
                                                     selection = self.B2D0KPiPi,
                                                     FILTER = self.EventFilter
                                                     )

        #self.StrippingAllUnbiasedB2DPiPiPiLine = StrippingLine('AllUnbiasedB2DPiPiPiLine'+name,
        #                                            prescale = config['UnbiasedB2DPiPiPiAll_Prescale'],
        #                                            postscale = config['UnbiasedB2DPiPiPiAll_Postscale'],
        #                                            #algos = [ self.EventFilter, self.UnbiasedB2DPiPiPiHLT1HLT2TIS]
        #                                            selection = self.UnbiasedB2DPiPiPiHLT1HLT2TIS,
        #                                            FILTER = self.EventFilter
        #                                            )

        self.StrippingAllB2DPiPiPiLine = StrippingLine('AllB2DPiPiPiLine'+name,
                                                    prescale = config['B2DPiPiPiAll_Prescale'],
                                                    postscale = config['B2DPiPiPiAll_Postscale'],
                                                    #algos = [ self.EventFilter, self.B2DPiPiPi]
                                                    selection = self.B2DPiPiPi,
                                                    FILTER = self.EventFilter
                                                    )

        self.StrippingAllB2DKPiPiLine = StrippingLine('AllB2DKPiPiLine'+name,
                                                   prescale = config['B2DKPiPiAll_Prescale'],
                                                   postscale = config['B2DKPiPiAll_Postscale'],
                                                   #algos = [ self.EventFilter, self.B2DKPiPi]
                                                   selection = self.B2DKPiPi,
                                                   FILTER = self.EventFilter
                                                   )

        self.StrippingAllB2DStarPiPiPiLine = StrippingLine('AllB2DStarPiPiPiLine'+name,
                                                        prescale = config['B2DStarPiPiPiAll_Prescale'],
                                                        postscale = config['B2DStarPiPiPiAll_Postscale'],
                                                        #algos = [ self.EventFilter, self.B2DStarPiPiPi]
                                                        selection = self.B2DStarPiPiPi,
                                                        FILTER = self.EventFilter
                                                        )

        self.StrippingAllB2DStarKPiPiLine = StrippingLine('AllB2DStarKPiPiLine'+name,
                                                       prescale = config['B2DStarKPiPiAll_Prescale'],
                                                       postscale = config['B2DStarKPiPiAll_Postscale'],
                                                       #algos = [ self.EventFilter, self.B2DStarKPiPi]
                                                       selection = self.B2DStarKPiPi,
                                                       FILTER = self.EventFilter
                                                       )

        self.StrippingAllB2DDLine = StrippingLine('AllB2DDLine'+name,
                                                    prescale = config['B2DDAll_Prescale'],
                                                    postscale = config['B2DDAll_Postscale'],
                                                    #algos = [ self.EventFilter, self.B2DD]
                                                    selection = self.B2DD,
                                                    FILTER = self.EventFilter
                                                    )

        self.StrippingAllB2DStarDLine = StrippingLine('AllB2DStarDLine'+name,
                                                    prescale = config['B2DStarDAll_Prescale'],
                                                    postscale = config['B2DStarDAll_Postscale'],
                                                    #algos = [ self.EventFilter, self.B2DStarD]
                                                    selection = self.B2DStarD,
                                                    FILTER = self.EventFilter
                                                    )

        self.StrippingAllLambdaB2LambdaCPiPiPiLine = StrippingLine('AllLambdaB2LambdaCPiPiPiLine'+name,
                                                    prescale = config['B2DPiPiPiAll_Prescale'],
                                                    postscale = config['B2DPiPiPiAll_Postscale'],
                                                    #algos = [ self.EventFilter, self.LambdaB2LambdaCPiPiPi]
                                                    selection = self.LambdaB2LambdaCPiPiPi,
                                                    FILTER = self.EventFilter
                                                    )

        self.StrippingAllLambdaB2LambdaCKPiPiLine = StrippingLine('AllLambdaB2LambdaCKPiPiLine'+name,
                                                    prescale = config['B2DKPiPiAll_Prescale'],
                                                    postscale = config['B2DKPiPiAll_Postscale'],
                                                    #algos = [ self.EventFilter, self.LambdaB2LambdaCKPiPi]
                                                    selection = self.LambdaB2LambdaCKPiPi,
                                                    FILTER = self.EventFilter
                                                    )

        self.StrippingAllLambdaB2LambdaCPPbarPiLine = StrippingLine('AllLambdaB2LambdaCPPbarPiLine'+name,
                                                    prescale = config['B2DPiPiPiAll_Prescale'],
                                                    postscale = config['B2DPiPiPiAll_Postscale'],
                                                    #algos = [ self.EventFilter, self.LambdaB2LambdaCPiPiPi]
                                                    selection = self.LambdaB2LambdaCPPbarPi,
                                                    FILTER = self.EventFilter
                                                    )

        self.StrippingAllLambdaB2LambdaCPPbarKLine = StrippingLine('AllLambdaB2LambdaCPPbarKLine'+name,
                                                    prescale = config['B2DKPiPiAll_Prescale'],
                                                    postscale = config['B2DKPiPiAll_Postscale'],
                                                    #algos = [ self.EventFilter, self.LambdaB2LambdaCPiPiPi]
                                                    selection = self.LambdaB2LambdaCPPbarK,
                                                    FILTER = self.EventFilter
                                                    )

        self.StrippingAllB2DStarDKLine = StrippingLine('AllB2DStarDKLine'+name,
                                                       prescale = config['B2DStarDKAll_Prescale'],
                                                       postscale = config['B2DStarDKAll_Postscale'],
                                                       selection = self.B2DStarDK,
                                                       FILTER = self.EventFilter
                                                       )
        self.StrippingAllB2D0DLine = StrippingLine('AllB2D0DLine'+name,
                                                   prescale = config['B2DDAll_Prescale'],
                                                   postscale = config['B2DDAll_Postscale'],
                                                   selection = self.B2D0D,
                                                   FILTER = self.EventFilter
                                                   )

        #---------------------------
        # Wrong-Sign Stripping Lines
        #---------------------------


        self.StrippingAllWSB2DPiPiPiLine = StrippingLine('AllWSB2DPiPiPiLine'+name,
                                                         prescale = config['WSB2D3H_Prescale'],
                                                         postscale = config['WSB2D3H_Postscale'],
                                                         #algos = [ self.EventFilter, self.WSB2DPiPiPi]
                                                         selection = self.WSB2DPiPiPi,
                                                         FILTER = self.EventFilter
                                                         )

        self.StrippingAllWSB2DKPiPiLine = StrippingLine('AllWSB2DKPiPiLine'+name,
                                                        prescale = config['WSB2D3H_Prescale'],
                                                        postscale = config['WSB2D3H_Postscale'],
                                                        #algos = [ self.EventFilter, self.WSB2DKPiPi]
                                                        selection = self.WSB2DKPiPi,
                                                        FILTER = self.EventFilter
                                                        )

        self.StrippingAllWSB2DStarPiPiPiLine = StrippingLine('AllWSB2DStarPiPiPiLine'+name,
                                                             prescale = config['WSB2D3H_Prescale'],
                                                             postscale = config['WSB2D3H_Postscale'],
                                                             #algos = [ self.EventFilter, self.WSB2DStarPiPiPi]
                                                             selection = self.WSB2DStarPiPiPi,
                                                             FILTER = self.EventFilter
                                                             )

        self.StrippingAllWSB2DStarKPiPiLine = StrippingLine('AllWSB2DStarKPiPiLine'+name,
                                                            prescale = config['WSB2D3H_Prescale'],
                                                            postscale = config['WSB2D3H_Postscale'],
                                                            #algos = [ self.EventFilter, self.WSB2DStarKPiPi]
                                                            selection = self.WSB2DStarKPiPi,
                                                            FILTER = self.EventFilter
                                                            )

        self.StrippingAllWSB2DDLine = StrippingLine('AllWSB2DDLine'+name,
                                                    prescale = config['WSB2D3H_Prescale'],
                                                    postscale = config['WSB2D3H_Postscale'],
                                                    #algos = [ self.EventFilter, self.WSB2DD]
                                                    selection = self.WSB2DD,
                                                    FILTER = self.EventFilter
                                                    )

        self.StrippingAllWSB2DStarDLine = StrippingLine('AllWSB2DStarDLine'+name,
                                                        prescale = config['WSB2D3H_Prescale'],
                                                        postscale = config['WSB2D3H_Postscale'],
                                                        #algos = [ self.EventFilter, self.WSB2DStarD]
                                                        selection = self.WSB2DStarD,
                                                        FILTER = self.EventFilter
                                                        )

        self.StrippingAllWSLambdaB2LambdaCPiPiPiLine = StrippingLine('AllWSLambdaB2LambdaCPiPiPiLine'+name,
                                                                     prescale = config['WSB2D3H_Prescale'],
                                                                     postscale = config['WSB2D3H_Postscale'],
                                                                     #algos = [ self.EventFilter, self.WSLambdaB2LambdaCPiPiPi]
                                                                     selection = self.WSLambdaB2LambdaCPiPiPi,
                                                                     FILTER = self.EventFilter
                                                                     )

        self.StrippingAllWSLambdaB2LambdaCKPiPiLine = StrippingLine('AllWSLambdaB2LambdaCKPiPiLine'+name,
                                                                    prescale = config['WSB2D3H_Prescale'],
                                                                    postscale = config['WSB2D3H_Postscale'],
                                                                    #algos = [ self.EventFilter, self.WSLambdaB2LambdaCKPiPi]
                                                                    selection = self.WSLambdaB2LambdaCKPiPi,
                                                                    FILTER = self.EventFilter
                                                                    )

        self.StrippingAllWSLambdaB2LambdaCPPbarPiLine = StrippingLine('AllWSLambdaB2LambdaCPPbarPiLine'+name,
                                                                    prescale = config['WSB2D3H_Prescale'],
                                                                    postscale = config['WSB2D3H_Postscale'],
                                                                    #algos = [ self.EventFilter, self.LambdaB2LambdaCPiPiPi]
                                                                    selection = self.WSLambdaB2LambdaCPPbarPi,
                                                                    FILTER = self.EventFilter
                                                                    )

        self.StrippingAllWSLambdaB2LambdaCPPbarKLine = StrippingLine('AllWSLambdaB2LambdaCPPbarKLine'+name,
                                                                    prescale = config['WSB2D3H_Prescale'],
                                                                    postscale = config['WSB2D3H_Postscale'],
                                                                     #algos = [ self.EventFilter, self.LambdaB2LambdaCPiPiPi]
                                                                     selection = self.WSLambdaB2LambdaCPPbarK,
                                                                     FILTER = self.EventFilter
                                                                   )

        self.StrippingAllWSB2DStarDKLine = StrippingLine('AllWSB2DStarDKLine'+name,
                                                         prescale = config['WSB2D3H_Prescale'],
                                                         postscale = config['WSB2D3H_Postscale'],
                                                         selection = self.WSB2DStarDK,
                                                         FILTER = self.EventFilter
                                                         )



        self.registerLine ( self.StrippingAllB2D0PiPiPiLine )
        self.registerLine ( self.StrippingAllB2D0KPiPiLine )
        #self.registerLine ( self.StrippingAllUnbiasedAB2DPiPiPiLine )
        self.registerLine ( self.StrippingAllB2DPiPiPiLine )
        self.registerLine ( self.StrippingAllB2DKPiPiLine )
        self.registerLine ( self.StrippingAllB2DStarPiPiPiLine )
        self.registerLine ( self.StrippingAllB2DStarKPiPiLine )
        self.registerLine ( self.StrippingAllB2DDLine )
        self.registerLine ( self.StrippingAllB2DStarDLine )
        self.registerLine ( self.StrippingAllLambdaB2LambdaCPiPiPiLine )	#might be an error in line
        self.registerLine ( self.StrippingAllLambdaB2LambdaCKPiPiLine )
        self.registerLine ( self.StrippingAllLambdaB2LambdaCPPbarPiLine )
        self.registerLine ( self.StrippingAllLambdaB2LambdaCPPbarKLine )
        self.registerLine ( self.StrippingAllB2DStarDKLine )
        self.registerLine ( self.StrippingAllWSB2DPiPiPiLine )
        self.registerLine ( self.StrippingAllWSB2DKPiPiLine )
        self.registerLine ( self.StrippingAllWSB2DStarPiPiPiLine )
        self.registerLine ( self.StrippingAllWSB2DStarKPiPiLine )
        self.registerLine ( self.StrippingAllWSB2DDLine )
        self.registerLine ( self.StrippingAllWSB2DStarDLine )
        self.registerLine ( self.StrippingAllWSLambdaB2LambdaCPiPiPiLine )
        self.registerLine ( self.StrippingAllWSLambdaB2LambdaCKPiPiLine )
        self.registerLine ( self.StrippingAllWSLambdaB2LambdaCPPbarPiLine )
        self.registerLine ( self.StrippingAllWSLambdaB2LambdaCPPbarKLine )
        self.registerLine ( self.StrippingAllWSB2DStarDKLine )
        self.registerLine ( self.StrippingAllB2D0DLine )

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
    for i in ['LoKiTracks.decorators']:
      if i not in modules : modules.append(i)


    _eventFilter = VoidFilter("EventfilterFor"+name)
    _eventFilter.Code =  "(recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(MaxTracks)s )" %locals()
    _eventFilter.Preambulo += ["from LoKiTracks.decorators import *"]

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

    _stdLoosePions = DataOnDemand(Location = "Phys/StdLoosePions/Particles")
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

    _stdLoosePions = DataOnDemand(Location = "Phys/StdTightPions/Particles")
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

    _stdLooseKaons = DataOnDemand(Location = "Phys/StdLooseKaons/Particles")
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

    _stdLooseKaons = DataOnDemand(Location = "Phys/StdTightKaons/Particles")
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

    _stdLooseProtons = DataOnDemand(Location = "Phys/StdLooseProtons/Particles")
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

def makePPbarh( name,
                 protonSel,
                 hSel,
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
                 Bach3HDRPV,
                 decayDesc
                 ):

    """
    Create and return PPbarX Selection object.
    #
    Arguments:
    name                  : name of the Selection.
    protonSel             : Proton selection object
    hSel                  : Pion/Kaon selection object
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


    _ppbarhAlg = CombineParticles(name)
    _ppbarhAlg.DecayDescriptor = decayDesc
    dauCut = "(PIDp > 0)"
    _ppbarhAlg.DaughtersCuts =  { "p+"      : dauCut}
    _ppbarhAlg.CombinationCut = "( (AM < %(Bach3HMassWindow)s *MeV) & (APT > %(Bach3HMinPT)s  *MeV)" \
                           " & (ANUM(PT < %(MinPT)s *MeV) <= 1) & (ACUTDOCA( %(Bach3HDocaMax)s *mm, '')) ) " %locals()
    _ppbarhAlg.MotherCut = "( (PT > %(Bach3HMinPT)s *MeV) & (VFASPF(VCHI2/VDOF)< %(Bach3HVtxChisq)s )  " \
                       " & (BPVVDCHI2 > %(Bach3HVtxSepChisq)s ) & (MIPCHI2DV(PRIMARY) > %(Bach3HIPChisq2PV)s )" \
                       " & (BPVDIRA > %(Bach3HDiraPV)s ) & (BPVVDZ> %(Bach3HZVtxSep)s *mm) & (BPVVDRHO > %(Bach3HDRPV)s *mm) " \
                       " & (MIPDV(PRIMARY)> %(Bach3HIP2PV)s *mm))" % locals()

    selName = 'Sel'+name
    PPbarhSelection = Selection(selName,
                               Algorithm = _ppbarhAlg,
                               RequiredSelections = [protonSel, hSel])
    return PPbarhSelection

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
                DDRPV,
                PionMinIPChisq,
                PionPiDDLL,
                KaonPiDDLL
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

    _d2kpi = DataOnDemand(Location = "Phys/StdLooseD02KPi/Particles")
    _d2pipi = DataOnDemand(Location = "Phys/StdLooseD02PiPi/Particles")
    _d2kk = DataOnDemand(Location = "Phys/StdLooseD02KK/Particles")
    _d2kpiDCS = DataOnDemand(Location = "Phys/StdLooseD02KPiDCS/Particles")

    _dFilterCode = "(PT>%(DMinPT)s*MeV)"%locals()
    _dFilterCode += "& (BPVVDZ> %(DZVtxSep)s *mm)"%locals()
    _dFilterCode += "& (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s)"%locals()
    _dFilterCode += "& (BPVVDCHI2 > %(DVtxSepChisq)s) "%locals()
    _dFilterCode += "& (BPVDIRA > %(DDiraPV)s) "%locals()
    _dFilterCode += "& (BPVVDRHO > %(DDRPV)s *mm)"%locals()
    _dFilterCode += "& (MIPDV(PRIMARY)> %(DIP2PV)s*mm)"%locals()

    _trkFilterCode = "CHILDCUT(PT > %(MinPT)s *MeV, 1)" %locals()
    _trkFilterCode += "& CHILDCUT(PT > %(MinPT)s *MeV, 2)" %locals()
    _trkFilterCode += "& CHILDCUT(MIPCHI2DV ( PRIMARY ) > %(PionMinIPChisq)s , 1)" %locals()
    _trkFilterCode += "& CHILDCUT(MIPCHI2DV ( PRIMARY ) > %(PionMinIPChisq)s , 2)" %locals()
    _pidpiPass = "(MAXTREE(ABSID=='pi+',PIDK)<%(PionPiDDLL)s)"%locals()

    _dMassWindow = "(ADMASS('D0') < %(DMassWindow)s *MeV ) " %locals()


    # D0 -->K- Pi+
    _code0 = _dMassWindow + " & " + _dFilterCode +" & " + _trkFilterCode + " & " + _pidpiPass
    theName = "D02KPiSel"+name
    _d02kpiFilter = FilterDesktop('FilterFor'+theName,Code = _code0)
    D2KPiSelection = Selection(theName, Algorithm = _d02kpiFilter, RequiredSelections = [_d2kpi])

    # D0 -->K- Pi+
    _code1 = _dMassWindow + " & " + _dFilterCode +" & " + _trkFilterCode + " & " + _pidpiPass
    theName = "D02PiPiSel"+name
    _d02pipiFilter = FilterDesktop('FilterFor'+theName,Code = _code1)
    D2PiPiSelection = Selection(theName, Algorithm = _d02pipiFilter, RequiredSelections = [_d2pipi])

    # D0 -->K- K+
    _code2 = _dMassWindow + " & " + _dFilterCode + " & " + _trkFilterCode
    theName = "D02KKSel"+name
    _d2kkFilter = FilterDesktop('FilterFor'+theName, Code = _code2)
    D2KKSelection = Selection(theName, Algorithm = _d2kkFilter, RequiredSelections = [_d2kk])

    # D0 -->K+ Pi- (DCS)
    _code3 = _dMassWindow + " & " + _dFilterCode +" & " + _trkFilterCode + " & " + _pidpiPass
    theName = "D02KPiDCSSel"+name
    _d02kpiDCSFilter = FilterDesktop('FilterFor'+theName,Code = _code3)
    D2KPiDCSSelection = Selection(theName, Algorithm = _d02kpiDCSFilter, RequiredSelections = [_d2kpiDCS])

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



    _dst2kpipi = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPi/Particles")
    _dst2pipipi = DataOnDemand(Location = "Phys/StdLooseDstarWithD02PiPi/Particles")
    _dst2kkpi = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KK/Particles")
    _dst2kpipiDCS = DataOnDemand(Location = "Phys/StdLooseDstarWithD02KPiDCS/Particles")


    # D*+ -->(K- Pi+) Pi+
    theName = "Dst2KPiPiSel"+name
    _d2kpipiFilter = FilterDesktop('FilterFor'+theName, Code = '(ALL)')
    D2KPiPiSelection = Selection(theName, Algorithm = _d2kpipiFilter, RequiredSelections = [_dst2kpipi])

    # D*+ -->(Pi- Pi+) Pi+
    theName = "Dst2PiPiPiSel"+name
    _d2pipipiFilter = FilterDesktop('FilterFor'+theName, Code = '(ALL)')
    D2PiPiPiSelection = Selection(theName, Algorithm = _d2pipipiFilter, RequiredSelections = [_dst2pipipi])

    # D*+ -->(K- K+) Pi+
    theName = "Dst2KKPiSel"+name
    _d2kkpiFilter = FilterDesktop('FilterFor'+theName, Code = '(ALL)')
    D2KKPiSelection = Selection(theName, Algorithm = _d2kkpiFilter, RequiredSelections = [_dst2kkpi])

    # D*+ -->(K+ Pi-) Pi+
    theName = "Dst2KPiPiDCSSel"+name
    _d2kpipidcsFilter = FilterDesktop('FilterFor'+theName, Code = '(ALL)')
    D2KPiPiDCSSelection = Selection(theName, Algorithm = _d2kpipidcsFilter, RequiredSelections = [_dst2kpipiDCS])

    selName = 'MergedSel'+name
    _dm = MergedSelection(selName, RequiredSelections = [ D2KPiPiSelection, D2KKPiSelection, D2PiPiPiSelection, D2KPiPiDCSSelection])
    return _dm



    ####


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
               DDRPV,
               PionMinIPChisq,
               PionPiDDLL,
               KaonPiDDLL
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

    _dMassWindow = "(ADMASS('D+') < %(DMassWindow)s *MeV)" %locals()
    _dsMassWindow = "(ADMASS('D_s+') < %(DsMassWindow)s *MeV)"  %locals()
    _danddsMassWindow = "( (ADMASS('D+') < %(DMassWindow)s *MeV) | (ADMASS('D_s+') < %(DsMassWindow)s *MeV) )"  %locals()


    _dFilterCode = "(PT>%(DMinPT)s*MeV)"%locals()
    _dFilterCode += "& (BPVVDZ> %(DZVtxSep)s *mm)"%locals()
    _dFilterCode += "& (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s)"%locals()
    _dFilterCode += "& (BPVVDCHI2 > %(DVtxSepChisq)s) "%locals()
    _dFilterCode += "& (BPVDIRA > %(DDiraPV)s) "%locals()
    _dFilterCode += "& (BPVVDRHO > %(DDRPV)s *mm)"%locals()
    _dFilterCode += "& (MIPDV(PRIMARY)> %(DIP2PV)s*mm)"%locals()
    _dFilterCode += "& (2 > NINGENERATION(PT<%(MinPT)s *MeV, 1))"%locals()

    _trkFilterCode = "CHILDCUT(MIPCHI2DV ( PRIMARY ) > %(PionMinIPChisq)s , 1)" %locals()
    _trkFilterCode += "& CHILDCUT(MIPCHI2DV ( PRIMARY ) > %(PionMinIPChisq)s , 2)" %locals()
    _trkFilterCode += "& CHILDCUT(MIPCHI2DV ( PRIMARY ) > %(PionMinIPChisq)s , 3)" %locals()
    _pidpiPass = "(MAXTREE(ABSID=='pi+',PIDK)<%(PionPiDDLL)s)"%locals()

    _d2kpipi = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPi/Particles")
    _d2pipipi = DataOnDemand(Location = "Phys/StdLooseDplus2PiPiPi/Particles")
    _d2kkpi = DataOnDemand(Location = "Phys/StdLooseDplus2KKPi/Particles")
    _d2kpipiOS = DataOnDemand(Location = "Phys/StdLooseDplus2KPiPiOppSignPi/Particles")


    # D+ -->K- Pi+ Pi+
    _code0 = _dMassWindow + " & " + _dFilterCode +" & " + _trkFilterCode + " & " + _pidpiPass
    theName = "D2KPiPiSel"+name
    _d2kpipiFilter = FilterDesktop('FilterFor'+theName,Code = _code0)
    D2KPiPiSelection = Selection(theName, Algorithm = _d2kpipiFilter, RequiredSelections = [_d2kpipi])


    # D+ -->K- K+ Pi+ OR D_s+ -->K- K+ Pi+
    _code1 = _danddsMassWindow + " & " + _dFilterCode + " & " + _trkFilterCode + " & " + _pidpiPass
    theName = "D2KKPiSel"+name
    _d2kkpiFilter = FilterDesktop('FilterFor'+theName, Code = _code1)
    D2KKPiSelection = Selection(theName, Algorithm = _d2kkpiFilter, RequiredSelections = [_d2kkpi])

    # D_s+ -->Pi- Pi+ Pi+  (don't see need for D+ --> PiPiPi)
    _code2 = _dsMassWindow + " & " +_dFilterCode + " & " + _trkFilterCode + " & " + _pidpiPass
    theName = "D2PiPiPiSel"+name
    _d2pipipiFilter = FilterDesktop('FilterFor'+theName, Code = _code2)
    D2PiPiPiSelection = Selection(theName, Algorithm = _d2pipipiFilter, RequiredSelections = [_d2pipipi])

    # D+ -->K- Pi+ Pi+ (OS)
    _code3 = _dsMassWindow + " & " + _dFilterCode + " & " + _trkFilterCode + " & " + _pidpiPass
    theName = "D2KPiPiOSSel"+name
    _d2kpipiOSFilter = FilterDesktop('FilterFor'+theName, Code = _code3)
    D2KPiPiOppSignPiSelection = Selection(theName, Algorithm = _d2kpipiOSFilter, RequiredSelections = [_d2kpipiOS])


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
                DDRPV,
                PionMinIPChisq,
                PionPiDDLL,
                KaonPiDDLL
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

    _lc = DataOnDemand(Location = "Phys/StdLooseLambdac2PKPi/Particles")

    _dFilterCode = "(PT>%(DMinPT)s*MeV)"%locals()
    _dFilterCode += "& (BPVVDZ> %(DZVtxSep)s *mm)"%locals()
    _dFilterCode += "& (MIPCHI2DV(PRIMARY) > %(DIPChisq2PV)s)"%locals()
    _dFilterCode += "& (BPVVDCHI2 > %(DVtxSepChisq)s) "%locals()
    _dFilterCode += "& (BPVDIRA > %(DDiraPV)s) "%locals()
    _dFilterCode += "& (BPVVDRHO > %(DDRPV)s *mm)"%locals()
    _dFilterCode += "& (MIPDV(PRIMARY)> %(DIP2PV)s*mm)"%locals()
    _dFilterCode += "& (2 > NINGENERATION(PT<%(MinPT)s *MeV, 1))"%locals()

    _trkFilterCode = "CHILDCUT(MIPCHI2DV ( PRIMARY ) > %(PionMinIPChisq)s , 1)" %locals()
    _trkFilterCode += "& CHILDCUT(MIPCHI2DV ( PRIMARY ) > %(PionMinIPChisq)s , 2)" %locals()
    _trkFilterCode += "& CHILDCUT(MIPCHI2DV ( PRIMARY ) > %(PionMinIPChisq)s , 3)" %locals()
    _pidpiPass = "(MAXTREE(ABSID=='pi+',PIDK)<%(PionPiDDLL)s)"%locals()


    _lcMassWindow = "(ADMASS('Lambda_c+') < %(DMassWindow)s *MeV ) " %locals()


    # Lambda_C+ -->P+ K- Pi+
    _code0 = _lcMassWindow + " & " + _dFilterCode +" & " + _trkFilterCode + " & " + _pidpiPass
    theName = "Lc2pKPiSel"+name
    _lc2pkpiFilter = FilterDesktop('FilterFor'+theName,Code = _code0)
    Lc2pKPiSelection = Selection(theName, Algorithm = _lc2pkpiFilter, RequiredSelections = [_lc])

    return Lc2pKPiSelection

def makeXccMeson(name,dstarSel,dSel):
    '''Creates and returns the X(cc) hybrid meson selection object from D* and D selection objects.'''
    alg = CombineParticles(name='DstarD'+name,DecayDescriptor="[eta_c(1S) -> D*(2010)+ D-]cc")
    alg.CombinationCut = 'AM < 6000*MeV'
    alg.MotherCut = 'M < 6000*MeV'
    return Selection('XccSel'+name,Algorithm=alg,RequiredSelections=[dstarSel,dSel])

def makeXccWSMeson(name,dstarSel,dSel):
    '''Creates and returns the WS X(cc) hybrid meson selection object from D* and D selection objects.'''
    alg = CombineParticles(name='DstarDWS'+name,DecayDescriptor="[eta_c(1S) -> D*(2010)+ D+]cc")
    alg.CombinationCut = 'AM < 6000*MeV'
    alg.MotherCut = 'M < 6000*MeV'
    return Selection('XccWSSel'+name,Algorithm=alg,RequiredSelections=[dstarSel,dSel])

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
                       " & (DZ > %(BDZVtxSep)s *mm) " \
                       " & (BPVVDZ> %(BZVtxSep)s *mm) & (BPVVDRHO > %(BDRPV)s *mm) & (MIPCHI2DV(PRIMARY)< %(BIPChisq2PV)s )" \
                       " & (BPVVDCHI2 > %(BVtxSepChisq)s ) & (BPVDIRA > %(BDiraPV)s )"   \
                       " & (MIPDV(PRIMARY)< %(BIP2PV)s *mm) )" % locals()


    #(NINGENERATION( (PT < %(MinPT)s *MeV) , 2) <= 1)" \

    selName = 'Sel'+name
    if dSel == hhhSel:
        return Selection(name, Algorithm  = _b2d3h, RequiredSelections = [dSel])
    else:
        return Selection(name, Algorithm  = _b2d3h, RequiredSelections = [dSel, hhhSel])

"""
def filterB2D3H( name,
                 bSel,
                 tightDMassWindow,
                 tightBMassWindow,
                 parentB,
                 parentD):

    #Filter the B-->D+3H Selection object for events in a tight signal region
    #Arguments:
    #name             : name of the Selection.
    #bSel             : B Selection object
    #tightDMassWindow : Tight D Mass Window
    #parentB          : parent B (use either B0 or B+)

    #Several final states (FS) considered here for parentD
    #[1] FS = B^+ --> D^0 3h
    #[2] FS = B0 or Bs --> (D+ or D_s+) 3h
    #[3] FS = B0 or Bs --> D*+ 3h
    #[4] FS = B0 or Bs --> (D+ or D_s+) + (D- or D_s-)
    #[5] FS = B0 or Bs --> D*+ (D- or D_s-)


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
"""

# $Id$
'''
Comment goes here.
'''

__author__ = ['Mat Charles']
__date__ = '01/Jan/2014'
__version__ = '$Revision: 1.1 $'

# Stuff below needs updating
__all__ = ( 'XibStarBuilder'               ## LineBuilder class specialization
            , 'default_name'            ## Default name
            , 'default_config'          ## Default config
          )



from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdAllLoosePions, StdAllLooseKaons, StdAllLooseProtons
from StandardParticles import StdLoosePions
from CommonParticles.StdLooseLambda import StdLooseLambdaLL, StdLooseLambdaDD
from GaudiKernel.SystemOfUnits import MeV, GeV, mm
from Configurables import TisTosParticleTagger


default_name = "XibStarToXibZero"

# Should match __configuration_keys__
default_config = {
      'LongTrackGEC' : 300
      , 'prescaleSignalDefault'  : 1.0
      , 'prescaleSignalJpsi' : 1.0
      , 'prescaleControlHadronic' : 1.0
      , 'prescaleControlMuonic' : 1.0
      , 'XibStar_PT_Min'        : 2500.0*MeV
}


## These configuration parameters should not be changed outside of this file.
_my_immutable_config = {
        'DisplacedKaonInputList'         : 'Phys/StdLooseANNKaons/Particles'
        ,'DisplacedPionInputList'        : 'Phys/StdLooseANNPions/Particles'
        ,'DisplacedProtonInputList'      : 'Phys/StdLooseANNProtons/Particles'
        ,'SoftPions_InputList'           : 'Phys/StdAllNoPIDsPions/Particles'
        ,'DisplacedJpsiInputList'        : 'Phys/StdLooseJpsi2MuMu/Particles'
        ,'DisplacedD0InputList'          : 'Phys/StdLooseD02KPi/Particles'
        ,'DisplacedDpInputList'          : 'Phys/StdLooseDplus2KPiPi/Particles'
        ,'DisplacedLcInputList'          : 'Phys/StdLooseLambdac2PKPi/Particles'
        ,'DisplacedTrack_P_Min'          : 2.0*GeV
        ,'DisplacedTrack_PT_Min'         : 250.0*MeV
        ,'DisplacedTrack_TRCHI2DOF_Max'  : 5.0
        ,'DisplacedTrack_MIPCHI2DV_Min'  : -1.0
        ,'DisplacedTrack_MIPCHI2DV_Tight_Min'  : 4.0
        ,'DisplacedTrack_GhostProb_Max'  : 0.7
        ,'Proton_ProbNNp_Min'            : 0.02
        ,'Proton_ProbNNp_Tight_Min'      : 0.05
        ,'Proton_ProbNNp_VeryTight_Min'      : 0.1
        ,'Kaon_ProbNNk_Min'              : 0.02
        ,'Kaon_ProbNNk_Tight_Min'        : 0.04
        ,'Kaon_ProbNNk_VeryTight_Min'        : 0.1
        ,'Pion_ProbNNpi_Min'             : 0.01
        ,'Pion_ProbNNpi_Tight_Min'       : 0.04
        ,'SoftTrack_TRCHI2DOF_Max'     :   5.0
        ,'SoftTrack_PT_Min'            :   0.0*MeV
        ,'SoftTrack_P_Min'             :   1.0*GeV
        ,'SoftTrack_ProbNN_e_Max'      : 0.2
        ,'SoftTrack_MIPCHI2DV_Min'     :   -999.0
        ,'XibStar_VtxChisq_Max'        :  20.0
        ,'XibStar_DMCutLower'          : -25.0*MeV
        ,'XibStar_DMCutUpper'          :  50.0*MeV
        ,'XibStar_DMCutLower_SL'       :-250.0*MeV
        ,'XibStar_DMCutUpper_SL'       : 250.0*MeV
        ,'Xic_ADAMASS_HalfWin'         :  80.0*MeV
        ,'Xic_ADMASS_HalfWin'          :  50.0*MeV
        ,'Xic_BPVDIRA_Min'             :   0.9
        ,'Xic_BPVVDCHI2_Min'           :  25.0
        ,'Xib_BPVVDCHI2_Min'           :  36.0
        ,'Xib_VtxChisqPerDOF_Max'      :  10.0
        ,'Xib_VtxChisqPerDOF_Tight_Max':   7.0
        ,'Xib_BPVDIRA_Min'             :   0.98
        ,'Xib_BPVDIRA_Tight_Min'       :   0.995
        ,'Xib_CombCutPT_Min'           :2000.0*MeV
        }


class XibStarBuilder(LineBuilder) :
    """
    Look for Xib*- -> Xib0 pi-
    """
    
    # Should match default_config
    __configuration_keys__ = ( 'LongTrackGEC'
                               , 'prescaleSignalDefault'
                               , 'prescaleSignalJpsi'
                               , 'prescaleControlHadronic'
                               , 'prescaleControlMuonic'
                               , 'XibStar_PT_Min')

    ## Possible parameters and default values copied from the definition
    ##   of StrippingLine. The point of this little wrapper is that only
    ##   lines with non-zero prescale and postscale will get instantiated,
    ##   saving overhead from lines that will never select anything.
    def _strippingLine ( self,
                          name             ,   # the base name for the Line
                          prescale  = 1.0  ,   # prescale factor
                          ODIN      = None ,   # ODIN predicate
                          L0DU      = None ,   # L0DU predicate
                          HLT       = None ,   # HltDecReports predicate
                          FILTER    = None ,   # 'VOID'-predicate, e.g. Global Event Cut
                          checkPV   = True ,   # Check PV before running algos
                          algos     = None ,   # the list of stripping members
                          selection = None ,
                          postscale = 1.0    ,   # postscale factor
                          MaxCandidates = "Override",   # Maxumum number
                          MaxCombinations = "Override", # Maxumum number
                          HDRLocation = None ) : # other configuration parameters

        if (prescale > 0) and (postscale > 0) :
            line = StrippingLine( name,
                                  prescale        = prescale,
                                  ODIN            = ODIN,
                                  L0DU            = L0DU,
                                  HLT             = HLT,
                                  FILTER          = FILTER,
                                  checkPV         = checkPV,
                                  algos           = algos,
                                  selection       = selection,
                                  postscale       = postscale,
                                  MaxCandidates   = MaxCandidates,
                                  MaxCombinations = MaxCombinations,
                                  HDRLocation     = HDRLocation )
            self.registerLine(line)
            return line
        else : 
            return False

    ## Top-level routine to make the stripping lines
    def __init__(self, name, config) :

        LineBuilder.__init__(self, name, config)

        # Set up global event cuts.
        # Conceptually these come first, although the place where they're
        # inserted into the line is at the bottom of the code.
        _globalEventCutsCode = "(recSummary (LHCb.RecSummary.nLongTracks, 'Rec/Track/Long') < %(LongTrackGEC)s )" % config
        _globalEventCuts = { 'Code' : _globalEventCutsCode, 'Preambulo' : ["from LoKiTracks.decorators import *"]}

        # Pick up standard kaons, pions
        self.stdPi = DataOnDemand(Location = _my_immutable_config['DisplacedPionInputList'])
        self.stdK  = DataOnDemand(Location = _my_immutable_config['DisplacedKaonInputList'])
        self.stdP  = DataOnDemand(Location = _my_immutable_config['DisplacedProtonInputList'])
        # Apply some generic filtering
        self.baseFiltPi = filterDisplacedTracks(name+'BaseFilteredPions',   self.stdPi)
        self.baseFiltK  = filterDisplacedTracks(name+'BaseFilteredKaons',   self.stdK)
        self.baseFiltP  = filterDisplacedTracks(name+'BaseFilteredProtons', self.stdP)
        # Remove ghosts
        self.ghostFiltPi = filterGhosts(name+'GhostFilteredPions',   self.baseFiltPi)
        self.ghostFiltK  = filterGhosts(name+'GhostFilteredKaons',   self.baseFiltK)
        self.ghostFiltP  = filterGhosts(name+'GhostFilteredProtons', self.baseFiltP)
        # Filter them for use as daughter particles from a displaced vertex:
        self.dauPi = filterPions(name+'FilteredPions', self.ghostFiltPi)
        self.dauK  = filterKaons(name+'FilteredKaons', self.ghostFiltK)
        self.dauP  = filterProtons(name+'FilteredProtons', self.ghostFiltP)
        # Tighter cuts, when needed
        self.dauTightPi = filterPionsTight(name+'TightFilteredPions', self.dauPi)
        self.dauTightK  = filterKaonsTight(name+'TightFilteredKaons', self.dauK)
        self.dauTightP  = filterProtonsTight(name+'TightFilteredProtons', self.dauP)
        # Very tight cuts
        self.dauVeryTightP = filterProtonsVeryTight(name+'VeryTightFilteredProtons', self.dauTightP)
        self.dauVeryTightK = filterKaonsVeryTight(name+'VeryTightFilteredKaons', self.dauTightK)

        # Soft pions get special handling:
        self.dauSoftPi_init = DataOnDemand(Location = _my_immutable_config['SoftPions_InputList'])
        self.dauSoftPi_base = filterSoft(name+'FilteredSoftPionsBase', self.dauSoftPi_init)
        self.dauSoftPi = filterGhosts(name+'FilteredSoftPionsGP', self.dauSoftPi_base)

        # Make some useful composite daughters:
        self.stdD0   = DataOnDemand(Location = _my_immutable_config['DisplacedD0InputList'])
        self.stdDp   = DataOnDemand(Location = _my_immutable_config['DisplacedDpInputList'])
        self.stdLc   = DataOnDemand(Location = _my_immutable_config['DisplacedLcInputList'])

        # Filter the Lc to be tighter:
        _LcFiltCutBase = "( MINTREE('pi+'==ABSID, PROBNNpi) > 0.01 ) & ( MINTREE('K+'==ABSID, PROBNNk) > 0.02 ) & ( MINTREE('p+'==ABSID, PROBNNp) > 0.02 ) & ( PT > 2.0*GeV ) & (ADMASS('Lambda_c+')<50*MeV)"
        _LcFiltCutGhostPi = "( MAXTREE('pi+'==ABSID, TRGHOSTPROB) < %(DisplacedTrack_GhostProb_Max)s )" % _my_immutable_config
        _LcFiltCutGhostK  = "( MAXTREE('K+'==ABSID, TRGHOSTPROB) < %(DisplacedTrack_GhostProb_Max)s )" % _my_immutable_config
        _LcFiltCutGhostP  = "( MAXTREE('p+'==ABSID, TRGHOSTPROB) < %(DisplacedTrack_GhostProb_Max)s )" % _my_immutable_config
        _LcFiltCutGhost   = _LcFiltCutGhostPi + " & " + _LcFiltCutGhostK + " & " + _LcFiltCutGhostP
        _LcFiltCut = _LcFiltCutBase + " & " + _LcFiltCutGhost
        self.Lc = filterGeneric(name+"DisplacedLcFilt", self.stdLc, _LcFiltCut)
        # Even tighter:
        _LcTightFiltCut = "( MINTREE('K+'==ABSID, PROBNNk) > 0.05 ) & ( MINTREE('p+'==ABSID, PROBNNp) > 0.1 )"
        self.TightLc = filterGeneric(name+"DisplacedLcTightFilt", self.Lc, _LcTightFiltCut)

        # Filter the D0 and D+ to be tighter...
        _DFiltCutGenericBase = "( MINTREE('pi+'==ABSID, PROBNNpi) > 0.01 ) & ( MINTREE('K+'==ABSID, PROBNNk) > 0.02 ) & ( PT > 2.0*GeV )"
        _DFiltCutGhostPi = "( MAXTREE('pi+'==ABSID, TRGHOSTPROB) < %(DisplacedTrack_GhostProb_Max)s )" % _my_immutable_config
        _DFiltCutGhostK  = "( MAXTREE('K+'==ABSID, TRGHOSTPROB) < %(DisplacedTrack_GhostProb_Max)s )" % _my_immutable_config
        _DFiltCutGeneric = _DFiltCutGenericBase + " & " + _DFiltCutGhostPi + " & " + _DFiltCutGhostK
        _DpFiltCut = _DFiltCutGeneric + " & (ADMASS('D+')<50*MeV)"
        _D0FiltCut = _DFiltCutGeneric + " & (ADMASS('D0')<50*MeV)"
        self.D0 = filterGeneric(name+"DisplacedD0Filt", self.stdD0, _D0FiltCut)
        self.Dp = filterGeneric(name+"DisplacedDpFilt", self.stdDp, _DpFiltCut)
        
        # Build Xic+
        self.combineXicPlusToPKPi = makeXic(name+"CombineXicPlusToPKpi",  [ self.dauVeryTightP, self.dauVeryTightK, self.dauTightPi ], 30.0,  '[Xi_c+ -> p+ K- pi+]cc')
        self.Xicp = self.combineXicPlusToPKPi
        # Build Xic0
        self.combineXicZeroToPKKpi  = makeXic(name+"CombineXicZeroToPKKpi",  [ self.dauTightP, self.dauTightK, self.dauTightPi ], 60.0,  '[Xi_c0 -> p+ K- K- pi+]cc')
        self.Xic0 = self.combineXicZeroToPKKpi
        
        # Some very generic cuts for making Xib0:
        # Nominal mass: 5788 MeV (in the LHCb software at some point in time...)
        _strCutXib0Comb = "( APT > %(Xib_CombCutPT_Min)s ) & ( AMAXDOCA('')<0.5*mm ) & ( AM>3800.0*MeV ) & ( AM<7200.0*MeV )" % _my_immutable_config
        _strCutXib0MothLoose = "( BPVVDCHI2 > %(Xib_BPVVDCHI2_Min)s ) & ( VFASPF(VCHI2PDOF)<%(Xib_VtxChisqPerDOF_Max)s )" % _my_immutable_config
        _strCutXib0MothReg = _strCutXib0MothLoose + " & ( MM>5200.0*MeV ) & ( MM<6400.0*MeV ) & ( BPVDIRA > %(Xib_BPVDIRA_Min)s )" % _my_immutable_config
        _strCutXib0MothTight = "( BPVVDCHI2 > %(Xib_BPVVDCHI2_Min)s ) & ( VFASPF(VCHI2PDOF)<%(Xib_VtxChisqPerDOF_Tight_Max)s ) & ( MM>5500.0*MeV ) & ( MM<6100.0*MeV ) & ( BPVDIRA > %(Xib_BPVDIRA_Tight_Min)s )" % _my_immutable_config

        # Build the Xib0 decay to a range of different final states:

        # Line 5 uses: Xib0 -> D0 p+ K-
        _name5 = 'XibToD0PK'
        self.XibMode5  = makeGeneric(name+'Combine'+_name5, [ self.D0, self.dauTightP, self.dauTightK ], '[ Xi_b0 ->  D0 p+ K- ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 6 uses: Xib0 -> D+ p+ K- pi-
        _name6 = 'XibToDpPKPi'
        self.XibMode6  = makeGeneric(name+'Combine'+_name6, [ self.Dp, self.dauTightP, self.dauTightK, self.dauPi ], '[ Xi_b0 -> D+ p+ K- pi- ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 7 uses: Xib0 -> Lc+ K-
        _name7 = 'XibToLcK'
        self.XibMode7  = makeGeneric(name+'Combine'+_name7, [ self.Lc, self.dauTightK ], '[ Xi_b0 -> Lambda_c+ K- ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 8 uses: Xib0 -> Xic+ pi-
        _name8 = 'XibToXicpPi'
        self.XibMode8  = makeGeneric(name+'Combine'+_name8, [ self.Xicp, self.dauPi ], '[ Xi_b0 -> Xi_c+ pi- ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 9 uses: Xib0 -> Xib0 pi- pi+
        _name9 = 'XibToXic0PiPi'
        self.XibMode9  = makeGeneric(name+'Combine'+_name9, [ self.Xic0, self.dauPi ], '[ Xi_b0 -> Xi_c0 pi- pi+ ]cc', _strCutXib0Comb, _strCutXib0MothTight)
        # Line 13 uses: Xib0 -> Lc+ K- pi+ pi-
        _name13 = 'XibToLcKPiPi'
        self.XibMode13  = makeGeneric(name+'Combine'+_name13, [ self.TightLc, self.dauVeryTightK, self.dauPi ], '[ Xi_b0 -> Lambda_c+ K- pi+ pi- ]cc', _strCutXib0Comb, _strCutXib0MothTight)

        # Build Xib* -> Xib0 pi+
        self._makeLineSuiteForXibZeroPi(name, _name5,  [ self.XibMode5,  self.dauSoftPi ], config)
        self._makeLineSuiteForXibZeroPi(name, _name6,  [ self.XibMode6,  self.dauSoftPi ], config)
        self._makeLineSuiteForXibZeroPi(name, _name7,  [ self.XibMode7,  self.dauSoftPi ], config)
        self._makeLineSuiteForXibZeroPi(name, _name8,  [ self.XibMode8,  self.dauSoftPi ], config)
        self._makeLineSuiteForXibZeroPi(name, _name9,  [ self.XibMode9,  self.dauSoftPi ], config)
        self._makeLineSuiteForXibZeroPi(name, _name13, [ self.XibMode13, self.dauSoftPi ], config)

        # Build the control lines for Xib0:
        self.control5  = self._strippingLine(name = name+"ControlXib_"+_name5,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode5)
        self.control6  = self._strippingLine(name = name+"ControlXib_"+_name6,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode6)
        self.control7  = self._strippingLine(name = name+"ControlXib_"+_name7,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode7)
        self.control8  = self._strippingLine(name = name+"ControlXib_"+_name8,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode8)
        self.control9  = self._strippingLine(name = name+"ControlXib_"+_name9,  prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode9)
        self.control13 = self._strippingLine(name = name+"ControlXib_"+_name13, prescale = config['prescaleControlHadronic'], postscale = 1.0, FILTER = _globalEventCuts, selection = self.XibMode13)

    ## Helper routine: for each mode, make a few related lines:
    def _makeLineSuiteForXibZeroPi(self, localNameBase, localNameMode, inputSelections, config) :
        # Grab global event cuts
        _globalEventCutsCode = "(recSummary (LHCb.RecSummary.nLongTracks, 'Rec/Track/Long') < %(LongTrackGEC)s )" % config
        _globalEventCuts = { 'Code' : _globalEventCutsCode, 'Preambulo' : ["from LoKiTracks.decorators import *"]}    
        # Make the selections:
        selRS = makeXibStarRS(localNameBase+'CombineXibStarRS'+localNameMode, inputSelections, config['XibStar_PT_Min'])
        selWS = makeXibStarWS(localNameBase+'CombineXibStarWS'+localNameMode, inputSelections, config['XibStar_PT_Min'])
        # Make the stripping lines:
        lineRS = self._strippingLine(name=localNameBase+localNameMode, prescale = config['prescaleSignalDefault'], postscale = 1.0, FILTER = _globalEventCuts, selection = selRS)
        lineWS = self._strippingLine(name=localNameBase+localNameMode+'_WS', prescale = config['prescaleSignalDefault'], postscale = 1.0, FILTER = _globalEventCuts, selection = selWS)
        


def filterDisplacedTracks(localName, inputSelection, configDict = _my_immutable_config) :
    _strCut = "(TRCHI2DOF<%(DisplacedTrack_TRCHI2DOF_Max)s)" \
              "& (P>%(DisplacedTrack_P_Min)s)" \
              "& (PT>%(DisplacedTrack_PT_Min)s)" \
              "& (MIPCHI2DV(PRIMARY)>%(DisplacedTrack_MIPCHI2DV_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCut)

def filterKaons(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(HASRICH)&(PROBNNk>%(Kaon_ProbNNk_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterPions(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(HASRICH)&(PROBNNpi>%(Pion_ProbNNpi_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterProtons(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(HASRICH)&(PROBNNp>%(Proton_ProbNNp_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterKaonsTight(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNk>%(Kaon_ProbNNk_Tight_Min)s) & (MIPCHI2DV(PRIMARY)>%(DisplacedTrack_MIPCHI2DV_Tight_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterPionsTight(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNpi>%(Pion_ProbNNpi_Tight_Min)s) & (MIPCHI2DV(PRIMARY)>%(DisplacedTrack_MIPCHI2DV_Tight_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterProtonsTight(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNp>%(Proton_ProbNNp_Tight_Min)s) & (MIPCHI2DV(PRIMARY)>%(DisplacedTrack_MIPCHI2DV_Tight_Min)s)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterKaonsVeryTight(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNk>%(Kaon_ProbNNk_VeryTight_Min)s) & (PROBNNk>PROBNNpi)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterProtonsVeryTight(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutDau = "(PROBNNp>%(Proton_ProbNNp_VeryTight_Min)s) & (PROBNNp>PROBNNpi)" % configDict
    return filterGeneric(localName, inputSelection, _strCutDau)

def filterSoft(localName, inputSelection, configDict = _my_immutable_config) :
    _strCut = "(TRCHI2DOF<%(SoftTrack_TRCHI2DOF_Max)s)" \
              "& (P>%(SoftTrack_P_Min)s)" \
              "& (PT>%(SoftTrack_PT_Min)s)" \
              "& (PROBNNe<%(SoftTrack_ProbNN_e_Max)s)" \
              "& (MIPCHI2DV(PRIMARY)>%(SoftTrack_MIPCHI2DV_Min)s)" % configDict    
    return filterGeneric(localName, inputSelection, _strCut)

def filterGhosts(localName, inputSelection, configDict = _my_immutable_config) :
    _strCutGhost = '(TRGHOSTPROB<%(DisplacedTrack_GhostProb_Max)s)' % configDict
    return filterGeneric(localName, inputSelection, _strCutGhost)

def makeXic(localName, inputSelections, inputChisqCut, inputDecay, configDict = _my_immutable_config) :
    _strCutCombRest = "( ADAMASS('Xi_c0') < %(Xic_ADAMASS_HalfWin)s ) & ( APT > 1.0*GeV )" % configDict
    _strCutComb = _strCutCombRest
    _strCutMothChisq = "(VFASPF(VCHI2)<%(inputChisqCut)s)" % locals()
    _strCutMothRest  = "(ADMASS('Xi_c0') < %(Xic_ADMASS_HalfWin)s)" \
                       "& (BPVDIRA > %(Xic_BPVDIRA_Min)s)" % configDict
    _strCutMoth = _strCutMothChisq + '&' + _strCutMothRest
    _output = makeGeneric(localName, inputSelections, inputDecay, _strCutComb, _strCutMoth)
    return _output

# CDF measures: m(Xib*) - m(Xib) - m(pi) = 14.84 +- 0.74 +- 0.28 MeV
def makeXibStarBase(localName, inputSelections, decay, cutMothDM, XibStar_PT_Min, configDict = _my_immutable_config) :
    _cutComb = '(AM > 5000.0*MeV) & (AM < 8000.0*MeV)'
    _cutMothBase = '(VFASPF(VCHI2/VDOF) < %(XibStar_VtxChisq_Max)s)' % configDict
    _cutMothPT = '(PT > %(XibStar_PT_Min)s)' % locals()
    _cutMoth = '(' + _cutMothBase + ' & ' + cutMothDM + '&' + _cutMothPT + ')'
    _output = makeGeneric(localName, inputSelections, decay, _cutComb, _cutMoth)
    return _output

def makeXibStar(localName, inputSelections, decay, XibStar_PT_Min, configDict = _my_immutable_config) :
    _cutMothDM = '(MM - CHILD(MM,1) - CHILD(MM,2) > %(XibStar_DMCutLower)s) & (MM - CHILD(MM,1) - CHILD(MM,2) < %(XibStar_DMCutUpper)s)' % configDict
    _output = makeXibStarBase(localName, inputSelections, decay, _cutMothDM, XibStar_PT_Min, configDict)
    return _output

def makeXibStarRS(localName, inputSelections, XibStar_PT_Min, configDict = _my_immutable_config) :
    _decay = '[ Sigma_b- -> Xi_b0 pi- ]cc'
    _output = makeXibStar(localName, inputSelections, _decay, XibStar_PT_Min, configDict)
    return _output

def makeXibStarWS(localName, inputSelections, XibStar_PT_Min, configDict = _my_immutable_config) :
    _decay = '[ Sigma_b+ -> Xi_b0 pi+ ]cc'
    _output = makeXibStar(localName, inputSelections, _decay, XibStar_PT_Min, configDict)
    return _output

def filterGeneric(localName, inputSelection, filterString) :
    _filter = FilterDesktop(Code = filterString)
    return Selection(localName,
                     Algorithm = _filter,
                     RequiredSelections = [ inputSelection ] )


def makeGeneric(localName, inputSelections, decay, cutComb, cutMoth) :
    _combine = CombineParticles( DecayDescriptor = decay,
                                 CombinationCut = cutComb,
                                 MotherCut = cutMoth )
    return Selection ( localName,
                       Algorithm = _combine,
                       RequiredSelections = inputSelections)

def mergeLists(localName, inputSelections) :
    return MergedSelection ( localName,
                             RequiredSelections = inputSelections )

    

# =============================================================================
## @@file
#  Configuration of CalibTracking Lines
#  Get two tracks near the D0 mass
#  @@author Matthew Kenzie matthew.william.kenzie@cern.ch
#  @@date 2012-07-28
# =============================================================================
"""
 script to configure CalibTracking trigger lines
"""
# =============================================================================
__author__  = "Matthew Kenzie matthew.william.kenzie@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 0.01 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

# =============================================================================
# Note that tracking, kaons and pions get picked up from Hlt1SharedParticles
# =============================================================================

class Hlt1CalibTrackingLinesConf( HltLinesConfigurableUser ) :
  __slots__ = {  'ParticlePT'             : 600     # MeV
                ,'ParticleP'              : 4000    # MeV
                ,'TrackCHI2DOF'           : 2       # dimensionless
                ,'CombMaxDaughtPT'        : 900     # MeV
                ,'CombAPT'                : 1500    # MeV
                ,'CombDOCA'               : 0.2     # mm
                ,'CombVCHI2DOF'           : 10      # dimensionless
                ,'CombVCHI2DOFLoose'      : 10      # dimensionless
                ,'CombDIRA'               : 0.9     # dimensionless
                ,'CombTAU'                : 0.2     # ps
                ,'D0MassWinLoose'         : 150     # MeV
                ,'D0MassWin'              : 100     # MeV
                ,'B0MassWinLoose'         : 250     # MeV
                ,'B0MassWin'              : 200     # MeV
                ,'D0DetachedDaughtsIPCHI2': 9       # dimensionless 
                ,'D0DetachedIPCHI2'       : 9       # dimensionless
                ,'BsPhiGammaMassMinLoose' : 3350    # MeV
                ,'BsPhiGammaMassMaxLoose' : 6900    # MeV
                ,'BsPhiGammaMassMin'      : 3850    # MeV
                ,'BsPhiGammaMassMax'      : 6400    # MeV
                ,'PhiMassWinLoose'        : 50      # MeV
                ,'PhiMassWin'             : 30      # MeV
                ,'PhiMassWinTight'        : 20      # MeV
                ,'PhiPT'                  : 1800    # MeV
                ,'PhiPTLoose'             : 500     # MeV
                ,'PhiSumPT'               : 3000    # MeV
                ,'PhiIPCHI2'              : 16      # dimensionless
                ,'B0SUMPT'                : 4000    # MeV
                ,'B0PT'                   : 1000    # MeV
                ,'GAMMA_PT_MIN'           : 2000    # MeV
                ,'Velo_Qcut'              : 3       # dimensionless
                ,'TrNTHits'               : 16.
                ,'ValidateTT'             : False
      }

  def KPi_Unit( self, props ) :

    props['KaonCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['PionCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['KPiDecay']        = """'[phi(1020) -> K- pi+]cc'""" #'strings( [ \'[phi(1020) -> K- pi+]cc\', \'phi(1020) -> K+ K-\', \'phi(1020) -> pi+ pi-\' ] )'
    props['KPiCombCut']      = ("""( ( in_range( PDGM('D0') - %(D0MassWinLoose)s * MeV , AM , PDGM('D0')   + %(D0MassWinLoose)s * MeV ) )   """ +
                                """| ( in_range( PDGM('B0') - %(B0MassWinLoose)s * MeV , AM , PDGM('B_s0') + %(B0MassWinLoose)s * MeV ) ) ) """ +
                                """& (APT>%(CombAPT)s*MeV)  """ +
                                """& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV) """ +
                                """& (ACUTDOCA(%(CombDOCA)s*mm,''))""") %props
    props['KPiMothCut']      = ("""( ( in_range( PDGM('D0') - %(D0MassWin)s * MeV , M , PDGM('D0')   + %(D0MassWin)s * MeV ) )      """ +
                                """| ( in_range( PDGM('B0') - %(B0MassWin)s * MeV , M , PDGM('B_s0') + %(B0MassWin)s * MeV ) ) )  """ +
                                """& (BPVDIRA > %(CombDIRA)s)  """ +
                                """& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s)  """ +
                                """& (BPVLTIME()>%(CombTAU)s*ps)""") %props

    KPi_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD, PDGM, AM",
                      "from LoKiPhys.decorators import PT",
                      "KPiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(KPiDecay)s, %(KPiCombCut)s, %(KPiMothCut)s )" %props
                    ]

    KPi_LineCode = """
    TC_HLT1COMBINER( '',
                     KPiCombinationConf,
                     'Hlt1SharedKaons', %(KaonCuts)s ,
                     'Hlt1SharedPions', %(PionCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKPis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKPis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1CalibTrackingKPis' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_KPiUnit = HltUnit(
        'Hlt1CalibTrackingKPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = KPi_Preambulo,
        Code = KPi_LineCode
        )

    return hlt1CalibTrackingLine_KPiUnit

  def KK_Unit( self, props ) :

    # KK is special because need to also make phis out of it in various ways
    # lifetime and vertex cuts get put in later where relevant

    props['KaonCuts']   = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['KKDecay']    = """'phi(1020) -> K+ K-'"""
    props['KKCombCut']  = ("""( ( in_range( PDGM('D0')        - %(D0MassWinLoose)s  * MeV , AM , PDGM('D0')        + %(D0MassWinLoose)s  * MeV ) )  """ +
                           """| ( in_range( PDGM('phi(1020)') - %(PhiMassWinLoose)s * MeV , AM , PDGM('phi(1020)') + %(PhiMassWinLoose)s * MeV ) )   """ +

                           """| ( in_range( PDGM('B0')        - %(B0MassWinLoose)s  * MeV , AM , PDGM('B_s0')      + %(B0MassWinLoose)s  * MeV ) ) )  """ +
                           """& (APT>%(CombAPT)s*MeV)  """ +
                           """& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV)  """ +
                           """& (ACUTDOCA(%(CombDOCA)s*mm,''))""") %props
    props['KKMothCut']   = ("""( ( in_range( PDGM('D0')        - %(D0MassWin)s  * MeV , M , PDGM('D0')        + %(D0MassWin)s  * MeV ) )  """ +
                            """| ( in_range( PDGM('phi(1020)') - %(PhiMassWin)s * MeV , M , PDGM('phi(1020)') + %(PhiMassWin)s * MeV ) )  """ +
                            """| ( in_range( PDGM('B0')        - %(B0MassWin)s  * MeV , M , PDGM('B_s0')      + %(B0MassWin)s  * MeV ) ) )  """ +
                            """& (BPVDIRA > %(CombDIRA)s) """) %props

    KK_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD, PDGM, AM",
                     "from LoKiPhys.decorators import PT",
                     "KKCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(KKDecay)s, %(KKCombCut)s, %(KKMothCut)s )" %props
                    ]

    KK_LineCode = """
    TC_HLT1COMBINER( '',
                     KKCombinationConf,
                     'Hlt1SharedKaons', %(KaonCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKKs', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKKs',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1CalibTrackingKKs' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_KKUnit = HltUnit(
        'Hlt1CalibTrackingKKUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = KK_Preambulo,
        Code = KK_LineCode
        )

    return hlt1CalibTrackingLine_KKUnit

  def PiPi_Unit( self, props ) :

    props['PionCuts']      = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )"""%props
    props['PiPiDecay']     = """'phi(1020) -> pi+ pi-'"""
    props['PiPiCombCut']   = ("""( ( in_range( PDGM('D0') - %(D0MassWinLoose)s * MeV , AM , PDGM('D0')   + %(D0MassWinLoose)s * MeV ) )  """ +
                              """| ( in_range( PDGM('B0') - %(B0MassWinLoose)s * MeV , AM , PDGM('B_s0') + %(B0MassWinLoose)s * MeV ) ) )  """ +
                              """& (APT>%(CombAPT)s*MeV)  """ +
                              """& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV)  """ +
                              """& (ACUTDOCA(%(CombDOCA)s*mm,'')) """) %props
    props['PiPiMothCut']   = ("""( ( in_range( PDGM('D0') - %(D0MassWin)s * MeV , M , PDGM('D0')   + %(D0MassWin)s * MeV ) )  """ +
                              """| ( in_range( PDGM('B0') - %(B0MassWin)s * MeV , M , PDGM('B_s0') + %(B0MassWin)s * MeV ) ) )  """ +
                              """& (BPVDIRA > %(CombDIRA)s)  """ +
                              """& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s)  """ +
                              """& (BPVLTIME()>%(CombTAU)s*ps)""") %props

    PiPi_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD, PDGM, AM",
                       "from LoKiPhys.decorators import PT",
                       "PiPiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(PiPiDecay)s, %(PiPiCombCut)s, %(PiPiMothCut)s )" %props
                    ]

    PiPi_LineCode = """
    TC_HLT1COMBINER( '',
                     PiPiCombinationConf,
                     'Hlt1SharedPions', %(PionCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToPiPis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nPiPis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1CalibTrackingPiPis' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_PiPiUnit = HltUnit(
        'Hlt1CalibTrackingPiPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = PiPi_Preambulo,
        Code = PiPi_LineCode
        )

    return hlt1CalibTrackingLine_PiPiUnit

  def D2KPi_Unit( self, props ) :

    D2KPi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKPis' )
    >>  in_range( PDGM('D0') - %(D0MassWin)s * MeV , M , PDGM('D0') + %(D0MassWin)s * MeV )
    >>  tee ( monitor( TC_SIZE > 0, '# pass D2KPi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nD2KPis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingKPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_D2KPiUnit = HltUnit(
        'Hlt1CalibTrackingD2KPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = D2KPi_LineCode
        )

    return hlt1CalibTrackingLine_D2KPiUnit

  def D2KPiDetached_Unit( self, props ) :

    D2KPiDetached_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKPis' )
    >>  ( in_range( PDGM('D0') - %(D0MassWin)s * MeV , M , PDGM('D0') + %(D0MassWin)s * MeV ) & (MINTREE(BPVIPCHI2(),(ISBASIC & HASTRACK))>%(D0DetachedDaughtsIPCHI2)s) & (BPVIPCHI2()<%(D0DetachedIPCHI2)s) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass D2KPiDetached', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nD2KPiDetacheds',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingKPiDetachedDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_D2KPiDetachedUnit = HltUnit(
        'Hlt1CalibTrackingD2KPiDetachedUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = D2KPiDetached_LineCode
        )

    return hlt1CalibTrackingLine_D2KPiDetachedUnit

  def D2KK_Unit( self, props ) :

    D2KK_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKKs' )
    >>  ( in_range( PDGM('D0') - %(D0MassWin)s * MeV , M , PDGM('D0') + %(D0MassWin)s * MeV ) & (BPVLTIME()>%(CombTAU)s*ps) & (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass D2KK', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nD2KKs',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingKKDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_D2KKUnit = HltUnit(
        'Hlt1CalibTrackingD2KKUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = D2KK_LineCode
        )

    return hlt1CalibTrackingLine_D2KKUnit

  def D2PiPi_Unit( self, props ) :

    D2PiPi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingPiPis' )
    >>  in_range( PDGM('D0') - %(D0MassWin)s * MeV , M , PDGM('D0') + %(D0MassWin)s * MeV )
    >>  tee ( monitor( TC_SIZE > 0, '# pass D2PiPi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nD2PiPis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1CalibTrackingPiPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_D2PiPiUnit = HltUnit(
        'Hlt1CalibTrackingD2PiPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = D2PiPi_LineCode
        )

    return hlt1CalibTrackingLine_D2PiPiUnit

  def B2KPi_Unit( self, props ) :

    B2KPi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKPis' )
    >>  ( in_range( PDGM('B0') - %(B0MassWin)s * MeV , M , PDGM('B_s0') + %(B0MassWin)s * MeV ) & (SUMTREE(PT,(ISBASIC & HASTRACK))>%(B0SUMPT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass B2KPi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nB2KPis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1B2HH_LTUNB_KPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_B2KPiUnit = HltUnit(
        'Hlt1CalibTrackingB2KPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = B2KPi_LineCode
        )

    return hlt1CalibTrackingLine_B2KPiUnit

  def B2KK_Unit( self, props ) :

    B2KK_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKKs' )
    >>  ( in_range( PDGM('B0') - %(B0MassWin)s * MeV , M , PDGM('B_s0') + %(B0MassWin)s * MeV ) & (SUMTREE(PT,(ISBASIC & HASTRACK))>%(B0SUMPT)s*MeV) & (BPVLTIME()>%(CombTAU)s*ps) & (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass B2KK', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nB2KKs',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1B2HH_LTUNB_KKDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_B2KKUnit = HltUnit(
        'Hlt1CalibTrackingB2KKUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = B2KK_LineCode
        )

    return hlt1CalibTrackingLine_B2KKUnit

  def B2PiPi_Unit( self, props ) :

    B2PiPi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingPiPis' )
    >>  ( (DMASS('B0')>-%(B0MassWin)s*MeV) & (DMASS('B_s0')<%(B0MassWin)s*MeV) & (SUMTREE(PT,(ISBASIC & HASTRACK))>%(B0SUMPT)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass B2PiPi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nB2PiPis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1B2HH_LTUNB_PiPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_B2PiPiUnit = HltUnit(
        'Hlt1CalibTrackingB2PiPiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = B2PiPi_LineCode
        )

    return hlt1CalibTrackingLine_B2PiPiUnit

  def IncPhi_Unit( self, props ) :

    IncPhi_Preambulo = [ "from LoKiPhys.decorators import PT, IPCHI2, MINTREE" ]

    IncPhi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKKs' )
    >>  ( ( in_range( PDGM('phi(1020)') - %(PhiMassWin)s * MeV , M , PDGM('phi(1020)') + %(PhiMassWin)s * MeV ) ) & ( PT > %(PhiPT)s * MeV ) & (MINTREE(BPVIPCHI2(),(ISBASIC & HASTRACK))>%(PhiIPCHI2)s) & (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass IncPhi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nIncPhis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1IncPhiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_IncPhiUnit = HltUnit(
        'Hlt1CalibTrackingIncPhiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = IncPhi_Preambulo,
        Code = IncPhi_LineCode
        )

    return hlt1CalibTrackingLine_IncPhiUnit

  def LTUNBPhi_Unit( self, props ) :

    LTUNBPhi_LineCode = """
    SELECTION( 'Hlt1CalibTrackingKKs' )
    >>  ( ( in_range( PDGM('phi(1020)') - %(PhiMassWin)s * MeV , M , PDGM('phi(1020)') + %(PhiMassWin)s * MeV ) ) & ( PT > %(PhiPTLoose)s * MeV ) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass LTUNBPhi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nLTUNBPhis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1LTUNBPhis')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit

    hlt1CalibTrackingLine_LTUNBPhiUnit = HltUnit(
        'Hlt1CalibTrackingLTUNBPhiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Code = LTUNBPhi_LineCode
        )

    return hlt1CalibTrackingLine_LTUNBPhiUnit

  def B2PhiPhi_Unit( self, props ) :

    props['PhiCuts']        = "(PT > %(PhiPT)s * MeV)" % props
    props['PhiPhiDecay']    = "'B_s0 -> phi(1020) phi(1020)'"
    props['PhiPhiCombCut']  = "(in_range(PDGM('B0') - %(B0MassWinLoose)s * MeV , AM , PDGM('B_s0') + %(B0MassWinLoose)s * MeV))" % props
    props['PhiPhiMothCut']  = ("( (in_range(PDGM('B0') - %(B0MassWin)s * MeV , M , PDGM('B_s0') + %(B0MassWin)s * MeV)) " +
                               "& (SUMTREE(PT, 'phi(1020)' == ABSID) > %(PhiSumPT)s * MeV) " +
                               "& (BPVLTIME() > %(CombTAU)s * ps) " +
                               "& (VFASPF(VCHI2/VDOF) < %(CombVCHI2DOFLoose)s) )") % props

    B2PhiPhi_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD, AM",
                           "from LoKiPhys.decorators import PT",
                           "B2PhiPhiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(PhiPhiDecay)s, %(PhiPhiCombCut)s, %(PhiPhiMothCut)s )" % props ]

    B2PhiPhi_LineCode = """
    TC_HLT1COMBINER( '',
                     B2PhiPhiCombinationConf,
                     'Hlt1LTUNBPhis', %(PhiCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToB2PhiPhis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nB2PhiPhis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1B2PhiPhi_LTUNBDecision' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_B2PhiPhiUnit = HltUnit(
        'Hlt1CalibTrackingB2PhiPhiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = B2PhiPhi_Preambulo,
        Code = B2PhiPhi_LineCode
        )

    return hlt1CalibTrackingLine_B2PhiPhiUnit

  def Gamma_Unit( self, props ) :

    from HltTracking.Hlt1Tracking import L0CaloCandidates

    Gamma_Preambulo = [L0CaloCandidates('Photon')]

    Gamma_LineCode = """
    L0CaloCandidates
    >>  tee  ( monitor( TC_SIZE > 0, '# pass CaloCandidates', LoKi.Monitoring.ContextSvc ) )
    >>  tee  ( monitor( TC_SIZE    , 'nCaloCandidates' , LoKi.Monitoring.ContextSvc ) )
    >>  TC_L0CALOTOPARTICLES( 'gamma',  '', (PT>%(GAMMA_PT_MIN)s*MeV) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToPhotons', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nPhotons',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1B2PhiGamma_Photons')
    >>  ~TC_EMPTY
    """ % props

    from Configurables import LoKi__HltUnit as HltUnit

    hlt1CalibTrackingLine_GammaUnit = HltUnit(
        'Hlt1CalibTrackingGammaUnit',
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = Gamma_Preambulo,
        Code = Gamma_LineCode
        )

    return hlt1CalibTrackingLine_GammaUnit

  def B2PhiGamma_Unit( self, props ) :

    props['PhiCuts']          = ("((in_range( (PDGM('phi(1020)') - %(PhiMassWinTight)s) * MeV , M , (PDGM('phi(1020)') + %(PhiMassWinTight)s) * MeV)) "
                                  "& (PT > %(PhiPT)s * MeV))") % props
    props['GammaCuts']        = "ALL"
    props['PhiGammaDecay']    = "'B_s0 -> phi(1020) gamma'"
    props['PhiGammaCombCut']  = ("(in_range(%(BsPhiGammaMassMinLoose)s * MeV, AM ,%(BsPhiGammaMassMaxLoose)s * MeV)) "
                                 "& (ASUM(PT) > %(B0SUMPT)s * MeV) "
                                 "& (APT > %(B0PT)s * MeV)") % props
    props['PhiGammaMothCut']  = ("((in_range( %(BsPhiGammaMassMin)s  * MeV, M ,%(BsPhiGammaMassMax)s * MeV)) "
                                 "& (BPVLTIME() > %(CombTAU)s * ps) "
                                 "& (VFASPF(VCHI2/VDOF) < %(CombVCHI2DOFLoose)s) )") % props

    B2PhiGamma_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD, AM, PDGM",
                             "from LoKiPhys.decorators import PT",
                             "B2PhiGammaCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(PhiGammaDecay)s, %(PhiGammaCombCut)s, %(PhiGammaMothCut)s )" % props ]

    B2PhiGamma_LineCode = """
    TC_HLT1COMBINER( '',
                     B2PhiGammaCombinationConf,
                     'Hlt1LTUNBPhis',          %(PhiCuts)s   ,
                     'Hlt1B2PhiGamma_Photons', %(GammaCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToB2PhiGammas', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nB2PhiGammas',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1B2PhiGamma_LTUNBDecision' )
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D

    hlt1CalibTrackingLine_B2PhiGammaUnit = HltUnit(
        'Hlt1CalibTrackingB2PhiGammaUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = B2PhiGamma_Preambulo,
        Code = B2PhiGamma_LineCode
        )

    return hlt1CalibTrackingLine_B2PhiGammaUnit

  def build_line(self, name, algos, l0):

    from HltLine.HltLine import Hlt1Line

    line = Hlt1Line (
          name ,
          prescale = self.prescale ,
          postscale = self.postscale ,
          L0DU = l0 ,
          ODIN = '' ,
          algos = algos
          )

  def __apply_configuration__(self) :

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    gec = Hlt1GECUnit('Loose')

    from HltTracking.HltPVs import PV3D
    pvs = PV3D('Hlt1')

    from Hlt1SharedParticles import Hlt1SharedParticles
    sharedParticles = Hlt1SharedParticles()
    pions = sharedParticles.pionUnit()
    kaons = sharedParticles.kaonUnit()

    to_build = {  'CalibTrackingKPi'  :         [ gec,
                                                  pvs,
                                                  pions,
                                                  kaons,
                                                  self.KPi_Unit(self.getProps()),
                                                  self.D2KPi_Unit(self.getProps())
                                                ] ,
                  'CalibTrackingKPiDetached'  : [ gec,
                                                  pvs,
                                                  pions,
                                                  kaons,
                                                  self.KPi_Unit(self.getProps()),
                                                  self.D2KPiDetached_Unit(self.getProps())
                                                 ] ,
                  'CalibTrackingKK'   :          [ gec,
                                                   pvs,
                                                   kaons,
                                                   self.KK_Unit(self.getProps()),
                                                   self.D2KK_Unit(self.getProps())
                                                  ] ,
                  'CalibTrackingPiPi' :           [ gec,
                                                    pvs,
                                                    pions,
                                                    self.PiPi_Unit(self.getProps()),
                                                    self.D2PiPi_Unit(self.getProps())
                                                  ] ,
                  'B2HH_LTUNB_KPi'    :           [ gec,
                                                    pvs,
                                                    pions,
                                                    kaons,
                                                    self.KPi_Unit(self.getProps()),
                                                    self.B2KPi_Unit(self.getProps())
                                                  ] ,
                  'B2HH_LTUNB_KK'     :           [ gec,
                                                    pvs,
                                                    kaons,
                                                    self.KK_Unit(self.getProps()),
                                                    self.B2KK_Unit(self.getProps())
                                                  ] ,
                  'B2HH_LTUNB_PiPi'   :           [ gec,
                                                    pvs,
                                                    pions,
                                                    self.PiPi_Unit(self.getProps()),
                                                    self.B2PiPi_Unit(self.getProps())
                                                  ] ,
                  'IncPhi'            :           [ gec,
                                                    pvs,
                                                    kaons,
                                                    self.KK_Unit(self.getProps()),
                                                    self.IncPhi_Unit(self.getProps())
                                                  ] ,
                  'B2PhiPhi_LTUNB'    :           [ gec,
                                                    pvs,
                                                    kaons,
                                                    self.KK_Unit(self.getProps()),
                                                    self.LTUNBPhi_Unit(self.getProps()),
                                                    self.B2PhiPhi_Unit(self.getProps())
                                                  ] ,
                  'B2PhiGamma_LTUNB'  :           [ gec,
                                                    pvs,
                                                    kaons,
                                                    self.Gamma_Unit(self.getProps()),
                                                    self.KK_Unit(self.getProps()),
                                                    self.LTUNBPhi_Unit(self.getProps()),
                                                    self.B2PhiGamma_Unit(self.getProps())
                                                  ]
               }

    for line, algos in to_build.iteritems():
      l0 = "L0_DECISION_PHYSICS"
      if 'Gamma' in line:
        l0 = "|".join( [ "L0_CHANNEL('%s')" % channel for channel in ['Photon', 'Electron'] ] )
      self.build_line( line, algos, l0 )


# =============================================================================
## @@file
#  Configuration of SMOG Lines
#  Get two tracks near the D0 mass
#  @@author Matthew Kenzie matthew.william.kenzie@cern.ch
#  @@date 2012-07-28
# =============================================================================
"""
 script to configure SMOG trigger lines
"""
# =============================================================================
__author__  = "Matthew Kenzie matthew.william.kenzie@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 0.01 $"
# =============================================================================

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

# =============================================================================
# Note that tracking, kaons and pions get picked up from Hlt1SharedParticles
# =============================================================================

class Hlt1SMOGLinesConf( HltLinesConfigurableUser ) :
  __slots__ = {  'ParticlePT'             : 600     # MeV
                ,'ParticleP'              : 4000    # MeV
                ,'TrackCHI2DOF'           : 2       # dimensionless
                ,'CombMaxDaughtPT'        : 900     # MeV
                ,'CombDOCA'               : 0.2     # mm
                ,'CombVCHI2DOF'           : 10      # dimensionless
                ,'MassWinLoose'           : 150     # MeV
                ,'MassWin'                : 100     # MeV
                ,'GenericMassMinLoose'    : 800     # MeV
                ,'GenericMassMin'         : 1000    # MeV
                ,'GenericMaxDaughtPT'     : 800     # MeV
                ,'SingleTrackPT'          : 800    # MeV
      }

  def KPi_Unit( self, props ) :

    props['KaonCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['PionCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['KPiDecay']        = """'[D0 -> K- pi+]cc'""" 
    props['KPiCombCut']      = ("""( in_range( PDGM('D0') - %(MassWinLoose)s * MeV , AM , PDGM('D0')   + %(MassWinLoose)s * MeV ) )""" +
                                """& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV) """ +
                                """& (ACUTDOCA(%(CombDOCA)s*mm,''))""") %props
    props['KPiMothCut']      = ("""( in_range( PDGM('D0') - %(MassWin)s * MeV , M , PDGM('D0')   + %(MassWin)s * MeV ) )""" +
                                """& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s)  """) %props

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
    >>  SINK ('Hlt1SMOGKPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    #from HltTracking.HltPVs import PV3D  ## Shouldn't be required

    hlt1SMOGLine_KPiUnit = HltUnit(
        'Hlt1SMOGKPiUnit',
        #PVSelection = "PV3D", ## Shouldn't be required
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = KPi_Preambulo,
        Code = KPi_LineCode
        )

    return hlt1SMOGLine_KPiUnit

  def KPiPi_Unit( self, props ) :

    props['KaonCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['PionCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['KPiPiDecay']        = """'[D+ -> K- pi+ pi+]cc'""" 
    props['KPiPiCombCut']      = ("""( in_range( PDGM('D+') - %(MassWinLoose)s * MeV , AM , PDGM('D+')   + %(MassWinLoose)s * MeV ) )""" +
                                """& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV) """ +
                                """& (ACUTDOCA(%(CombDOCA)s*mm,''))""") %props
    props['KPiPiMothCut']      = ("""( in_range( PDGM('D+') - %(MassWin)s * MeV , M , PDGM('D+')   + %(MassWin)s * MeV ) )""" +
                                """& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s)  """) %props

    KPiPi_Preambulo = [ "from LoKiArrayFunctors.decorators import ADAMASS, ACUTDOCA, DAMASS, AMAXCHILD, PDGM, AM",
                      "from LoKiPhys.decorators import PT",
                      "KPiPiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(KPiPiDecay)s, %(KPiPiCombCut)s, %(KPiPiMothCut)s )" %props
                    ]

    KPiPi_LineCode = """
    TC_HLT1COMBINER( '',
                     KPiPiCombinationConf,
                     'Hlt1SharedKaons', %(KaonCuts)s ,
                     'Hlt1SharedPions', %(PionCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKPiPis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKPiPis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1SMOGKPiPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    #from HltTracking.HltPVs import PV3D  ## Shouldn't be required

    hlt1SMOGLine_KPiPiUnit = HltUnit(
        'Hlt1SMOGKPiPiUnit',
        #PVSelection = "PV3D", ## Shouldn't be required
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = KPiPi_Preambulo,
        Code = KPiPi_LineCode
        )

    return hlt1SMOGLine_KPiPiUnit

  def KKPi_Unit( self, props ) :

    props['KaonCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['PionCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['KKPiDecay']        = """'[D_s+ -> K+ K- pi+]cc'""" 
    props['KKPiCombCut']      = ("""( in_range( PDGM('D_s+') - %(MassWinLoose)s * MeV , AM , PDGM('D_s+')   + %(MassWinLoose)s * MeV ) )""" +
                                """& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV) """ +
                                """& (ACUTDOCA(%(CombDOCA)s*mm,''))""") %props
    props['KKPiMothCut']      = ("""( in_range( PDGM('D_s+') - %(MassWin)s * MeV , M , PDGM('D_s+')   + %(MassWin)s * MeV ) )""" +
                                """& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s)  """) %props

    KKPi_Preambulo = [ "from LoKiArrayFunctors.decorators import ADAMASS, ACUTDOCA, DAMASS, AMAXCHILD, PDGM, AM",
                      "from LoKiPhys.decorators import PT",
                      "KKPiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(KKPiDecay)s, %(KKPiCombCut)s, %(KKPiMothCut)s )" %props
                    ]

    KKPi_LineCode = """
    TC_HLT1COMBINER( '',
                     KKPiCombinationConf,
                     'Hlt1SharedKaons', %(KaonCuts)s ,
                     'Hlt1SharedPions', %(PionCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKKPis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKKPis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1SMOGKKPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    #from HltTracking.HltPVs import PV3D  ## Shouldn't be required

    hlt1SMOGLine_KKPiUnit = HltUnit(
        'Hlt1SMOGKKPiUnit',
        #PVSelection = "PV3D", ## Shouldn't be required
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = KKPi_Preambulo,
        Code = KKPi_LineCode
        )

    return hlt1SMOGLine_KKPiUnit

  def pKPi_Unit( self, props ) :

    props['ProtonCuts']      = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props 
    props['KaonCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['PionCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['pKPiDecay']       = """'[Lambda_c+ -> p+ K- pi+]cc'""" 
    props['pKPiCombCut']     = ("""( in_range( PDGM('Lambda_c+') - %(MassWinLoose)s * MeV , AM , PDGM('Lambda_c+')   + %(MassWinLoose)s * MeV ) )""" +
                                """& (AMAXCHILD(PT)>%(CombMaxDaughtPT)s*MeV) """ +
                                """& (ACUTDOCA(%(CombDOCA)s*mm,''))""") %props
    props['pKPiMothCut']     = ("""( in_range( PDGM('Lambda_c+') - %(MassWin)s * MeV , M , PDGM('Lambda_c+')   + %(MassWin)s * MeV ) )""" +
                                """& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s)  """) %props

    pKPi_Preambulo = [ "from LoKiArrayFunctors.decorators import ADAMASS, ACUTDOCA, DAMASS, AMAXCHILD, PDGM, AM",
                       "from LoKiPhys.decorators import PT",
                       "pKPiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(pKPiDecay)s, %(pKPiCombCut)s, %(pKPiMothCut)s )" %props
                     ]

    pKPi_LineCode = """
    TC_HLT1COMBINER( '',
                     pKPiCombinationConf,
                     'Hlt1SharedProtons', %(ProtonCuts)s ,
                     'Hlt1SharedKaons', %(KaonCuts)s ,
                     'Hlt1SharedPions', %(PionCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass TopKPis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'npKPis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1SMOGpKPiDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    #from HltTracking.HltPVs import PV3D  ## Shouldn't be required

    hlt1SMOGLine_pKPiUnit = HltUnit(
        'Hlt1SMOGpKPiUnit',
        #PVSelection = "PV3D", ## Shouldn't be required
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = pKPi_Preambulo,
        Code = pKPi_LineCode
        )

    return hlt1SMOGLine_pKPiUnit

  def GenericKPi_Unit( self, props ) :

    props['KaonCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['PionCuts']        = """( (PT>%(ParticlePT)s) & (P>%(ParticleP)s) & (TRCHI2DOF<%(TrackCHI2DOF)s) )""" %props
    props['KPiDecay']        = """'[D0 -> K- pi+]cc'""" 
    props['KPiCombCut']      = ("""( AM > %(GenericMassMinLoose)s * MeV ) """ +
                                """& (AMAXCHILD(PT)>%(GenericMaxDaughtPT)s*MeV) """ +
                                """& (ACUTDOCA(%(CombDOCA)s*mm,''))""") %props
    props['KPiMothCut']      = ("""( M > %(GenericMassMin)s * MeV ) """ +
                                """& (VFASPF(VCHI2/VDOF)<%(CombVCHI2DOF)s)  """) %props

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
    >>  SINK ('Hlt1SMOGGenericDecision')
    >>  ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    #from HltTracking.HltPVs import PV3D  ## Shouldn't be required

    hlt1SMOGLine_KPiUnit = HltUnit(
        'Hlt1SMOGGenericUnit',
        #PVSelection = "PV3D", ## Shouldn't be required
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = KPi_Preambulo,
        Code = KPi_LineCode
        )

    return hlt1SMOGLine_KPiUnit
  
  def SingleTrackUnit( self, props) :
    
    lineCode = """
    SELECTION( 'Hlt1SharedPions' )
    >> ( PT > %(SingleTrackPT)s * MeV )
    >> tee ( monitor( TC_SIZE > 0, '# pass SingleTrack', LoKi.Monitoring.ContextSvc ) )
    >> tee ( monitor( TC_SIZE, 'nSingleTracks',          LoKi.Monitoring.ContextSvc ) )
    >> SINK ( 'Hlt1SMOGSingleTrackDecision' )
    >> ~TC_EMPTY
    """ %props

    from Configurables import LoKi__HltUnit as HltUnit
    
    hlt1SMOGLine_SingleTrackUnit = HltUnit(
        'Hlt1SMOGSingleTrackUnit',
        #PVSelection = "PV3D", ## Shouldn't be required
        #OutputLevel = 1,
        Monitor = True,
        Code = lineCode
        )

    return hlt1SMOGLine_SingleTrackUnit
  
  def build_line(self, name, algos, l0):

    from HltLine.HltLine import Hlt1Line

    line = Hlt1Line (
          name ,
          prescale = self.prescale ,
          postscale = self.postscale ,
          L0DU = l0 ,
          ODIN = '(ODIN_BXTYP == LHCb.ODIN.Beam1)',
          algos = algos
          )

  def __apply_configuration__(self) :

    from Hlt1Lines.Hlt1GECs import Hlt1GECUnit
    gec = Hlt1GECUnit('Loose')

    #from HltTracking.HltPVs import PV3D
    #pvs = PV3D('Hlt1')

    from Hlt1SharedParticles import Hlt1SharedParticles
    sharedParticles = Hlt1SharedParticles()
    protos  = sharedParticles.protoParticleUnit()
    pions   = sharedParticles.pionUnit()
    kaons   = sharedParticles.kaonUnit()
    protons = sharedParticles.protonUnit()

    to_build = {  'SMOGKPi'  :         [ gec,
                                         #pvs,
                                         pions,
                                         kaons,
                                         self.KPi_Unit(self.getProps()),
                                         #self.D2KPi_Unit(self.getProps())
                                       ] ,
                  'SMOGKPiPi':         [ gec,
                                         #pvs,
                                         pions,
                                         kaons,
                                         self.KPiPi_Unit(self.getProps()),
                                         #self.D2KPiPi_Unit(self.getProps())
                                       ] ,
                  'SMOGKKPi' :         [ gec,
                                         #pvs,
                                         pions,
                                         kaons,
                                         self.KKPi_Unit(self.getProps()),
                                         #self.Ds2KKPi_Unit(self.getProps())
                                       ] ,
                  'SMOGpKPi' :         [ gec,
                                         #pvs,
                                         protons,
                                         pions,
                                         kaons,
                                         self.pKPi_Unit(self.getProps()),
                                         #self.Lc2pKPi_Unit(self.getProps())
                                       ] , 
                  'SMOGGeneric':       [ gec,
                                         pions,
                                         kaons,
                                         self.GenericKPi_Unit(self.getProps()),
                                       ] ,
                  'SMOGSingleTrack':   [ gec,
                                         pions,
                                         self.SingleTrackUnit(self.getProps())
                                       ]
               }

    for line, algos in to_build.iteritems():
      l0 = "L0_ALL"
      self.build_line( line, algos, l0 )


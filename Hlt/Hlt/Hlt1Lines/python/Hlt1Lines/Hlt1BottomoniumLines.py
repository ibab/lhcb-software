# =============================================================================
## @@file
#  Configuration of Bottomonium Lines
#  @@author Simone Stracka simone.stracka@cern.ch
#  @@date 2015-01-18
# =============================================================================
"""
 script to configure Bottomonium trigger lines
"""
# =============================================================================
__author__  = "Simone Stracka simone.stracka@cern.ch"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision$"
# =============================================================================

from HltLine.HltLinesConfigurableUser import HltLinesConfigurableUser

# =============================================================================
# Note that tracking, kaons and pions get picked up from Hlt1SharedParticles
# =============================================================================

class Hlt1BottomoniumLinesConf( HltLinesConfigurableUser ) :
  __slots__ = {
                  'PhiDauPT'             : 650     # MeV
                 ,'PhiDauP'              : 3000    # MeV
                 ,'PhiDauCHI2DOF'        : 3       # dimensionless
                 ,'PhiDauIPCHI2'         : 12
                  
                 ,'PhiCombMaxDaughtPT'        : 1500    # MeV
                 ,'PhiCombAPT'                : 1000    # MeV
                 ,'PhiCombDOCA'               : 0.1     # mm
                 ,'PhiCombVCHI2DOFLoose'      : 10      # dimensionless
                 
                 ,'PhiMassWinLoose'        : 50      # MeV
                 ,'PhiMassWin'             : 30      # MeV
                 ,'PhiMassWinTight'        : 30      # MeV

                 ,'PhiPT'                  : 1200    # MeV
                 ,'PhiPTLoose'             : 1000    # MeV
                 ,'PhiSumPT'               : 1000    # MeV

                 ,'Bb2PhiPhiAMassMin'             :  8500    # MeV
                 ,'Bb2PhiPhiAMassMax'             : 11000    # MeV
                 ,'Bb2PhiPhiMassMin'              :  8900    # MeV
                 ,'Bb2PhiPhiMassMax'              : 10500    # MeV

                 ,'KstDauPT'             : 1500    # MeV
                 ,'KstDauP'              : 5000    # MeV
                 ,'KstDauCHI2DOF'        : 2       # dimensionless  
                 ,'KstDauIPCHI2'         : 12      
                  
                 ,'KstCombMaxDaughtPT'        : 2000    # MeV
                 ,'KstCombAPT'                : 2000    # MeV
                 ,'KstCombDOCA'               : 0.1     # mm
                 ,'KstCombVCHI2DOFLoose'      : 9       # dimensionless

                 ,'KstMassWinLoose'        : 100      # MeV
                 ,'KstMassWin'             : 75       # MeV
                 ,'KstMassWinTight'        : 75       # MeV

                 ,'KstPT'                  : 2000     # MeV
                 ,'KstPTLoose'             : 2000     # MeV
                 ,'KstSumPT'               : 5000     # MeV

                 ,'Bb2KstKstAMassMin'             :  8500    # MeV
                 ,'Bb2KstKstAMassMax'             : 11000    # MeV
                 ,'Bb2KstKstMassMin'              :  8900    # MeV
                 ,'Bb2KstKstMassMax'              : 10000    # MeV                 
                                 
                 ,'l0'                     : 'L0_ALL'
                 
                 }

  def Phi_Unit( self, props ) :

    props['KaonCuts']   = """( (PT>%(PhiDauPT)s) & (P>%(PhiDauP)s) & (TRCHI2DOF<%(PhiDauCHI2DOF)s) & (BPVIPCHI2()<%(PhiDauIPCHI2)s) )""" %props

    props['KKDecay']    = """'phi(1020) -> K+ K-'"""

    props['KKCombCut']  = ("""(in_range( PDGM('phi(1020)')-%(PhiMassWinLoose)s*MeV, AM,PDGM('phi(1020)')+ %(PhiMassWinLoose)s *MeV)) """ +
                           """& (APT>%(PhiCombAPT)s*MeV)  """ +
                           """& (AMAXCHILD(PT)>%(PhiCombMaxDaughtPT)s*MeV)  """ +
                           """& (ACUTDOCA(%(PhiCombDOCA)s*mm,''))""") %props
    
    props['KKMothCut']   = ("""( in_range( PDGM('phi(1020)') - %(PhiMassWin)s * MeV , M , PDGM('phi(1020)') + %(PhiMassWin)s * MeV))""" ) %props
    
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
                    >>  SINK ( 'Hlt1BottomoniumKKs' )
                    >>  ~TC_EMPTY
                    """ %props
    
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D
    
    hlt1BottomoniumLine_KKUnit = HltUnit(
      'Hlt1BottomoniumKKUnit',
      PVSelection = "PV3D",
      #OutputLevel = 1,
      Monitor = True,
      Preambulo = KK_Preambulo,
      Code = KK_LineCode
      )
    
    return hlt1BottomoniumLine_KKUnit


  def Kst_Unit( self, props ) :

    props['KaonCuts']   = """( (PT>%(KstDauPT)s) & (P>%(KstDauP)s) & (TRCHI2DOF<%(KstDauCHI2DOF)s) & (BPVIPCHI2()<%(KstDauIPCHI2)s) )""" %props
    props['PionCuts']   = """( (PT>%(KstDauPT)s) & (P>%(KstDauP)s) & (TRCHI2DOF<%(KstDauCHI2DOF)s) & (BPVIPCHI2()<%(KstDauIPCHI2)s) )""" %props
    props['KPiDecay']   = """'[K*(892)0 -> K+ pi-]cc'"""

    props['KPiCombCut']  = ("""(in_range( PDGM('K*(892)0')-%(KstMassWinLoose)s*MeV,AM,PDGM('K*(892)0')+ %(KstMassWinLoose)s *MeV))""" +
                           """& (APT>%(KstCombAPT)s*MeV)  """ +
                           """& (AMAXCHILD(PT)>%(KstCombMaxDaughtPT)s*MeV)  """ +
                           """& (ACUTDOCA(%(KstCombDOCA)s*mm,''))""") %props
    
    props['KPiMothCut']   = ("""( in_range( PDGM('K*(892)0') - %(KstMassWin)s * MeV , M, PDGM('K*(892)0')+ %(KstMassWin)s * MeV))""" ) %props
    
    KPi_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD, PDGM, AM",
                      "from LoKiPhys.decorators import PT",
                      "KPiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf( %(KPiDecay)s, %(KPiCombCut)s, %(KPiMothCut)s )" %props
                     ]

    KPi_LineCode = """
    TC_HLT1COMBINER( '',
    KPiCombinationConf,
    'Hlt1SharedPions', %(PionCuts)s ,
    'Hlt1SharedKaons', %(KaonCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToKPis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nKPis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1BottomoniumKPis' )
    >>  ~TC_EMPTY
    """ %props
    
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D
    
    hlt1BottomoniumLine_KPiUnit = HltUnit(
      'Hlt1BottomoniumKPiUnit',
      PVSelection = "PV3D",
      #OutputLevel = 1,
      Monitor = True,
      Preambulo = KPi_Preambulo,
      Code = KPi_LineCode
      )
    
    return hlt1BottomoniumLine_KPiUnit


  def HighPTPhi_Unit( self, props ) :

    HighPTPhi_LineCode = """
    SELECTION( 'Hlt1BottomoniumKKs' ) 
    >>  ( ( in_range( PDGM('phi(1020)') - %(PhiMassWinTight)s * MeV , M , PDGM('phi(1020)') + %(PhiMassWinTight)s * MeV ) ) & ( PT > %(PhiPTLoose)s * MeV ) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass HighPTPhi', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nHighPTPhis',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1HighPTPhis')
    >>  ~TC_EMPTY
    """ %props
    
    from Configurables import LoKi__HltUnit as HltUnit
    
    hlt1BottomoniumLine_HighPTPhiUnit = HltUnit(
      'Hlt1BottomoniumHighPTPhiUnit',
      PVSelection = "PV3D",
      #OutputLevel = 1,
      Monitor = True,
      Code = HighPTPhi_LineCode
      )

    return hlt1BottomoniumLine_HighPTPhiUnit

  def HighPTKst_Unit( self, props ) :

    HighPTKst_LineCode = """
    SELECTION( 'Hlt1BottomoniumKPis' )
    >>  ( ( in_range( PDGM('K*(892)0') - %(KstMassWinTight)s * MeV , M , PDGM('K*(892)0') + %(KstMassWinTight)s * MeV ) ) & ( PT > %(KstPTLoose)s * MeV ) )
    >>  tee ( monitor( TC_SIZE > 0, '# pass HighPTKst', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE, 'nHighPTKsts',          LoKi.Monitoring.ContextSvc ) )
    >>  SINK ('Hlt1HighPTKsts')
    >>  ~TC_EMPTY
    """ %props
    
    from Configurables import LoKi__HltUnit as HltUnit
    
    hlt1BottomoniumLine_HighPTKstUnit = HltUnit(
      'Hlt1BottomoniumHighPTKstUnit',
      PVSelection = "PV3D",
      #OutputLevel = 1,
      Monitor = True,
      Code = HighPTKst_LineCode
      )

    return hlt1BottomoniumLine_HighPTKstUnit


  def Bb2PhiPhi_Unit( self, props ) :

    props['PhiCuts']        = "(PT > %(PhiPT)s * MeV)" % props
    props['PhiPhiDecay']    = "'J/psi(1S) -> phi(1020) phi(1020)'"
    props['PhiPhiCombCut']  = "( in_range( %(Bb2PhiPhiAMassMin)s * MeV , AM , %(Bb2PhiPhiAMassMax)s * MeV ) )" % props
    props['PhiPhiMothCut']  = ("( (in_range( %(Bb2PhiPhiMassMin)s * MeV , M , %(Bb2PhiPhiMassMax)s * MeV)) " +
                               "& (SUMTREE(PT, 'phi(1020)' == ABSID) > %(PhiSumPT)s * MeV) " +
                               "& (VFASPF(VCHI2/VDOF) < %(PhiCombVCHI2DOFLoose)s) )") % props

    Bb2PhiPhi_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD, AM",
                            "from LoKiPhys.decorators import PT",
                            "Bb2PhiPhiCombinationConf = LoKi.Hlt1.Hlt1CombinerConf(%(PhiPhiDecay)s,%(PhiPhiCombCut)s, %(PhiPhiMothCut)s )" % props ]

    Bb2PhiPhi_LineCode = """
    TC_HLT1COMBINER( '',
    Bb2PhiPhiCombinationConf,
    'Hlt1HighPTPhis', %(PhiCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToBb2PhiPhis', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nBb2PhiPhis',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1Bottomonium2PhiPhiDecision' )
    >>  ~TC_EMPTY
    """ %props
    
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D
    
    hlt1BottomoniumLine_Bb2PhiPhiUnit = HltUnit(
        'Hlt1BottomoniumBb2PhiPhiUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = Bb2PhiPhi_Preambulo,
        Code = Bb2PhiPhi_LineCode
        )

    return hlt1BottomoniumLine_Bb2PhiPhiUnit


  def Bb2KstKst_Unit( self, props ) :

    props['KstCuts']        = "(PT > %(KstPT)s * MeV)" % props
    props['KstKstDecay']    = "'J/psi(1S) -> K*(892)0 K*(892)~0'"
    props['KstKstCombCut']  = "( in_range( %(Bb2KstKstAMassMin)s * MeV , AM , %(Bb2KstKstAMassMax)s * MeV ) )" % props
    props['KstKstMothCut']  = ("( (in_range( %(Bb2KstKstMassMin)s * MeV , M , %(Bb2KstKstMassMax)s * MeV)) " +
                               "& (SUMTREE(PT, 'K*(892)0' == ABSID) > %(KstSumPT)s * MeV) " +
                               "& (VFASPF(VCHI2/VDOF) < %(KstCombVCHI2DOFLoose)s) )") % props

    Bb2KstKst_Preambulo = [ "from LoKiArrayFunctors.decorators import AP, APT, ADAMASS, ACUTDOCA, DAMASS, ASUM, AMAXCHILD, AM",
                            "from LoKiPhys.decorators import PT",
                            "Bb2KstKstCombinationConf = LoKi.Hlt1.Hlt1CombinerConf(%(KstKstDecay)s,%(KstKstCombCut)s, %(KstKstMothCut)s )" % props ]

    Bb2KstKst_LineCode = """
    TC_HLT1COMBINER( '',
    Bb2KstKstCombinationConf,
    'Hlt1HighPTKsts', %(KstCuts)s )
    >>  tee ( monitor( TC_SIZE > 0, '# pass ToBb2KstKsts', LoKi.Monitoring.ContextSvc ) )
    >>  tee ( monitor( TC_SIZE    , 'nBb2KstKsts',         LoKi.Monitoring.ContextSvc ) )
    >>  SINK ( 'Hlt1Bottomonium2KstarKstarDecision' )
    >>  ~TC_EMPTY
    """ %props
    
    from Configurables import LoKi__HltUnit as HltUnit
    from HltTracking.HltPVs import PV3D
    
    hlt1BottomoniumLine_Bb2KstKstUnit = HltUnit(
        'Hlt1BottomoniumBb2KstKstUnit',
        PVSelection = "PV3D",
        #OutputLevel = 1,
        Monitor = True,
        Preambulo = Bb2KstKst_Preambulo,
        Code = Bb2KstKst_LineCode
        )

    return hlt1BottomoniumLine_Bb2KstKstUnit

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

    to_build = {
      'Bottomonium2PhiPhi'    :   [ gec,
                                    pvs,
                                    kaons,
                                    self.Phi_Unit(self.getProps()),
                                    self.HighPTPhi_Unit(self.getProps()),
                                    self.Bb2PhiPhi_Unit(self.getProps())
                                    ] , 
      'Bottomonium2KstarKstar':   [ gec,
                                    pvs,
                                    kaons,
                                    pions,
                                    self.Kst_Unit(self.getProps()),
                                    self.HighPTKst_Unit(self.getProps()),
                                    self.Bb2KstKst_Unit(self.getProps())
                                    ] 
      }

    for line, algos in to_build.iteritems():
      l0 = "L0_ALL"
      self.build_line( line, algos, l0 )


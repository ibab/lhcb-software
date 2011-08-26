
__author__ = ['Phillip Urquijo, Alessandra Borgia']
__date__ = '08/05/2010'
__version__ = '$Revision: 1.4 $'


'''
B->Xu mu nu exclusive reconstruction in Xu=pi/rho/K/K* channels
'''
# =============================================================================
##
#  B->Xu mu nu exclusive reconstruction in Xu=pi/rho/K/K* channels
#
#  Stripping lines for charmless semileptonic B+/B0/Bs decays
#  to final states with hadron+muon, where the hadron is
#  either a charged track or decays to 2 or 3 charged tracks.
#  The charmless hadronic signal modes are:
#
#  B0  -> pi+ mu- nu, 
#  Bs0 -> K+  mu- nu, 
#  B-  -> rho0 mu- nu, 
#  Bs0 -> K*+ mu- nu, 
#
#  The lines will lead to measurements of SU3 symmetry tests
#  in B decays, and to the exclusive determination of Vub in
#  B+/B0/Bs decays. The priority is toward Bs decay
#  semileptonics.  
#
#  The nature of the topology leads to large backgrounds,
#  hence large data samples are required to accurately
#  determine the background. We anticipate that this analysis
#  will require of order 1fb-1.
#
#  Stripping XX, with requirements that the
#  rate <0.05% and timing <1ms/evt.
#  
#  LINES: Prescale [adimensional]
#  
#  Pi line: B0->pi+ mu- nu signal line.
#  Prescale = 0.02
#  
#  Pi SS line:: B0->pi+ mu+ nu Background to the pi line.
#  Prescale = 0.05
#  
#  K line: Bs0->K+ mu- nu signal line.
#  Prescale = 0.07
#  
#  K SS line:: Bs0->K+ mu+ nu Background to the K line.
#  Prescale = 0.06
#  
#  Rho line: B+->rho0(-> pi+pi-) mu- nu signal line through rho(770).
#  Prescale = 1.0
#  
#  Rho WS line: B+->rho0(->pi+pi+) mu- nu background line to the rho line.
#  Prescale= 1.0
#  
#  K* line: Bs0->K*+(Ks(-> pi+pi-)pi+) mu- nu signal line through K*(892).
#  Prescale= 1.0
#  
#  K* SS line: Bs0->K*-(Ks(-> pi+pi-)pi-) mu- nu background line to the K* line.
#  Prescale= 1.0
# =============================================================================
## 

"""
Stripping lines for charmless semileptonic B+/B0/Bs decays
to final states with hadron+muon, where the hadron is
either a charged track or decays to 2 or 3 charged tracks.
The charmless hadronic signal modes are:

B0  -> pi+ mu- nu, 
Bs0 -> K+  mu- nu, 
B+  -> rho0 mu+ nu, 
Bs0 -> K*+ mu- nu, 

The lines will lead to measurements of SU3 symmetry tests
in B decays, and to the exclusive determination of Vub in
B+/B0/Bs decays. The priority is toward Bs decay
semileptonics.  

The nature of the topology leads to large backgrounds,
hence large data samples are required to accurately
determine the background. We anticipate that this analysis
will require of order 1fb-1.

Stripping XX, with requirements that the
rate <0.05% and timing <1ms/evt.

LINES: Prescale [adimensional]
Pi line: B0->pi+ mu- nu signal line.
Prescale = 0.02

Pi SS line:: B0->pi+ mu+ nu Background to the pi line.
Prescale = 0.05

K line: Bs0->K+ mu- nu signal line.
Prescale = 0.07

K SS line:: Bs0->K+ mu+ nu Background to the K line.
Prescale = 0.06

Rho line: B+->rho0(-> pi+pi-) mu- nu signal line through rho(770).
Prescale = 1.0

Rho WS line: B+->rho0(->pi+pi+) mu- nu background line to the rho line.
Prescale= 1.0

K* line: Bs0->K*+(Ks(-> pi+pi-)pi+) mu- nu signal line through K*(892).
Prescale= 1.0

K* SS line: Bs0->K*-(Ks(-> pi+pi-)pi-) mu- nu background line to the K* line.
Prescale= 1.0
=============================================================================
 
Last modification $Date: 2011-07-21 $
               by $Author: aborgia $
"""

confdict= {
    #Muons
    "MuonTRCHI2"          : 4.   ,#adimensional
    "MuonP"               : 3000. ,#MeV
    "MuonPT"              : 1000. ,#MeV
    "MuonPIDK"            : 0.    ,#adimensional
    "MuonPIDmu"           : 3.    ,#adimensional
    "MuonPIDp"            : 0.    ,#adimensional
    #        "MuonMINIP"           : 0.05  ,#mm
    "MuonMINIPCHI2"       : 12     ,#adminensional
    #Xu
    #Pi Channel
    "PionTRCHI2"          : 6.   ,#adimensional
    "PionP"               : 3000. ,#MeV
    "PionPT"              : 800.  ,#MeV
    "PionPIDK"            : 0.    ,#adimensional
    "PionPIDmu"           : 0.    ,#adimensional
    "PionPIDp"            : 0.    ,#adimensional
    #        "PionMINIP"           : 0.05  ,#mm
    "PionMINIPCHI2"       : 16     ,#adminensional
    #K channel
    "KaonTRCHI2"          : 6.   ,#adimensional
    "KaonP"               : 3000. ,#MeV
    "KaonPT"              : 800.  ,#MeV
    "KaonPIDK"            : 5.    ,#adimensional // JUST CHANGED THIS
    "KaonPIDmu"           : 0.    ,#adimensional
    "KaonPIDp"            : 0.    ,#adimensional
    #        "KaonMINIP"          : 0.05  ,#mm
    "KaonMINIPCHI2"       : 16     ,#adminensional
    #Rho channel
    "RhoMassWindow"       : 150.  ,#MeV
    "RhoVCHI2DOF"         : 6    ,#adimensional
    "RhoPT"               : 500.  ,#MeV
    "RhoLeadingPionPT"    : 800.  ,#MeV
    #        "RhoMINIP"           : 0.05  ,#mm 
    "RhoMINIPCHI2"        : 4     ,#adimensional
    "RhoChPionPT"         : 300.  ,#MeV
    "RhoChPionTRCHI2"     : 10.  ,#MeV
    "RhoChPionPIDK"       : -10.   ,#adminensional
    "RhoFDCHI2"           : 100.  ,#adimensional
    "RhoFD"               : 6,#mm?
    "RhoIPMin"            : 0.3   ,#mm    
    "RhoDIRA"             : 0.9,
    #        "RhoChPionMINIP"     : 0.05  ,#mm
    "RhoChPionMINIPCHI2"  : 9     ,#adimensional
    #K* channel
    "KSMassWindow"        : 300.  ,#MeV
    "KSVCHI2DOF"          : 6    ,#adimensional
    "KSPT"                : 400.  ,#MeV
    #        "KSMINIP"             : 0.05  ,#mm 
    "KSMINIPCHI2"         : 4     ,#adimensional
    "KstarMassWindow"     : 100.  ,#MeV
    "KstarVCHI2DOF"       : 6    ,#adimensional
    "KstarPT"             : 400.  ,#MeV
    #        "KstarMINIP"          : 0.05  ,#mm 
    "KstarMINIPCHI2"      : 4     ,#adimensional
    "KstarChPionPT"       : 100.  ,#MeV
    "KstarChPionTRCHI2"   : 10.  ,#MeV
    "KstarChPionPIDK"     : -10.   ,#adminensional
    "KstarFDCHI2"         : 100,  #adiminseional
    "KstarFD"               : 5,#mm?
    "KstarIPMin"          : 0.13   ,#mm   
    #        "KstarChPionMINIP"    : 0.05  ,#mm
    "KstarChPionMINIPCHI2": 9     ,#adimensional
    "KstarLeadingPionPT"  : 800.  ,#MeV
    #Xu
    "PiMuMassLowLoose"    : 1230. ,#MeV
    "KMuMassLowTight"     : 1370. ,#MeV
    "RhoMuMassLowTight"   : 2000. ,#MeV
    "KstarMuMassLowTight" : 2000. ,#MeV
    #B
    "BVCHI2DOF"           : 4     ,#adminensional
    "BDIRA"               : 0.99,  #adminensional
    "BFDCHI2LOW"             : 15,   #adimensional
    "BFDCHI2HIGH"             : 225,   #adimensional
    "XMuMassUpper"        : 5500, # MeV
    "Enu"                 : 1850  #MeV
    }


from Gaudi.Configuration import *
from StrippingUtils.Utils import LineBuilder

import logging

name="B2XuMuNu"

class B2XuMuNuBuilder(LineBuilder):
    """
    Definition of B->Xu mu nu (Xu=pi,rho,k,k*) stripping
    """
    
    __configuration_keys__ = [
        #Muons
        "MuonTRCHI2"              ,      
        "MuonP"                   ,
        "MuonPT"                  ,
        "MuonPIDK"                ,
        "MuonPIDmu"               ,
        "MuonPIDp"                ,
        #        "MuonMINIP"      ,
        "MuonMINIPCHI2"           ,
        #Xu                       ,
        #Pi Channel               ,
        "PionTRCHI2"              ,
        "PionP"                   ,
        "PionPT"                  ,
        "PionPIDK"                ,
        "PionPIDmu"               ,
        "PionPIDp"                ,
        #        "PionMINIP"      ,
        "PionMINIPCHI2"           ,
        #K channel                ,
        "KaonTRCHI2"              ,
        "KaonP"                   ,
        "KaonPT"                  ,
        "KaonPIDK"                ,
        "KaonPIDmu"               ,
        "KaonPIDp"                ,
        #        "KaonMINIP"      ,
        "KaonMINIPCHI2"           ,
        #Rho channel              ,
        "RhoMassWindow"           ,
        "RhoVCHI2DOF"             ,
        "RhoPT"                   ,
        "RhoLeadingPionPT"        ,
        #        "RhoMINIP"       ,
        "RhoMINIPCHI2"            ,
        "RhoChPionPT"             ,
        "RhoChPionTRCHI2"         ,
        "RhoChPionPIDK"           ,
        #        "RhoChPionMINIP" , 
        "RhoChPionMINIPCHI2"      ,
        "RhoFDCHI2"               ,
        "RhoFD"                   ,
        "RhoIPMin"                ,
        "RhoDIRA"                 ,
        #K* channel               ,
        "KSMassWindow"            ,
        "KSVCHI2DOF"              ,
        "KSPT"                    ,
        #        "KSMINIP"        ,
        "KSMINIPCHI2"             ,
        "KstarMassWindow"         ,
        "KstarVCHI2DOF"           ,
        "KstarPT"                 ,
        #        "KstarMINIP"     ,
        "KstarMINIPCHI2"          ,
        "KstarChPionPT"           ,
        "KstarChPionTRCHI2"       ,
        "KstarChPionPIDK"         ,
        "KstarFDCHI2"             ,
        "KstarFD"                 ,
        "KstarIPMin"              ,
        #        "KstarChPionMINIP",  
        "KstarChPionMINIPCHI2"    ,
        "KstarLeadingPionPT"      ,
        #Xu                       ,
        "PiMuMassLowLoose"        ,
        "KMuMassLowTight"         ,
        "RhoMuMassLowTight"       ,
        "KstarMuMassLowTight"     ,
        #B                        ,
        "BVCHI2DOF"               ,
        "BDIRA"                   ,
        "BFDCHI2LOW"              ,
        "BFDCHI2HIGH"             ,
        "XMuMassUpper"            ,
        "Enu"
        ]

    def __init__(self,name,config):
        LineBuilder.__init__(self, name, config)

        self._muonSel=None
        self._pionSel=None
        self._kaonSel=None
        
        self._muonFilter()
        self._pionFilter()
        self._kaonFilter()

        
        self._rho770Sel=None
        self._rho770WSSel=None
        self._KshortSel=None
        self._KstarSel=None
        
        self._Rho02PiPiFilter()
        self._Rho02PiPiWSFilter()
        self._KS02PiPiFilter()
        self._Kstar2KSPiFilter()
        
        self.registerLine(self._Pi_line())
        self.registerLine(self._PiSS_line())
        self.registerLine(self._Rho_line())
        self.registerLine(self._RhoWS_line())
        self.registerLine(self._K_line())
        self.registerLine(self._KSS_line())
        self.registerLine(self._Kstar_line())
        self.registerLine(self._KstarSS_line())
        
    
    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (PIDmu-PIDpi> %(MuonPIDmu)s )"\
               "& (PIDmu-PIDp> %(MuonPIDp)s )"\
               "& (PIDmu-PIDK> %(MuonPIDK)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
    
    def _NominalPiSelection( self ):
        return "(TRCHI2DOF < %(PionTRCHI2)s )&  (P> %(PionP)s *MeV) &  (PT> %(PionPT)s *MeV)"\
               "& (PIDpi-PIDK> %(PionPIDK)s )& (PIDpi-PIDp> %(PionPIDp)s )& (PIDpi-PIDmu> %(PionPIDmu)s )"\
               "& (MIPCHI2DV(PRIMARY)> %(PionMINIPCHI2)s )"
    
    def _NominalKSelection( self ):
        return "(TRCHI2DOF < %(KaonTRCHI2)s )&  (P> %(KaonP)s *MeV) &  (PT> %(KaonPT)s *MeV)"\
               "& (PIDK-PIDpi> %(KaonPIDK)s )& (PIDK-PIDp> %(KaonPIDp)s )& (PIDK-PIDmu> %(KaonPIDmu)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(KaonMINIPCHI2)s )"

    def _Pi_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import DataOnDemand
        return StrippingLine(self._name+'Bd2PiLine',
                             # HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2TopoMu2BodyDecision') | HLT_PASS('Hlt2TopoMu3BodyDecision')", 
                             prescale = 0.02,
                             #FILTER = {'Code' :
                             #          """
                             #          ( recSummary ( LHCb.RecSummary.nSPDhits, 'Raw/Spd/Digits'  ) <  500 ) &
                             #          ( recSummary ( LHCb.RecSummary.nITClusters, 'Raw/IT/Clusters' ) < 1000 ) &
                             #          (recSummaryTrack(LHCb.RecSummary.nVeloTracks, TrVELO) <50  ) &
                             #          ( CONTAINS ( 'Rec/Track/Best'  ) <  400 ) 
                             #          """ ,
                             #          'Preambulo' : [ "from LoKiTracks.decorators import *",
                             #                          'from LoKiNumbers.decorators    import *',
                             #                          'from LoKiCore.functions    import *' ]
                             #          },
                             algos = [ self._Bd2PiMuNu()])
    
    def _PiSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bd2PiSSLine', prescale = 0.05,
                             #HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2TopoMu2BodyDecision') | HLT_PASS('Hlt2TopoMu3BodyDecision')", 
                            # FILTER = {'Code' :
                            #           """
                            #           ( recSummary ( LHCb.RecSummary.nSPDhits, 'Raw/Spd/Digits'  ) <  500 ) &
                            #           ( recSummary ( LHCb.RecSummary.nITClusters, 'Raw/IT/Clusters' ) < 1000 ) &
                            #           (recSummaryTrack(LHCb.RecSummary.nVeloTracks, TrVELO) <50  ) &
                            #           ( CONTAINS ( 'Rec/Track/Best'  ) <  400 ) 
                            #           """ ,
                            #           'Preambulo' : [ "from LoKiTracks.decorators import *",
                            #                           'from LoKiNumbers.decorators    import *',
                            #                           'from LoKiCore.functions    import *' ]
                            #           },
                             algos = [ self._Bd2PiMuNuSS()])

    def _Rho_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bu2RhoLine', prescale = 1.0,
                             #HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2TopoMu2BodyDecision') | HLT_PASS('Hlt2TopoMu3BodyDecision')", 
                             #FILTER = {'Code' :
                             #          """
                             #          ( recSummary ( LHCb.RecSummary.nSPDhits, 'Raw/Spd/Digits'  ) <  500 ) &
                             #          ( recSummary ( LHCb.RecSummary.nITClusters, 'Raw/IT/Clusters' ) < 1000 ) &
                             #          (recSummaryTrack(LHCb.RecSummary.nVeloTracks, TrVELO) <50  ) &
                             #          ( CONTAINS ( 'Rec/Track/Best'  ) <  400 ) 
                             #          """ ,
                             #          'Preambulo' : [ "from LoKiTracks.decorators import *",
                             #                          'from LoKiNumbers.decorators    import *',
                             #                          'from LoKiCore.functions    import *' ]
                             #          },
                             algos = [ self._Bu2RhoMuNu()])
        
    def _RhoWS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bu2RhoWSLine', prescale = 1.0,
                             #HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2TopoMu2BodyDecision') | HLT_PASS('Hlt2TopoMu3BodyDecision')", 
                             #FILTER = {'Code' :
                             #          """
                             #          ( recSummary ( LHCb.RecSummary.nSPDhits, 'Raw/Spd/Digits'  ) <  500 ) &
                             #          ( recSummary ( LHCb.RecSummary.nITClusters, 'Raw/IT/Clusters' ) < 1000 ) &
                             #          (recSummaryTrack(LHCb.RecSummary.nVeloTracks, TrVELO) <50  ) &
                             #          ( CONTAINS ( 'Rec/Track/Best'  ) <  400 ) 
                             #          """ ,
                             #          'Preambulo' : [ "from LoKiTracks.decorators import *",
                             #                          'from LoKiNumbers.decorators    import *',
                             #                          'from LoKiCore.functions    import *' ]
                             #          },
                             algos = [ self._Bu2RhoMuNuWS()])
    
    def _K_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bs2KLine', prescale = 0.07,
                             #HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2TopoMu2BodyDecision') | HLT_PASS('Hlt2TopoMu3BodyDecision')", 
                             #FILTER = {'Code' :
                             #          """
                             #          ( recSummary ( LHCb.RecSummary.nSPDhits, 'Raw/Spd/Digits'  ) <  500 ) &
                             #          ( recSummary ( LHCb.RecSummary.nITClusters, 'Raw/IT/Clusters' ) < 1000 ) &
                             #          (recSummaryTrack(LHCb.RecSummary.nVeloTracks, TrVELO) <50  ) &
                             #          ( CONTAINS ( 'Rec/Track/Best'  ) <  400 ) 
                             #          """ ,
                             #          'Preambulo' : [ "from LoKiTracks.decorators import *",
                             #                          'from LoKiNumbers.decorators    import *',
                             #                          'from LoKiCore.functions    import *' ]
                             #          },
                             algos = [ self._Bs2KMuNu()])
    
    def _KSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bs2KSSLine', prescale = 0.06,
                             #HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2TopoMu2BodyDecision') | HLT_PASS('Hlt2TopoMu3BodyDecision')", 
                             #FILTER = {'Code' :
                             #          """
                             #          ( recSummary ( LHCb.RecSummary.nSPDhits, 'Raw/Spd/Digits'  ) <  500 ) &
                             #          ( recSummary ( LHCb.RecSummary.nITClusters, 'Raw/IT/Clusters' ) < 1000 ) &
                             #          (recSummaryTrack(LHCb.RecSummary.nVeloTracks, TrVELO) <50  ) &
                             #          ( CONTAINS ( 'Rec/Track/Best'  ) <  400 ) 
                             #          """ ,
                             #          'Preambulo' : [ "from LoKiTracks.decorators import *",
                             #                          'from LoKiNumbers.decorators    import *',
                             #                          'from LoKiCore.functions    import *' ]
                             #          },
                             algos = [ self._Bs2KMuNuSS()])
    
    def _Kstar_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bs2KstarLine', prescale = 1.0,
                             #HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2TopoMu2BodyDecision') | HLT_PASS('Hlt2TopoMu3BodyDecision')", 
                             #FILTER = {'Code' :
                             #          """
                             #          ( recSummary ( LHCb.RecSummary.nSPDhits, 'Raw/Spd/Digits'  ) <  500 ) &
                             #          ( recSummary ( LHCb.RecSummary.nITClusters, 'Raw/IT/Clusters' ) < 1000 ) &
                             #          (recSummaryTrack(LHCb.RecSummary.nVeloTracks, TrVELO) <50  ) &
                             #          ( CONTAINS ( 'Rec/Track/Best'  ) <  400 ) 
                             #          """ ,
                             #          'Preambulo' : [ "from LoKiTracks.decorators import *",
                             #                          'from LoKiNumbers.decorators    import *',
                             #                          'from LoKiCore.functions    import *' ]
                             #          },
                             algos = [ self._Bs2KstarMuNu()])
    
    def _KstarSS_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        return StrippingLine(self._name+'Bs2KstarSSLine', prescale = 1.0,
                             #HLT = "HLT_PASS('Hlt2SingleMuonDecision') | HLT_PASS('Hlt2TopoMu2BodyDecision') | HLT_PASS('Hlt2TopoMu3BodyDecision')", 
                             #FILTER = {'Code' :
                             #          """
                             #          ( recSummary ( LHCb.RecSummary.nSPDhits, 'Raw/Spd/Digits'  ) <  500 ) &
                             #          ( recSummary ( LHCb.RecSummary.nITClusters, 'Raw/IT/Clusters' ) < 1000 ) &
                             #          (recSummaryTrack(LHCb.RecSummary.nVeloTracks, TrVELO) <50  ) &
                             #          ( CONTAINS ( 'Rec/Track/Best'  ) <  400 ) 
                             #          """ ,
                             #          'Preambulo' : [ "from LoKiTracks.decorators import *",
                             #                          'from LoKiNumbers.decorators    import *',
                             #                          'from LoKiCore.functions    import *' ]
                             #          },
                             algos = [ self._Bs2KstarMuNuSS()])



    def _muonFilter( self ):
        if self._muonSel is not None:
            return self._muonSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseMuons
        _mu = FilterDesktop( Code = self._NominalMuSelection() % self._config )
        _muSel=Selection("Mu_for"+self._name,
                         Algorithm=_mu,
                         RequiredSelections = [StdLooseMuons])
        
        self._muonSel=_muSel
        
        return _muSel

    
    def _pionFilter( self ):
        if self._pionSel is not None:
            return self._pionSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        _pi = FilterDesktop( Code = self._NominalPiSelection() % self._config )
        _piSel=Selection("Pi_for"+self._name,
                         Algorithm=_pi,
                         RequiredSelections = [StdLoosePions])
        
        self._pionSel=_piSel

        return _piSel

    def _kaonFilter( self ):
        if self._kaonSel is not None:
            return self._kaonSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseKaons
        
        _ka = FilterDesktop( Code = self._NominalKSelection() % self._config )
        _kaSel=Selection("K_for"+self._name,
                         Algorithm=_ka,
                         RequiredSelections = [StdLooseKaons])
        
        self._kaonSel=_kaSel
        
        return _kaSel

    def _Rho02PiPiFilter( self ):
        if self._rho770Sel is not None:
            return self._rho770Sel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        _rho770 = CombineParticles(
            DecayDescriptors = ["[rho(770)0 -> pi- pi+]cc"] ,
            DaughtersCuts   = {"pi+":"(PT> %(RhoChPionPT)s *MeV) &   (TRCHI2DOF < %(RhoChPionTRCHI2)s )"\
                               "& (MIPCHI2DV(PRIMARY)> %(RhoChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(RhoChPionPIDK)s )" % self._config
                               },
            CombinationCut = "(ADAMASS('rho(770)0')< %(RhoMassWindow)s)" % self._config,
            MotherCut       = "(MAXTREE('pi+'==ABSID,PT )>%(RhoLeadingPionPT)s *MeV ) & (VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
            "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s ) & (BPVVDCHI2 > %(RhoFDCHI2)s) & (BPVVD > %(RhoFD)s *mm)"\
            "& (BPVDIRA> %(RhoDIRA)s) & (BPVIP()> %(RhoIPMin)s *mm)" %self._config
            )
        _rho770Sel=Selection("Rho02PiPi_for"+self._name,
                             Algorithm=_rho770,
                             RequiredSelections = [StdLoosePions])
        
        self._rho770Sel=_rho770Sel
        
        return _rho770Sel

    def _Rho02PiPiWSFilter( self ):
        if self._rho770WSSel is not None:
            return self._rho770WSSel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        _rho770 = CombineParticles(
            DecayDescriptors = ["[rho(770)0 -> pi+ pi+]cc"] ,
            DaughtersCuts   = {"pi+":"(PT> %(RhoChPionPT)s *MeV) &   (TRCHI2DOF < %(RhoChPionTRCHI2)s )"\
                               "& (MIPCHI2DV(PRIMARY)> %(RhoChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(RhoChPionPIDK)s )" % self._config
                               },
            CombinationCut = "(ADAMASS('rho(770)0')< %(RhoMassWindow)s)" % self._config,
            MotherCut       = "(MAXTREE('pi+'==ABSID,PT )>%(RhoLeadingPionPT)s *MeV ) & (VFASPF(VCHI2/VDOF) < %(RhoVCHI2DOF)s ) & (PT > %(RhoPT)s *MeV) "\
            "& (MIPCHI2DV(PRIMARY)> %(RhoMINIPCHI2)s ) & (BPVVDCHI2 > %(RhoFDCHI2)s)  & (BPVVD > %(RhoFD)s *mm) & (BPVDIRA> %(RhoDIRA)s) "\
            "& (BPVIP()> %(RhoIPMin)s *mm)"%self._config
            )
        _rho770Sel=Selection("Rho02PiPiWS_for"+self._name,
                             Algorithm=_rho770,
                             RequiredSelections = [StdLoosePions])
        
        self._rho770WSSel=_rho770Sel
        return _rho770Sel

    def _KS02PiPiFilter( self, trail="" ):
        if self._KshortSel is not None:
            return self._KshortSel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions

        _KS = CombineParticles(
            DecayDescriptors = ["[KS0 -> pi- pi+]cc"] ,
            CombinationCut = "(ADAMASS('KS0')< %(KSMassWindow)s)" % self._config ,
            DaughtersCuts   = {"pi+":"(PT> %(KstarChPionPT)s *MeV) &  (TRCHI2DOF < %(KstarChPionTRCHI2)s )"\
                                   "& (MIPCHI2DV(PRIMARY)> %(KstarChPionMINIPCHI2)s ) & (PIDpi-PIDK> %(KstarChPionPIDK)s )" % self._config
                               },
            MotherCut       = "(VFASPF(VCHI2/VDOF) < %(KSVCHI2DOF)s ) & (PT > %(KSPT)s *MeV) "\
                              "& (MIPCHI2DV(PRIMARY)> %(KSMINIPCHI2)s )" % self._config
        )
        _KSSel=Selection("KS02PiPi_for"+self._name,
                         Algorithm=_KS,
                         RequiredSelections = [StdLoosePions])
        
        self._KshortSel=_KSSel
        return _KSSel

    def _Kstar2KSPiFilter( self, trail="" ):
        if self._KstarSel is not None:
            return self._KstarSel
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        _Kstar = CombineParticles(
            DecayDescriptors = ["[K*(892)+ -> KS0 pi+]cc"] ,
            CombinationCut = "(ADAMASS('K*(892)+')< %(KstarMassWindow)s)" % self._config,
            DaughtersCuts   = {"pi+":"(PT> %(KstarChPionPT)s *MeV) &  (TRCHI2DOF < %(KstarChPionTRCHI2)s )"\
                               "& (MIPCHI2DV(PRIMARY)>%(KstarChPionMINIPCHI2)s) & (PIDpi-PIDK>%(KstarChPionPIDK)s)" % self._config
                               },
            MotherCut       = "(PT > %(KstarPT)s *MeV) & (MAXTREE('pi+'==ABSID,PT)>%(KstarLeadingPionPT)s *MeV )"\
            "& (MIPCHI2DV(PRIMARY)> %(KstarMINIPCHI2)s) & (BPVVDCHI2 > %(KstarFDCHI2)s)"\
            "& (BPVVD > %(KstarFD)s *mm) & (VFASPF(VCHI2/VDOF) < %(KstarVCHI2DOF)s ) & (BPVIP()> %(KstarIPMin)s *mm)" % self._config 
            )
        
        _KstarSel=Selection("Kstar2KSPi_for"+self._name,
                            Algorithm=_Kstar,
                            RequiredSelections = [StdLoosePions, self._KS02PiPiFilter()])
        self._KstarSel=_KstarSel
        
        return _KstarSel
    
    def _Bd2PiMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _PiMu = CombineParticles(
            DecayDescriptors = ["[B~0 -> pi+ mu-]cc"],
            CombinationCut = "(AM>%(PiMuMassLowLoose)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config ,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVDCHI2> %(BFDCHI2LOW)s) & (BPVDIRA> %(BDIRA)s)" % self._config ,
            ReFitPVs = True
            )
        
        _PiMuSel=Selection("PiMu_for"+self._name,
                         Algorithm=_PiMu,
                         RequiredSelections = [self._muonFilter(), self._pionFilter()])
        return _PiMuSel

    def _Bd2PiMuNuSS( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _PiMu = CombineParticles(
            DecayDescriptors = ["[B~0 -> pi- mu-]cc"],
            CombinationCut = "(AM>%(PiMuMassLowLoose)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config ,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s ) & (BPVVDCHI2> %(BFDCHI2LOW)s) & (BPVDIRA> %(BDIRA)s)" % self._config ,
            ReFitPVs = True
            )
        
        _PiMuSel=Selection("PiMuSS_for"+self._name,
                         Algorithm=_PiMu,
                         RequiredSelections = [self._muonFilter(), self._pionFilter()])
        return _PiMuSel

    
    def _Bs2KMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KMu = CombineParticles(
            DecayDescriptors = ["[B_s~0 -> K+ mu-]cc"],
            CombinationCut = "(AM>%(KMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)" % self._config,
            ReFitPVs = True
            )
        
        _KMuSel=Selection("KMu_for"+self._name,
                         Algorithm=_KMu,
                         RequiredSelections = [self._muonFilter(), self._kaonFilter()])
        return _KMuSel
    
    def _Bs2KMuNuSS( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KMu = CombineParticles(
            DecayDescriptors = ["[B_s~0 -> K- mu-]cc"],
            CombinationCut = "(AM>%(KMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)" % self._config,
            ReFitPVs = True
            )
        
        _KMuSel=Selection("KMuSS_for"+self._name,
                         Algorithm=_KMu,
                         RequiredSelections = [self._muonFilter(), self._kaonFilter()])
        return _KMuSel
        
    def _Bu2RhoMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _RhoMu = CombineParticles(
            DecayDescriptors = ["[B+ -> rho(770)0 mu+]cc"],
            CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = " ( ((((5279.17*5279.17) -(MASS(1,2))*(MASS(1,2))))/(2*5279.17)) < %(Enu)s*MeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"% self._config,
            ReFitPVs = True
            )
        
        _RhoMuSel=Selection("RhoMu_for"+self._name,
                         Algorithm=_RhoMu,
                         RequiredSelections = [self._muonFilter(), self._Rho02PiPiFilter()])
        return _RhoMuSel

    def _Bu2RhoMuNuWS( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _RhoMu = CombineParticles(
            DecayDescriptors = ["[B+ -> rho(770)0 mu+]cc"],
            CombinationCut = "(AM>%(RhoMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = " ( ((((5279.17*5279.17) -(MASS(1,2))*(MASS(1,2))))/(2*5279.17)) < %(Enu)s*MeV) & (VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"% self._config,
            ReFitPVs = True
            )
        
        _RhoMuSel=Selection("RhoMuWS_for"+self._name,
                         Algorithm=_RhoMu,
                         RequiredSelections = [self._muonFilter(), self._Rho02PiPiWSFilter()])
        return _RhoMuSel

    def _Bs2KstarMuNu( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KstarMu = CombineParticles(
            DecayDescriptors = ["[B_s~0 -> K*(892)+ mu-]cc"],
            CombinationCut = "(AM>%(KstarMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)" % self._config,
            ReFitPVs = True
            )
        
        _KstarMuSel=Selection("KstarMu_for"+self._name,
                         Algorithm=_KstarMu,
                         RequiredSelections = [self._muonFilter(), self._Kstar2KSPiFilter()])
        
        return _KstarMuSel

    def _Bs2KstarMuNuSS( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _KstarMu = CombineParticles(
            DecayDescriptors = ["[B_s~0 -> K*(892)+ mu+]cc"],
            CombinationCut = "(AM>%(KstarMuMassLowTight)s*MeV) & (AM<%(XMuMassUpper)s*MeV)" % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)" % self._config,
            ReFitPVs = True
            )
        
        
        _KstarMuSel=Selection("KstarMuSS_for"+self._name,
                         Algorithm=_KstarMu,
                         RequiredSelections = [self._muonFilter(), self._Kstar2KSPiFilter()])
        
        return _KstarMuSel
    
    

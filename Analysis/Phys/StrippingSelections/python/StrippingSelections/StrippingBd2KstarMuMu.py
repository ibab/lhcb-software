__author__ = 'Patrick Koppenburg, Rob Lambert, Mitesh Patel'
__date__ = '21/01/2009'
__version__ = '$Revision: 1.8 $'

"""
Bd->K*MuMu selections 
"""
from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

class StrippingBd2KstarMuMuConf(LHCbConfigurableUser):
    """
    Definition of B->MuMuK* stripping

    @todo Now only new (low lumi) cuts in slots. The nominal lumi stripping lines
    by Rob Lambert are hard-coded.
    """
    
    __slots__ = { 
                   'BMassLow'           : 5050       # MeV, low mass cut
                ,  'BMassHighWin'       :  500       # MeV, high mass window
                ,  'BMassMedWin'        :  300       # MeV, high mass window
                ,  'BMassLowWin'        :  150       # MeV, high mass window
                ,  'BDIRA'              : 0.9999     # adimentional
                ,  'BFlightCHI2'        : 16         # adimentional 
                ,  'BFlightCHI2Tight'   : 100        # adimentional 
                ,  'BIPCHI2'            : 64         # adimentional  
                ,  'BIPCHI2Tight'       : 36         # adimentional  
                ,  'BVertexCHI2'        : 36         # adimentional
                ,  'BVertexCHI2Tight'   : 25         # adimentional
                ,  'KstarHighMass'      : 2500       # MeV
                ,  'IntDIRA'            : -0.95      # adimentional
                ,  'IntVertexCHI2Tight' : 25         # adimentional
                ,  'IntFlightCHI2'    : 9          # adimentional
                ,  'TrackChi2'          : 10         # adimentional
                   }

####################################################################################################
# Mitesh's selections. Stripping workshop December 2009
#
#####
# The signal line
#
    def _Early_Bd(self):
         """
         Common Bd for Eraly data selections

         @todo These should be coded in a smarter way.
         """
         from Configurables import CombineParticles
         Early_loose_Bd = CombineParticles("Early_Signal_Bd2KstarMuMu")
         Early_loose_Bd.InputLocations = ["StdLooseDiMuon", "StdVeryLooseDetachedKst2Kpi"]
         Early_loose_Bd.DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"
         Early_loose_Bd.DaughtersCuts = {
            'K*(892)0':  "(BPVDIRA> %(IntDIRA)s ) & (INTREE((ABSID=='pi+') & (TRCHI2DOF< %(TrackChi2)s ))) & (INTREE((ABSID=='K+') & (TRCHI2DOF< %(TrackChi2)s )))" % self.getProps() 
          , 'J/psi(1S)': "(BPVDIRA> %(IntDIRA)s ) & (2 == NINTREE((ABSID=='mu-') & (TRCHI2DOF< %(TrackChi2)s )))" % self.getProps()
            }
         Early_loose_Bd.CombinationCut = "(AM > %(BMassLow)s *MeV) & (ADAMASS('B0') < %(BMassHighWin)s *MeV)"  % self.getProps()
         Early_loose_Bd.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BVertexCHI2)s ) & (BPVDIRA> %(BDIRA)s ) & (BPVVDCHI2 > %(BFlightCHI2)s ) & (BPVIPCHI2() < %(BIPCHI2)s )"  % self.getProps()
         return Early_loose_Bd


    def Early_SignalLine(self):
        """
        The Signal line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember       
        from CommonParticles.StdVeryLooseDetachedKstar import StdVeryLooseDetachedKst2Kpi
        from CommonParticles.StdLooseDiMuon import StdLooseDiMuon
        return StrippingLine('Bd2KstarMuMu_Early_Signal'
                             , prescale = 1
                             , algos = [ StdLooseDiMuon,
                                         StdVeryLooseDetachedKst2Kpi,
                                         self._Early_Bd() ]
                             )

#####
# The same sign line
#
    def _Early_SameSignMuMu(self):
        """
        The MuMu algorithm for same sign dimuons: clone of standard DiMuon
        """
        from CommonParticles.StdLooseDiMuon import StdLooseDiMuon
        
        # this violates charge
        return StdLooseDiMuon.clone("SameSignDiMuonForBd2KstarMuMu",
                                    DecayDescriptor = "[J/psi(1S) -> mu+ mu+]cc")
        
    def _Early_SameSignBd(self):
        """
        The Bd algorithm for same sign dimuons : clone of Bd algorithm
        """
        return self._Early_Bd().clone("Early_SameSign_Bd2KstarMuMu",
                                      InputLocations = ["SameSignDiMuonForBd2KstarMuMu",
                                                        "StdVeryLooseDetachedKst2Kpi"])

    def Early_SameSignLine(self):
        """
        The same sign line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember       
        from CommonParticles.StdVeryLooseDetachedKstar import StdVeryLooseDetachedKst2Kpi
        return StrippingLine('Bd2KstarMuMu_Early_SameSign'
                             , prescale = 1
                             , algos = [ self._Early_SameSignMuMu(),
                                         StdVeryLooseDetachedKst2Kpi,
                                         self._Early_SameSignBd()]
                             )
    
#####
# The wide K* mass line
#
    def _Early_WideKstar(self):
        """
        The Wide K*
        """
        from CommonParticles.StdVeryLooseDetachedKstar import StdVeryLooseDetachedKst2Kpi
        return StdVeryLooseDetachedKst2Kpi.clone("WideKstar4Bd2KstarMuMu",
                                                 CombinationCut = "AM < %(KstarHighMass)s" % self.getProps(),
                                                 MotherCut = "(VFASPF(VCHI2/VDOF) < %(IntVertexCHI2Tight)s ) & ( BPVVDCHI2 > %(IntFlightCHI2)s )" % self.getProps() ) 
     
    def _Early_WideKstarBd(self):
        """
        The Bd with a wide K*
        """
        algo = self._Early_Bd().clone("Early_WideKstar_Bd2KstarMuMu",
                                      InputLocations = ["StdLooseDiMuon", "WideKstar4Bd2KstarMuMu"])
        jcuts = algo.DaughtersCuts['J/psi(1S)']
        jcuts = jcuts+" & (VFASPF(VCHI2/VDOF) < %(IntVertexCHI2Tight)s )" % self.getProps()
        algo.DaughtersCuts['J/psi(1S)'] = jcuts
        algo.CombinationCut = "(ADAMASS('B0') < %(BMassMedWin)s *MeV)"  % self.getProps()
        algo.MotherCut = "(VFASPF(VCHI2/VDOF) < %(BVertexCHI2Tight)s ) & (BPVDIRA> %(BDIRA)s ) & (BPVVDCHI2 > %(BFlightCHI2Tight)s ) & (BPVIPCHI2() < %(BIPCHI2Tight)s )"  % self.getProps()
        return algo
    
        
    def Early_WideKstarLine(self):
        """
        The Wide K* line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember       
        from CommonParticles.StdLooseDiMuon import StdLooseDiMuon
        return StrippingLine('Bd2KstarMuMu_Early_WideKstar'
                             , prescale = 1
                             , algos = [ StdLooseDiMuon ,
                                         self._Early_WideKstar(),
                                         self._Early_WideKstarBd() ]
                             )

#####
# The no muon-ID line

    def _Early_NoMuIDMuMu(self):
        """
        The MuMu algorithm for no MuID dimuons: clone of standard DiMuon
        """
        from CommonParticles.StdLooseDiMuon import StdLooseDiMuon
        
        # this violates charge
        return StdLooseDiMuon.clone("NoMuIDDiMuonForBd2KstarMuMu",
                                    InputLocations = ["StdLooseMuons",
                                                      "StdNoPIDsPions"],
                                    DecayDescriptor = "[J/psi(1S) -> pi+ mu-]cc")
        
    def _Early_NoMuIDBd(self):
        """
        The Bd algorithm for no MuID dimuons : clone of Bd algorithm
        """
        algo = self._Early_WideKstarBd().clone("Early_NoMuID_Bd2KstarMuMu",
                                               InputLocations = ["NoMuIDDiMuonForBd2KstarMuMu",
                                                                 "StdVeryLooseDetachedKst2Kpi"])
        algo.DaughtersCuts['J/psi(1S)'] =  "(BPVDIRA> %(IntDIRA)s ) & (INTREE((ABSID=='mu-') & (TRCHI2DOF< %(TrackChi2)s ))) & (INTREE((ABSID=='pi+') & (TRCHI2DOF< %(TrackChi2)s ))) & ( BPVVDCHI2 > %(IntFlightCHI2)s )" % self.getProps()
        algo.CombinationCut = "(ADAMASS('B0') < %(BMassLowWin)s *MeV)"  % self.getProps()
        return algo

    def Early_NoMuIDLine(self):
        """
        The no MuID line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember       
        from CommonParticles.StdVeryLooseDetachedKstar import StdVeryLooseDetachedKst2Kpi
        return StrippingLine('Bd2KstarMuMu_Early_NoMuID'
                             , prescale = 0.2   # lower prescale
                             , algos = [ self._Early_NoMuIDMuMu(),
                                         StdVeryLooseDetachedKst2Kpi,
                                         self._Early_NoMuIDBd()]
                             )
    
#####
# The e-mu line

    def _Early_eMu(self):
        """
        The MuMu algorithm for eMu: clone of standard DiMuon
        """
        from CommonParticles.StdLooseDiMuon import StdLooseDiMuon
        
        # this violates charge
        return StdLooseDiMuon.clone("MuonEleForBd2KstarMuMu",
                                    InputLocations = ["StdLooseMuons",
                                                      "StdLooseElectrons"],
                                    DecayDescriptor = "[J/psi(1S) -> e+ mu-]cc")
        
    def _Early_eMuBd(self):
        """
        The Bd algorithm for eMu : clone of Bd algorithm
        """
        algo = self._Early_Bd().clone("Early_eMu_Bd2KstarMuMu",
                                      InputLocations = ["MuonEleForBd2KstarMuMu",
                                                        "StdVeryLooseDetachedKst2Kpi"])
        algo.DaughtersCuts['J/psi(1S)'] = "(BPVDIRA> %(IntDIRA)s ) & (INTREE((ABSID=='mu-') & (TRCHI2DOF< %(TrackChi2)s ))) & (INTREE((ABSID=='e+') & (TRCHI2DOF< %(TrackChi2)s ))) & ( BPVVDCHI2 > %(IntFlightCHI2)s )"  % self.getProps()
        
        return algo

    def Early_eMuLine(self):
        """
        The eMu line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember       
        from CommonParticles.StdVeryLooseDetachedKstar import StdVeryLooseDetachedKst2Kpi
        return StrippingLine('Bd2KstarMuMu_Early_eMu'
                             , prescale = 1
                             , algos = [ self._Early_eMu(),
                                         StdVeryLooseDetachedKst2Kpi,
                                         self._Early_eMuBd()]
                             )

#
####################################################################################################
# Rob's selections. See RDWG talk, 08.07.2009, Rob Lambert
#
    def _Strip_loose_Bd2KstarMuMu(self):
        """
        CombineParticles for Rob's loose B->MuMuK*. This is the common loosest set of cuts.
        """
        from Configurables import CombineParticles
        
        Strip_loose_Bd2KstarMuMu = CombineParticles("Strip_loose_Bd2KstarMuMu")
        
        Strip_loose_Bd2KstarMuMu.InputLocations = ["StdLooseDiMuon", "StdLooseDetachedKst2Kpi"]
        Strip_loose_Bd2KstarMuMu.DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"
        Strip_loose_Bd2KstarMuMu.DaughtersCuts = {
            'K*(892)0':  ("(BPVVDZ>-50*mm) & "+
                          "(INTREE((ABSID=='pi+') & ((PIDpi-PIDmu)>-10) & ((PIDpi-PIDK)>-10))) &"+
                          "(INTREE((ABSID=='K+') & ((PIDK-PIDmu)>-10) & ((PIDK-PIDpi)>-5)))" ),
            'J/psi(1S)' : "(2 == NINTREE((ABSID=='mu+') & ((PIDmu-PIDpi)>-10) & ((PIDmu-PIDK)>-10) & (MIPCHI2DV(PRIMARY)>0)) )"
            }
        Strip_loose_Bd2KstarMuMu.CombinationCut = "(ADAMASS('B0')<300*MeV)"
        Strip_loose_Bd2KstarMuMu.MotherCut=( "(MIPDV(PRIMARY)<0.08*mm) & (MIPCHI2DV(PRIMARY) < 150) & (PT>300*MeV) & (BPVDIRA>0.9998) & "+
                                             "(VFASPF(VCHI2/VDOF)<64) & (DMASS('B0')<250*MeV) & (DMASS('B0')>-150*MeV)"
                                             )

        return Strip_loose_Bd2KstarMuMu


    def _ForNominal_Bd2KstarMuMu_High(self):
        """
        Loosest filter
        """
        from Configurables import FilterDesktop
        ForNominal_Bd2KstarMuMu_High = FilterDesktop("ForNominal_Bd2KstarMuMu_High")
        ForNominal_Bd2KstarMuMu_High.InputLocations = ["Strip_loose_Bd2KstarMuMu"]
        ForNominal_Bd2KstarMuMu_High.Code=( "(INTREE((ABSID=='K+') & (TRCHI2DOF < 36))) & "+
                                        "(INTREE((ABSID=='pi+') & (TRCHI2DOF < 36))) & "+
                                        "(2 == NINTREE((ABSID=='mu+') & (MIPCHI2DV(PRIMARY)>4) & (TRCHI2DOF < 16))) & "+
                                        "(INTREE((ABSID=='K*(892)0') & (BPVVDCHI2>25) & (VFASPF(VCHI2/VDOF)<36))) & "+
                                        "(PT>300*MeV) & (BPVDIRA>0.9998) & (BPVVDCHI2>36) & (VFASPF(VCHI2/VDOF)<36) "
                                        )
        return ForNominal_Bd2KstarMuMu_High

    def _ForNominal_Bd2KstarMuMu_Med(self):
        """
        Medium filter
        """
        from Configurables import FilterDesktop
        ForNominal_Bd2KstarMuMu_Med = FilterDesktop("ForNominal_Bd2KstarMuMu_Med")
        ForNominal_Bd2KstarMuMu_Med.InputLocations = ["ForNominal_Bd2KstarMuMu_High"]
        ForNominal_Bd2KstarMuMu_Med.Code = ( "(INTREE((ABSID=='K+') & ((PIDK-PIDpi)>-5) & ((PIDK-PIDmu)>-5) & (TRCHI2DOF < 9))) & "+
                                         "(2 == NINTREE((ABSID=='mu+') & ((PIDmu-PIDpi)>-5) & (TRCHI2DOF < 9))) & "+
                                         "(INTREE((ABSID=='pi+') & (TRCHI2DOF < 16))) & "+
                                         "(INTREE((ABSID=='K*(892)0') & (BPVVDZ>-30*mm) & (VFASPF(VCHI2/VDOF)<16) & (PT>300*MeV))) & "+
                                         "(INTREE((ABSID=='J/psi(1S)') & (BPVVDCHI2>25) )) & "+
                                         "(BPVDIRA>0.9999) & (BPVVDCHI2>64) & (MIPCHI2DV(PRIMARY) < 36) & (VFASPF(VCHI2/VDOF)<16) "
                                         )
        return ForNominal_Bd2KstarMuMu_Med
    
    def _ForNominal_Bd2KstarMuMu_Low(self):
        """
        Hard filter
        """
        from Configurables import FilterDesktop
        ForNominal_Bd2KstarMuMu_Low = FilterDesktop("ForNominal_Bd2KstarMuMu_Low")
        ForNominal_Bd2KstarMuMu_Low.InputLocations = ["Strip_loose_Bd2KstarMuMu"]
        ForNominal_Bd2KstarMuMu_Low.Code = ( "(INTREE((ABSID=='K+') & ((PIDK-PIDpi)>-5) & ((PIDK-PIDmu)>-5))) & "+
                                         "(2 == NINTREE((ABSID=='mu+') & ((PIDmu-PIDpi)>-5) & (MIPDV(PRIMARY) > 0.08*mm))) & "+
                                         "(INTREE((ABSID=='K*(892)0') & (BPVVDZ>-5*mm) & (PT>1000*MeV) & (MIPDV(PRIMARY) > 0.06*mm))) & "+
                                         "(BPVDIRA>0.99995) & (VFASPF(VMINVDDV(PRIMARY))>1*mm) & (MIPDV(PRIMARY) < 0.06*mm)"
                                         )
        return ForNominal_Bd2KstarMuMu_Low
        

    def line_for_nominal_high(self):
        """
        The '10 Hz' line
        """
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember       
        return StrippingLine('Bd2KstarMuMu_ForNominal_High'
                             , prescale = 1
                             , algos = [ self._Strip_loose_Bd2KstarMuMu(),
                                         self._ForNominal_Bd2KstarMuMu_High() ]
                             )

    def line_for_nominal_med(self):
        """
        The '2 Hz' line
        """
        from Configurables import FilterDesktop
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember

         
        return self.line_for_nominal_high().clone("Bd2KstarMuMu_ForNominal_Med"
                                              , algos = [ self._Strip_loose_Bd2KstarMuMu(),
                                                          self._ForNominal_Bd2KstarMuMu_High(),
                                                          self._ForNominal_Bd2KstarMuMu_Med() ]
                                              )
    
    def line_for_nominal_low(self):
        """
        The '1 Hz' line
        """
        from Configurables import FilterDesktop
        from StrippingConf.StrippingLine import StrippingLine, StrippingMember

        return self.line_for_nominal_high().clone("Bd2KstarMuMu_ForNominal_Low"
                                             , algos = [ self._Strip_loose_Bd2KstarMuMu(),
                                                         self._ForNominal_Bd2KstarMuMu_Low() ]
                                             )

    def getProps(self) :
        """
        From HltLinesConfigurableUser
        @todo Should be shared between Hlt and stripping
        """
        d = dict()
        for (k,v) in self.getDefaultProperties().iteritems() :
            d[k] = getattr(self,k) if hasattr(self,k) else v
        return d

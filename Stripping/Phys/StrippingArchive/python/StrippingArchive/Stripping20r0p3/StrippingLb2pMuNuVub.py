
__author__ = ['William Sutcliffe']
__date__ = '19/01/2014'
__version__ = '$Revision: 1.0 $'

'''
Lb->p mu nu exclusive reconstruction
'''
# =============================================================================
##
#  Lb->p mu nu exclusive reconstruction. 
#
#  Stripping lines for the charmless semileptonic decay Lambda_b -> p mu nu.
#
#  The lines will lead to a  measurement of the differential branching
#  of Lb -> p mu nu as a function of q^2. 
#
#  This together with form factor predictions from either light cone
#  sum rules or lattice QCD will allow an exclusive determination of |Vub|.
# 
#  Three lines are included:
#  Two of which strip the opposite sign (right sign) proton and muon and combinations.
#  One of which has no prescale and strips protons and muons with a corrected mass >
#  4 GeV.  The other line is prescaled by a factor of 0.5 and strips < 4 GeV in corrected
#  mass.  This exploits the fact that the corrected mass for Lb->pmunu peaks at the Lambda_b
#  mass.  The final line strips same sign proton and muon cominations over the whole corrected 
#  mass region.  This will be ussed to extract shapes for the combinatorial background.
#
#  Stripping XX, with requirements that the
#  rate <0.05% and timing <0.5ms/evt.
## 

"""
  Lb->p mu nu exclusive reconstruction. 

  Stripping lines for the charmless semileptonic decay Lambda_b -> p mu nu.

  The lines will lead to a  measurement of the differential branching
  of Lb -> p mu nu as a function of q^2. 

  This together with form factor predictions from either light cone
  sum rules or lattice QCD will allow an exclusive determination of |Vub|.
 
  Three lines are included:
  Two of which strip the opposite sign (right sign) proton and muon and combinations.
  One of which has no prescale and strips protons and muons with a corrected mass >
  4 GeV.  The other line is prescaled by a factor of 0.5 and strips < 4 GeV in corrected
  mass.  This exploits the fact that the corrected mass for Lb->pmunu peaks at the Lambda_b
  mass.  The final line strips same sign proton and muon cominations over the whole corrected 
  mass region.  This will be ussed to extract shapes for the combinatorial background.

  Stripping XX, with requirements that the
  rate <0.05% and timing <0.5ms/evt.

  Last modification $Date: 2014-January-19 $
               by $Author: William $
"""

confdict= {
    "GEC_nLongTrk"        : 250.   ,#adimensional
    "TRGHOSTPROB"         : 0.35   ,#adimensional
    #Muon Cuts
    "MuonGHOSTPROB"       : 0.35   ,#adimensional
    "MuonTRCHI2"          : 4.     ,#adimensional
    "MuonP"               : 3000.  ,#MeV
    "MuonPT"              : 1500.  ,#MeV
    "MuonMINIPCHI2"       : 16.    ,#adminensional
    #Proton Cuts 
    "ProtonTRCHI2"        : 6.     ,#adimensional
    "ProtonP"             : 15000. ,#MeV
    "ProtonPT"            : 1000.  ,#MeV
    "ProtonPIDK"          : 10.     ,#adimensional 
    "ProtonPIDp"          : 10.     ,#adimensional
    "ProtonMINIPCHI2"     : 16.    ,#adminensional
    #B Mother Cuts
    "BVCHI2DOF"           : 4.     ,#adminensional
    "BDIRA"               : 0.9994  ,#adminensional
    "BFDCHI2HIGH"         : 150.   ,#adimensional
    "pMuMassLower"        : 1000.  ,#MeV
    "pMuPT"               : 1500.  ,#MeV
    "pMuMCORR"            : 4000.   #MeV
    }

from Gaudi.Configuration import *
from StrippingUtils.Utils import LineBuilder

import logging

# Define a make TOS filter function
def makeTOSFilter(name,specs):
    from Configurables import TisTosParticleTagger
    tosFilter = TisTosParticleTagger(name+'TOSFilter')
    tosFilter.TisTosSpecs = specs
    tosFilter.ProjectTracksToCalo = False
    tosFilter.CaloClustForCharged = False
    tosFilter.CaloClustForNeutral = False
    tosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tosFilter
# Define a tosSelection function
def tosSelection(sel,specs):
    from PhysSelPython.Wrappers import Selection
    '''Filters Selection sel to be TOS.'''
    tosFilter = makeTOSFilter(sel.name(),specs)
    return Selection(sel.name()+'TOS', Algorithm=tosFilter,
                     RequiredSelections=[sel])

default_name="LbpMuNu"

class Lb2pMuNuVubBuilder(LineBuilder):
    """
    Definition of Lb->p mu nu stripping
    """
    
    __configuration_keys__ = [
        "GEC_nLongTrk"
        ,"TRGHOSTPROB"          
        ,"MuonGHOSTPROB"
        ,"MuonTRCHI2"          
        ,"MuonP"               
        ,"MuonPT"              
        ,"MuonMINIPCHI2"       
        ,"ProtonTRCHI2"          
        ,"ProtonP"               
        ,"ProtonPT"              
        ,"ProtonPIDK"             
        ,"ProtonPIDp"            
        ,"ProtonMINIPCHI2"
        ,"BVCHI2DOF"
        ,"BDIRA"               
        ,"BFDCHI2HIGH"         
        ,"pMuMassLower"     
	,"pMuPT"     
	,"pMuMCORR"     
        ]

    def __init__(self,name,config):
        LineBuilder.__init__(self, name, config)

        from PhysSelPython.Wrappers import Selection, DataOnDemand

        self.GECs = { "Code":"( recSummaryTrack(LHCb.RecSummary.nLongTracks, TrLONG) < %(GEC_nLongTrk)s )" % config,
                      "Preambulo": ["from LoKiTracks.decorators import *"]}
        
        self._muonSel=None
        self._muonFilter()

        self._protonSel=None
        self._protonFilter()

        self._fakeprotonSel=None

        self._fakemuonSel=None

        self._Definitions()

        self.registerLine(self._high_Lb_line())
        self.registerLine(self._low_Lb_line())
        self.registerLine(self._SS_Lb_line())
        
    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (TRGHOSTPROB < %(MuonGHOSTPROB)s)"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
    
    def _NominalPSelection( self ):
        return "(TRCHI2DOF < %(ProtonTRCHI2)s )&  (P> %(ProtonP)s *MeV) &  (PT> %(ProtonPT)s *MeV)"\
               "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
               "& (PIDp-PIDpi> %(ProtonPIDp)s )& (PIDp-PIDK> %(ProtonPIDK)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(ProtonMINIPCHI2)s )"\
               "& (switch(ISMUON,1,0) < 1)"

    def _low_Lb_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        hlt = "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision')"\
              "| HLT_PASS_RE('Hlt2TopoMu2Body.*Decision')"
        ldu = "L0_CHANNEL_RE('Muon')" 
        return StrippingLine(self._name+'LowMcorrLine', prescale = 0.5,
                             FILTER=self.GECs, 
                             algos = [ self._Lb2pMuNuVub_low_Lb()], HLT = hlt, L0DU = ldu)


    ###### High Corrected Mass Line  ######
    def _high_Lb_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        hlt = "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision')"\
              "| HLT_PASS_RE('Hlt2TopoMu2Body.*Decision')"
        ldu = "L0_CHANNEL_RE('Muon')" 
        return StrippingLine(self._name+'HighMcorrLine', prescale = 1.0,
                             FILTER=self.GECs,
                             algos = [ self._Lb2pMuNuVub_high_Lb()], HLT = hlt, L0DU = ldu)

    ###### Low q^2 Line Same Sign ######
    def _SS_Lb_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        hlt = "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision')"\
              "| HLT_PASS_RE('Hlt2TopoMu2Body.*Decision')"
        ldu = "L0_CHANNEL_RE('Muon')" 
        return StrippingLine(self._name+'SSLine', prescale = 1.0,
                             FILTER=self.GECs, 
                             algos = [ self._Lb2pMuNuVubSS_Lb()], HLT = hlt, L0DU = ldu)

    ##### Muon Filter ######
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
        _muSel = tosSelection(_muSel,{'L0.*Muon.*Decision%TOS':0})
        
        self._muonSel=_muSel
        
        return _muSel


    ###### Proton Filter ######
    def _protonFilter( self ):
        if self._protonSel is not None:
            return self._protonSel
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseProtons
        
        _pr = FilterDesktop( Code = self._NominalPSelection() % self._config )
        _prSel=Selection("p_for"+self._name,
                         Algorithm=_pr,
                         RequiredSelections = [StdLooseProtons])
        
        self._protonSel=_prSel
        
        return _prSel


    def _Definitions(self):
        return [ 
            "from LoKiPhys.decorators import *",
            "Lb_PT = PT",
	    "Lb_MCORR = BPVCORRM",
                           ]


    ###### Lb->pMuNu High M_corr Opposite Sign ######
    def _Lb2pMuNuVub_high_Lb( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
	_pMu = CombineParticles(DecayDescriptors = ["[Lambda_b0 -> p+ mu-]cc"], ReFitPVs = True
)
        _pMu.Preambulo = self._Definitions()
	_pMu.CombinationCut = "(AM>%(pMuMassLower)s*MeV)" % self._config
	_pMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
                    "& (Lb_PT > %(pMuPT)s)"\
		    "& (Lb_MCORR > %(pMuMCORR)s)"\
                    "& (BPVVDCHI2 >%(BFDCHI2HIGH)s)" % self._config
	_pMu.ReFitPVs = True
            
        _pMuSel=Selection("pMu_high_Lb_for"+self._name,
                         Algorithm=_pMu,
                         RequiredSelections = [self._muonFilter(), self._protonFilter()])
         
        _LbSel = tosSelection(_pMuSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _LbSel

    ###### Lb->pMuNu Low M_corr Opposite Sign ######
    def _Lb2pMuNuVub_low_Lb( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
	_pMu = CombineParticles(DecayDescriptors = ["[Lambda_b0 -> p+ mu-]cc"], ReFitPVs = True
)
        _pMu.Preambulo = self._Definitions()
	_pMu.CombinationCut = "(AM>%(pMuMassLower)s*MeV)" % self._config
	_pMu.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
                    "& (Lb_PT > %(pMuPT)s)"\
		    "& (Lb_MCORR < %(pMuMCORR)s)"\
                    "& (BPVVDCHI2 >%(BFDCHI2HIGH)s)" % self._config
	_pMu.ReFitPVs = True
            
        _pMuSel=Selection("pMu_low_Lb_for"+self._name,
                         Algorithm=_pMu,
                         RequiredSelections = [self._muonFilter(), self._protonFilter()])
         
        _LbSel = tosSelection(_pMuSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _LbSel



    ###### Lb->pMuNu Low q^2 SS Sign ######
    def _Lb2pMuNuVubSS_Lb( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
	_pMuSS = CombineParticles(DecayDescriptors = ["[Lambda_b0 -> p+ mu+]cc"], ReFitPVs = True
)
        _pMuSS.Preambulo = self._Definitions() 
	_pMuSS.CombinationCut = "(AM>%(pMuMassLower)s*MeV)"  % self._config
	_pMuSS.MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
                    "& (Lb_PT > %(pMuPT)s)"\
		    "& (BPVVDCHI2 >%(BFDCHI2HIGH)s)" % self._config
	_pMuSS.ReFitPVs = True
            
        _pMuSSSel=Selection("pMuSS_Lb_for"+self._name,
                         Algorithm=_pMuSS,
                         RequiredSelections = [self._muonFilter(), self._protonFilter()])
        _pMuSSSel = tosSelection(_pMuSSSel,{'Hlt2.*TopoMu2Body.*Decision%TOS':0,'Hlt2.*SingleMuon.*Decision%TOS':0})
        return _pMuSSSel
	    

    

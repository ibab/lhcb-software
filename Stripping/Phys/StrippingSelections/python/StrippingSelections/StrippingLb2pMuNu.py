
__author__ = ['William Sutcliffe']
__date__ = '28/03/2013'
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
#  Four lines are included:
#  One pair of which is optimised for low q^2 values where predictions from
#  light cone sum rules are most accurate.  This pair is further broken down
#  into 2 lines for opposite sign and same sign (SS) combinations.
#  
#  The other pair is optimised for high q^2 values where predictions from
#  lattice QCD are most accurate.  Again this pair is further broken down
#  into 2 lines for opposite sign and same sign (SS) combinations.
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
 
Four lines are included:
One pair of which is optimised for low q^2 values where predictions from
light cone sum rules are most accurate.  This pair is further broken down
into 2 lines for opposite sign and same sign (SS) combinations.
  
The other pair is optimised for high  q^2 values where predictions from
lattice QCD are most accurate.  Again this pair is further broken down
into 2 lines for opposite sign and same sign (SS) combinations.

Stripping XX, with requirements that the
rate <0.05% and timing <0.5ms/evt.
Last modification $Date: 2013-March-28 $
               by $Author: William $
"""

confdict= {
    "GEC_nLongTrk"        : 250.  , #adimensional
    "TRGHOSTPROB"         : 0.5    ,#adimensional
    #Muon Cuts
    "MuonGHOSTPROB"       : 0.5   ,#adimensional
    "MuonTRCHI2"          : 4.    ,#adimensional
    "MuonP"               : 3000. ,#MeV
    "MuonPT"              : 1600. ,#MeV
    "MuonMINIPCHI2"       : 12    ,#adminensional
    #Proton Cuts 
    "ProtonTRCHI2"          : 6.   ,#adimensional
    "ProtonP"               : 15000. ,#MeV
    "ProtonPT"              : 1000.  ,#MeV
    "ProtonPIDK"            : 0.    ,#adimensional 
    "ProtonPIDp"            : 5.    ,#adimensional
    "ProtonMINIPCHI2"       : 16     ,#adminensional
    #B Mother Cuts
    "BVCHI2DOF"           : 4.    ,#adminensional
    "BVCHI2DOFTight"      : 2.    ,#adminensional
    "BDIRA"               : 0.999  ,#adminensional
    "BFDCHI2HIGH"         : 125.  ,#adimensional
    #B Mass Minima
    "pMuMassLow"          : 2250. ,#MeV
    "pMuMassLowTight"     : 2750. ,#MeV
    "pMuMassUpper"        : 5600. ,   #MeV
    "BPVIPChi2"           : 25.   ,   #adminensional  
    "pMuPT"               : 1500. ,   #MeV
    "PassymLow"           : -0.4  ,   #adminensional
    "PassymLower"         : -0.65 ,  #adimensional
    "PassymUpper"         : 0.2       #adimensional
    }

from Gaudi.Configuration import *
from StrippingUtils.Utils import LineBuilder

import logging

default_name="LbpMuNu"

class Lb2pMuNuBuilder(LineBuilder):
    """
    Definition of Lb->p mu nu stripping
    """
    
    __configuration_keys__ = [
        "GEC_nLongTrk"
        ,"MuonGHOSTPROB"
        ,"TRGHOSTPROB"          
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
        ,"BVCHI2DOFTight"      
        ,"BDIRA"               
        ,"BFDCHI2HIGH"         
        ,"pMuMassLow"     
        ,"pMuMassLowTight"     
        ,"pMuMassUpper"        
        ,"BPVIPChi2" 
	,"pMuPT"     
	,"PassymLow"  
	,"PassymLower"  
	,"PassymUpper" 
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

        self.registerLine(self._lowq2_line())
        self.registerLine(self._SS_lowq2_line())
        self.registerLine(self._highq2_line())
        self.registerLine(self._SS_highq2_line())
        
    def _NominalMuSelection( self ):
        return "(TRCHI2DOF < %(MuonTRCHI2)s ) &  (P> %(MuonP)s *MeV) &  (PT> %(MuonPT)s* MeV)"\
               "& (TRGHOSTPROB < %(MuonGHOSTPROB)s)"\
               "& (MIPCHI2DV(PRIMARY)> %(MuonMINIPCHI2)s )"
    
    def _NominalPSelection( self ):
        return "(TRCHI2DOF < %(ProtonTRCHI2)s )&  (P> %(ProtonP)s *MeV) &  (PT> %(ProtonPT)s *MeV)"\
               "& (TRGHOSTPROB < %(TRGHOSTPROB)s)"\
               "& (PIDp-PIDpi> %(ProtonPIDp)s )& (PIDp-PIDK> %(ProtonPIDK)s ) "\
               "& (MIPCHI2DV(PRIMARY)> %(ProtonMINIPCHI2)s )"

    ###### Low q^2 Line######
    def _lowq2_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        hlt = "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision')"\
              "| HLT_PASS_RE('Hlt2TopoMu2Body.*Decision')"
        ldu = "L0_CHANNEL_RE('Muon')" 
        return StrippingLine(self._name+'Lowq2Line', prescale = 1.0,
                             FILTER=self.GECs, 
                             algos = [ self._Lb2pMuNu_lowq2()], HLT = hlt, L0DU = ldu)

    ###### Low q^2 Line Same Sign######
    def _SS_lowq2_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        hlt = "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision')"\
              "| HLT_PASS_RE('Hlt2TopoMu2Body.*Decision')"
        ldu = "L0_CHANNEL_RE('Muon')" 
        return StrippingLine(self._name+'SSLowq2Line', prescale = 1.0,
                             FILTER=self.GECs, 
                             algos = [ self._Lb2pMuNuSS_lowq2()], HLT = hlt, L0DU = ldu)

    
    ###### High q^2 Line######
    def _highq2_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        hlt = "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision')"\
              "| HLT_PASS_RE('Hlt2TopoMu2Body.*Decision')"
        ldu = "L0_CHANNEL_RE('Muon')" 
        return StrippingLine(self._name+'Highq2Line', prescale = 1.0,
                             FILTER=self.GECs, 
                             algos = [ self._Lb2pMuNu_highq2()], HLT = hlt, L0DU = ldu)

    ###### High q^2 Same Sign Line######
    def _SS_highq2_line( self ):
        from StrippingConf.StrippingLine import StrippingLine
        hlt = "HLT_PASS_RE('Hlt2.*SingleMuon.*Decision')"\
              "| HLT_PASS_RE('Hlt2TopoMu2Body.*Decision')"
        ldu = "L0_CHANNEL_RE('Muon')" 
        return StrippingLine(self._name+'SSHighq2Line', prescale = 1.0,
                             FILTER=self.GECs, 
                             algos = [ self._Lb2pMuNuSS_highq2()], HLT = hlt, L0DU = ldu)

    ######--######
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

    ######Proton Filter######
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
    

    ######Lb->pMuNu low q2 Opposite Sign######
    def _Lb2pMuNu_lowq2( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _pMu = CombineParticles(
            DecayDescriptors = ["[Lambda_b0 -> p+ mu-]cc"],
            CombinationCut = "(AM>%(pMuMassLowTight)s*MeV) & (AM<%(pMuMassUpper)s*MeV)"\
	    "& (((AMINCHILD(P,'p+'==ABSID) - AMINCHILD(P,'mu-'==ABSID))/ (AMINCHILD(P,'p+'==ABSID) + AMINCHILD(P,'mu-'==ABSID)))>%(PassymLow)s)"\
	    "&  (APT> %(pMuPT)s* MeV)"  % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2HIGH)s)" % self._config,
            ReFitPVs = True
            )
        _pMuSel=Selection("pMu_lowq2_for"+self._name,
                         Algorithm=_pMu,
                         RequiredSelections = [self._muonFilter(), self._protonFilter()])
        return _pMuSel
	    
    ######Lb->pMuNu low q2 Same Sign######
    def _Lb2pMuNuSS_lowq2( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _pMu = CombineParticles(
            DecayDescriptors = ["[Lambda_b0 -> p+ mu+]cc"],
            CombinationCut = "(AM>%(pMuMassLowTight)s*MeV) & (AM<%(pMuMassUpper)s*MeV)"\
	    "& (((AMINCHILD(P,'p+'==ABSID) - AMINCHILD(P,'mu-'==ABSID))/ (AMINCHILD(P,'p+'==ABSID) + AMINCHILD(P,'mu-'==ABSID)))>%(PassymLow)s)"\
	    "&  (APT> %(pMuPT)s* MeV)"  % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2HIGH)s)" % self._config,
            ReFitPVs = True
            )
        _pMuSel=Selection("pMuSS_lowq2_for"+self._name,
                         Algorithm=_pMu,
                         RequiredSelections = [self._muonFilter(), self._protonFilter()])
        return _pMuSel

    ###### Lb->pMuNu high q2 opposite sign ######
    def _Lb2pMuNu_highq2( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _pMu = CombineParticles(
            DecayDescriptors = ["[Lambda_b0 -> p+ mu-]cc"],
            CombinationCut = "(AM>%(pMuMassLow)s*MeV) & (AM<%(pMuMassUpper)s*MeV)"\
	    "& (((AMINCHILD(P,'p+'==ABSID) - AMINCHILD(P,'mu-'==ABSID))/ (AMINCHILD(P,'p+'==ABSID) + AMINCHILD(P,'mu-'==ABSID)))>%(PassymLower)s)"\
	    "& (((AMINCHILD(P,'p+'==ABSID) - AMINCHILD(P,'mu-'==ABSID))/ (AMINCHILD(P,'p+'==ABSID) + AMINCHILD(P,'mu-'==ABSID)))<%(PassymUpper)s)"\
	    "&  (APT> %(pMuPT)s* MeV)"  % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2HIGH)s) & (BPVIPCHI2() > %(BPVIPChi2)s)" % self._config,
            ReFitPVs = True
            )
        _pMuSel=Selection("pMu_highq2_for"+self._name,
                         Algorithm=_pMu,
                         RequiredSelections = [self._muonFilter(), self._protonFilter()])
        return _pMuSel
	    
    ###### Lb->pMuNu high q2 same sign ######
    def _Lb2pMuNuSS_highq2( self ):
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        _pMu = CombineParticles(
            DecayDescriptors = ["[Lambda_b0 -> p+ mu+]cc"],
            CombinationCut = "(AM>%(pMuMassLow)s*MeV) & (AM<%(pMuMassUpper)s*MeV)"\
	    "& (((AMINCHILD(P,'p+'==ABSID) - AMINCHILD(P,'mu-'==ABSID))/ (AMINCHILD(P,'p+'==ABSID) + AMINCHILD(P,'mu-'==ABSID)))>%(PassymLower)s)"\
	    "& (((AMINCHILD(P,'p+'==ABSID) - AMINCHILD(P,'mu-'==ABSID))/ (AMINCHILD(P,'p+'==ABSID) + AMINCHILD(P,'mu-'==ABSID)))<%(PassymUpper)s)"\
	    "&  (APT> %(pMuPT)s* MeV)"  % self._config,
            MotherCut = "(VFASPF(VCHI2/VDOF)< %(BVCHI2DOF)s) & (BPVDIRA> %(BDIRA)s)"\
            "& (BPVVDCHI2 >%(BFDCHI2HIGH)s) & (BPVIPCHI2() > %(BPVIPChi2)s)" % self._config,
            ReFitPVs = True
            )
        _pMuSel=Selection("pMuSS_highq2_for"+self._name,
                         Algorithm=_pMu,
                         RequiredSelections = [self._muonFilter(), self._protonFilter()])
        return _pMuSel
    

__author__  = 'Raja Nandakumar, Fergus Wilson, Stefania Ricciardi'
__date__    = '04/11/2015'
__version__ = '$Revision: 0 $'

__all__ = ( 'B2XLLConf', 'default_config' ) 

"""
  B --> ll X selections where:

  ll = e+e- / mu+mu- / mu+e- / mu+e+ / e+e+ / mu+mu+
  X  = rho0 / D0 /  D*+ / J/psi / psi(2S) / K*0 / phi / K+ / pi+ / proton
  and anti-particles of the above
"""

default_config = {
    'NAME'         : 'B2XLL',
    'BUILDERTYPE'  : 'B2XLLConf',
    'CONFIG'       :
        {
        'BFlightCHI2'      : 100
        , 'BDIRA'          : 0.9995
        , 'BIPCHI2'        : 25
        , 'BVertexCHI2'    : 9
        , 'DiLeptonPT'     : 0
        , 'DiLeptonFDCHI2' : 16
        , 'DiLeptonIPCHI2' : 0
        , 'LeptonIPCHI2'   : 9
        , 'LeptonPT'       : 300
        , 'KaonIPCHI2'     : 9
        , 'KaonPT'         : 400
        , 'DiHadronMass'   : 2600
        , 'UpperMass'      : 5500
        , 'BMassWindow'    : 1500
        , 'PIDe'           : 0
        , 'mmXLinePrescale'  : 1
        , 'meXLinePrescale'  : 1
        , 'eeXLinePrescale'  : 1
        , 'mmXSSLinePrescale': 1
        , 'meXSSLinePrescale': 1
        , 'eeXSSLinePrescale': 1
        },
    'WGs'     : [ 'RD' ],
    'STREAMS' : [ 'Leptonic' ]
    }


from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection, AutomaticData
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class B2XLLConf(LineBuilder) :
    """
    Builder for R_X measurements
    """

    # now just define keys. Default values are fixed later
    __configuration_keys__ = (
        'BFlightCHI2'
        , 'BDIRA'
        , 'BIPCHI2'
        , 'BVertexCHI2'
        , 'DiLeptonPT'
        , 'DiLeptonFDCHI2'
        , 'DiLeptonIPCHI2'
        , 'LeptonIPCHI2'
        , 'LeptonPT'
        , 'KaonIPCHI2'
        , 'KaonPT'
        , 'DiHadronMass'
        , 'UpperMass'
        , 'BMassWindow'
        , 'PIDe'
        , 'mmXLinePrescale'
        , 'meXLinePrescale'
        , 'eeXLinePrescale'
        , 'mmXSSLinePrescale'
        , 'meXSSLinePrescale'
        , 'eeXSSLinePrescale'
      )

    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)

        self._name = name

        mmXLine_name = name+"_mm"
        meXLine_name = name+"_me"
        eeXLine_name = name+"_ee"
        mmXSSLine_name = name+"_mmSS"
        meXSSLine_name = name+"_meSS"
        eeXSSLine_name = name+"_eeSS"

        from StandardParticles import StdLooseKaons as Kaons
        from StandardParticles import StdLoosePions as Pions
        from StandardParticles import StdLooseProtons as Protons
        from StandardParticles import StdLooseKstar2Kpi as Kstars
        from StandardParticles import StdLoosePhi2KK as Phis
        from StandardParticles import StdLooseD02KPi as DZeros
        from StandardParticles import StdLooseDplus2hhh as DPlus
        from StandardParticles import StdLooseDstarWithD02KPi as DStars
        from StandardParticles import StdLooseRho0 as Rhos
        from StandardParticles import StdLooseJpsi2MuMu as JPsis
        from StandardParticles import StdLoosePhotons as Gammas

        # 1 : Make the particles we will be actually using
        SelKaons  = self._filterHadron( name = "KaonsFor" + self._name, sel = Kaons, params = config )
        SelPions  = self._filterHadron( name = "PionsFor" + self._name, sel = Pions, params = config )
        SelKstars = self._filterHadron( name = "KstarsFor"+ self._name, sel = Kstars,params = config )
        SelRhos   = self._filterHadron( name = "RhosFor"  + self._name, sel = Rhos,  params = config )
        SelPhis   = self._filterHadron( name = "PhisFor"  + self._name, sel = Phis,  params = config )
        SelJPsis  = self._filterHadron( name = "JPsisFor" + self._name, sel = JPsis, params = config )
        SelProtons= self._filterHadron( name = "ProtonsFor"+ self._name, sel = Protons, params = config )
        SelDZeros = self._filterHadron( name = "DZerosFor" + self._name, sel = DZeros,  params = config )
        SelDPlus  = self._filterHadron( name = "DPlusFor"  + self._name, sel = DPlus,   params = config )
        SelDStars = self._filterHadron( name = "DStarsFor" + self._name, sel = DStars,  params = config )
        SelPi0s   = self._myPi0s("PiZeros")
        SelKs     = self._myKshort("KShorts")
        SelOmega  = self._omega2PiPiPi0( name="OmegasFor"+self._name, Pions=SelPions, Pi0=SelPi0s )
        SKStPl1   = self._kstarPlus(name="KStarPlus1For"+self._name, Kaons=SelKs,    Pions=SelPions)
        SKStPl2   = self._kstarPlus(name="KStarPlus2For"+self._name, Kaons=SelKaons, Pions=SelPi0s)
        SelKStarPlus=MergedSelection(name="KStarPlusFor"+self._name, RequiredSelections=[SKStPl1,SKStPl2])
        Self0s    = self._myf0(name="f0(980)sFor"+self._name, pions=SelPions)
        SelPsi2S  = self._psi2s2jpsihh( name="Psi2SFor"+self._name, jpsi=SelJPsis, pions=SelPions )
        SelDsPlus = self._dSPlus( name="DsPlusFor"+self._name, Pion=SelPions, Kaons=SelKaons )
        SelDsStars= self._dSStarPlus( name="DsStarFor"+self._name, DPlus=SelDsPlus, Gamma=Gammas )

        # 2 : Dileptons
        MuMu = self._makeMuMu( "MuMuFor"+ self._name, params = config )
        MuE  = self._makeMuE ( "MuEFor" + self._name, params = config )
        EE   = self._makeEE  ( "EEFor"  + self._name, params = config )

        MuMu_SS= self._makeMuMu( "MuMuSSFor"+ self._name, params = config, samesign=True )
        MuE_SS = self._makeMuE ( "MuESSFor" + self._name, params = config, samesign=True )
        EE_SS  = self._makeEE  ( "EESSFor"  + self._name, params = config, samesign=True )

        SelMuMu= self._filterDiLepton("SelMuMuFor"+ self._name, dilepton = MuMu,params = config, idcut = None )
        SelMuE = self._filterDiLepton("SelMuEFor" + self._name, dilepton = MuE, params = config, idcut = None )
        SelEE  = self._filterDiLepton("SelEEFor"  + self._name, dilepton = EE,  params = config, idcut = None )

        SelMuMu_SS= self._filterDiLepton("SelMuMuSSFor"+ self._name, dilepton = MuMu_SS, params= config, idcut = None )
        SelMuE_SS = self._filterDiLepton("SelMuESSFor" + self._name, dilepton = MuE_SS, params = config, idcut = None )
        SelEE_SS  = self._filterDiLepton("SelEESSFor"  + self._name, dilepton = EE_SS, params  = config, idcut = None )

        # 3 : Combine
        Wanted = [ SelKaons, SelPions, SelKstars, SelRhos, SelPhis, SelJPsis, SelProtons, SelDZeros, SelDPlus, SelDStars, SelOmega, Self0s, SelKStarPlus, SelDsPlus, SelDsStars ]
        SelmmX = self._makeB2LLX(mmXLine_name, dilepton=SelMuMu, XPart=Wanted, params=config, masscut="ADAMASS('B+') <  %(BMassWindow)s *MeV"% config)
        SelmeX = self._makeB2LLX(meXLine_name, dilepton=SelMuE,  XPart=Wanted, params=config, masscut="ADAMASS('B+') <  %(BMassWindow)s *MeV" % config )
        SeleeX = self._makeB2LLX(eeXLine_name, dilepton=SelEE,   XPart=Wanted, params=config, masscut="ADAMASS('B+') <  %(BMassWindow)s *MeV" % config )
        SelmmX_SS = self._makeB2LLX(mmXSSLine_name, dilepton=SelMuMu_SS, XPart=Wanted, params=config, masscut="ADAMASS('B+') <  %(BMassWindow)s *MeV" % config )
        SelmeX_SS = self._makeB2LLX(meXSSLine_name, dilepton=SelMuE_SS,  XPart=Wanted, params=config, masscut="ADAMASS('B+') <  %(BMassWindow)s *MeV" % config )
        SeleeX_SS = self._makeB2LLX(eeXSSLine_name, dilepton=SelEE_SS,   XPart=Wanted, params=config, masscut="ADAMASS('B+') <  %(BMassWindow)s *MeV" % config )
       
        # 4 : Declare Lines
        SPDFilter = {
            'Code'      : " ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < 600 )" ,
            'Preambulo' : [ "from LoKiNumbers.decorators import *", "from LoKiCore.basic import LHCb" ]
            }
        self.eeXLine = StrippingLine(eeXLine_name+"Line", prescale = config['eeXLinePrescale'], postscale = 1, selection = SeleeX, RelatedInfoTools = self._RelInfoTools(SeleeX),
                                     FILTER = SPDFilter, RequiredRawEvents = [], MDSTFlag = True )
        self.mmXLine = StrippingLine(mmXLine_name+"Line", prescale = config['mmXLinePrescale'], postscale = 1, selection = SelmmX, RelatedInfoTools = self._RelInfoTools(SelmmX),
                                     FILTER = SPDFilter, RequiredRawEvents = [], MDSTFlag = True )
        self.meXLine = StrippingLine(meXLine_name+"Line", prescale = config['meXLinePrescale'], postscale = 1, selection = SelmeX, RelatedInfoTools = self._RelInfoTools(SelmeX),
                                     FILTER = SPDFilter, RequiredRawEvents = [], MDSTFlag = True )
        self.mmX_SSLine = StrippingLine(mmXSSLine_name+"Line", prescale = config['mmXSSLinePrescale'], postscale = 1, selection = SelmmX_SS, RelatedInfoTools = self._RelInfoTools(SelmmX_SS),
                                        FILTER = SPDFilter, RequiredRawEvents = [], MDSTFlag = True )
        self.meX_SSLine = StrippingLine(meXSSLine_name+"Line", prescale = config['meXSSLinePrescale'], postscale = 1, selection = SelmeX_SS, RelatedInfoTools = self._RelInfoTools(SelmeX_SS),
                                        FILTER = SPDFilter, RequiredRawEvents = [], MDSTFlag = True )
        self.eeX_SSLine = StrippingLine(eeXSSLine_name+"Line", prescale = config['eeXSSLinePrescale'], postscale = 1, selection = SeleeX_SS, RelatedInfoTools = self._RelInfoTools(SeleeX_SS),
                                        FILTER = SPDFilter, RequiredRawEvents = [], MDSTFlag = True )

        # 5 : register Line
        self.registerLine( self.mmXLine )
        self.registerLine( self.meXLine )
        self.registerLine( self.eeXLine )
        self.registerLine( self.mmX_SSLine )
        self.registerLine( self.meX_SSLine )
        self.registerLine( self.eeX_SSLine )

#####################################################
    def _RelInfoTools(self, selection) :
        """
        Return related information for the given selection
        """
        RelInfo = [{ "Type": "RelInfoConeVariables", "ConeAngle":1.0, "Location":"ConeIsoInfo" },
                   { "Type": "RelInfoVertexIsolation", "Location":"VtxIsoInfo" },
                   { "Type": "RelInfoVertexIsolationBDT", "Location":"VtxIsoInfoBDT" }]
        return RelInfo

#####################################################
    def _filterHadron( self, name, sel, params ):
        """
        Filter for all hadronic final states
        """
        # requires all basic particles to have IPCHI2 > KaonIPCHI2
        # and hadron PT > KaonPT
        #need to add the ID here
        _Code = "(PT > %(KaonPT)s *MeV) & " \
                "((ISBASIC & (MIPCHI2DV(PRIMARY) > %(KaonIPCHI2)s)) | " \
                "(NDAUGHTERS == NINTREE( ISBASIC &  (MIPCHI2DV(PRIMARY) > %(KaonIPCHI2)s))))" % params
        if (name.startswith("JPsisFor")):
            _Code += "& (M <  3200*MeV)" # Avoid psi(2S) resonance in the uppser sideband
        else :
            _Code += "& (M <  %(DiHadronMass)s*MeV) " % params
        _Filter = FilterDesktop(Code = _Code)
        return Selection( name, Algorithm = _Filter, RequiredSelections = [ sel ] )

#####################################################
    def _filterDiLepton(self, name, dilepton, params, idcut = None ) :
        """
        Handy interface for dilepton filter
        """
        _Code = "(ID=='J/psi(1S)') & "\
                "(PT > %(DiLeptonPT)s *MeV) & "\
                "(MM < %(UpperMass)s *MeV) & "\
                "(MINTREE(ABSID<14,PT)>%(LeptonPT)s *MeV) & "\
                "(MINTREE(ABSID<14,MIPCHI2DV(PRIMARY))>%(LeptonIPCHI2)s) & "\
                "(VFASPF(VCHI2/VDOF)<9) & (BPVVDCHI2> %(DiLeptonFDCHI2)s) & "\
                "(MIPCHI2DV(PRIMARY) > %(DiLeptonIPCHI2)s )" % params

        # add additional cut on PID if requested
        if idcut: _Code += ( " & " + idcut ) 

        _Filter = FilterDesktop( Code = _Code )
    
        return Selection(name, Algorithm = _Filter, RequiredSelections = [ dilepton ] )

#####################################################
    def _psi2s2jpsihh( self, name, jpsi, pions ):
        """
        Combine J/Psi and 2 pions to make psi(2S)
        """
        _psi2S = CombineParticles()
        _psi2S.DecayDescriptor = "psi(2S) -> J/psi(1S) pi+ pi-"
        _psi2S.CombinationCut = "(ADAMASS('psi(2S)')<200*MeV)"
        _psi2S.MotherCut = "(ADMASS('psi(2S)')<200*MeV)"
        return Selection(name, Algorithm = _psi2S, RequiredSelections = [ jpsi, pions, pions ])

#####################################################
    def _myPi0s(self, name):
        """
        Filter Pi0 from Std Pi0
        """  
        from StandardParticles import StdLooseResolvedPi0 as _pi0resolved
        from StandardParticles import StdLooseMergedPi0 as _pi0merged
        _filter_pi0resolved = FilterDesktop(Code="PT>800*MeV")
        _filter_pi0merged = FilterDesktop(Code="PT>800*MeV")        
        _selpi0resolved = Selection("Selection_"+name+"_pi0resolved", RequiredSelections=[_pi0resolved], Algorithm=_filter_pi0resolved)
        _selpi0merged = Selection("Selection_"+name+"_pi0merged", RequiredSelections=[_pi0merged], Algorithm=_filter_pi0merged)
        _sel = MergedSelection("Selection_"+name+"_pi0", RequiredSelections=[_selpi0resolved,_selpi0merged])
        return _sel

#####################################################
    def _omega2PiPiPi0( self, name, Pions, Pi0):
        """
        Make omega -> pi+ pi- pi0 
        """
        _omega2pipipizero = CombineParticles()
        _omega2pipipizero.DecayDescriptor = "omega(782) -> pi+ pi- pi0"
        _omega2pipipizero.CombinationCut = "(ADAMASS('omega(782)') < 200 *MeV)"
        _omega2pipipizero.MotherCut = "(ADMASS('omega(782)') < 200 *MeV) & (VFASPF(VPCHI2)> 0.00001)"
        _omegaConf = _omega2pipipizero.configurable("Combine_"+name+"_PiPiPi0")
        _selOMEGA2PIPIPIZERO = Selection( "Selection_"+name+"_omega2pipipizero",
                                          Algorithm = _omegaConf,
                                          RequiredSelections = [ Pions, Pi0 ] )
        return _selOMEGA2PIPIPIZERO

#####################################################
    def _myf0(self, name, pions):
        """
        Copy of the stuff which makes the StdLooseDetachedDipion
        """
        _f0 = CombineParticles()
        _f0.DecayDescriptor = "f_0(980) -> pi+ pi-"
        _f0.CombinationCut = "(AM<2700.0*MeV)"
        _f0.MotherCut = "(M<2700.0*MeV)"
        _f0Conf = _f0.configurable("Combine_"+name)
        _self0 = Selection("SelectionOff0For"+name, RequiredSelections=[pions], Algorithm=_f0Conf)
        return _self0

#####################################################
    def _myKshort(self, name):
        """
        Filter kshort from StdLooseKshort
        """  
        from StandardParticles import StdLooseKsDD as _ksdd
        from StandardParticles import StdLooseKsLL as _ksll
        _filter_ksdd = FilterDesktop(Code = "(ADMASS('KS0') < 30*MeV) & (BPVLTIME() > 0.5*ps)")
        _filter_ksll = FilterDesktop(Code = "(ADMASS('KS0') < 30*MeV) & (BPVLTIME() > 0.5*ps)")
        _selksdd = Selection("Selection_"+name+"_Ksdd", RequiredSelections = [ _ksdd ], Algorithm = _filter_ksdd)
        _selksll = Selection("Selection_"+name+"_Ksll", RequiredSelections = [ _ksll ], Algorithm = _filter_ksll)
        _sel = MergedSelection("Selection_"+name+"_Kshort", RequiredSelections = [ _selksdd, _selksll ])
        return _sel

#####################################################
    def _dSPlus( self, name, Pion, Kaons ):
        """
        Make Ds -> K K Pi
        """
        _dsPlus = CombineParticles()
        _dsPlus.DecayDescriptor = "[D_s+ -> K+ K- pi+]cc"
        _dsPlus.MotherCut = "(ADMASS('D_s+') < 300 *MeV)"
        _dsPlusConf = _dsPlus.configurable("Combine_"+name+"_Dsplus")
        _selDsPlus = Selection( "Selection_"+name+"_DsPlus", Algorithm = _dsPlusConf, RequiredSelections = [ Pion, Kaons ] )
        return _selDsPlus

#####################################################
    def _dSStarPlus( self, name, DPlus, Gamma ):
        """
        Make Ds* -> Ds (=D_s+ for us) Gamma
        """
        _dsstar = CombineParticles()
        _dsstar.DecayDescriptor = "[D*_s+ -> D_s+ gamma]cc"
        _dsstar.MotherCut = "(ADMASS('D*_s+') < 300 *MeV)"
        _dsstar.DaughtersCuts = { 'gamma' : '(CL > 0.25)' }
        _dsstarConf = _dsstar.configurable("Combine_"+name+"_Dsstar")
        _selDsSTAR = Selection( "Selection_"+name+"_DsStar", Algorithm = _dsstarConf, RequiredSelections = [ DPlus, Gamma ] )
        return _selDsSTAR

#####################################################
    def _kstarPlus( self, name, Kaons, Pions):
        """
        Make K*(892)+ -> K+ pi0 or Kspi+
        """
        _kstar = CombineParticles()
        _kstar.DecayDescriptors = ["[K*(892)+ -> K+ pi0]cc", "[K*(892)+ -> KS0 pi+]cc"]
        _kstar.MotherCut = "(ADMASS('K*(892)+') < 300 *MeV)"
        _kstarConf = _kstar.configurable("Combine_"+name+"_KPi")
        _selKSTAR2KPIZERO = Selection( "Selection_"+name+"_Kstar2kaonpion", Algorithm = _kstarConf, RequiredSelections = [ Kaons, Pions ] )
        return _selKSTAR2KPIZERO

#####################################################
    def _makeB2LLX( self, name, dilepton, XPart, params, masscut = "(ADAMASS('B+')<1500*MeV)" ):

        """
        CombineParticles / Selection for the B 
        """

        _Cut   = "((VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s ) "\
                 "& (BPVIPCHI2()< %(BIPCHI2)s ) "\
                 "& (BPVDIRA> %(BDIRA)s ) "\
                 "& (BPVVDCHI2> %(BFlightCHI2)s ))" % params
        
        _Decays =  ["[ B+ -> J/psi(1S) K+ ]cc",
                    "[ B+ -> J/psi(1S) pi+ ]cc",
                    "[ B+ -> J/psi(1S) p+ ]cc",
                    "[ B+ -> J/psi(1S) D+ ]cc",
                    "[ B+ -> J/psi(1S) D*(2010)+ ]cc",
                    "[ B+ -> J/psi(1S) K*(892)+ ]cc",
                    "[ B+ -> J/psi(1S) D_s+ ]cc",
                    "[ B+ -> J/psi(1S) D*_s+ ]cc",
                    "[ B0 -> J/psi(1S) K*(892)0 ]cc",
                    "[ B0 -> J/psi(1S) phi(1020) ]cc",
                    "[ B0 -> J/psi(1S) D0 ]cc",
                    "[ B0 -> J/psi(1S) rho(770)0 ]cc",
                    "[ B0 -> J/psi(1S) J/psi(1S) ]cc",
                    "[ B0 -> J/psi(1S) psi(2S) ]cc", ## Checking
                    "[ B0 -> J/psi(1S) omega(782) ]cc",
                    "[ B0 -> J/psi(1S) f_0(980) ]cc",
                    ]
        
        _Combine = CombineParticles(DecayDescriptors = _Decays,
                                    CombinationCut = masscut,
                                    MotherCut = _Cut )
        
        _Merge   = MergedSelection("Merge"+name, 
                                   RequiredSelections = XPart )
        
        
        return Selection(name,
                         Algorithm = _Combine,
                         RequiredSelections = [ dilepton, _Merge ]) 

#####################################################
    def _makeMuE( self, name, params, samesign = False):
        """
        Makes MuE combinations 
        """
        from StandardParticles import StdLooseElectrons as Electrons 
        from StandardParticles import StdLooseMuons as Muons

        _DaughtersCut = "(PT > %(LeptonPT)s) & " \
                        "(MIPCHI2DV(PRIMARY)>%(LeptonIPCHI2)s)" % params

        _MassCut = "(AM > 100*MeV)" 
        
        _DecayDescriptor = "[J/psi(1S) -> mu+ e-]cc"
        if samesign: _DecayDescriptor = "[J/psi(1S) -> mu+ e+]cc"

        
        _Combine = CombineParticles( DecayDescriptor = _DecayDescriptor,
                                     CombinationCut  = _MassCut,
                                     MotherCut       = "(VFASPF(VCHI2/VDOF) < 9)")


        _ElectronCut = _DaughtersCut
        _MuonCut     = _DaughtersCut
        
        _Combine.DaughtersCuts   = {
            "e+"  : _ElectronCut,
            "mu+" : _MuonCut
            }
        
        return Selection(name,
                         Algorithm = _Combine,
                         RequiredSelections = [ Muons, Electrons ] )
#####################################################
    def _makeMuMu( self, name, params, samesign = False):
        """
        Makes MuMu combinations 
        """
        from StandardParticles import StdLooseMuons as Muons
        _DaughtersCut = "(PT > %(LeptonPT)s) & " \
                        "(MIPCHI2DV(PRIMARY)>%(LeptonIPCHI2)s)" % params
        _MassCut = "(AM > 100*MeV)"
        _DecayDescriptor = "[J/psi(1S) -> mu+ mu-]cc"
        if samesign: _DecayDescriptor = "[J/psi(1S) -> mu+ mu+]cc"
        _Combine = CombineParticles( DecayDescriptor = _DecayDescriptor,
                                     CombinationCut  = _MassCut,
                                     MotherCut       = "(VFASPF(VCHI2/VDOF) < 9)")
        _MuonCut     = _DaughtersCut
        _Combine.DaughtersCuts   = {
            "mu-" : _MuonCut,
            "mu+" : _MuonCut
            }
        return Selection(name,
                         Algorithm = _Combine,
                         RequiredSelections = [ Muons ] )

#####################################################
    def _makeEE( self, name, params, samesign = False):
        """
        Makes EE combinations 
        """
        from StandardParticles import StdLooseElectrons as Electrons 
        _DaughtersCut = "(PT > %(LeptonPT)s) & " \
                        "(MIPCHI2DV(PRIMARY)>%(LeptonIPCHI2)s)" % params
        _MassCut = "(AM > 100*MeV)"
        _DecayDescriptor = "[J/psi(1S) -> e+ e-]cc"
        if samesign: _DecayDescriptor = "[J/psi(1S) -> e+ e+]cc"
        _Combine = CombineParticles( DecayDescriptor = _DecayDescriptor,
                                     CombinationCut  = _MassCut,
                                     MotherCut       = "(VFASPF(VCHI2/VDOF) < 9)")
        _ElectronCut = _DaughtersCut
        _Combine.DaughtersCuts   = {
            "e+" : _ElectronCut,
            "e-" : _ElectronCut
            }
        return Selection(name,
                         Algorithm = _Combine,
                         RequiredSelections = [ Electrons ] )

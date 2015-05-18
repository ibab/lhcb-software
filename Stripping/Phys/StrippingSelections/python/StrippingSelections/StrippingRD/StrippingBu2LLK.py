__author__  = 'Patrick Koppenburg, Alex Shires, Thomas Blake, Luca Pescatore, Simone Bifani, Yasmine Amhis, Paula Alvarez Cartelle'
__date__    = '16/06/2014'
__version__ = '$Revision: 3 $'

__all__ = ( 'Bu2LLKConf', 'default_config' ) 

"""
  B --> ll K selections for:
  B --s> ee K   versus B --> mumu K
  B --> ee K*  versus B --> mumu K*
  B --> ee phi versus B --> mumu phi
  Lb --> ee Lambda(*) versus Lb --> mumu Lambda(*)
  B --> gamma K* , B --> gamma phi  and  Lb --> gamma Lambda(*)   with converted photons
  B --> emu K, B--> emu K* and B--> emu Phi with OS and SS leptons
"""

default_config = {
    'NAME'                      : 'Bu2LLK',
    'BUILDERTYPE'               : 'Bu2LLKConf',
    'CONFIG'                    :
        {
        'BFlightCHI2'           : 100   
        , 'BDIRA'               : 0.9995 
        , 'BIPCHI2'             : 25   
        , 'BVertexCHI2'         : 9    
        , 'DiLeptonPT'          : 0    
        , 'DiLeptonFDCHI2'      : 16   
        , 'DiLeptonIPCHI2'      : 0    
        , 'LeptonIPCHI2'        : 9   
        , 'LeptonPT'            : 300  
        , 'KaonIPCHI2'          : 9
        , 'KaonPT'              : 400
        , 'DiHadronMass'        : 2600
        , 'UpperMass'           : 5500
        , 'BMassWindow'         : 1500
        , 'PIDe'                : 0
        , 'Bu2eeKLinePrescale'  : 1
        , 'Bu2mmKLinePrescale'  : 1
        , 'Bu2meKLinePrescale'  : 1
        , 'Bu2meKSSLinePrescale': 1
        , 'RelatedInfoTools'    : [
            {'Type'             : 'RelInfoVertexIsolation',
             'Location'         : 'VertexIsoInfo'},
            {'Type'             : 'RelInfoVertexIsolationBDT',
             'Location'         : 'VertexIsoBDTInfo'},
            {'Type'             : 'RelInfoConeVariables',
             'ConeAngle'        : 1.0,
             'Location'         : 'ConeIsoInfo'},
            {'Type'             : 'RelInfoConeVariables',
             'ConeAngle'        : 0.5,
             'RecursionLevel'   : 2,
             'Locations'        : {'Phys/StdAllLooseElectrons'    : ['ConeIsoInfoE1', 'ConeIsoInfoE2'],
                                   'Phys/StdDiElectronFromTracks' : ['ConeIsoInfoE1', 'ConeIsoInfoE2'],
                                   'Phys/StdAllLooseMuons'        : ['ConeIsoInfoM1', 'ConeIsoInfoM2'],
                                   'Phys/StdAllLooseKaons'        : 'ConeIsoInfoK',
                                   'Phys/StdAllLooseProtons'     : 'ConeIsoInfoProton',
                                   'Phys/StdAllLoosePions'        : 'ConeIsoInfoPi'}},
            {'Type'             : 'RelInfoTrackIsolationBDT',
             'RecursionLevel'   : 2,
             'Locations'        : {'Phys/StdAllLooseElectrons'    : ['TrackIsoBDTInfoE1', 'TrackIsoBDTInfoE2'],
                                   'Phys/StdDiElectronFromTracks' : ['TrackIsoBDTInfoE1', 'TrackIsoBDTInfoE2'],
                                   'Phys/StdAllLooseMuons'        : ['TrackIsoBDTInfoM1', 'TrackIsoBDTInfoM2'],
                                   'Phys/StdAllLooseKaons'        : 'TrackIsoBDTInfoK',
                                   'Phys/StdAllLooseProtons'      : 'TrackIsoBDTInfoProton',
                                   'Phys/StdAllLoosePions'        : 'TrackIsoBDTInfoPi'}},
            {'Type'             : 'RelInfoBs2MuMuTrackIsolations',
             'RecursionLevel'   : 2,
             'Locations'        : {'Phys/StdAllLooseElectrons'    : ['TrackIsoBs2MMInfoE1', 'TrackIsoBs2MMInfoE2'],
                                   'Phys/StdDiElectronFromTracks' : ['TrackIsoBs2MMInfoE1', 'TrackIsoBs2MMInfoE2'],
                                   'Phys/StdAllLooseMuons'        : ['TrackIsoBs2MMInfoM1', 'TrackIsoBs2MMInfoM2'],
                                   'Phys/StdAllLooseKaons'        : 'TrackIsoBs2MMInfoK',
                                   'Phys/StdAllLooseProtons'      : 'TrackIsoBs2MMInfoProton',
                                   'Phys/StdAllLoosePions'        : 'TrackIsoBs2MMInfoPi'},
             'tracktype'        : 3,
             'angle'            : 0.27,
             'fc'               : 0.60,
             'doca_iso'         : 0.13,
             'ips'              : 3.0,
             'svdis'            : -0.15,
             'svdis_h'          : 30.,
             'pvdis'            : 0.5,
             'pvdis_h'          : 40.,
             'makeTrackCuts'    : False,
             'IsoTwoBody'       : False}
            ],
        },
    'WGs'                       : [ 'RD' ],
    'STREAMS'                   : [ 'Leptonic' ]
    }
    

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from PhysSelPython.Wrappers import Selection, DataOnDemand, MergedSelection, AutomaticData
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder

class Bu2LLKConf(LineBuilder) :
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
        , 'Bu2eeKLinePrescale'
        , 'Bu2mmKLinePrescale'
        , 'Bu2meKLinePrescale'
        , 'Bu2meKSSLinePrescale'
        , 'RelatedInfoTools'
      )
    
    def __init__(self, name, config):
        LineBuilder.__init__(self, name, config)

        self._name = name

        mmXLine_name = name+"_mm"
        eeXLine_name = name+"_ee"
        meXLine_name = name+"_me"
        meXSSLine_name = name+"_meSS"

        from StandardParticles import StdLooseKaons as Kaons
        from StandardParticles import StdLooseKstar2Kpi as Kstars
        from StandardParticles import StdLoosePhi2KK  as Phis
        from StandardParticles import StdLooseKsLL as KshortsLL 
        from StandardParticles import StdLooseKsDD as KshortsDD    
        from StandardParticles import StdLooseLambdaLL as LambdasLL
        from StandardParticles import StdLooseLambdaDD as LambdasDD  
        from StandardParticles import StdLooseLambdastar2pK as Lambdastars  
        # 1 : Make high IP, Pt kaons, K*'s and Phi's
        SelKaons  = self._filterHadron( name   = "KaonsFor" + self._name,
                                        sel    = Kaons # "Phys/StdLooseKaons/Particles",
                                        ,params = config )

        SelKstars = self._filterHadron( name   = "KstarsFor"+ self._name,
                                        sel    =  Kstars # "Phys/StdLooseKstar2Kpi/Particles",
                                        ,params = config )

        SelPhis   = self._filterHadron( name = "PhisFor" + self._name,
                                        sel  =  Phis # "Phys/StdLoosePhi3KK/Particles",
                                        ,params = config )
   
        SelKshortsLL = self._filterHadron( name   = "KshortsLLFor"+ self._name,
                                        sel    =  KshortsLL 
                                        ,params = config )
      
        SelKshortsDD = self._filterHadron( name   = "KshortsDDFor"+ self._name,
                                        sel    =  KshortsDD
                                        ,params = config )

        SelLambdasLL = self._filterHadron( name   = "LambdasLLFor"+ self._name,
                                        sel    =  LambdasLL
                                        ,params = config )

        SelLambdasDD = self._filterHadron( name   = "LambdasDDFor"+ self._name,
                                        sel    =  LambdasDD
                                        ,params = config )

        SelLambdastars = self._filterHadron( name   = "LambdastarsFor"+ self._name,
                                        sel    =  Lambdastars 
                                        ,params = config )
 
        # 2 : Dileptons
        from StandardParticles import StdDiElectronFromTracks as DiElectronsFromTracks
        from StandardParticles import StdLooseDiElectron as DiElectrons
        from StandardParticles import StdLooseDiMuon as DiMuons 

        ElecID = "(PIDe> %(PIDe)s)" % config
        MuonID = "(HASMUON)&(ISMUON)"

        MuE  = self._makeMuE( "MuEFor" + self._name, params = config, electronid = ElecID, muonid = MuonID )
        MuE_SS  = self._makeMuE( "MuESSFor" + self._name, params = config, electronid = ElecID, muonid = MuonID, samesign=True )

        DiElectronID = "(2 == NINTREE((ABSID==11)&(PIDe> %(PIDe)s)))" % config
        DiMuonID     = "(2 == NINTREE((ABSID==13)&(HASMUON)&(ISMUON)))"
                
        
        SelDiElectron = self._filterDiLepton("SelDiElectronFor" + self._name, 
                                             dilepton = DiElectrons,
                                             params   = config,
                                             idcut    = DiElectronID )
        
        SelDiElectronFromTracks = self._filterDiLepton("SelDiElectronFromTracksFor" + self._name, 
                                                       dilepton = DiElectronsFromTracks,
                                                       params   = config,
                                                       idcut    = DiElectronID )
        
        SelDiMuon = self._filterDiLepton("SelDiMuonsFor" + self._name, 
                                         dilepton = DiMuons,
                                         params   = config, idcut = DiMuonID )
        
        SelMuE = self._filterDiLepton("SelMuEFor" + self._name, 
                                      dilepton = MuE,
                                      params   = config,
                                      idcut    = None )

        SelMuE_SS = self._filterDiLepton("SelMuESSFor" + self._name, 
                                         dilepton = MuE_SS,
                                         params   = config,
                                         idcut    = None )

        from StandardParticles import StdAllLooseGammaLL as PhotonConversion
        SelPhoton         = self._filterPhotons("SelPhotonFor" + self._name, photons = PhotonConversion ) 
        
        # 3 : Combine
        SelB2eeX = self._makeB2LLX(eeXLine_name,
                                   dilepton = SelDiElectron,
                                   hadrons = [ SelKaons, SelKstars, SelPhis, SelKshortsLL, SelKshortsDD, SelLambdasLL, SelLambdasDD , SelLambdastars  ],
                                   params  = config,
                                   masscut = "ADAMASS('B+') <  %(BMassWindow)s *MeV" % config )
                                       
        
        SelB2eeXFromTracks  = self._makeB2LLX(eeXLine_name+"2",
                                              dilepton = SelDiElectronFromTracks,
                                              hadrons = [ SelKaons, SelKstars, SelPhis, SelKshortsLL,  SelKshortsDD,  SelLambdasLL, SelLambdasDD, SelLambdastars  ],
                                              params  = config,
                                              masscut = "ADAMASS('B+') <  %(BMassWindow)s *MeV" % config )
        
        SelB2mmX = self._makeB2LLX(mmXLine_name,
                                   dilepton = SelDiMuon,
                                   hadrons = [ SelKaons, SelKstars, SelPhis ,  SelKshortsLL, SelKshortsDD, SelLambdasLL , SelLambdasDD, SelLambdastars  ],
                                   params  = config,
                                   masscut = "ADAMASS('B+') <  %(BMassWindow)s *MeV"% config) 

        
        
        SelB2meX = self._makeB2LLX(meXLine_name,
                                   dilepton = SelMuE,
                                   hadrons = [ SelKaons, SelKstars, SelPhis , SelKshortsLL, SelKshortsDD, SelLambdasLL, SelLambdasDD, SelLambdastars  ],
                                   params  = config,
                                   masscut = "ADAMASS('B+') <  %(BMassWindow)s *MeV" % config )  


        SelB2meX_SS = self._makeB2LLX(meXSSLine_name,
                                      dilepton = SelMuE_SS,
                                      hadrons = [ SelKaons, SelKstars, SelPhis , SelKshortsLL, SelKshortsDD, SelLambdasLL, SelLambdasDD, SelLambdastars  ],
                                      params  = config,
                                      masscut = "ADAMASS('B+') <  %(BMassWindow)s *MeV" % config )  

       
        SelB2gammaX = self._makeB2GammaX(eeXLine_name + "3",
                                      photons = SelPhoton, 
                                      hadrons  = [ SelKstars, SelPhis, SelLambdasLL,  SelLambdasDD, SelLambdastars  ], 
                                      params   = config,
                                      masscut  = "ADAMASS('B+') <  %(BMassWindow)s *MeV"% config )
        
        # 4 : Declare Lines

        SPDFilter = {
            'Code'      : " ( recSummary(LHCb.RecSummary.nSPDhits,'Raw/Spd/Digits') < 600 )" ,
            'Preambulo' : [ "from LoKiNumbers.decorators import *", "from LoKiCore.basic import LHCb" ]
            }
        
        self.B2eeXLine = StrippingLine(eeXLine_name+"Line",
                                       prescale = config['Bu2eeKLinePrescale'],
                                       postscale = 1,
                                       selection = SelB2eeX,
                                       RelatedInfoTools = config['RelatedInfoTools'],
                                       FILTER = SPDFilter, 
                                       RequiredRawEvents = [],
                                       MDSTFlag = True )

        self.B2eeXFromTracksLine = StrippingLine(eeXLine_name+"Line2",
                                                 prescale = config['Bu2eeKLinePrescale'],
                                                 postscale = 1,
                                                 selection = SelB2eeXFromTracks,
                                                 RelatedInfoTools = config['RelatedInfoTools'],
                                                 FILTER = SPDFilter, 
                                                 RequiredRawEvents = [],
                                                 MDSTFlag = True )
        
        self.B2mmXLine = StrippingLine(mmXLine_name+"Line",
                                       prescale = config['Bu2mmKLinePrescale'],
                                       postscale = 1,
                                       selection = SelB2mmX,
                                       RelatedInfoTools = config['RelatedInfoTools'],
                                       FILTER = SPDFilter, 
                                       RequiredRawEvents = [],
                                       MDSTFlag = True )
        
        self.B2meXLine = StrippingLine( meXLine_name+"Line",
                                        prescale       = config['Bu2meKLinePrescale'],
                                        postscale      = 1,
                                        selection      = SelB2meX,
                                        RelatedInfoTools = config['RelatedInfoTools'],
                                        FILTER = SPDFilter, 
                                        RequiredRawEvents = [],
                                        MDSTFlag = True )


        self.B2meX_SSLine = StrippingLine( meXSSLine_name+"Line",
                                        prescale       = config['Bu2meKSSLinePrescale'],
                                        postscale      = 1,
                                        selection      = SelB2meX_SS,
                                        RelatedInfoTools = config['RelatedInfoTools'],
                                        FILTER = SPDFilter, 
                                        RequiredRawEvents = [],
                                        MDSTFlag = True )


        self.B2gammaXLine = StrippingLine(eeXLine_name+"Line3",
                                          prescale = config['Bu2eeKLinePrescale'],
                                          postscale = 1,
                                          #selection = SelB2eeXFromTracks,
                                          selection = SelB2gammaX, 
                                          RelatedInfoTools = config['RelatedInfoTools'],
                                          FILTER = SPDFilter, 
                                          RequiredRawEvents = [],
                                          MDSTFlag = True )

        # 5 : register Line
        self.registerLine( self.B2eeXLine )
        self.registerLine( self.B2eeXFromTracksLine )
        self.registerLine( self.B2mmXLine )
        self.registerLine( self.B2meXLine )
        self.registerLine( self.B2gammaXLine )
        self.registerLine( self.B2meX_SSLine )

#####################################################
    def _filterHadron( self, name, sel, params ):
        """
        Filter for all hadronic final states
        """
        # requires all basic particles to have IPCHI2 > KaonIPCHI2
        # and hadron PT > KaonPT
        #need to add the ID here
        _Code = "(PT > %(KaonPT)s *MeV) & " \
                "(M <  %(DiHadronMass)s*MeV) & " \
                "((ISBASIC & (MIPCHI2DV(PRIMARY) > %(KaonIPCHI2)s)) | " \
                "(NDAUGHTERS == NINTREE( ISBASIC &  (MIPCHI2DV(PRIMARY) > %(KaonIPCHI2)s))))" % params

        _Filter = FilterDesktop(Code = _Code)
        
        return Selection( name, Algorithm = _Filter, RequiredSelections = [  sel ] )
        
#####################################################
    def _filterDiLepton(self, name, dilepton, params, idcut = None ) :
        """
        Handy interface for dilepton filter
        """
        _Code = "(PT > %(DiLeptonPT)s *MeV) & "\
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
    def _filterPhotons( self, name, photons ):
        """
        Filter photon conversions
        """

        _Code = "(PT > 1000*MeV) & (HASVERTEX) & (VFASPF(VCHI2/VDOF)<9)"

        _Filter = FilterDesktop( Code = _Code )

        return Selection( name, Algorithm = _Filter, RequiredSelections = [ photons ] )

#####################################################
    def _makeB2LLX( self, name, dilepton, hadrons, params, masscut = "(ADAMASS('B+')< 1500 *MeV" ):

        """
        CombineParticles / Selection for the B 
        """

        _Cut   = "((VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s ) "\
                 "& (BPVIPCHI2()< %(BIPCHI2)s ) "\
                 "& (BPVDIRA> %(BDIRA)s ) "\
                 "& (BPVVDCHI2> %(BFlightCHI2)s ))" % params
        
        _Decays =  ["[ B+ -> J/psi(1S) K+ ]cc",
                    "[ B0 -> J/psi(1S) K*(892)0 ]cc",
                    "[ B_s0 -> J/psi(1S) phi(1020)]cc",
                    "[ B0 -> J/psi(1S) KS0]cc", 
                    "[ Lambda_b0 -> J/psi(1S) Lambda0 ]cc",                            
                    "[ Lambda_b0 -> J/psi(1S) Lambda(1520)0]cc"] 
        
        _Combine = CombineParticles(DecayDescriptors = _Decays,
                                    CombinationCut = masscut,
                                    MotherCut = _Cut )
        
        _Merge   = MergedSelection("Merge"+name, 
                                   RequiredSelections = hadrons )
        
        
        return Selection(name,
                         Algorithm = _Combine,
                         RequiredSelections = [ dilepton, _Merge ]) 

#####################################################
    def _makeB2GammaX( self, name, photons, hadrons, params, masscut = "(ADAMASS('B+')< 1500 *MeV" ):
        """
        CombineParticles / Selection for the B 
        """

        _Cut   = "((VFASPF(VCHI2/VDOF)< %(BVertexCHI2)s ) "\
                 "& (BPVIPCHI2()< %(BIPCHI2)s ) "\
                 "& (BPVDIRA> %(BDIRA)s ) "\
                 "& (BPVVDCHI2> %(BFlightCHI2)s ))" % params
        
        _Decays =  [ "[ B0 -> gamma K*(892)0 ]cc",
                     "[ B_s0 -> gamma phi(1020)]cc", 
                     "[ Lambda_b0 -> gamma  Lambda0]cc",                   
                     "[ Lambda_b0 -> gamma Lambda(1520)0]cc" ]

        
        _Combine = CombineParticles(DecayDescriptors = _Decays,
                                    CombinationCut = masscut,
                                    MotherCut = _Cut )
        
        _Merge   = MergedSelection("Merge"+name, 
                                   RequiredSelections = hadrons )
        
        
        return Selection(name,
                         Algorithm = _Combine,
                         RequiredSelections = [ _Merge, photons ]) 
#####################################################
    def _makeMuE( self, name, params, electronid = None, muonid = None , samesign = False):
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

        if electronid: _ElectronCut += ( "&" + electronid )
        if muonid    : _MuonCut     += ( "&" + muonid )
        
        
        _Combine.DaughtersCuts   = {
            "e+"  : _ElectronCut,
            "mu+" : _MuonCut
            }
        
        return Selection(name,
                         Algorithm = _Combine,
                         RequiredSelections = [ Muons, Electrons ] )

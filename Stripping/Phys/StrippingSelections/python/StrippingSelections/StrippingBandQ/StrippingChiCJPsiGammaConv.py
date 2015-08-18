__author__ = 'Edwige Tournefier'
__date__ = '08/06/2012'
__version__ = '$ 1.0 $'

"""
ChiC -> JPsi Gamma -> Mu Mu e e
"""
moduleName = 'ChiCJPsiGammaConv'

__all__ = ('StrippingChiCJPsiGammaConvConf',
           'default_config')


default_config =  {
    'ChiCJPsiGammaConv' : {
    'WGs'               : ['BandQ'],
    'BUILDERTYPE'       : 'StrippingChiCJPsiGammaConvConf',
    'CONFIG'            : {'trackChi2'               :    3.0
                           , 'MuPTMin'               :    400 #MeV #Can be tightened to 600 MeV
                           , 'MuPMin'               :    8000 #MeV
                           , 'JPsiMassMin'           :    3.0 # GeV
                           , 'JPsiMassMax'           :    3.2 # GeV
                           , 'JPsiPTMin'             :     2 # GeV
                           , 'JPsi_PIDmu'            :    0. 
                           , 'JPsiVertexChi2'        :   25.0
                           , 'UpsilonMassMin'        :    9.0  #GeV
                           , 'UpsilonMassMax'        :    12.9 #GeV
                           , 'UpsilonVertexChi2'     :   25.0
                           , 'UpsilonPTMin'          :    0.9 # GeV
                           , 'Upsilon_PIDmu'         :    0. 
                           , 'eDLLe'                 :   0.0
                           , 'GammaEEMass'           :   100.0 #MeV
                           , 'GammaEEPt'             :   580.0 #MeV
                           , 'mMinChiCRaw'           :   2.9 #GeV
                           , 'mMaxChiCRaw'           :   4.8 #GeV
                           , 'mMinChiCFit'           :   3.0 #GeV
                           , 'mMaxChiCFit'           :   4.7 ##GeV
                           , 'mMinChiBRaw'           :   9.4 #GeV
                           , 'mMaxChiBRaw'           :   13.2 #GeV
                           , 'mMinChiBFit'           :   9.5 #GeV
                           , 'mMaxChiBFit'           :   13.0  #GeV
                           , 'PrescaleChi'          :   1.0
                           , 'PostscaleChi'         :   1.0
                           },
    'STREAMS': {'Dimuon':
                ['StrippingChiCJPsiGammaConvChibLine',
                 'StrippingChiCJPsiGammaConvChicLine']
                }
    }
    }

from Gaudi.Configuration import *
from Configurables import FilterDesktop, CombineParticles, OfflineVertexFitter
from PhysSelPython.Wrappers import Selection, DataOnDemand
from StrippingConf.StrippingLine import StrippingLine
from StrippingUtils.Utils import LineBuilder


class StrippingChiCJPsiGammaConvConf(LineBuilder):
    """
    Stripping selection for ChiC -> JPsi Gamma
    where JPsi -> mu mu and Gamma -> e+ e- (i.e. conversions before the magnet)
    
    """
    
    __configuration_keys__ = ( 
        'trackChi2'             # :    5.0
        , 'MuPTMin'             # :    400 #MeV
        , 'MuPMin'             # :    8000 #MeV
        , 'JPsiMassMin'         # :    2.8 #GeV
        , 'JPsiMassMax'         # :    3.4 #GeV
        , 'JPsiVertexChi2'      # :   25.0
        , 'JPsiPTMin'           # :     1.9 # GeV
        , 'JPsi_PIDmu'          # :    0. 
        , 'UpsilonMassMin'         # :    
        , 'UpsilonMassMax'         # :    
        , 'UpsilonVertexChi2'      # :   25.0
        , 'UpsilonPTMin'           # :    
        , 'Upsilon_PIDmu'          # :    0. 
        , 'GammaEEMass'         # :   200.0 #MeV
        , 'GammaEEPt'           # :   500.0 #MeV
        , 'eDLLe'               # :   0.0
        , 'mMinChiCRaw'         # :   2.9 #GeV
        , 'mMaxChiCRaw'         # :   4.8 #GeV
        , 'mMinChiCFit'         # :   3.0 #GeV
        , 'mMaxChiCFit'         # :   4.6 #GeV
        , 'mMinChiBRaw'         #  :   9.4 #GeV
        , 'mMaxChiBRaw'         #  :   12.2 #GeV
        , 'mMinChiBFit'         #  :   9.5 #GeV
        , 'mMaxChiBFit'         #  :   12.0  #GeV
        , 'PrescaleChi'        # :   1.0
        , 'PostscaleChi'       # :   1.0
        )



    def __init__(self, name, config) :
        LineBuilder.__init__(self, name, config)

        self.SelJPsi           = self.filterDiMu(name+"JPsi",
                                                 MuonPT   = config['MuPTMin'],
                                                 MuonP    = config['MuPMin'],
                                                 MuonTRCHI2DOF  = config['trackChi2'],
                                                 MuonPID      = config['JPsi_PIDmu'],
                                                 MuMuMinMass    = config['JPsiMassMin'],
                                                 MuMuMaxMass    = config['JPsiMassMax'],
                                                 MuMuVCHI2PDOF = config['JPsiVertexChi2'],
                                                 MuMuPT      = config['JPsiPTMin']
                                                 )

        self.SelUpsilon           = self.filterDiMu(name+"Upsilon",
                                                    MuonPT   = config['MuPTMin'],
                                                    MuonP    = config['MuPMin'],
                                                    MuonTRCHI2DOF  = config['trackChi2'],
                                                    MuonPID      = config['Upsilon_PIDmu'],
                                                    MuMuMinMass    = config['UpsilonMassMin'],
                                                    MuMuMaxMass    = config['UpsilonMassMax'],
                                                    MuMuVCHI2PDOF = config['UpsilonVertexChi2'],
                                                    MuMuPT      = config['UpsilonPTMin']
                                                    )



        self.SelGammaEE        = self.filterGammaEE(name+"GammaEE",
                                                    eDLLe        = config['eDLLe'],
                                                    GammaEEMass  = config['GammaEEMass'],
                                                    GammaEEPt    = config['GammaEEPt']
                                                    )

        self.SelSymGammaEE        = self.filterSymGammaEE(name+"SymGammaEE",
                                              eDLLe        = config['eDLLe'],
                                              GammaEEMass  = config['GammaEEMass'],
                                              GammaEEPt    = config['GammaEEPt']
                                              )

        self.SelChiC           = self.ChiC(name+"ChiC",
                                           self.SelJPsi,
                                           self.SelGammaEE,
                                           mMinChiCRaw = config['mMinChiCRaw'],       
                                           mMaxChiCRaw = config['mMaxChiCRaw'],      
                                           mMinChiCFit = config['mMinChiCFit'],       
                                           mMaxChiCFit = config['mMaxChiCFit'])              

        self.SelChiCSym           = self.ChiC(name+"ChiCSym",
                                           self.SelJPsi,
                                           self.SelSymGammaEE,
                                           mMinChiCRaw = config['mMinChiCRaw'],       
                                           mMaxChiCRaw = config['mMaxChiCRaw'],      
                                           mMinChiCFit = config['mMinChiCFit'],       
                                           mMaxChiCFit = config['mMaxChiCFit'])              

        self.SelChiB           = self.ChiC(name+"ChiB",
                                           self.SelUpsilon,
                                           self.SelGammaEE,
                                           mMinChiCRaw = config['mMinChiBRaw'],       
                                           mMaxChiCRaw = config['mMaxChiBRaw'],      
                                           mMinChiCFit = config['mMinChiBFit'],       
                                           mMaxChiCFit = config['mMaxChiBFit'])              

        self.SelChiBSym           = self.ChiC(name+"ChiBSym",
                                           self.SelUpsilon,
                                           self.SelSymGammaEE,
                                           mMinChiCRaw = config['mMinChiBRaw'],       
                                           mMaxChiCRaw = config['mMaxChiBRaw'],      
                                           mMinChiCFit = config['mMinChiBFit'],       
                                           mMaxChiCFit = config['mMaxChiBFit'])              
            

        self.ChiCLine           = StrippingLine(name+"ChicLine",
                                                prescale  = config['PrescaleChi'],
                                                postscale = config['PostscaleChi'],
                                                selection = self.SelChiC)

        self.ChiCSymLine           = StrippingLine(name+"ChicSymLine",
                                                prescale  = config['PrescaleChi'],
                                                postscale = config['PostscaleChi'],
                                                selection = self.SelChiCSym)

        self.ChiBLine           = StrippingLine(name+"ChibLine",
                                                prescale  = config['PrescaleChi'],
                                                postscale = config['PostscaleChi'],
                                                selection = self.SelChiB)

        self.ChiBSymLine           = StrippingLine(name+"ChibSymLine",
                                                prescale  = config['PrescaleChi'],
                                                postscale = config['PostscaleChi'],
                                                selection = self.SelChiBSym)


        self.registerLine(self.ChiCLine)

        self.registerLine(self.ChiCSymLine)

        self.registerLine(self.ChiBLine)

        self.registerLine(self.ChiBSymLine)


    #####################################################################################################################
    #    
    # filter candidates J/Psi -> mu mu
    #
    ######################################################################################################################  
    def filterDiMu( self, name,
                      MuonPT,
                      MuonP,
                      MuonTRCHI2DOF,
                      MuonPID,
                      MuMuMinMass,
                      MuMuMaxMass,
                      MuMuVCHI2PDOF,
                      MuMuPT 
                      ):
        
        _StdLooseDiMuon = DataOnDemand( Location = 'Phys/StdLooseDiMuon/Particles' )
        
        MuonCut = "(MINTREE('mu+'==ABSID,PT) > %(MuonPT)s )  & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)  & (MINTREE('mu+'==ABSID,PIDmu)> %(MuonPID)s)" % locals()
        MuonCut += "& (MINTREE('mu+'==ABSID,P) > %(MuonP)s )" % locals()
#        MuonCut = " (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(MuonTRCHI2DOF)s)  & (MINTREE('mu+'==ABSID,PIDmu)> %(MuonPID)s )" % locals()
        
        MuMuCut = "(MM > %(MuMuMinMass)s *GeV) & (MM < %(MuMuMaxMass)s *GeV) & (VFASPF(VCHI2PDOF)< %(MuMuVCHI2PDOF)s) & (PT > %(MuMuPT)s *GeV)" % locals()
        
        _MuMu = FilterDesktop( name+"FilterChiCMuMu", Code = MuonCut + " & " + MuMuCut  )
        
        return Selection( name + "_SelMuMu",
                          Algorithm = _MuMu,
                          RequiredSelections = [ _StdLooseDiMuon ]
                          )



    ######################################################################################################################  
    #
    # Filter gamma-> e+ e- candidates
    # 
    #
    ######################################################################################################################  
    def filterGammaEE(self, name, eDLLe, GammaEEMass, GammaEEPt):
        
        _StdAllTightGammaDD = DataOnDemand( Location = 'Phys/StdAllTightGammaDD/Particles' )

        EleCut = " (MINTREE('e+'==ABSID,PIDe)> %(eDLLe)s)" % locals()
        GammaCut = " (MM < %(GammaEEMass)s *MeV) & ( PT > %(GammaEEPt)s *MeV) " % locals()

        _Gamma = FilterDesktop( name+"FilterChiCGamma", Code = EleCut + " & " + GammaCut  )
        
        return Selection( name + "_SelGamma",
                          Algorithm = _Gamma,
                          RequiredSelections = [ _StdAllTightGammaDD ]
                          )


    ######################################################################################################################  
    #
    # Filter gamma-> e+ e- candidates
    # 
    #
    ######################################################################################################################  
    def filterSymGammaEE(self, name, eDLLe, GammaEEMass, GammaEEPt):
        
        _StdAllTightSymGammaDD = DataOnDemand( Location = 'Phys/StdAllTightSymGammaDD/Particles' )

        EleCut = " (MINTREE('e+'==ABSID,PIDe)> %(eDLLe)s)" % locals()
        GammaCut = " (MM < %(GammaEEMass)s) & ( PT > %(GammaEEPt)s) " % locals()

        _Gamma = FilterDesktop( name="FilterChiCSymGamma", Code = EleCut + " & " + GammaCut  )
        
        return Selection( name + "_SelSymGamma",
                          Algorithm = _Gamma,
                          RequiredSelections = [ _StdAllTightSymGammaDD ]
                          )


    ######################################################################################################################
    #
    # Build ChiC Candidates
    #
    ######################################################################################################################
    def ChiC(self, name, inputSelJPsi, inputSelGamma, mMinChiCRaw, mMaxChiCRaw, mMinChiCFit, mMaxChiCFit):

        combCut   = "(AM > %(mMinChiCRaw)s *GeV) & (AM < %(mMaxChiCRaw)s*GeV)"  % locals()
        motherCut = "(MM > %(mMinChiCFit)s *GeV) & (MM < %(mMaxChiCFit)s*GeV)"  % locals()
        
        ChiC                  = CombineParticles("ChiC"+name)
        ChiC.DecayDescriptor  = "chi_c1(1P) -> J/psi(1S) gamma"
        ChiC.CombinationCut   = combCut
        ChiC.MotherCut        = motherCut
        ChiC.ParticleCombiners.update( { "" : "OfflineVertexFitter:PUBLIC"}  )
        
        ChiCSel = Selection(name+"_ChiCSel", Algorithm = ChiC, RequiredSelections = [inputSelJPsi, inputSelGamma])

        return ChiCSel
        

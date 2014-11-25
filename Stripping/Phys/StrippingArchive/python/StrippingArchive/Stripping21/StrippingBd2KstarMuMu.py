

__author__ = 'Jeremy Dickens'
__date__ = '16/02/2011'
__version__ = '$Revision: 1.4 $'

__all__ = ( 'StrippingBdToKstarMuMuConf' )

'''
Bd->K*mumu stripping selection using LoKi::Hybrid and python
Default line applies a slightly loosened offline selection
'''

## Note this is just for testing the stripping on 25/02/2011
## Cuts may well change before the stripping is launched

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from StrippingUtils.Utils import LineBuilder

Stripping_BdToKstarMuMu_TestDictonary = {
    'UseNoPIDsHadrons'          : True,
    
    'Prescale_BdToKstarMuMu'    : 1.0,
    'Postscale_BdToKstarMuMu'   : 1.0,

    'Prescale_BdToKstarMuMuSS'  : 1.0,
    'Postscale_BdToKstarMuMuSS' : 1.0,

    'Prescale_BuToKMuMu'        : 1.0,
    'Postscale_BuToKMuMu'       : 1.0,

    'Prescale_BuToKMuMuSS'      : 1.0,
    'Postscale_BuToKMuMuSS'     : 1.0,
   
    # B cuts
    'B_Comb_MassLow'      : 4600.0,
    'B_Comb_MassHigh'     : 6000.0,
    'B_MassLow'           : 4850.0,
    'B_MassHigh'          : 5780.0,
    'B_VertexCHI2'        :    6.0,
    'B_IPCHI2'            :   16.0,
    'B_DIRA'              :    0.014,
    'B_FlightCHI2'        :  121.0,
    'B_Dau_MaxIPCHI2'     : 9.0,
    
    # Daughter cuts
    'Dau_VertexCHI2'      :   12.0,
    'Dau_DIRA'            :   -0.9,
    
    # Kstar cuts
    'Kstar_Comb_MassLow'  :  550.0,
    'Kstar_Comb_MassHigh' : 2200.0,
    'Kstar_MassLow'       :  600.0,
    'Kstar_MassHigh'      : 2000.0,
    'Kstar_MinIPCHI2'     :    4.0,
    'Kstar_FlightChi2'    :   25.0,
    'Kstar_Dau_MaxIPCHI2' : 9.0,
    
    # JPsi (dimu) cuts
    'Dimu_FlightChi2'     :   81.0,
    'Dimu_Dau_MaxIPCHI2'  : 9.0,
    
    # Track cuts
    'Track_CHI2nDOF'      :    5.0,
 
    # Hadron cuts
    'Hadron_MinIPCHI2'    :    9.0,

    # Muon cuts
    'Muon_MinIPCHI2'      :    9.0,
    'Muon_IsMuon'         :   False,
    'MuonNoPIDs_PIDmu'    :    0.0
    }

class StrippingBdToKstarMuMuConf(LineBuilder):
    '''
    Configuration object for a Bd -> K* mu+ mu- line
    
    The Lines object holds the configured line.
    '''

    __configuration_keys__ = (
        'UseNoPIDsHadrons',
        
        'Prescale_BdToKstarMuMu',
        'Postscale_BdToKstarMuMu',
        
        'Prescale_BdToKstarMuMuSS',
        'Postscale_BdToKstarMuMuSS',

        'Prescale_BuToKMuMu',
        'Postscale_BuToKMuMu',
        
        'Prescale_BuToKMuMuSS',
        'Postscale_BuToKMuMuSS',
   
        # B cuts
        'B_Comb_MassLow',
        'B_Comb_MassHigh',
        'B_MassLow',
        'B_MassHigh',
        'B_VertexCHI2',
        'B_IPCHI2',
        'B_DIRA',
        'B_FlightCHI2',
        'B_Dau_MaxIPCHI2',
        
        # Daughter cuts
        'Dau_VertexCHI2',
        'Dau_DIRA',
        
        # Kstar cuts
        'Kstar_Comb_MassLow',
        'Kstar_Comb_MassHigh',
        'Kstar_MassLow',
        'Kstar_MassHigh',
        'Kstar_MinIPCHI2',
        'Kstar_FlightChi2',
        'Kstar_Dau_MaxIPCHI2',

        # JPsi (dimu) cuts
        'Dimu_FlightChi2',
        'Dimu_Dau_MaxIPCHI2',
        
        # Track cuts
        'Track_CHI2nDOF',
        
        # Hadron cuts
        'Hadron_MinIPCHI2',
        
        # Muon cuts
        'Muon_MinIPCHI2',
        'Muon_IsMuon',
	'MuonNoPIDs_PIDmu'
        )

    def __init__(self, name, config):
        '''The constructor of the configuration class.
        Requires a configuration dictionary, config, which must provide all the settings for cuts which are not hard coded'''
        
        LineBuilder.__init__(self, name, config)

        self.name = name

        self.BdToKstarMuMuLineName = self.name + "_BdToKstarMuMu"
        self.BuToKMuMuLineName = self.name + "_BuToKMuMu"

        
        # Define cut strings
        self.BdCombCut = "(AM > %(B_Comb_MassLow)s * MeV) & (AM < %(B_Comb_MassHigh)s * MeV)" %config

        self.BdCut = "(M > %(B_MassLow)s * MeV) & " \
                     "(M < %(B_MassHigh)s * MeV) & " \
                     "(VFASPF(VCHI2/VDOF) < %(B_VertexCHI2)s) & " \
                     "(BPVIPCHI2() < %(B_IPCHI2)s) & " \
                     "(BPVDIRA> %(B_DIRA)s) & " \
                     "(BPVVDCHI2 > %(B_FlightCHI2)s) & " \
                     "(MAXTREE(ISBASIC,MIPCHI2DV(PRIMARY))> %(B_Dau_MaxIPCHI2)s )" %config

        DaughterCuts = "(VFASPF(VCHI2/VDOF) < %(Dau_VertexCHI2)s) & " \
                       "(BPVDIRA> %(Dau_DIRA)s)" %config
        
        self.KstarCombCut = "(AM > %(Kstar_Comb_MassLow)s * MeV) & " \
                            "(AM < %(Kstar_Comb_MassHigh)s * MeV) & " \
                            "(ADOCACHI2CUT(20.,''))" %config

        self.KstarCut = DaughterCuts + " & (M > %(Kstar_MassLow)s * MeV) & " \
                        "(M < %(Kstar_MassHigh)s * MeV) & " \
                        "(BPVVDCHI2 > %(Kstar_FlightChi2)s) & " \
                        "(MIPCHI2DV(PRIMARY) > %(Kstar_MinIPCHI2)s) & " \
                        "(MAXTREE(ISBASIC,MIPCHI2DV(PRIMARY))> %(Kstar_Dau_MaxIPCHI2)s )" %config
        
        self.DiMuonCut = DaughterCuts + " & (BPVVDCHI2 > %(Dimu_FlightChi2)s) & " \
                         "(MAXTREE(ISBASIC,MIPCHI2DV(PRIMARY))> %(Dimu_Dau_MaxIPCHI2)s )" %config

        TrackCuts = "(TRCHI2DOF < %(Track_CHI2nDOF)s)" %config

        HadronCuts = "(MIPCHI2DV(PRIMARY) > %(Hadron_MinIPCHI2)s)" %config
        
        self.KaonCut = TrackCuts + " & " + HadronCuts
        self.PionCut = TrackCuts + " & " + HadronCuts
        
        self.MuonCut = TrackCuts + " & (MIPCHI2DV(PRIMARY) > %(Muon_MinIPCHI2)s)" %config

        #if(config["Muon_IsMuon"]):
        #    self.MuonCut += " & (ISMUON)"

        self.KstarFilterCut  = self.KstarCut + " & (INTREE(ABSID=='K+') & " + self.KaonCut + ") & (INTREE(ABSID=='pi+') & " + self.PionCut + ")"
        #self.DiMuonFilterCut = self.DiMuonCut + " & (2 == NINTREE((ABSID=='mu-') & " + self.MuonCut + "))"

        #########################
        ## Make the selections ##
        #########################

        Sel_Kaon = self.__Selection_FilterKaon__(self.name, self.KaonCut, config['UseNoPIDsHadrons'])
        
        Sel_Kstar = self.__Selection_CreateKstar__(self.name, self.KstarCombCut, self.KstarCut, self.KaonCut, self.PionCut, config['UseNoPIDsHadrons'] )
        
        #if(config['UseNoPIDsHadrons']):
        
        # must build the K* ourselves from the NoPIDs particles
        #    Sel_Kstar = self.__Selection_CreateKstar__(self.name, self.KstarCombCut, self.KstarCut, self.KaonCut, self.PionCut)
        #else:    
        # can just filter the StdVeryLooseDetachedKst2Kpi built using StdLooseKaons and StdLoosePions
        #    Sel_Kstar = self.__Selection_FilterKstar__(self.name, self.KstarFilterCut)
        
            
        Sel_DiMuon   = self.__Selection_CreateDiMuon__(self.name, self.DiMuonCut, self.MuonCut, config['Muon_IsMuon'] )
        Sel_DiMuonSS = self.__Selection_CreateDiMuonSS__(self.name, self.DiMuonCut, self.MuonCut, config['Muon_IsMuon'] ) 
        Sel_DiMuonLowP = self.__Selection_CreateDiMuonLowP__(self.name, self.DiMuonCut, self.MuonCut, config['Muon_IsMuon'], config['MuonNoPIDs_PIDmu'] ) 
       
        Sel_DiMuonSSLowP = self.__Selection_CreateDiMuonSSLowP__(self.name, self.DiMuonCut, self.MuonCut, config['Muon_IsMuon'], config['MuonNoPIDs_PIDmu']  ) 
       

        Sel_BdToKstarMuMu = self.__Selection_CreateBdToKstarMuMu__(self.name, [Sel_DiMuon, Sel_Kstar], self.BdCombCut, self.BdCut)
        Sel_BdToKstarMuMuSS = self.__Selection_CreateBdToKstarMuMu__(self.name + "SS", [Sel_DiMuonSS, Sel_Kstar], self.BdCombCut, self.BdCut)
        Sel_BdToKstarMuMuLowP = self.__Selection_CreateBdToKstarMuMu__(self.name+"LowP", [Sel_DiMuonLowP, Sel_Kstar], self.BdCombCut, self.BdCut)
       
        Sel_BdToKstarMuMuSSLowP = self.__Selection_CreateBdToKstarMuMu__(self.name + "SSLowP", [Sel_DiMuonSSLowP, Sel_Kstar], self.BdCombCut, self.BdCut)
     

        Sel_BuToKMuMu = self.__Selection_CreateBuToKMuMu__(self.name, [Sel_DiMuon, Sel_Kaon], self.BdCombCut, self.BdCut)
        Sel_BuToKMuMuSS = self.__Selection_CreateBuToKMuMu__(self.name + "SS", [Sel_DiMuonSS, Sel_Kaon], self.BdCombCut, self.BdCut)

        ###################################
        ## Construct the stripping lines ##
        ###################################
        
        from StrippingConf.StrippingLine import StrippingLine

        ## --- Bd -> K* mu mu (default) line --- 
        Line_BdToKstarMuMu_Name = self.BdToKstarMuMuLineName + "Line"
        Line_BdToKstarMuMu = StrippingLine( Line_BdToKstarMuMu_Name,
                                            prescale = config['Prescale_BdToKstarMuMu'],
                                            postscale = config['Postscale_BdToKstarMuMu'],
                                            selection = Sel_BdToKstarMuMu )
        self.registerLine(Line_BdToKstarMuMu)        
        
        ## --- Bd -> K* mu mu same sign line --- 
        Line_BdToKstarMuMuSS_Name = self.BdToKstarMuMuLineName + "SSLine"
        Line_BdToKstarMuMuSS = StrippingLine( Line_BdToKstarMuMuSS_Name,
                                              prescale = config['Prescale_BdToKstarMuMuSS'],
                                              postscale = config['Postscale_BdToKstarMuMuSS'],
                                              selection = Sel_BdToKstarMuMuSS )
        self.registerLine(Line_BdToKstarMuMuSS)


        ## --- Bd -> K* mu mu (low p ) line --- 
        Line_BdToKstarMuMuLowP_Name = self.BdToKstarMuMuLineName + "LowPLine"
        Line_BdToKstarMuMuLowP = StrippingLine( Line_BdToKstarMuMuLowP_Name,
                                                prescale = config['Prescale_BdToKstarMuMu'],
                                                postscale = config['Postscale_BdToKstarMuMu'],
                                                selection = Sel_BdToKstarMuMuLowP )
        self.registerLine(Line_BdToKstarMuMuLowP)        
        

                
        ## --- Bd -> K* mu mu SS (low p ) line --- 

        Line_BdToKstarMuMuSSLowP_Name = self.BdToKstarMuMuLineName + "SSLowPLine"
        Line_BdToKstarMuMuSSLowP = StrippingLine( Line_BdToKstarMuMuSSLowP_Name,
                                              prescale = config['Prescale_BdToKstarMuMuSS'],
                                              postscale = config['Postscale_BdToKstarMuMuSS'],
                                              selection = Sel_BdToKstarMuMuSSLowP )
        self.registerLine(Line_BdToKstarMuMuSSLowP)


        
        ## --- Bu -> K mu mu (default) line --- 
        Line_BuToKMuMu_Name = self.BuToKMuMuLineName + "Line"
        Line_BuToKMuMu = StrippingLine( Line_BuToKMuMu_Name,
                                        prescale = config['Prescale_BuToKMuMu'],
                                        postscale = config['Postscale_BuToKMuMu'],
                                        selection =  Sel_BuToKMuMu)
        self.registerLine(Line_BuToKMuMu)        
        
        ## --- Bu -> K mu mu same sign line --- 
        Line_BuToKMuMuSS_Name = self.BuToKMuMuLineName + "SSLine"
        Line_BuToKMuMuSS = StrippingLine( Line_BuToKMuMuSS_Name,
                                          prescale = config['Prescale_BuToKMuMuSS'],
                                          postscale = config['Postscale_BuToKMuMuSS'],
        selection =  Sel_BuToKMuMuSS)
        self.registerLine(Line_BuToKMuMuSS)

        #self.printCuts()
        
    def printCuts(self):
        
        '''Print the compiled cut values'''

        print "-----------------------------------------"
        print "--  Bd -> K* mu mu stripping line cuts --"
        print "-----------------------------------------"
        print " "
        print "Bd cut:                ", self.BdCut
        print "Bd combination cut:    ", self.BdCombCut
        print "Kstar cut:             ", self.KstarCut
        print "Kstar combination cut: ", self.KstarCombCut
        print "DiMuon cut:            ", self.DiMuonCut
        print "Kaon cut:              ", self.KaonCut
        print "Pion cut:              ", self.PionCut
        print "Muon cut:              ", self.MuonCut

        print " "
        print "-----------------------------------------------------------"
        print "-- the cuts below should be compiled from the ones above --"
        print "-----------------------------------------------------------"
        print " "
        #print "DiMuonFilter cut:   ", self.DiMuonFilterCut
        print "KstarFilter cut:    ", self.KstarFilterCut
        
        return True
    
    ##########################################
    ## Create selections for StrippingLines ##
    ##########################################
        
    #################
    ## Filter Kaon ## 
    #################

    def __Selection_FilterKaon__(self, lName, KaonCuts, UseNoPIDsHadrons):
        '''
        Kaon filter for Bu -> K mu mu (from StdLooseKaons)
        '''

        from StandardParticles import StdAllLooseKaons
        from StandardParticles import StdAllNoPIDsKaons

        MyStdKaons = None
        
        if(UseNoPIDsHadrons == True):
            MyStdKaons = StdAllNoPIDsKaons
        else:
            MyStdKaons = StdAllLooseKaons
  
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        FilterKaon = FilterDesktop()
        FilterKaon.Code = KaonCuts

        from PhysSelPython.Wrappers import Selection
        SelKaon = Selection("Sel_" + lName + "_Kaon", Algorithm = FilterKaon, RequiredSelections = [MyStdKaons])
        
        return SelKaon

    ##################
    ## Filter Kstar ## 
    ##################

    def __Selection_CreateKstar__(self, lName, KstarCombCut, KstarCut, KaonCut, PionCut, UseNoPIDsParticles ):
        '''
        Make K* using StdNoPID common particles
        '''

        from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsKaons, StdAllLooseKaons, StdAllLoosePions

        _requires =  [StdAllNoPIDsKaons, StdAllNoPIDsPions] if UseNoPIDsParticles else [StdAllLooseKaons, StdAllLoosePions] 
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineKstar = CombineParticles()

        CombineKstar.DecayDescriptor = "[K*(892)0 -> K+ pi-]cc"

        CombineKstar.CombinationCut = KstarCombCut
        CombineKstar.MotherCut      = KstarCut
        CombineKstar.DaughtersCuts = { "K+" : KaonCut, "pi-" : PionCut }

        from PhysSelPython.Wrappers import Selection
        SelKstar = Selection("Sel_" + lName + "_Kstar", Algorithm = CombineKstar, RequiredSelections = _requires ) #[StdNoPIDsPions, StdNoPIDsKaons])

        return SelKstar
    
    def __Selection_FilterKstar__(self, lName, KstarCuts):
        '''
        Kstar filter for Bd -> KstarMuMu (from StdVeryLooseDetachedKst2Kpi)
        '''
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        FilterKstar = FilterDesktop()
        FilterKstar.Code = KstarCuts
        
        from PhysSelPython.Wrappers import DataOnDemand
        StdKstar = DataOnDemand(Location = "Phys/StdVeryLooseDetachedKst2Kpi/Particles")
        
        from PhysSelPython.Wrappers import Selection        
        SelKstar = Selection("Sel_" + lName + "_Kstar", Algorithm = FilterKstar, RequiredSelections = [StdKstar])
        
        return SelKstar
        
    ##########################
    ## Make / Filter DiMuon ## 
    ##########################
    
    def __Selection_FilterDiMuon__(self, lName, DiMuonCuts, MuonCuts, IsMuonFlag ):
        '''
        DiMuon filter for Bd -> KstarMuMu (from StdVeryLooseDiMuon)
        '''

        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        FilterDiMuon = FilterDesktop()
        FilterDiMuon.Code = DiMuonCuts + " & (2 == NINTREE((ABSID=='mu-') & "+MuonCuts+"))"

        from PhysSelPython.Wrappers import DataOnDemand

        StdDiMuon = None
        
        if IsMuonFlag :
            StdDiMuon = DataOnDemand(Location = "Phys/StdLooseDiMuon/Particles")
        else:
            StdDiMuon = DataOnDemand(Location = "Phys/StdVeryLooseDiMuon/Particles")
            
        from PhysSelPython.Wrappers import Selection
        SelDiMuon = Selection("Sel_" + lName + "_DiMuon", Algorithm = FilterDiMuon, RequiredSelections = [StdDiMuon])
        
        return SelDiMuon

    def __Selection_CreateDiMuon__(self, lName, DiMuonCuts, MuonCuts, IsMuonFlag):
        '''
        Create a new dimuon from scratch
        '''
        from  GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineDiMuon = CombineParticles()
        CombineDiMuon.DecayDescriptor = "J/psi(1S) -> mu- mu+"
        CombineDiMuon.DaughtersCuts = { "mu+" : MuonCuts, "mu-" : MuonCuts }
        CombineDiMuon.MotherCut     = DiMuonCuts

        from StandardParticles import StdAllLooseMuons, StdAllVeryLooseMuons
        #Muons = StdAllLooseMuons if IsMuonFlag else StdAllVeryLooseMuons
        Muons = StdAllLooseMuons if IsMuonFlag else StdAllVeryLooseMuons
         
        from PhysSelPython.Wrappers import Selection
        SelDiMuon = Selection("Sel_" + lName + "_DiMuon", Algorithm = CombineDiMuon, RequiredSelections = [ Muons ] )
        return SelDiMuon
        
    def __Selection_CreateDiMuonSS__(self, lName, DiMuonCuts, MuonCuts, IsMuonFlag):
        '''
        Clone the StdVeryLooseDiMuon to build same sign candidates
        '''
        from  GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineDiMuonSS = CombineParticles()
        CombineDiMuonSS.DecayDescriptor = "[J/psi(1S) -> mu+ mu+]cc"
        CombineDiMuonSS.DaughtersCuts = { "mu+" : MuonCuts, "mu-" : MuonCuts }
        CombineDiMuonSS.MotherCut     = DiMuonCuts

        from StandardParticles import StdAllLooseMuons, StdAllVeryLooseMuons
        Muons = StdAllLooseMuons if IsMuonFlag else StdAllVeryLooseMuons
        
        from PhysSelPython.Wrappers import Selection
        SelDiMuonSS = Selection("Sel_" + lName + "_DiMuonSS", Algorithm = CombineDiMuonSS, RequiredSelections = [ Muons ] )
        return SelDiMuonSS


    def __Selection_CreateDiMuonLowP__(self, lName, DiMuonCuts, MuonCuts, IsMuonFlag, MuonPIDmu ):
        '''
        Create a new dimuon from scratch
        '''
        from StandardParticles import StdAllNoPIDsMuons, StdAllVeryLooseMuons
        

        #_requires =  [StdAllNoPIDsMuons, StdAllVeryLooseMuons] 
        _requires =  [StdAllNoPIDsMuons] 

        from  GaudiConfUtils.ConfigurableGenerators import CombineParticles

        CombineDiMuonLowP = CombineParticles()
        CombineDiMuonLowP.DecayDescriptor = "J/psi(1S) -> mu+ mu-"
        CombineDiMuonLowP.DaughtersCuts = {  "mu+" : MuonCuts , "mu-" : MuonCuts }
        if IsMuonFlag :
            CombineDiMuonLowP.CombinationCut =  "( ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)>0.5,1) & ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)<0.5,2) &  ACHILDCUT(ISMUON, 1) & ACHILDCUT(PIDmu > "+str(MuonPIDmu) + " ,2) ) | ( ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)>0.5,2) & ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)<0.5,1) &  ACHILDCUT(ISMUON, 2) & ACHILDCUT(PIDmu > "+ str(MuonPIDmu) +" ,1) )"

        else:
            CombineDiMuonLowP.CombinationCut =  "( ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)>0.5,1) & ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)<0.5,2) &  ACHILDCUT(ISMUONLOOSE, 1) & ACHILDCUT(PIDmu > "+str(MuonPIDmu) + " ,2) ) | ( ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)>0.5,2) & ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)<0.5,1) &  ACHILDCUT(ISMUONLOOSE, 2) & ACHILDCUT(PIDmu > "+ str(MuonPIDmu) +" ,1) )"

        CombineDiMuonLowP.MotherCut     = DiMuonCuts

        from StandardParticles import StdAllLooseMuons, StdAllVeryLooseMuons
        Muons = StdAllLooseMuons if IsMuonFlag else StdAllVeryLooseMuons
        
        from PhysSelPython.Wrappers import Selection
        SelDiMuonLowP = Selection("Sel_" + lName + "_DiMuonLowP", Algorithm = CombineDiMuonLowP ,RequiredSelections = _requires )
        return SelDiMuonLowP

    def __Selection_CreateDiMuonSSLowP__(self, lName, DiMuonCuts, MuonCuts, IsMuonFlag, MuonPIDmu):
        '''
        Clone the StdVeryLooseDiMuon to build same sign candidates
        '''
        from StandardParticles import StdAllNoPIDsMuons, StdAllVeryLooseMuons
        _requires =  [StdAllNoPIDsMuons] 
        from  GaudiConfUtils.ConfigurableGenerators import CombineParticles
        muhigh = '(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) '
        mulow = '(0.5>PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) &  ( PIDmu > 3.0 )' 
        CombineDiMuonSSLowP = CombineParticles()
        CombineDiMuonSSLowP.DecayDescriptor = "[J/psi(1S) -> mu+ mu+]cc"
        CombineDiMuonSSLowP.DaughtersCuts = { "mu+" : MuonCuts,  "mu-": MuonCuts  }
        
        if IsMuonFlag:
            CombineDiMuonSSLowP.CombinationCut =  "( ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)>0.5,1) & ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)<0.5,2) &  ACHILDCUT(ISMUON, 1) & ACHILDCUT(PIDmu > "+str(MuonPIDmu) +",2) ) | ( ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)>0.5,2) & ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)<0.5,1) &  ACHILDCUT(ISMUON, 2) & ACHILDCUT(PIDmu > "+str(MuonPIDmu) + " ,1) )"
        else:
            CombineDiMuonSSLowP.CombinationCut =  "( ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)>0.5,1) & ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)<0.5,2) &  ACHILDCUT(ISMUONLOOSE, 1) & ACHILDCUT(PIDmu > 0.0 ,2) ) | ( ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)>0.5,2) & ACHILDCUT(PPINFO(LHCb.ProtoParticle.InAccMuon,-1)<0.5,1) &  ACHILDCUT(ISMUONLOOSE, 2) & ACHILDCUT(PIDmu > 0.0 ,1) )"
       
        CombineDiMuonSSLowP.MotherCut     = DiMuonCuts

        from StandardParticles import StdAllLooseMuons, StdAllVeryLooseMuons
        Muons = StdAllLooseMuons if IsMuonFlag else StdAllVeryLooseMuons
        
        from PhysSelPython.Wrappers import Selection
        SelDiMuonSSLowP = Selection("Sel_" + lName + "_DiMuonSSLowP", Algorithm = CombineDiMuonSSLowP, RequiredSelections = _requires )
        return SelDiMuonSSLowP





##  from CommonParticles.StdVeryLooseDiMuon import StdVeryLooseDiMuon
##         from CommonParticles.StdLooseDimuon import StdLooseDiMuon
    
##         DiMuonSS = None

##         if IsMuonFlag:
##             DiMuonSS = StdLooseDiMuon.clone("Combine_" + lName + "_DiMuonSS")
##         else:
##             DiMuonSS = StdVeryLooseDiMuon.clone("Combine_" + lName + "_DiMuonSS")

##         DiMuonSS.DecayDescriptor = "[J/psi(1S) -> mu+ mu+]cc"

##         DiMuonSS.DaughtersCuts = { "mu+" :  MuonCuts }
##         DiMuonSS.MotherCut = DiMuonCuts

##         from PhysSelPython.Wrappers import Selection
##         SelDiMuonSS = Selection("Sel_" + lName + "_DiMuonSS", Algorithm = DiMuonSS, RequiredSelections = self.GetRequiredSels(DiMuonSS))

##         return SelDiMuonSS
        
    #####################
    ## Construct the B ##
    #####################
    
    def __Selection_CreateBdToKstarMuMu__(self, lName, InputSelections, CombCut, MotherCut):
        '''
        Bd -> K* mu mu Selection
        '''
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineBd = CombineParticles()

        CombineBd.DecayDescriptor = "[B0 -> K*(892)0 J/psi(1S)]cc"

        CombineBd.CombinationCut = CombCut
        CombineBd.MotherCut = MotherCut

        from PhysSelPython.Wrappers import Selection
        SelBd = Selection("Sel_" + lName + "_Bd", Algorithm = CombineBd, RequiredSelections = InputSelections)
        
        return SelBd

    def __Selection_CreateBuToKMuMu__(self, lName, InputSelections, CombCut, MotherCut):
        '''
        Bu -> K mu mu Selection
        '''
       
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineBu = CombineParticles()
        
        CombineBu.DecayDescriptor = "[B+ -> K+ J/psi(1S)]cc"

        CombineBu.CombinationCut = CombCut
        CombineBu.MotherCut = MotherCut
        
        from PhysSelPython.Wrappers import Selection
        SelBu = Selection("Sel_" + lName + "_Bu", Algorithm = CombineBu, RequiredSelections = InputSelections)
        
        return SelBu

    def GetRequiredSels(self, Alg):
        """
        Extract the list of RequiredSelections from an object with fixes input locations
        """

        if hasattr(Alg, 'Inputs'):

            from PhysSelPython.Wrappers import DataOnDemand

            _requires = []
            for loc in Alg.Inputs:
                _requires += [ DataOnDemand( Location = loc ) ]

                
            Alg.Inputs = []
            return _requires
        
        else:
            raise Exception("Algorithm does not have property 'Inputs'") 
            

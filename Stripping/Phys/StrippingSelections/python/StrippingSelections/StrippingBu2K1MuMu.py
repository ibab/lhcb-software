

__author__ = 'Evelina Gersabeck'
__date__ = '10/01/2012'
__version__ = '$Revision: 1.0 $'

__all__ = ( 'StrippingBuToK1MuMuConf' )

'''
Bu->K1mumu stripping selection using LoKi::Hybrid and python
Default line applies a slightly loosened offline selection
'''

from Gaudi.Configuration import *
from LHCbKernel.Configuration import *

from StrippingUtils.Utils import LineBuilder

Stripping_BuToK1MuMu_TestDictonary = {
    'UseNoPIDsHadrons'          : True,
    
    'Prescale_BuToK1MuMu'    : 1.0,
    'Postscale_BuToK1MuMu'   : 1.0,

    'Prescale_BuToK1MuMuSS'  : 1.0,
    'Postscale_BuToK1MuMuSS' : 1.0,
    
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
    
    # K1 cuts
    'K1_Comb_MassLow'  :  720.0,
    'K1_Comb_MassHigh' : 2450.0,
    'K1_MassLow'       :  750.0,
    'K1_MassHigh'      : 2400.0,
    'K1_MinIPCHI2'     :    4.0,
    'K1_FlightChi2'    :   25.0,
    'K1_Dau_MaxIPCHI2' : 9.0,

    
    # JPsi (dimu) cuts
    'Dimu_FlightChi2'     :   81.0,
    'Dimu_Dau_MaxIPCHI2'  : 9.0,
    
    # Track cuts
    'Track_CHI2nDOF'      :    2.5,
 
    # Hadron cuts
    'Hadron_MinIPCHI2'    :    16.0,
    'Hadron_PT'           :    500,
    'K_PIDK_min'          :      0,
    'pi_PIDK_max'        :      5,

    # Muon cuts
    'Muon_MinIPCHI2'      :    16.0,
    'Muon_IsMuon'         :   False,
    'MuonNoPIDs_PIDmu'    :    0.0
    }

class StrippingBuToK1MuMuConf(LineBuilder):
    '''
    Configuration object for a Bu -> K1 mu+ mu- line
    
    The Lines object holds the configured line.
    '''
    
    __configuration_keys__ = (
        'UseNoPIDsHadrons',
        
        'Prescale_BuToK1MuMu',
        'Postscale_BuToK1MuMu',
        
        'Prescale_BuToK1MuMuSS',
        'Postscale_BuToK1MuMuSS',
        
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
        
            
        # K1 cuts
        'K1_Comb_MassLow',
        'K1_Comb_MassHigh',
        'K1_MassLow',
        'K1_MassHigh',
        'K1_MinIPCHI2',
        'K1_FlightChi2',
        'K1_Dau_MaxIPCHI2',

        
        # JPsi (dimu) cuts
        'Dimu_FlightChi2',
        'Dimu_Dau_MaxIPCHI2',
        
        # Track cuts
        'Track_CHI2nDOF',
        
        # Hadron cuts
        'Hadron_MinIPCHI2',
        'Hadron_PT',         
        'K_PIDK_min',         
        'pi_PIDK_max',       
        
        # Muon cuts
        'Muon_MinIPCHI2',
        'Muon_IsMuon',
	'MuonNoPIDs_PIDmu'
        )
    
    def __init__(self, name, config):
        '''The constructor of the configuration class.
        Requires a configuration dictionary, config, which must provide all the settings for cuts which are not hard coded'''
        
        LineBuilder.__init__(self, name, config)

 #       _globalEventCuts = (
 #           "(recSummary (LHCb.RecSummary.nLongTracks, 'Rec/Track/Long') < 800 )"
 #           ) % config
        
        self.name = name
        
        self.BuToK1MuMuLineName = self.name + "_BuToK1MuMu"
        
        
        # Define cut strings
        self.BuCombCut = "(AM > %(B_Comb_MassLow)s * MeV) & (AM < %(B_Comb_MassHigh)s * MeV)" %config
        
        self.BuCut = "(M > %(B_MassLow)s * MeV) & " \
                     "(M < %(B_MassHigh)s * MeV) & " \
                     "(VFASPF(VCHI2/VDOF) < %(B_VertexCHI2)s) & " \
                     "(BPVIPCHI2() < %(B_IPCHI2)s) & " \
                     "(BPVDIRA> %(B_DIRA)s) & " \
                     "(BPVVDCHI2 > %(B_FlightCHI2)s) & " \
                     "(MAXTREE(ISBASIC,MIPCHI2DV(PRIMARY))> %(B_Dau_MaxIPCHI2)s )" %config
        
        DaughterCuts = "(VFASPF(VCHI2/VDOF) < %(Dau_VertexCHI2)s) & " \
                       "(BPVDIRA> %(Dau_DIRA)s)" %config
                
        self.K1CombCut = "(AM > %(K1_Comb_MassLow)s * MeV) & " \
                         "(AM < %(K1_Comb_MassHigh)s * MeV) & " \
                         "(ADOCACHI2CUT(20.,''))" %config
        
        self.K1Cut = DaughterCuts + " & (M > %(K1_MassLow)s * MeV) & " \
                     "(M < %(K1_MassHigh)s * MeV) & " \
                     "(BPVVDCHI2 > %(K1_FlightChi2)s) & " \
                     "(MIPCHI2DV(PRIMARY) > %(K1_MinIPCHI2)s) &" \
                     "(MAXTREE(ISBASIC,MIPCHI2DV(PRIMARY))> %(K1_Dau_MaxIPCHI2)s )" %config
        
        
        self.DiMuonCut = DaughterCuts + " & (BPVVDCHI2 > %(Dimu_FlightChi2)s) & " \
                         "(MAXTREE(ISBASIC,MIPCHI2DV(PRIMARY))> %(Dimu_Dau_MaxIPCHI2)s )" %config
        
        TrackCuts = "(TRCHI2DOF < %(Track_CHI2nDOF)s) " %config
        
        HadronCuts = "(MIPCHI2DV(PRIMARY) > %(Hadron_MinIPCHI2)s)&"\
                     "(PT > %(Hadron_PT)s * MeV)" %config
        
        self.KaonCut = TrackCuts + " & " + HadronCuts #+ "& (PIDK > %(K_PIDK_min)s)" %config
        self.PionCut = TrackCuts + " & " + HadronCuts #+ "& (PIDK < %(pi_PIDK_max)s)" %config
        
        self.MuonCut = TrackCuts + " & (MIPCHI2DV(PRIMARY) > %(Muon_MinIPCHI2)s)" %config
        
        #if(config["Muon_IsMuon"]):
        #    self.MuonCut += " & (ISMUON)"

        
        self.K1FilterCut  = self.K1Cut + " & (INTREE(ABSID=='K+') & " + self.KaonCut + ") & (INTREE(ABSID=='pi+') & " + self.PionCut + ")"
        #self.DiMuonFilterCut = self.DiMuonCut + " & (2 == NINTREE((ABSID=='mu-') & " + self.MuonCut + "))"
        
        #########################
        ## Make the selections ##
        #########################

        Sel_Kaon = self.__Selection_FilterKaon__(self.name, self.KaonCut, config['UseNoPIDsHadrons'])
        
        Sel_K1 = self.__Selection_CreateK1__(self.name, self.K1CombCut, self.K1Cut, self.KaonCut, self.PionCut, config['UseNoPIDsHadrons'] )
        
        
        Sel_DiMuon   = self.__Selection_CreateDiMuon__(self.name, self.DiMuonCut, self.MuonCut, config['Muon_IsMuon'] )
        Sel_DiMuonSS = self.__Selection_CreateDiMuonSS__(self.name, self.DiMuonCut, self.MuonCut, config['Muon_IsMuon'] ) 
                
                
        Sel_BuToK1MuMu = self.__Selection_CreateBuToK1MuMu__(self.name, [Sel_DiMuon, Sel_K1], self.BuCombCut, self.BuCut)
        Sel_BuToK1MuMuSS = self.__Selection_CreateBuToK1MuMu__(self.name + "SS", [Sel_DiMuonSS, Sel_K1], self.BuCombCut, self.BuCut)
        
                
        ###################################
        ## Construct the stripping lines ##
        ###################################
        
        from StrippingConf.StrippingLine import StrippingLine
                        
        ## --- Bu -> K1 mu mu (default) line --- 
        Line_BuToK1MuMu_Name = self.BuToK1MuMuLineName + "Line"
        Line_BuToK1MuMu = StrippingLine( Line_BuToK1MuMu_Name,
                                         prescale = config['Prescale_BuToK1MuMu'],
                                         postscale = config['Postscale_BuToK1MuMu'],
                                         selection = Sel_BuToK1MuMu )
        self.registerLine(Line_BuToK1MuMu)        
        
        ## --- Bu -> K1 mu mu same sign line --- 
        Line_BuToK1MuMuSS_Name = self.BuToK1MuMuLineName + "SSLine"
        Line_BuToK1MuMuSS = StrippingLine( Line_BuToK1MuMuSS_Name,
                                           prescale = config['Prescale_BuToK1MuMuSS'],
                                           postscale = config['Postscale_BuToK1MuMuSS'],
                                           selection = Sel_BuToK1MuMuSS )
        self.registerLine(Line_BuToK1MuMuSS)
               
        #self.printCuts()
        
    def printCuts(self):
            
        '''Print the compiled cut values'''
        
        print "-----------------------------------------"
        print "--  Bu -> K1 mu mu stripping line cuts --"
        print "-----------------------------------------"
        print " "
        print "Bu cut:                ", self.BuCut
        print "Bu combination cut:    ", self.BuCombCut
        print "K1 cut:                ", self.K1Cut
        print "K1 combination cut   : ", self.K1CombCut
        print "DiMuon cut:            ", self.DiMuonCut
        print "Kaon cut:              ", self.KaonCut
        print "Pion cut:              ", self.PionCut
        print "Muon cut:              ", self.MuonCut
        
        print " "
        print "-----------------------------------------------------------"
        print "-- the cuts below should be compiled from the ones above --"
        print "-----------------------------------------------------------"
        print " "
        
        print "DiMuonFilter cut:   ", self.DiMuonFilterCut
        
        print "K1Filter cut:       ", self.K1FilterCut
        
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
        from StandardParticles import StdAllLooseKaons    # StdLooseKaons have IP + PT cut 
        from StandardParticles import StdAllNoPIDsKaons   # StdNoPIDsKaons have IP + PT cut 
               
        MyStdKaons = None
        

        if(UseNoPIDsHadrons == True):
            MyStdKaons = StdAllNoPIDsKaons
        else:
            MyStdKaons = StdAllLooseKaons

#just a test
#        MyStdKaons = StdAllLooseKaons
            
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        FilterKaon = FilterDesktop()
        FilterKaon.Code = KaonCuts
        
        from PhysSelPython.Wrappers import Selection
        SelKaon = Selection("Sel_" + lName + "_Kaon", Algorithm = FilterKaon, RequiredSelections = [MyStdKaons])
        
        return SelKaon
    
        
    
    ##################
    ## Filter K1 ## 
    ##################
    
    def __Selection_CreateK1__(self, lName, K1CombCut, K1Cut, KaonCut, PionCut, UseNoPIDsParticles ):
        '''
        Make K1 using StdNoPID common particles
        '''
        
        from StandardParticles import StdAllNoPIDsPions, StdAllNoPIDsKaons, StdAllLooseKaons, StdAllLoosePions

        #just a test!!!!!
        _requires =  [StdAllNoPIDsKaons, StdAllNoPIDsPions] if UseNoPIDsParticles else [StdAllLooseKaons, StdAllLoosePions] 
        #_requires = [StdAllLooseKaons, StdAllLoosePions]         
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineK1 = CombineParticles()
        
        CombineK1.DecayDescriptor = "[K_1(1270)+ -> K+ pi- pi+]cc"
        
        CombineK1.CombinationCut = K1CombCut
        CombineK1.MotherCut      = K1Cut
        CombineK1.DaughtersCuts = { "K+" : KaonCut, "pi-" : PionCut }
        
        from PhysSelPython.Wrappers import Selection
        SelK1 = Selection("Sel_" + lName + "_K1", Algorithm = CombineK1, RequiredSelections = _requires ) #[StdNoPIDsPions, StdNoPIDsKaons])
        
        return SelK1
    
    
    ##########################
    ## Make / Filter DiMuon ## 
    ##########################
    
    def __Selection_FilterDiMuon__(self, lName, DiMuonCuts, MuonCuts, IsMuonFlag ):
        '''
        DiMuon filter for Bu -> K1MuMu (from StdVeryLooseDiMuon)
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
    
    def __Selection_CreateBuToK1MuMu__(self, lName, InputSelections, CombCut, MotherCut):
        '''
        Bu -> K1 mu mu Selection
        '''
        
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        CombineBu = CombineParticles()
        
        CombineBu.DecayDescriptor = "[B+ -> K_1(1270)+ J/psi(1S)]cc"
        
        CombineBu.CombinationCut = CombCut
        CombineBu.MotherCut = MotherCut
        
        from PhysSelPython.Wrappers import Selection
        SelBu_K1 = Selection("Sel_" + lName + "_Bu_K1", Algorithm = CombineBu, RequiredSelections = InputSelections)
        
        return SelBu_K1
    

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
            

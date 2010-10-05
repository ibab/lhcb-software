# $Id: StrippingBs2ChicPhi_Chic2KKPiPi.py,v 1.2 2010-05-10 12:12:05 jpalac Exp $


__author__ = ['Katarzyna Senderowska']
__date__ = '04/10/2010'
__version__ = '$Revision: 1.2 $'


#Vertex Fitter
combiner='OfflineVertexFitter'


confdict={
    'Tuned'   : {
                 'Prescale'    : 1.0 ,
                 'Postscale'   : 1.0 ,
                 #K parameters
                 'K_phi_PT'               : 500.,
                 'K_phi_TRCHI2'           : 5.,
                 'K_phi_IPCHI2'           : 9.,
                 #phi parameters
                 'phiVDZ'                 : 0., 
                 'phiPT'                  : 1000.,
                 'phiIPCHI2'              : 9.,
                 'phiVCHI2/VDOF'          : 16.,
                 #pi (for chi_c decay) parameters
                 'pi_chic_PT'             : 450., 
                 'pi_chic_TRCHI2'         : 5,
                 'pi_chic_IPCHI2'         : 9.,
                 #rho (for chi_c decay) parameters
                 'rho_chic_VDZ'           : 0.,
                 'rho_chic_VCHI2/VDOF'    : 16.,
                 #K (for chi_c decay) parameters
                 'K_chic_PT'              : 450.,
                 'K_chic_TRCHI2'          : 5.,
                 'K_chic_IPCHI2'          : 9.,
                 #phi (for eta_c decay) parameters
                 'phi_chic_VDZ'           : 0.,
                 'phi_chic_VCHI2/VDOF'    : 16.,
                 #chi_c parameters
                 'chicVDZ'                : 0.,   
                 'chicPT'                 : 1000., 
                 'chicIPCHI2'             : 9.,
                 'chicVCHI2/VDOF'         : 16.,               
                 #Bs parameters
                 'BsVDZ'                  : 0.,
                 'BsDIRA'                 : 0.995,
                 'BsIPCHI2'               : 9.,
                 'BsVCHI2/VDOF'           : 16.,
                },
    }


class Bs2ChicPhi_Chic2KKPiPiAllLinesConf(object):
    
    Lines=[]
    
    confObjects={}
    
    def __init__(self, config, offLines=[]):
        for aline in config.keys():
            if aline not in offLines:
                lineconf=Bs2ChicPhi_Chic2KKPiPiOneLineConf(aline, config[aline])
                self.confObjects[aline]=lineconf
                self.Lines.append(lineconf.Line)
                
    def printCuts(self):
        for aline in self.confObjects.keys():
            print '===='
            self.confObjects[aline].printCuts()
            

class Bs2ChicPhi_Chic2KKPiPiOneLineConf(object):
    
    Line=None
    Selections=[]
    TopSelectionSeq=None

    ###  cuts ###
    
    KCut=''
    PhiCombCut=''    
    PhiCut=''
    Pi_chic_Cut=''
    Rho_chic_CombCut=''
    K_chic_Cut=''
    Phi_chic_CombCut=''
    ChicCombCut=''
    ChicCut=''
    BsCombCut=''
    BsCut=''
    
    PhiSel=None
    Phi_chic_Sel=None
    Rho_chic_Sel=None
    ChicSel=None
    BsSel=None
    
    LineSuffix=''
    
    __configuration_keys__=[
        'Prescale',
        'Postscale',     
        #K parameters
        'K_phi_PT',
        'K_phi_TRCHI2',
        'K_phi_IPCHI2',
        #phi parameters
        'phiVDZ',        
        'phiPT',
        'phiIPCHI2',
        'phiVCHI2/VDOF',
        #pi (for chi_c decay) parameters
        'pi_chic_PT',
        'pi_chic_TRCHI2',        
        'pi_chic_IPCHI2',             
        #rho (for chi_c decay) parameters
        'rho_chic_VDZ',
        'rho_chic_VCHI2/VDOF',    
        #K (for chi_c decay) parameters
        'K_chic_PT',
        'K_chic_TRCHI2',         
        'K_chic_IPCHI2',    
        #phi (for chi_c decay) parameters
        'phi_chic_VDZ',
        'phi_chic_VCHI2/VDOF',              
        #chi_c parameters
        'chicVDZ',  
        'chicPT',
        'chicIPCHI2', 
        'chicVCHI2/VDOF',                                
        #Bs parameters
        'BsVDZ',  
        'BsDIRA',
        'BsIPCHI2',
        'BsVCHI2/VDOF',            
        ]
    
    def __init__(self, LineSuffix, config):

        from StrippingSelections.Utils import checkConfig
        
        checkConfig(Bs2ChicPhi_Chic2KKPiPiOneLineConf.__configuration_keys__,config)    

        self.LineSuffix=LineSuffix
        
        self.KCut = "(PT > %(K_phi_PT)s*MeV) &"\
                    "(TRCHI2DOF < %(K_phi_TRCHI2)s) &"\
                    "(MIPCHI2DV(PRIMARY) > %(K_phi_IPCHI2)s) " % config
 
        self.PhiCombCut = "(ADAMASS('phi(1020)') < 50*MeV) &"\
                          "(APT > %(phiPT)s*MeV) " % config

        self.PhiCut = "(BPVVDZ > %(phiVDZ)s) &"\
                      "(VFASPF(VCHI2/VDOF) < %(phiVCHI2/VDOF)s) " % config
        
        self.Pi_chic_Cut = "(PT > %(pi_chic_PT)s*MeV) &"\
                           "(TRCHI2DOF < %(pi_chic_TRCHI2)s) &"\
                           "(MIPCHI2DV(PRIMARY) > %(pi_chic_IPCHI2)s) " % config

        self.Rho_chic_CombCut = "(AM > 400.*MeV) &"\
                               "(AM < 3413.*MeV) " % config

        self.Rho_chic_Cut = "(BPVVDZ > %(rho_chic_VDZ)s) &"\
                            "(VFASPF(VCHI2/VDOF) < %(rho_chic_VCHI2/VDOF)s) " % config

        self.K_chic_Cut = "(PT > %(K_chic_PT)s*MeV) &"\
                          "(TRCHI2DOF < %(K_chic_TRCHI2)s) &"\
                          "(MIPCHI2DV(PRIMARY) > %(K_chic_IPCHI2)s) " % config
        
        self.Phi_chic_CombCut = "(AM > 400.*MeV) &"\
                                "(AM < 3413.*MeV) " % config

        self.Phi_chic_Cut = "(BPVVDZ > %(phi_chic_VDZ)s) &" \
                            "(VFASPF(VCHI2/VDOF) < %(phi_chic_VCHI2/VDOF)s) " % config

        self.ChicCombCut = "(ADAMASS('chi_c0(1P)')<100.*MeV) &"\
                           "(APT > %(chicPT)s*MeV) " % config

        self.ChicCut = "(BPVVDZ > %(chicVDZ)s) &"\
                       "(MIPCHI2DV(PRIMARY) > %(chicIPCHI2)s) &"\
                       "(VFASPF(VCHI2/VDOF) < %(chicVCHI2/VDOF)s) " % config
                
        self.BsCombCut = "(ADAMASS('B_s0')<300.*MeV) " % config

        self.BsCut = "(BPVVDZ > %(BsVDZ)s) &"\
                     "(BPVDIRA > %(BsDIRA)s) &"\
                     "(MIPCHI2DV(PRIMARY) < %(BsIPCHI2)s) &"\
                     "(VFASPF(VCHI2/VDOF) < %(BsVCHI2/VDOF)s) " % config



        ###  selections ###
        
               
        self.__MakePhi__()
        self.__MakeChic__()
        self.__MakeBs__()
        
        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import SelectionSequence
        

        ### stripping line ###
        Bs2ChicPhi_Chic2KKPiPiLine=StrippingLine("Bs2ChicPhi_Chic2KKPiPi"+self.LineSuffix,
                                                 prescale = config['Prescale'],
                                                 postscale = config['Postscale'],
                                                 algos = [ self.BsSel ]
                                                 )
        
        ### Collect them all together in a nice way ###
        self.Line=Bs2ChicPhi_Chic2KKPiPiLine
        self.Selections=[self.PhiSel, self.Phi_chic_Sel, self.Rho_chic_Sel, self.ChicSel, self.BsSel]
   

      
    def printCuts(self):
        print 'name', self.LineSuffix
        print 'KCut', self.KCut
        print 'PhiCombCut', self.PhiCombCut
        print 'PhiCut', self.PhiCut
        print 'Pi_chic_Cut', self.Pi_chic_Cut
        print 'Rho_chic_CombCut', self.Rho_chic_CombCut
        print 'Rho_chic_Cut', self.Rho_chic_Cut
        print 'K_chic_Cut', self.K_chic_Cut
        print 'Phi_chic_CombCut', self.Phi_chic_CombCut
        print 'Phi_chic_Cut', self.Phi_chic_Cut
        print 'ChicCut', self.ChicCut
        print 'ChicCombCut', self.ChicCombCut
        print 'BsCombCut', self.BsCombCut
        print 'BsCut', self.BsCut

        
    ############ Functions to make Selections #######################
        
    def __MakePhi__(self):

        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        PhiForBs2ChicPhi_Chic2KKPiPi = CombineParticles(self.LineSuffix+"PhiForBs2ChicPhi_Chic2KKPiPi")
        PhiForBs2ChicPhi_Chic2KKPiPi.DecayDescriptor = "[phi(1020) -> K+ K-]cc" 
        PhiForBs2ChicPhi_Chic2KKPiPi.DaughtersCuts = { "K-"  : self.KCut }
        PhiForBs2ChicPhi_Chic2KKPiPi.CombinationCut = self.PhiCombCut
        PhiForBs2ChicPhi_Chic2KKPiPi.MotherCut = self.PhiCut
        
        StdTightKaons = DataOnDemand(Location = 'Phys/StdTightKaons')

        SelPhiForBs2ChicPhi_Chic2KKPiPi = Selection(" SelPhiForBs2ChicPhi_Chic2KKPiPi"+self.LineSuffix, Algorithm=PhiForBs2ChicPhi_Chic2KKPiPi, RequiredSelections = [StdTightKaons])
        
        self.PhiSel = SelPhiForBs2ChicPhi_Chic2KKPiPi

   
    def __MakeChic__(self):

        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        #phi for chi_c selection
        PhiForChicForBs2ChicPhi_Chic2KKPiPi = CombineParticles(self.LineSuffix+"PhiForChicForBs2ChicPhi_Chic2KKPiPi")
        PhiForChicForBs2ChicPhi_Chic2KKPiPi.DecayDescriptor = "[phi(1020) -> K+ K-]cc" 
        PhiForChicForBs2ChicPhi_Chic2KKPiPi.DaughtersCuts = { "K-"  : self.K_chic_Cut }
        PhiForChicForBs2ChicPhi_Chic2KKPiPi.CombinationCut = self.Phi_chic_CombCut 
        PhiForChicForBs2ChicPhi_Chic2KKPiPi.MotherCut = self.Phi_chic_Cut        
        PhiForChicForBs2ChicPhi_Chic2KKPiPi.ParticleCombiners = { ''  : combiner } 
        
        StdTightKaons = DataOnDemand(Location = 'Phys/StdTightKaons')
        
        SelPhiForChicForBs2ChicPhi_Chic2KKPiPi = Selection("SelPhiForChicForBs2ChicPhi_Chic2KKPiPi"+self.LineSuffix, Algorithm=PhiForChicForBs2ChicPhi_Chic2KKPiPi, RequiredSelections = [StdTightKaons])
            
        self.Phi_chic_Sel = SelPhiForChicForBs2ChicPhi_Chic2KKPiPi

        #rho for eta_c selection
        RhoForChicForBs2ChicPhi_Chic2KKPiPi = CombineParticles(self.LineSuffix+"RhoForChicForBs2ChicPhi_Chic2KKPiPi")
        RhoForChicForBs2ChicPhi_Chic2KKPiPi.DecayDescriptor = "[rho(770)0 -> pi+ pi-]cc" 
        RhoForChicForBs2ChicPhi_Chic2KKPiPi.DaughtersCuts = { "pi-"  : self.Pi_chic_Cut }
        RhoForChicForBs2ChicPhi_Chic2KKPiPi.CombinationCut = self.Rho_chic_CombCut 
        RhoForChicForBs2ChicPhi_Chic2KKPiPi.MotherCut = self.Rho_chic_Cut        
        RhoForChicForBs2ChicPhi_Chic2KKPiPi.ParticleCombiners = { '' : combiner } 
        
        StdTightPions = DataOnDemand(Location = 'Phys/StdTightPions')
        
        SelRhoForChicForBs2ChicPhi_Chic2KKPiPi = Selection("SelRhoForChicForBs2ChicPhi_Chic2KKPiPi"+self.LineSuffix, Algorithm=RhoForChicForBs2ChicPhi_Chic2KKPiPi, RequiredSelections = [StdTightPions])
            
        self.Rho_chic_Sel = SelRhoForChicForBs2ChicPhi_Chic2KKPiPi


        ChicForBs2ChicPhi_Chic2KKPiPi = CombineParticles(self.LineSuffix+"ChicForBs2ChicPhi_Chic2KKPiPi")
        ChicForBs2ChicPhi_Chic2KKPiPi.DecayDescriptor = "chi_c0(1P) -> rho(770)0 rho(770)0" 
        ChicForBs2ChicPhi_Chic2KKPiPi.CombinationCut = self.ChicCombCut
        ChicForBs2ChicPhi_Chic2KKPiPi.MotherCut = self.ChicCut 
        ChicForBs2ChicPhi_Chic2KKPiPi.ParticleCombiners = { '' : combiner } 
        
        SelChicForBs2ChicPhi_Chic2KKPiPi = Selection("SelChicForBs2ChicPhi_Chic2KKPiPi"+self.LineSuffix, Algorithm= ChicForBs2ChicPhi_Chic2KKPiPi, RequiredSelections = [self.Rho_chic_Sel, self.Phi_chic_Sel])
            
        self.ChicSel = SelChicForBs2ChicPhi_Chic2KKPiPi
    
    def __MakeBs__(self):
        
        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        Bs2ChicPhi_Chic2KKPiPi = CombineParticles(self.LineSuffix+'Bs2ChicPhi_Chic2KKPiPi')    
        Bs2ChicPhi_Chic2KKPiPi.DecayDescriptors = ["B_s0 -> chi_c0(1P) phi(1020)"]
        Bs2ChicPhi_Chic2KKPiPi.CombinationCut = self.BsCombCut
        Bs2ChicPhi_Chic2KKPiPi.MotherCut = self.BsCut
        Bs2ChicPhi_Chic2KKPiPi.ParticleCombiners = { '' : combiner } 
        
        SelBs2ChicPhi_Chic2KKPiPi = Selection("SelBs2ChicPhi_Chic2KKPiPi"+self.LineSuffix, Algorithm=Bs2ChicPhi_Chic2KKPiPi, RequiredSelections = [self.ChicSel, self.PhiSel])
       
        
        self.BsSel = SelBs2ChicPhi_Chic2KKPiPi

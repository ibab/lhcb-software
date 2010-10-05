# $Id: StrippingBs2ChicPhi_Chic2PiPiPiPi.py,v 1.2 2010-05-10 12:12:05 jpalac Exp $


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
                 'K_PT'             : 500.,
                 'K_TRCHI2'         : 5.,
                 'K_IPCHI2'         : 9.,
                 #phi parameters
                 'phiVDZ'           : 0., 
                 'phiPT'            : 1000.,
                 'phiIPCHI2'        : 9.,
                 'phiVCHI2/VDOF'    : 16.,
                 #pi parameters
                 'piPT'             : 400.,
                 'piTRCHI2'         : 5.,
                 'piIPCHI2'         : 9.,
                 #rho parameters
                 'rhoVDZ'           : 0.,
                 'rhoVCHI2/VDOF'    : 16.,
                 #chi_c parameters
                 'chicVDZ'          : 0.,   
                 'chicPT'           : 1000.,
                 'chicIPCHI2'       : 9.,
                 'chicVCHI2/VDOF'   : 16.,               
                 #Bs parameters
                 'BsVDZ'            : 0.,
                 'BsDIRA'           : 0.995,
                 'BsIPCHI2'         : 9.,
                 'BsVCHI2/VDOF'     : 16.,
                },
    }


class Bs2ChicPhi_Chic2PiPiPiPiAllLinesConf(object):
    
    Lines=[]
    
    confObjects={}
    
    def __init__(self, config, offLines=[]):
        for aline in config.keys():
            if aline not in offLines:
                lineconf=Bs2ChicPhi_Chic2PiPiPiPiOneLineConf(aline, config[aline])
                self.confObjects[aline]=lineconf
                self.Lines.append(lineconf.Line)
                
    def printCuts(self):
        for aline in self.confObjects.keys():
            print '===='
            self.confObjects[aline].printCuts()
            

class Bs2ChicPhi_Chic2PiPiPiPiOneLineConf(object):
    
    Line=None
    Selections=[]
    TopSelectionSeq=None

    ###  cuts ###

    KCut=''
    PhiCombCut=''
    PhiCut=''
    PiCut=''
    RhoCombCut=''
    RhoCut=''
    ChicCombCut=''
    ChicCut=''
    BsCombCut=''
    BsCut=''
    
    PhiSel=None
    RhoSel=None
    ChicSel=None
    BsSel=None
    
    LineSuffix=''
    
    __configuration_keys__=[
        'Prescale',
        'Postscale',     
        #K parameters
        'K_PT',
        'K_TRCHI2',
        'K_IPCHI2',
        #phi parameters
        'phiVDZ',        
        'phiPT',
        'phiIPCHI2',
        'phiVCHI2/VDOF',
        #pi parameters
        'piPT',
        'piTRCHI2',       
        'piIPCHI2', 
        #rho parameters
        'rhoVDZ', 
        'rhoVCHI2/VDOF',              
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
        
        checkConfig(Bs2ChicPhi_Chic2PiPiPiPiOneLineConf.__configuration_keys__,config)    

        self.LineSuffix=LineSuffix
               
        self.KCut = "(PT > %(K_PT)s*MeV) &"\
                    "(TRCHI2DOF < %(K_TRCHI2)s) &"\
                    "(MIPCHI2DV(PRIMARY) > %(K_IPCHI2)s) " % config                    

        self.PhiCombCut = "(ADAMASS('phi(1020)') < 50*MeV) &"\
                          "(APT > %(phiPT)s*MeV) " % config

        self.PhiCut = "(BPVVDZ > %(phiVDZ)s) &"\
                      "(VFASPF(VCHI2/VDOF) < %(phiVCHI2/VDOF)s) " % config

        self.PiCut = "(PT > %(piPT)s*MeV) &"\
                     "(TRCHI2DOF < %(piTRCHI2)s) &"\
                     "(MIPCHI2DV(PRIMARY) > %(piIPCHI2)s) " % config

        self.RhoCombCut = "(AM > 400.*MeV) &"\
                          "(AM < 3413.*MeV) " % config

        self.RhoCut = "(BPVVDZ > %(rhoVDZ)s) &"\
                      "(VFASPF(VCHI2/VDOF) < %(rhoVCHI2/VDOF)s) " % config

        self.ChicCombCut = "(ADAMASS('chi_c0(1P)')<100.*MeV) &"\
                           "(APT > %(chicPT)s*MeV) " % config

        self.ChicCut = "(BPVVDZ > %(rhoVDZ)s) &"\
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
        Bs2ChicPhi_Chic2PiPiPiPiLine=StrippingLine("Bs2ChicPhi_Chic2PiPiPiPi"+self.LineSuffix,
                                                   prescale = config['Prescale'],
                                                   postscale = config['Postscale'],
                                                   algos = [ self.BsSel ]
                                                   )
        
        ### Collect them all together in a nice way ###
        self.Line=Bs2ChicPhi_Chic2PiPiPiPiLine
        self.Selections=[self.PhiSel, self.RhoSel, self.ChicSel, self.BsSel]


        
    def printCuts(self):
        print 'name', self.LineSuffix
        print 'KCut', self.KCut
        print 'PhiCombCut', self.PhiCombCut
        print 'PhiCut', self.PhiCut
        print 'PiCut', self.PiCut
        print 'RhoCombCut', self.RhoCombCut
        print 'RhoCut', self.RhoCut
        print 'ChicCombCut', self.ChicCombCut
        print 'ChicCut', self.ChicCut
        print 'BsCombCut', self.BsCombCut
        print 'BsCut', self.BsCut

        
    ############ Functions to make Selections #######################
        
    def __MakePhi__(self):

        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        PhiForBs2ChicPhi_Chic2PiPiPiPi = CombineParticles(self.LineSuffix+"PhiForBs2ChicPhi_Chic2PiPiPiPi")
        PhiForBs2ChicPhi_Chic2PiPiPiPi.DecayDescriptor = "[phi(1020) -> K+ K-]cc" 
        PhiForBs2ChicPhi_Chic2PiPiPiPi.DaughtersCuts = { "K-"  : self.KCut }
        PhiForBs2ChicPhi_Chic2PiPiPiPi.CombinationCut = self.PhiCombCut
        PhiForBs2ChicPhi_Chic2PiPiPiPi.MotherCut = self.PhiCut
        
        StdTightKaons = DataOnDemand(Location = 'Phys/StdTightKaons')

        SelPhiForBs2ChicPhi_Chic2PiPiPiPi = Selection(" SelPhiForBs2ChicPhi_Chic2PiPiPiPi"+self.LineSuffix, Algorithm=PhiForBs2ChicPhi_Chic2PiPiPiPi, RequiredSelections = [StdTightKaons])
        
        self.PhiSel = SelPhiForBs2ChicPhi_Chic2PiPiPiPi
        
    
    def __MakeChic__(self):

        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        
        RhoForChicForBs2ChicPhi_Chic2PiPiPiPi = CombineParticles(self.LineSuffix+"RhoForChicForBs2ChicPhi_Chic2PiPiPiPi")
        RhoForChicForBs2ChicPhi_Chic2PiPiPiPi.DecayDescriptor = "[rho(770)0 -> pi+ pi-]cc" 
        RhoForChicForBs2ChicPhi_Chic2PiPiPiPi.DaughtersCuts = { "pi-"  : self.PiCut }
        RhoForChicForBs2ChicPhi_Chic2PiPiPiPi.CombinationCut = self.RhoCombCut
        RhoForChicForBs2ChicPhi_Chic2PiPiPiPi.MotherCut = self.RhoCut        
        RhoForChicForBs2ChicPhi_Chic2PiPiPiPi.ParticleCombiners = { '' : combiner } 
        
        StdTightPions = DataOnDemand(Location = 'Phys/StdTightPions')
        
        SelRhoForChicForBs2ChicPhi_Chic2PiPiPiPi = Selection("SelRhoForChicForBs2ChicPhi_Chic2PiPiPiPi"+self.LineSuffix, Algorithm=RhoForChicForBs2ChicPhi_Chic2PiPiPiPi, RequiredSelections = [StdTightPions])
            
        self.RhoSel = SelRhoForChicForBs2ChicPhi_Chic2PiPiPiPi


        ChicForBs2ChicPhi_Chic2PiPiPiPi = CombineParticles(self.LineSuffix+"ChicForBs2ChicPhi_Chic2PiPiPiPi")
        ChicForBs2ChicPhi_Chic2PiPiPiPi.DecayDescriptor = "chi_c0(1P) -> rho(770)0 rho(770)0" 
        ChicForBs2ChicPhi_Chic2PiPiPiPi.CombinationCut = self.ChicCombCut
        ChicForBs2ChicPhi_Chic2PiPiPiPi.MotherCut = self.ChicCut 
        ChicForBs2ChicPhi_Chic2PiPiPiPi.ParticleCombiners = { '' : combiner } 
        
        SelChicForBs2ChicPhi_Chic2PiPiPiPi = Selection("SelChicForBs2ChicPhi_Chic2PiPiPiPi"+self.LineSuffix, Algorithm= ChicForBs2ChicPhi_Chic2PiPiPiPi, RequiredSelections = [self.RhoSel])
            
        self.ChicSel = SelChicForBs2ChicPhi_Chic2PiPiPiPi
    
    def __MakeBs__(self):
        
        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        Bs2ChicPhi_Chic2PiPiPiPi = CombineParticles(self.LineSuffix+'Bs2ChicPhi_Chic2PiPiPiPi')    
        Bs2ChicPhi_Chic2PiPiPiPi.DecayDescriptors = ["B_s0 -> chi_c0(1P) phi(1020)"]
        Bs2ChicPhi_Chic2PiPiPiPi.CombinationCut = self.BsCombCut
        Bs2ChicPhi_Chic2PiPiPiPi.MotherCut = self.BsCut
        Bs2ChicPhi_Chic2PiPiPiPi.ParticleCombiners = { '' : combiner } 
        
        SelBs2ChicPhi_Chic2PiPiPiPi = Selection("SelBs2ChicPhi_Chic2PiPiPiPi"+self.LineSuffix, Algorithm=Bs2ChicPhi_Chic2PiPiPiPi, RequiredSelections = [self.ChicSel, self.PhiSel])
       
        
        self.BsSel = SelBs2ChicPhi_Chic2PiPiPiPi

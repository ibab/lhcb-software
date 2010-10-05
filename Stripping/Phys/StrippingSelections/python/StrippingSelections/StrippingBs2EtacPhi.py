# $Id: StrippingBs2EtacPhi.py,v 1.2 2010-05-10 12:12:05 jpalac Exp $


__author__ = ['Katarzyna Senderowska']
__date__ = '04/10/2010'
__version__ = '$Revision: 1.2 $'


#Vertex Fitter
combiner='OfflineVertexFitter'


confdict={    
    'Tuned'   : {
                 'Prescale'    : 1.0 ,
                 'Postscale'   : 1.0 ,
                 #phi parameters
                 'K_phi_PT'               : 500.,
                 'K_phi_TRCHI2'           : 5.,
                 'K_phi_IPCHI2'           : 9.,
                 #phi parameters
                 'phiVDZ'                 : 0., 
                 'phiPT'                  : 1000., 
                 'phiIPCHI2'              : 9.,
                 'phiVCHI2/VDOF'          : 16.,
                 #pi (for eta_c decay) parameters
                 'pi_etac_PT'             : 450., 
                 'pi_etac_TRCHI2'         : 5,
                 'pi_etac_IPCHI2'         : 9.,
                 #rho (for eta_c decay) parameters
                 'rho_etac_VDZ'           : 0.,
                 'rho_etac_VCHI2/VDOF'    : 16.,
                 #K (for eta_c decay) parameters
                 'K_etac_PT'              : 450.,
                 'K_etac_TRCHI2'          : 5.,
                 'K_etac_IPCHI2'          : 9.,
                 #phi (for eta_c decay) parameters
                 'phi_etac_VDZ'           : 0.,
                 'phi_etac_VCHI2/VDOF'    : 16.,
                 #eta_c parameters
                 'etacVDZ'                : 0.,    
                 'etacPT'                 : 1000.,
                 'etacIPCHI2'             : 9.,
                 'etacVCHI2/VDOF'         : 16.,
                 #Bs parameters
                 'BsVDZ'                  : 0.,
                 'BsDIRA'                 : 0.995,
                 'BsIPCHI2'               : 9.,
                 'BsVCHI2/VDOF'           : 16.,
                 },
    }



class Bs2EtacPhiAllLinesConf(object):
    
    Lines=[]
    
    confObjects={}
    
    def __init__(self, config, offLines=[]):
        for aline in config.keys():
            if aline not in offLines:
                lineconf=Bs2EtacPhiOneLineConf(aline, config[aline])
                self.confObjects[aline]=lineconf
                self.Lines.append(lineconf.Line)
                
    def printCuts(self):
        for aline in self.confObjects.keys():
            print '===='
            self.confObjects[aline].printCuts()
            

class Bs2EtacPhiOneLineConf(object):
    
    Line=None
    Selections=[]
    TopSelectionSeq=None

    ###  cuts ###
    
    KCut=''
    PhiCombCut=''
    PhiCut=''
    Pi_etac_Cut=''
    Rho_etac_CombCut=''
    K_etac_Cut=''
    Phi_etac_CombCut=''
    EtacCombCut=''
    EtacCut=''
    BsCombCut=''
    BsCut=''

    PhiSel=None
    Phi_etac_Sel=None
    Rho_etac_Sel=None
    EtacSel=None
    BsSel=None
    
    LineSuffix=''
    
    __configuration_keys__=[
        'Prescale',
        'Postscale',            
        #phi parameters
        'K_phi_PT',
        'K_phi_TRCHI2',
        'K_phi_IPCHI2',
        #phi parameters
        'phiVDZ',
        'phiPT',
        'phiIPCHI2', 
        'phiVCHI2/VDOF',                   
        #pi (for eta_c decay) parameters
        'pi_etac_PT',
        'pi_etac_TRCHI2',        
        'pi_etac_IPCHI2',             
        #rho (for eta_c decay) parameters
        'rho_etac_VDZ',
        'rho_etac_VCHI2/VDOF',    
        #K (for eta_c decay) parameters
        'K_etac_PT',
        'K_etac_TRCHI2',         
        'K_etac_IPCHI2',    
        #phi (for eta_c decay) parameters
        'phi_etac_VDZ',
        'phi_etac_VCHI2/VDOF',             
        #eta_c parameters
        'etacVDZ', 
        'etacPT',
        'etacIPCHI2',
        'etacVCHI2/VDOF',                     
        #Bs parameters
        'BsVDZ',
        'BsDIRA',
        'BsIPCHI2',
        'BsVCHI2/VDOF',        
        ]

    
    def __init__(self, LineSuffix, config):

        from StrippingSelections.Utils import checkConfig
        
        checkConfig(Bs2EtacPhiOneLineConf.__configuration_keys__,config)
        
        self.LineSuffix=LineSuffix

        self.KCut = "(PT > %(K_phi_PT)s*MeV) &"\
                    "(TRCHI2DOF < %(K_phi_TRCHI2)s) &"\
                    "(MIPCHI2DV(PRIMARY) > %(K_phi_IPCHI2)s) " % config

        self.PhiCombCut = "(ADAMASS('phi(1020)') < 50*MeV) &"\
                          "(APT > %(phiPT)s*MeV) " % config

        self.PhiCut = "(BPVVDZ > %(phiVDZ)s) &"\
                      "(VFASPF(VCHI2/VDOF) < %(phiVCHI2/VDOF)s) " % config
        
        self.Pi_etac_Cut = "(PT > %(pi_etac_PT)s*MeV) &"\
                           "(TRCHI2DOF < %(pi_etac_TRCHI2)s) &"\
                           "(MIPCHI2DV(PRIMARY) > %(pi_etac_IPCHI2)s) " % config

        self.Rho_etac_CombCut = "(AM > 400.*MeV) &"\
                                "(AM < 3413.*MeV) " % config
 
        self.Rho_etac_Cut = "(BPVVDZ > %(rho_etac_VDZ)s) &"\
                            "(VFASPF(VCHI2/VDOF) < %(rho_etac_VCHI2/VDOF)s) " % config

        self.K_etac_Cut = "(PT > %(K_etac_PT)s*MeV) &"\
                          "(TRCHI2DOF < %(K_etac_TRCHI2)s) &"\
                          "(MIPCHI2DV(PRIMARY) > %(K_etac_IPCHI2)s) " % config

        self.Phi_etac_CombCut = "(AM > 400.*MeV) &"\
                                "(AM < 3413.*MeV) " % config
        
        self.Phi_etac_Cut = "(BPVVDZ > %(phi_etac_VDZ)s) &" \
                            "(VFASPF(VCHI2/VDOF) < %(phi_etac_VCHI2/VDOF)s) " % config
      
        self.EtacCombCut = "(ADAMASS('eta_c(1S)')<100.*MeV) &"\
                           "(APT > %(etacPT)s*MeV) " % config
        
        self.EtacCut = "(BPVVDZ > %(etacVDZ)s) &"\
                       "(MIPCHI2DV(PRIMARY) > %(etacIPCHI2)s) &"\
                       "(VFASPF(VCHI2/VDOF) < %(etacVCHI2/VDOF)s) " % config

        self.BsCombCut = "(ADAMASS('B_s0')<300.*MeV) " % config
      
        self.BsCut = "(BPVVDZ > %(BsVDZ)s) &"\
                     "(BPVDIRA > %(BsDIRA)s) &"\
                     "(MIPCHI2DV(PRIMARY) < %(BsIPCHI2)s) &"\
                     "(VFASPF(VCHI2/VDOF) < %(BsVCHI2/VDOF)s) " % config
   


        ###  selections ###

        self.__MakeEtac__()
        self.__MakePhi__()
        self.__MakeBs__()
        
        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import SelectionSequence
        

        ### stripping line ###
        Bs2EtacPhiLine=StrippingLine("Bs2EtacPhi"+self.LineSuffix,
                                     prescale = config['Prescale'],
                                     postscale = config['Postscale'],
                                     algos = [ self.BsSel ]
                                     )
        
        ### Collect them all together in a nice way ###
        self.Line=Bs2EtacPhiLine
        self.Selections=[self.PhiSel, self.Phi_etac_Sel, self.Rho_etac_Sel, self.EtacSel, self.BsSel]
   

        
    def printCuts(self):
        print 'name', self.LineSuffix
        print 'KCut', self.KCut
        print 'PhiCombCut', self.PhiCombCut
        print 'PhiCut', self.PhiCut
        print 'Pi_etac_Cut', self.Pi_etac_Cut
        print 'Rho_etac_CombCut', self.Rho_etac_CombCut
        print 'Rho_etac_Cut', self.Rho_etac_Cut
        print 'K_etac_Cut', self.K_etac_Cut
        print 'Phi_etac_CombCut', self.Phi_etac_CombCut
        print 'Phi_etac_Cut', self.Phi_etac_Cut
        print 'EtacCut', self.EtacCut
        print 'EtacCombCut', self.EtacCombCut
        print 'BsCombCut', self.BsCombCut
        print 'BsCut', self.BsCut

        
    ############ Functions to make Selections #######################

        
    def __MakePhi__(self):

        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        PhiForBs2EtacPhi = CombineParticles(self.LineSuffix+"PhiForBs2EtacPhi")
        PhiForBs2EtacPhi.DecayDescriptor = "[phi(1020) -> K+ K-]cc" 
        PhiForBs2EtacPhi.DaughtersCuts = { "K-"  : self.KCut }
        PhiForBs2EtacPhi.CombinationCut = self.PhiCombCut
        PhiForBs2EtacPhi.MotherCut = self.PhiCut
        
        StdTightKaons = DataOnDemand(Location = 'Phys/StdTightKaons')

        SelPhiForBs2EtacPhi = Selection(" SelPhiForBs2EtacPhi"+self.LineSuffix, Algorithm=PhiForBs2EtacPhi, RequiredSelections = [StdTightKaons])
        
        self.PhiSel = SelPhiForBs2EtacPhi
                
    
    def __MakeEtac__(self):

        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        #phi for eta_c selection
        PhiForEtacForBs2EtacPhi = CombineParticles(self.LineSuffix+"PhiForEtacForBs2EtacPhi")
        PhiForEtacForBs2EtacPhi.DecayDescriptor = "[phi(1020) -> K+ K-]cc" 
        PhiForEtacForBs2EtacPhi.DaughtersCuts = { "K-"  : self.K_etac_Cut }
        PhiForEtacForBs2EtacPhi.CombinationCut = self.Phi_etac_CombCut  
        PhiForEtacForBs2EtacPhi.MotherCut = self.Phi_etac_Cut        
        PhiForEtacForBs2EtacPhi.ParticleCombiners = { ''  : combiner } 
        
        StdTightKaons = DataOnDemand(Location = 'Phys/StdTightKaons')
        
        SelPhiForEtacForBs2EtacPhi = Selection("SelPhiForEtacForBs2EtacPhi"+self.LineSuffix, Algorithm=PhiForEtacForBs2EtacPhi, RequiredSelections = [StdTightKaons])
            
        self.Phi_etac_Sel = SelPhiForEtacForBs2EtacPhi

        #rho for eta_c selection
        RhoForEtacForBs2EtacPhi = CombineParticles(self.LineSuffix+"RhoForEtacForBs2EtacPhi")
        RhoForEtacForBs2EtacPhi.DecayDescriptor = "[rho(770)0 -> pi+ pi-]cc" 
        RhoForEtacForBs2EtacPhi.DaughtersCuts = { "pi-"  : self.Pi_etac_Cut }
        RhoForEtacForBs2EtacPhi.CombinationCut = self.Rho_etac_CombCut  
        RhoForEtacForBs2EtacPhi.MotherCut = self.Rho_etac_Cut        
        RhoForEtacForBs2EtacPhi.ParticleCombiners = { ''  : combiner } 
        
        StdTightPions = DataOnDemand(Location = 'Phys/StdTightPions')
        
        SelRhoForEtacForBs2EtacPhi = Selection("SelRhoForEtacForBs2EtacPhi"+self.LineSuffix, Algorithm=RhoForEtacForBs2EtacPhi, RequiredSelections = [StdTightPions])
            
        self.Rho_etac_Sel = SelRhoForEtacForBs2EtacPhi

        #eta_c selection
        EtacForBs2EtacPhi = CombineParticles(self.LineSuffix+"EtacForBs2EtacPhi")
        EtacForBs2EtacPhi.DecayDescriptor = "eta_c(1S) -> rho(770)0 phi(1020)" 
        EtacForBs2EtacPhi.CombinationCut = self.EtacCombCut
        EtacForBs2EtacPhi.MotherCut = self.EtacCut 
        EtacForBs2EtacPhi.ParticleCombiners = { ''  : combiner } 
        
        SelEtacForBs2EtacPhi = Selection("SelEtacForBs2EtacPhi"+self.LineSuffix, Algorithm= EtacForBs2EtacPhi, RequiredSelections = [self.Rho_etac_Sel, self.Phi_etac_Sel])
            
        self.EtacSel = SelEtacForBs2EtacPhi

    
    def __MakeBs__(self):
        
        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection

        #B_s0 selection
        Bs2EtacPhi = CombineParticles(self.LineSuffix+'Bs2EtacPhi')    
        Bs2EtacPhi.DecayDescriptors = ["B_s0 -> eta_c(1S) phi(1020)"]
        Bs2EtacPhi.CombinationCut = self.BsCombCut
        Bs2EtacPhi.MotherCut = self.BsCut
        Bs2EtacPhi.ParticleCombiners = { ''  : combiner } 
        
        SelBs2EtacPhi = Selection("SelBs2EtacPhi"+self.LineSuffix, Algorithm=Bs2EtacPhi, RequiredSelections = [self.EtacSel, self.PhiSel])
      
        
        self.BsSel=SelBs2EtacPhi






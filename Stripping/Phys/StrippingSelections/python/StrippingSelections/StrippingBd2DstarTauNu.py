# $Id: StrippingBd2DstarTauNu.py,v 1.6 2010-10-25 00:13:54 akeun Exp $
'''
Module for constuction of Bd->DstarTauNu lines

==== Description of the lines ====

Bd->DstarTauNu, with Dstar -> pi (D0 -> K pi)

There are two lines:

1) Loose. Looser cuts on IP(chi2), vertex chi2 and masses. Prescaled by 0.1.
2) Tight.

==== Description of the configuration ====

The selecton cuts are stored in the dictionaries: confdict['Loose'] or 'Tight'.

Most cuts are stored per configuration, some common (Dstar) cuts are hardcoded.
Most Dstar cuts are taken from the Bd2DstarMuNu line; as the aim is to measure
Br(Bd2DstarMuNu)/Br(Bd2DstarTauNu). The tight selection is the same, but due to
ms/event limitations, the Dstars are not made from scratch in this line.
StdLooseDstarWithD02KPi is taken as a starting point. The cuts within this
initial selection (MIPCHI2>4) can therefore not be loosened.

The configuration class makes all the selections and the lines, when passed a correct dictionary.

The lines look basically like this:

1) Filter Dstars
2) Filter Pions
3) Create Pion pairs
4) Create Taus
5) Create Bd

To look at all the configurable cuts, see StrippingBd2DstarTauNu.confdict

==== How to use ====

To configure all lines, just instantiate the class:

all=Bd2DstarTauNuAllLinesConf("Bd2DstarTauNu",confdict)

Then to print all cuts do:

all.printCuts()

You can configure one line at a time with the Bd2DstarTauNuOneLineConf class:

one=Bd2DstarTauNuOneLineConf("Bd2DstarTauNu"+"Loose",confdict["Loose"])
'''
__author__ = [ 'Anne Keune' ]
__date__ = '2010-10-26'
__version = '$Revision: 1.6 $'

#### Next is the dictionary of all tunable cuts ########
#### It is separated into the different lines   ########

confdict={
    'Loose' : { 'Prescale'    : 0.1 ,
                'Postscale'   : 1.0 ,
                #tau: single pi cuts
                'SinglePiPT'  : 150 ,
                'SinglePiIPChi2' : 1.0,
                #tau: di pi cuts
                'DiPiPT'      : 600,
                'DiPiLowM'    : 600,
                'DiPiHighM'   : 1800,
                'DiPiVxChi2'  : 9,
                'DiPiDira'    : 0.99,
                'DiPiIPChi2'  : 0,
                #tau: di pi combi cut
                'DiPiMaxIPChi2' : 20,
                #tau: tri pi cuts
                'TriPiPT'     : 800,
                'TriPiLowM'   : 1000,
                'TriPiHighM'  : 2000,
                'TriPiVxChi2' : 9,
                'TriPiDira'   : 0.99,
                'TriPiIPChi2' : 0,
                #Bd cuts
                'BdVxChi2'    : 0,
                'BdDira'      : 0.99,
                'VxOrder'     : -3.0,
                #Bd combi cuts
                'BdLowDM'     : -1279,
                'BdHighDM'    : 500,
                #kaon parameters
                'KPT'         : 350, # 350  # MeV
                'KTrChi2'     :  8,  # 10   
                'KIPChi2'     :  0,  # 4 already in StdLooseD0
                'KTrPID'      : -5,  #-5
                'KIP'         :  0,
                #pion parameters
                'PiPT'        : 350, # MeV
                'PiTrChi2'    :  8, # 10   
                'PiIPChi2'    :  0, # 4 already in StdLooseD0
                'PiIP'        :  0,
                #D0-resonance parameters
                'D0MassW'     : 60, # 40    # MeV
                'D0_BPVVDCHI2': 50,
                #Dstar-resonance parameters
                'slowPiTrChi2' : 8, # 10 
                'Dstar_PT'     : 1250, # 1250 # MeV
                'Dstar_VCHI2'  : 25, # 20 # 
                'DstarMassW'   : 80, # 50 # MeV
                #Bd-resonance parameters
                'B0d_VCHI2'    : 10, # 10 # 
                'B0dD0_DeltaZVtx' : -10, # -2.5  # mm
                'B0dPV_DeltaZVtx' : -100, # 0.5  # mm
                },
    'Tight'   : {'Prescale'    : 1.0 ,
                 'Postscale'   : 1.0 ,
                 #tau: single pi cuts
                 'SinglePiPT'  : 150 ,
                 'SinglePiIPChi2' : 4,
                 #tau: di pi cuts
                 'DiPiPT'      : 600,
                 'DiPiLowM'    : 750,
                 'DiPiHighM'   : 1500,
                 'DiPiVxChi2'  : 9,
                 'DiPiDira'    : 0.99,
                 'DiPiIPChi2'  : 5,
                 #tau: di pi combi cut
                 'DiPiMaxIPChi2' : 20,
                 #tau: tri pi cuts
                 'TriPiPT'     : 800,
                 'TriPiLowM'   : 1200,
                 'TriPiHighM'  : 1780,
                 'TriPiVxChi2' : 9,
                 'TriPiDira'   : 0.99,
                 'TriPiIPChi2' : 5,
                 #Bd cuts
                 'BdVxChi2'    : 0,
                 'BdDira'      : 0.99,
                 'VxOrder'     : -3.0,
                 #Bd combi cuts
                 'BdLowDM'     : -1279,
                 'BdHighDM'    : 300,
                 #kaon parameters
                 'KPT'         : 350, # MeV
                 'KTrChi2'     : 8,
                 'KIPChi2'     : 4,
                 'KTrPID'      :-5,
                 'KIP'         : 0.04,
                 #pion parameters
                 'PiPT'        : 350, # MeV
                 'PiTrChi2'    : 8,
                 'PiIPChi2'    : 4,
                 'PiIP'        : 0.04,
                 #D0-resonance parameters
                 'D0MassW'     : 40, # MeV
                 'D0_BPVVDCHI2': 50,
                 #Dstar-resonance parameters
                 'slowPiTrChi2' : 8, 
                 'Dstar_PT'     : 1250, #  MeV
                 'Dstar_VCHI2'  : 20, 
                 'DstarMassW'   : 50, 
                 #Bd-resonance parameters
                 'B0d_VCHI2'    : 10, 
                 'B0dD0_DeltaZVtx' : -3.0, # mm
                 'B0dPV_DeltaZVtx' : -0.5 #  mm
                 }
    }


from StrippingUtils.Utils import LineBuilder, MasterLineBuilder

default_name="Bd2DstarTauNu"

class Bd2DstarTauNuAllLinesConf(MasterLineBuilder):
    """
    Configuration object for both Bd2DstarTauNu lines

    Bd2DstarTauNu attempts to strip (((Kpi)pi)(pipipi).
    Two different lines are used, and for more information call help on the module
    
    usage:
    configdict={'LineNameSuffix' : {...},
                'LineNameSuffix2': {...} }
    
    Bd2DstarTauNuAllLinesConf(name, config )
    
    
    To only configure/run one line, it's better to use the Bd2DstarTauNuOneLineConf class.
    
    The created lines appear as a list in the lines() method
    
    To print out all the cuts, use the printCuts method
    """
    
    def __init__(self, name, config):
        '''In the constructor we make all the lines, and configure them all
        name is the initial name of the lines, i.e. Bd2DstarTauNu
        config is the dictionary of {LineSuffix : configuration}'''
        MasterLineBuilder.__init__(self, name, config, Bd2DstarTauNuOneLineConf)
        
    def printCuts(self):
        '''Print out all the configured cuts for the lines you asked for'''
        for aline in self.slaves():
            print '===='
            aline.printCuts()
            

class Bd2DstarTauNuOneLineConf(LineBuilder):
    """
    Configuration object for a Bd2DstarTauNu line

    usage: config={...}
    Bd2DstarTauNuConf(name+LineSuffix, config)
    Will make lines ending in LineSuffix with the config configurations
    
    The cuts are configuration parameter only if they are different between the lines,
    common cuts are hardcoded.
    
    Use conf.printCuts to check the cuts in python
    The selections are available individually as PionSel, PiPiSel, TauSel, DstarSel and B0Sel
    The Line object, a member of this class, holds the configured line
    """
    
    Line=None
    Selections=[]
    TopSelectionSeq=None

    PionCut=''
    DiPionCut=''
    DiPionCombCut=''
    TriPionCut=''
    KCut=''
    PiCut=''
    slowPiCut=''
    D0Cut=''
    DstarCut=''
    totalDstarCut=''
    B0CombCut=''
    B0Cut=''
    
    PionSel=None
    PiPiSel=None
    TauSel=None
    DstarSel=None
    B0Sel=None
    
    
    __configuration_keys__=[
        'Prescale',
        'Postscale',
        #tau: single pi cuts
        'SinglePiPT',
        'SinglePiIPChi2',
        #tau: di pi cuts
        'DiPiPT',
        'DiPiLowM',
        'DiPiHighM',
        'DiPiVxChi2',
        'DiPiDira',
        'DiPiIPChi2',
        #tau: di pi combi cut
        'DiPiMaxIPChi2',
        #tau: tri pi cuts
        'TriPiPT',
        'TriPiLowM',
        'TriPiHighM',
        'TriPiVxChi2',
        'TriPiDira',
        'TriPiIPChi2',
        #Bd cuts
        'BdVxChi2',
        'BdDira',
        'VxOrder',
        #Bd combi cuts
        'BdLowDM',
        'BdHighDM',
        #kaon parameters
        'KPT',
        'KTrChi2',
        'KIPChi2',
        'KTrPID',
        'KIP',
        #pion parameters
        'PiPT',
        'PiTrChi2',
        'PiIPChi2',
        'PiIP',
        #D0-resonance parameters
        'D0MassW',
        'D0_BPVVDCHI2',
        #Dstar-resonance parameters
        'slowPiTrChi2',
        'Dstar_PT',
        'Dstar_VCHI2',
        'DstarMassW',
        #B-resonance parameters
        'B0d_VCHI2',
        'B0dD0_DeltaZVtx',
        'B0dPV_DeltaZVtx'
        ]
    
    def __init__(self, name, config):
        '''The constructor of the configuration class.
        Requires a name which is added to the end of each algorithm name
        and a configuration dictionary, config, which must provide all the settings
        which differ between the lines'''
        LineBuilder.__init__(self, name, config)
        
        #Tau: Cuts for single pions
        self.PionCut = "(PT > %(SinglePiPT)s*MeV) & (MIPCHI2DV(PRIMARY)> %(SinglePiIPChi2)s ) & (TRCHI2DOF < 5.0) & (PIDK < 8.0)" %config

        #Tau: Cuts for di pions
        self.DiPionCut = "(PT>%(DiPiPT)s*MeV) & in_range(%(DiPiLowM)s,M,%(DiPiHighM)s) & "\
                          "(VFASPF(VCHI2/VDOF)<%(DiPiVxChi2)s) & (BPVDIRA > %(DiPiDira)s) & "\
                          "(MIPCHI2DV(PRIMARY)> %(DiPiIPChi2)s)" %config

        self.DiPionCombCut = "(AMAXCHILD('pi-'==ABSID,BPVIPCHI2())> %(DiPiMaxIPChi2)s)" %config
      
        #Tau: Cuts for tri pions
        self.TriPionCut = "(PT>%(TriPiPT)s*MeV) & in_range(%(TriPiLowM)s,M,%(TriPiHighM)s) & "\
                           "(VFASPF(VCHI2/VDOF)<%(TriPiVxChi2)s) & (BPVDIRA > %(TriPiDira)s) & "\
                           "(MIPCHI2DV(PRIMARY)> %(TriPiIPChi2)s)" %config


        # StdLoose D* has the following decay chain:  D*+ -> pi ( D0 -> K pi )

        # Cuts for D0        
        self.KCut  = "& CHILDCUT(CHILDCUT( (PT > %(KPT)s*MeV) & (ISLONG) & "\
                     " (TRCHI2DOF < %(KTrChi2)s ) & (MIPDV(PRIMARY) > %(KIP)s) & "\
                     " (MIPCHI2DV(PRIMARY)> %(KIPChi2)s ) & "\
                     " (PIDK >  %(KTrPID)s ),1),2) " % config
        
        self.PiCut = "& CHILDCUT(CHILDCUT( (PT> %(PiPT)s*MeV) & (ISLONG) & "\
                     " (TRCHI2DOF < %(PiTrChi2)s) & (MIPDV(PRIMARY) > %(PiIP)s) & "\
                     " (MIPCHI2DV(PRIMARY)> %(PiIPChi2)s ),2),2) " % config


        self.D0Cut = "& CHILDCUT( (PT>1600*MeV) & (ADMASS('D0') < %(D0MassW)s *MeV ) &  " \
                     " (BPVVDCHI2 > %(D0_BPVVDCHI2)s) & (VFASPF(VCHI2/VDOF)<10),2)  "% config

        # Cuts for Dstar        
        self.slowPiCut= "& CHILDCUT( (PT>110*MeV) & (ISLONG)  & "\
                        " (TRCHI2DOF < %(slowPiTrChi2)s) & "\
                        " (MIPDV(PRIMARY) > 0.04*mm),1) " % config        

        
        self.DstarCut = " (VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2)s ) & (M-MAXTREE('D0'==ABSID,M)<160 * MeV) &"\
                        " (PT>%(Dstar_PT)s *MeV) & (ADMASS('D*(2010)+')< %(DstarMassW)s*MeV)" % config

        self.totalDstarCut = self.DstarCut + self.KCut + self.PiCut + self.D0Cut + self.slowPiCut

        #new B0 Cuts:
        self.B0CombCut="(DAMASS('B0') > %(BdLowDM)s*MeV) & (DAMASS('B0') < %(BdHighDM)s*MeV)" %config
        self.B0Cut = " (VFASPF(VCHI2/VDOF) > %(BdVxChi2)s) & (BPVDIRA > %(BdDira)s) & (D2DVVD(2)-D2DVVD(1)> %(VxOrder)s) &"\
                    " (MINTREE(ABSID=='D0', VFASPF(VZ))-VFASPF(VZ) > %(B0dD0_DeltaZVtx)s*mm) & (BPVVDZ > %(B0dPV_DeltaZVtx)s*mm)" %config

        ### Now make all the selections ###

        self.__FilterDstars__()

        self.__FilterPions__()
        self.__MakeDiPions__()
        self.__MakeTaus__()
        
        self.__MakeB0d__()
        
        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import SelectionSequence
        
        ### Now make a stripping line ###
        B0dLine=StrippingLine(self._name,
                              prescale = config['Prescale'],
                              postscale = config['Postscale'],
                              algos = [ self.B0Sel ]
                              )
        self.registerLine(B0dLine)
        
        ### Collect them all together in a nice way ###
        self.Line=B0dLine
        self.Selections=[self.DstarSel, self.PionSel, self.PiPiSel, self.TauSel, self.B0Sel]
        
    def printCuts(self):
        '''Print the compiled cut values'''
        print 'name', self._name
        print 'PionCut', self.PionCut
        print 'DiPionCut', self.DiPionCut
        print 'DiPionCombCut', self.DiPionCombCut
        print 'TriPionCut', self.TriPionCut
        print 'KCut', self.KCut
        print 'PiCut', self.PiCut
        print 'slowPiCut', self.slowPiCut
        print 'D0Cut', self.D0Cut
        print 'DstarCut', self.DstarCut
        print 'B0CombCut', self.B0CombCut
        print 'B0Cut', self.B0Cut

        
    ############ Functions to make Selections #######################

    def __FilterDstars__(self):

        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        DstarsForB0d = FilterDesktop(
            Code = self.totalDstarCut
            )
        MyStdDstars = DataOnDemand(Location = 'Phys/StdLooseDstarWithD02KPi/Particles')
        SelDstarsForB0d = Selection("SelDstarsFor"+self._name,
                                   Algorithm=DstarsForB0d, RequiredSelections = [MyStdDstars])
        
        self.DstarSel=SelDstarsForB0d
        
    def __FilterPions__(self):
        """
        to speed up the selection, single pions are filtered before combinations are made.
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        

        PionsForB0d = FilterDesktop(Code = self.PionCut)
        SelPionsForB0d = Selection("SelPionsFor"+self._name,
                                   Algorithm=PionsForB0d, RequiredSelections = [StdLoosePions])
        
        self.PionSel=SelPionsForB0d

    def __MakeDiPions__(self):
        """
        Selecting pion pairs at first
        [rho(770)0 -> pi+ pi-]cc, [rho(770)+ -> pi+ pi+]cc
        As we can cut on di-pion mass well, this cuts down on combinatorics and speeds up the stripping line.
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        PiPiForTau = CombineParticles(
            DecayDescriptors = ["[rho(770)0 -> pi+ pi-]cc", "[rho(770)+ -> pi+ pi+]cc"],
            CombinationCut = self.DiPionCombCut,
            MotherCut      = self.DiPionCut
            )
        
        SelPiPiForTau = Selection("SelPiPiFor"+self._name, Algorithm=PiPiForTau,
                                  RequiredSelections = [self.PionSel])
        
        self.PiPiSel=SelPiPiForTau

    def __MakeTaus__(self):
        """
        Selecting pion pairs at first
        [rho(770)0 -> pi+ pi-]cc, [rho(770)+ -> pi+ pi+]cc
        As we can cut on di-pion mass well, this cuts down on combinatorics and speeds up the stripping line.
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        Tau2PiPiPi = CombineParticles(
            DecayDescriptors = ["[tau+ -> pi+ rho(770)0]cc", "[tau+ -> pi- rho(770)+]cc"],
            MotherCut      = self.TriPionCut
            )
        
        SelTau2PiPiPi = Selection("SelTau2PiPiPiFor"+self._name, Algorithm=Tau2PiPiPi,
                                  RequiredSelections = [self.PionSel,self.PiPiSel])
        
        self.TauSel=SelTau2PiPiPi
    
    def __MakeB0d__(self):
        """
        B0d selection:
        [B0 -> D*- tau+]cc, [B0 -> D*+ tau+]cc
        use of wrong charge combination is useful for background subtraction
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        CombBd2DstarTauNu = CombineParticles(        
            DecayDescriptors = ["[B0 -> D*(2010)- tau+]cc", "[B0 -> D*(2010)+ tau+]cc"],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.B0Cut,
            )
        
        SelBd2DstarTauNu = Selection("Sel"+self._name, Algorithm=CombBd2DstarTauNu,
                                    RequiredSelections = [self.DstarSel,self.TauSel])
        
        self.B0Sel=SelBd2DstarTauNu

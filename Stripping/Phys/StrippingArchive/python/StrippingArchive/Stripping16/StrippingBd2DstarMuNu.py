# $Id: StrippingBd2DstarMuNu.py,v 1.6 2010-09-03 00:05:35 svecchi Exp $
'''
Module for constuction of Bd->DstarMuNu lines

==== Description of the lines ====

Bd->DstarMuNuX with X being extra particles neutrino, gamma, pi0, whatever,
Dstar->KKpi, as required for B-tagging studies and the measurement of bbar cross section using
this exclusive semileptonic decay.
(Dstar+->D0pi+-, D0->Kpi)

There are three lines:

A) Loose, Right and Wrong Dst mu combinations, loose cuts to study cross section in eta bins, to be prescaled as rate increases.
   Will disappear as the trigger requirements become too tight 
B) Tight, Right and Wrong Dst mu combination, to study OS and SSpi Tagging.



The "Loose" is initially scaled to 50%, which should be fine up to XX pb-1. Can be further prescaled if the retention increase

The "Tight" selection  expects XX 10^7 events in 1 fb-1, and should not be prescaled so that the tagging can be studied with the highest statistics. Its cuts are tighter than the Loose line.
It will be used also for bbar cross section measurement, by studying the efficiency of cuts in the Looser prescaled line


==== Description of the configuration ====

The selection cuts are stored in the dictionaries: confdict["Loose"] or "Tight".
The cuts are stored only if they are different between the lines, common cuts are hardcoded.

The configuration class makes all the selections and the lines, when passed a correct dictionary.

The lines look basically like this:

1) Choose a Muon
2) Make a D0
3) Make a Dstar
4) Make a B

To look at all the configurable cuts, see StrippingBd2DstarMuNu.confdict

==== How to use ====

To configure all lines, just instantiate the class:

all=Bd2DstarMuNuAllLinesConf("Bd2DstarMuNu",confdict)

Then to print all cuts do:

all.printCuts()

You can configure one line at a time with the Bd2DstarMuNuOneLineConf class:

one=Bd2DstarMuNuOneLineConf("Bd2DstarMuNuLoose",confdict["Loose"])
'''
__author__ = [ 'Stefania Vecchi, Marta Calvi' ]
__date__ = '2010-10-04'
__version = '$Revision: 1.6 $'

#### Which VertexFitter to use? ####


#### Next is the dictionary of all tunable cuts ########
#### It is separated into the different lines   ########


confdict={
    'Loose' : { 'Prescale'    : 0.3 ,
                'Postscale'   : 1.0 ,
                #muon paramters # loose # Tight
                'MuPT'        : 700, # 800  # MeV
                'MuTrPID'     :-20,  # -5   
                'MuIPChi2'    :  0,  # 4    
                #kaon parameters
                'KPT'         : 350, # 350  # MeV
                'KIPChi2'     :  0,  # 4    
                'KTrPID'      : -5,  #-5    
                #pion parameters
                'PiPT'        : 350, # MeV
                'PiIPChi2'    :  0, # 4    
                #D0-resonance parameters
                'D0MassW'     : 60, # 40    # MeV
                'D0_BPVVDCHI2': 50,
                #Dstar-resonance parameters
                'Dstar_PT'     : 1250, # 1250 # MeV
                'Dstar_VCHI2'  : 25, # 20 # 
                'DstarMassW'   : 80, # 50 # MeV
                #Bd-resonance parameters
                'B0d_VCHI2'    : 10, # 10 # 
                'B0dD0_DeltaZVtx' : -10, # -2.5  # mm
                'B0dPV_DeltaZVtx' : -100, # 0.5  # mm
                },
##CB    'LooseNoPresc' : { 'Prescale'    : 1. ,
##CB                'Postscale'   : 1.0 ,
##CB                #muon paramters # loose # Tight
##CB                'MuPT'        : 700, # 800  # MeV
##CB                'MuTrPID'     :-20,  # -5   
##CB                'MuIPChi2'    :  0,  # 4    
##CB                #kaon parameters
##CB                'KPT'         : 350, # 350  # MeV
##CB                'KIPChi2'     :  0,  # 4    
##CB                'KTrPID'      : -5,  #-5    
##CB                #pion parameters
##CB                'PiPT'        : 350, # MeV
##CB                'PiIPChi2'    :  0, # 4    
##CB                #D0-resonance parameters
##CB                'D0MassW'     : 60, # 40    # MeV
##CB                'D0_BPVVDCHI2': 50,
##CB                #Dstar-resonance parameters
##CB                'Dstar_PT'     : 1250, # 1250 # MeV
##CB                'Dstar_VCHI2'  : 25, # 20 # 
##CB                'DstarMassW'   : 80, # 50 # MeV
##CB                #Bd-resonance parameters
##CB                'B0d_VCHI2'    : 10, # 10 # 
##CB                'B0dD0_DeltaZVtx' : -10, # -2.5  # mm
##CB                'B0dPV_DeltaZVtx' : -100, # 0.5  # mm
##CB                },
    'Tight'   : {'Prescale'    : 1.0 ,
                 'Postscale'   : 1.0 ,
                 #muon paramters # 
                 'MuPT'        : 800, # MeV
                 'MuTrPID'     :-5,
                 'MuIPChi2'    : 4,
                 #kaon parameters
                 'KPT'         : 350, # MeV
                 'KIPChi2'     : 4,
                 'KTrPID'      :-5, 
                 #pion parameters
                 'PiPT'        : 350, # MeV
                 'PiIPChi2'    : 4,
                 #D0-resonance parameters
                 'D0MassW'     : 40, # MeV
                 'D0_BPVVDCHI2': 50,
                 #Dstar-resonance parameters
                 'Dstar_PT'     : 1250, #  MeV
                 'Dstar_VCHI2'  : 20, 
                 'DstarMassW'   : 50, 
                 #Bd-resonance parameters
                 'B0d_VCHI2'    : 10, 
                 'B0dD0_DeltaZVtx' : -2.5, # mm
                 'B0dPV_DeltaZVtx' : 0.5 #  mm
                 }
    }




from StrippingUtils.Utils import LineBuilder, MasterLineBuilder

name = "Bd2DstarMuNu"


class Bd2DstarMuNuAllLinesConf(MasterLineBuilder):
    """
    Configuration object for all Bd2DstarMuNu lines

    Bd2DstarMuNu attempts to strip ((Kpi)pi) Mu.
    Several different lines are used, and for more information call help on the module
    
    usage:
    configdict={'LineNameSuffix' : {...},
                'LineNameSuffix2': {...} }
    
    Bd2DstarMuNuAllLinesConf(name, config)
    
    To turn off lines which otherwise would be created, add the name
    of the line to offLines.
    
    To only configure/run one line, it's better to use the Bd2DstarMuNuOneLineConf class.
    
    The created lines appear as a list in the lines() function
    
    To print out all the cuts, use the printCuts method
    """
        
    def __init__(self, name, config):
        '''In the constructor we make all the lines, and configure them all
        config is the dictionary of {LineSuffix : configuration}'''
        MasterLineBuilder.__init__(self, name, config, Bd2DstarMuNuOneLineConf)
        
    def printCuts(self):
        '''Print out all the configured cuts for the lines you asked for'''
        for aline in self.slaves():
            print '===='
            aline.printCuts()
            

class Bd2DstarMuNuOneLineConf(LineBuilder):
    """
    Configuration object for a Bd2DstarMuNu line

    usage: config={...}
    Bd2DstarMuNuConf(name+LineSuffix, config)
    Will make lines ending in LineSuffix with the config configurations
    
    The cuts are configuration parameter only if they are different between the lines,
    common cuts are hardcoded.
    
    Use conf.printCuts to check the cuts in python
    The selections are available individually as MuSel, DSel and B0Sel
    The lines() method, a member of this class, holds the configured line
    """
    
    Line=None
    Selections=[]
    TopSelectionSeq=None
    
    MuCut=''
    KCut=''
    PiCut=''
    slowPiCut=''
    D0CombCut=''
    D0Cut=''
    DstarCombCut=''
    DstarCut=''
    B0CombCut=''
    B0Cut=''
    
    MuSel=None
    D0Sel=None
    DstarSel=None
    B0Sel=None
    
        
    __configuration_keys__=[
        'Prescale',
        'Postscale',
        #muon paramters
        'MuPT',
        'MuTrPID',
        'MuIPChi2',
        #kaon parameters
        'KPT',
        'KIPChi2',
        'KTrPID',
        #pion parameters
        'PiPT',
        'PiIPChi2',
        #D0-resonance parameters
        'D0MassW',
        'D0_BPVVDCHI2',
        #Dstar-resonance parameters
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
        Requires a name which is added to the end of each algorithm name,
        and a configuration dictionary, config, which must provide all the settings
        which differ between the lines'''
        LineBuilder.__init__(self, name, config)
        ### first we define the cuts from the configuration ###
        ### it's nice to see all the cuts in one place      ###

        # Cuts for Mu        
        self.MuCut = " (PT >  %(MuPT)s *MeV) & (P > 2.0*GeV) & (ISMUON) & (HASMUON) & (ISLONG) & "\
                     " (MIPCHI2DV(PRIMARY) > %(MuIPChi2)s) &  (PIDmu > %(MuTrPID)s) " % config

        # Cuts for D0        
        self.KCut  = " (PT > %(KPT)s*MeV) & (P > 2.0*GeV) & (ISLONG) & "\
                     " (MIPDV(PRIMARY) > 0.04*mm) & "\
                     " (MIPCHI2DV(PRIMARY)> %(KIPChi2)s ) & "\
                     " (PIDK >  %(KTrPID)s ) " % config
        
        self.PiCut = " (PT> %(PiPT)s*MeV) & (P > 2.0*GeV) & (ISLONG) & "\
                     " (MIPDV(PRIMARY) > 0.04*mm) & "\
                     " (MIPCHI2DV(PRIMARY)> %(PiIPChi2)s ) " % config

        self.D0CombCut = " (APT>1200*MeV) & (ADAMASS('D0') < 70*MeV  )"% config

        self.D0Cut = " (PT>1600*MeV) & (ADMASS('D0') < %(D0MassW)s *MeV ) &  " \
                     " (BPVVDCHI2 > %(D0_BPVVDCHI2)s) & (VFASPF(VCHI2/VDOF)<10)  "% config


        # Cuts for Dstar        
        self.slowPiCut= " (PT>110*MeV) & (ISLONG)  & "\
                        " (MIPDV(PRIMARY) > 0.04*mm) " % config        

        self.DstarCombCut="(ADAMASS('D*(2010)+')< %(DstarMassW)s*MeV) & (APT>%(Dstar_PT)s *MeV) " % config
        
        self.DstarCut = " (VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2)s ) & (M-MAXTREE('D0'==ABSID,M)<160 * MeV)" % config

        # Cuts for B0         
        self.BCombCut="((AM > 3000.*MeV) )"
        
        self.BCut = " (BPVDIRA> 0.999 ) & "\
                    "  in_range(3000.*MeV,M,5280.*MeV) & "\
                    " (MINTREE(ABSID=='D0', VFASPF(VZ))-VFASPF(VZ) > %(B0dD0_DeltaZVtx)s*mm) & (BPVVDZ > %(B0dPV_DeltaZVtx)s*mm) & "\
                    " (VFASPF(VCHI2/VDOF)< %(B0d_VCHI2)s ) " % config

        ### Now make all the selections ###
        
        self.__MakeMuSel__()
        self.__MakeD0__()
        self.__MakeDstar__()
        self.__MakeB0d__()
        
        from StrippingConf.StrippingLine import StrippingLine
        
        ### Now make a stripping line ###
        B0dLine=StrippingLine(self._name,
                              prescale = config['Prescale'],
                              postscale = config['Postscale'],
                              algos = [ self.B0Sel ]
                              )
        
        self.registerLine(B0dLine)
        ### Collect them all together in a nice way ###
        self.Line=B0dLine
        #self.TopSelectionSeq=SeqBd2DstarMuNu
        self.Selections=[self.MuSel, self.D0Sel, self.DstarSel, self.B0Sel]
        
    def printCuts(self):
        '''Print the compiled cut values'''
        print 'name', self._name
        print 'MuCut', self.MuCut
        print 'KCut', self.KCut
        print 'PiCut', self.PiCut
        print 'slowPiCut', self.slowPiCut
        print 'D0CombCut', self.D0CombCut
        print 'D0Cut', self.D0Cut
        print 'DstarCombCut', self.DstarCombCut
        print 'DstarCut', self.DstarCut
        print 'B0CombCut', self.B0CombCut
        print 'B0Cut', self.B0Cut
        
    ############ Functions to make Selections #######################
        
    def __MakeMuSel__(self):
        """
        the bachelor muon selection, takes some keyword arguements to make a muon selection
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseMuons

        MuForB0d = FilterDesktop(Code=self.MuCut)
        SelMuForB0d = Selection("SelMuFor"+self._name,
                                Algorithm=MuForB0d, RequiredSelections = [StdLooseMuons])
        
        self.MuSel=SelMuForB0d

    def __MakeD0__(self):
        """
        D0 for the selection
        Here [D0 -> K+ pi-]cc
        Which can be assosciated in this selection to:
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseKaons, StdLoosePions
        
        D02KpiForB0d = CombineParticles(
            DecayDescriptor = "[D0 -> K- pi+]cc " ,
            DaughtersCuts = {
                "K+"  : self.KCut,
                "pi+" : self.PiCut
                } ,
            CombinationCut = self.D0CombCut,
            MotherCut = self.D0Cut
            )        
        
        SelD02KpiForB0d = Selection("SelD02KpiFor"+self._name,
                                    Algorithm=D02KpiForB0d,
                                    RequiredSelections = [StdLooseKaons,StdLoosePions])
        
        self.D0Sel=SelD02KpiForB0d
    
    def __MakeDstar__(self):
        """
        Dstar for the selection 
        Here [D*(2010)+ -> pi+ D0]cc
        
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLoosePions
        
        DstarForB0d = CombineParticles(
            DecayDescriptor =  "[D*(2010)+ -> pi+ D0]cc" ,
            DaughtersCuts = {
                "pi+" : self.slowPiCut
                } ,
            CombinationCut = self.DstarCombCut,
            MotherCut = self.DstarCut
            )
                
        SelDstarForB0d = Selection("SelDstarFor"+self._name,
                                    Algorithm=DstarForB0d,
                                    RequiredSelections = [self.D0Sel,StdLoosePions])
        
        self.DstarSel=SelDstarForB0d
    
    def __MakeB0d__(self):
        """
        B0d selection:
        [B0 -> D*- mu+]cc, [B0 -> D*+ mu+]cc
        use of wrong charge combination is useful for background subtraction
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        CombBd2DstarMuNu = CombineParticles(        
            DecayDescriptors = ["[B0 -> D*(2010)- mu+]cc", "[B0 -> D*(2010)+ mu+]cc"], #includes wrong charges
            CombinationCut = self.BCombCut,
            MotherCut      = self.BCut
            )
        
        SelBd2DstarMuNu = Selection("Sel"+self._name, Algorithm=CombBd2DstarMuNu,
                                    RequiredSelections = [self.MuSel,self.DstarSel])
        
        self.B0Sel=SelBd2DstarMuNu

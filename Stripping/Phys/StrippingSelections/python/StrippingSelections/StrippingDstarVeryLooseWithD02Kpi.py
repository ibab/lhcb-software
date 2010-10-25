# $Id: StrippingDstarVeryLooseWithD02Kpi.py,v 1.6 2010-09-03 00:05:35 svecchi Exp $
'''
Module for constuction of Dstar->D0pi lines

==== Description of the lines ====

Dstar prompt is needed as calibration channel for the measurement of the bbar cross section using the channel
Bd->DstarMuNu
(Dstar+->D0pi+-, D0->Kpi DCS included)

There are three lines:

A) Loose, Right and Wrong Dst mu combinations, loose cuts to study cross section in eta bins, to be prescaled as rate increases.
   Will disappear as the trigger requirements become too tight



==== Description of the configuration ====

The selection cuts are stored in the dictionaries: confdict['Loose'] 
The cuts are stored only if they are different between the lines, common cuts are hardcoded.

The configuration class makes all the selections and the lines, when passed a correct dictionary.

The lines look basically like this:

1) Make a D0
2) Make a Dstar

To look at all the configurable cuts, see StrippingDstarVeryLooseWithD02Kpi.confdict

==== How to use ====

To configure all lines, just instantiate the class:

all=DstarVeryLooseWithD02KpiAllLinesConf(confdict)

Then to print all cuts do:

all.printCuts()

You can configure one line at a time with the DstarVeryLooseWithD02KpiOneLineConf class:

one=DstarVeryLooseWithD02KpiOneLineConf('Loose',confdict['Loose'])
'''
__author__ = [ 'Stefania Vecchi, Marta Calvi' ]
__date__ = '2010-10-04'
__version = '$Revision: 1.6 $'

#### Which VertexFitter to use? ####

#combiner='LoKi::VertexFitter'
combiner='OfflineVertexFitter'

#### Next is the dictionary of all tunable cuts ########
#### It is separated into the different lines   ########


confdict={
    'Loose' : { 'Prescale'    : 0.03 ,
                'Postscale'   : 1.0 ,
                #kaon parameters
                'KPT'         : 350,# MeV
                #'KTrChi2'     :  8,
                'KIPChi2'     :  0,
                'KTrPID'      : -5,
                #pion parameters
                'PiPT'        : 350, # MeV
                #'PiTrChi2'    :  8, # 10   # 5
                'PiIPChi2'    :  0, # 4    # 4
                #D0-resonance parameters
                'D0MassW'     : 60, # 40  # 40 #MeV
                'D0_BPVVDCHI2': 50,
                #Dstar-resonance parameters
                #'slowPiTrChi2': 8, # 10 # 5
                'Dstar_PT'    : 1250, # 1250 # 1250 # MeV
                'Dstar_VCHI2' : 25, # 20 # 15
                'DstarMassW'  : 80 # 50 # 80 MeV
                }
    }





class DstarVeryLooseWithD02KpiAllLinesConf(object):
    """
    Configuration object for all DstarVeryLooseWithD02Kpi lines

    DstarVeryLooseWithD02Kpi attempts to strip ((Kpi)pi) Mu.
    Several different lines are used, and for more information call help on the module
    
    usage:
    configdict={'LineNameSuffix' : {...},
                'LineNameSuffix2': {...} }
    
    DstarVeryLooseWithD02KpiAllLinesConf(config, offLines=[] )
    
    To turn off lines which otherwise would be created, add the name
    of the line to offLines.
    
    To only configure/run one line, it's better to use the DstarVeryLooseWithD02KpiOneLineConf class.
    
    The created lines appear as a list in the Lines object, member variable
    
    To print out all the cuts, use the printCuts method
    """
    
    Lines=[]
    
    confObjects={}
    
    def __init__(self, config, offLines=[]):
        '''In the constructor we make all the lines, and configure them all
        config is the dictionary of {LineSuffix : configuration}
        offlines is a list of lines to turn off'''
        for aline in config.keys():
            if aline not in offLines:
                lineconf=DstarVeryLooseWithD02KpiOneLineConf(aline, config[aline])
                
                self.confObjects[aline]=lineconf
                self.Lines.append(lineconf.Line)
                
    def printCuts(self):
        '''Print out all the configured cuts for the lines you asked for'''
        for aline in self.confObjects.keys():
            print '===='
            self.confObjects[aline].printCuts()
            

class DstarVeryLooseWithD02KpiOneLineConf(object):
    """
    Configuration object for a DstarVeryLooseWithD02Kpi line

    usage: config={...}
    DstarVeryLooseWithD02KpiConf(LineSuffix, config)
    Will make lines ending in LineSuffix with the config configurations
    
    The cuts are configuration parameter only if they are different between the lines,
    common cuts are hardcoded.
    
    Use conf.printCuts to check the cuts in python
    The selections are available individually as DSel and DstarSel
    The Line object, a member of this class, holds the configured line
    """
    
    Line=None
    Selections=[]
    TopSelectionSeq=None
    
    KCut=''
    PiCut=''
    slowPiCut=''
    D0CombCut=''
    D0Cut=''
    DstarCombCut=''
    DstarCut=''
    
    D0Sel=None
    DstarSel=None
    
    LineSuffix=''
    
    __configuration_keys__=[
        'Prescale',
        'Postscale',
        #kaon parameters
        'KPT',
        #'KTrChi2',
        'KIPChi2',
        'KTrPID',
        #pion parameters
        'PiPT',
        #'PiTrChi2',
        'PiIPChi2',
        #D0-resonance parameters
        'D0MassW',
        'D0_BPVVDCHI2',
        #Dstar-resonance parameters
        #'slowPiTrChi2',
        'Dstar_PT',
        'Dstar_VCHI2',
        'DstarMassW'
        ]
    
    def __init__(self, LineSuffix, config):
        '''The constructor of the configuration class.
        Requires a name which is added to the end of each algorithm name, LineSuffix
        and a configuration dictionary, config, which must provide all the settings
        which differ between the lines'''

        from StrippingSelections.Utils import checkConfig
        
        checkConfig(DstarVeryLooseWithD02KpiOneLineConf.__configuration_keys__,
                    config)
        
        self.LineSuffix=LineSuffix
        
        ### first we define the cuts from the configuration ###
        ### it's nice to see all the cuts in one place      ###

        # Cuts for D0        
        self.KCut  = " (PT > %(KPT)s*MeV) & (P > 2.0*GeV) & (ISLONG) & "\
                     " (MIPDV(PRIMARY) > 0.04*mm) &"\
                     " (MIPCHI2DV(PRIMARY)> %(KIPChi2)s ) & "\
                     " (PIDK >  %(KTrPID)s ) " % config
                     
        self.PiCut = " (PT> %(PiPT)s*MeV) & (P > 2.0*GeV) & (ISLONG) & "\
                     " (MIPDV(PRIMARY) > 0.04*mm) &"\
                     " (MIPCHI2DV(PRIMARY)> %(PiIPChi2)s ) " % config
        
        self.D0CombCut = " (APT>1200*MeV) & (ADAMASS('D0') < 70*MeV  )"% config

        self.D0Cut = " (PT>1600*MeV) & (ADMASS('D0') < %(D0MassW)s *MeV ) &  " \
                     " (BPVVDCHI2 > %(D0_BPVVDCHI2)s) & (VFASPF(VCHI2/VDOF)<10)  & "\
                     " (BPVDIRA > 0.9995)"% config    # <==== DIRA cut is the only difference
                                                      # <==== wrt Dstar selection in StrippingBd2DstarMuNu.Loose

        # Cuts for Dstar        
        self.slowPiCut= "  (PT>110*MeV) & (ISLONG) & "\
                        "     (MIPDV(PRIMARY) > 0.04*mm) " % config        

        self.DstarCombCut="(ADAMASS('D*(2010)+')< %(DstarMassW)s*MeV) & (APT>%(Dstar_PT)s *MeV) " % config
        
        self.DstarCut = " (VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2)s ) & (M-MAXTREE('D0'==ABSID,M)<160 * MeV)" % config


        ### Now make all the selections ###
        
        self.__MakeD0__()
        self.__MakeDstar__()
        
        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import SelectionSequence
        #SeqDstarVeryLooseWithD02Kpi = SelectionSequence("SeqDstarVeryLooseWithD02Kpi"+self.LineSuffix, TopSelection = self.DstarSel)
        ### Now make a stripping line ###
        DstarLine=StrippingLine("DstarVeryLooseWithD02Kpi"+self.LineSuffix,
                              prescale = config['Prescale'],
                              postscale = config['Postscale'],
                              algos = [ self.DstarSel ]
                              )
        
        ### Collect them all together in a nice way ###
        self.Line=DstarLine
        #self.TopSelectionSeq=SeqDstarVeryLooseWithD02Kpi
        self.Selections=[self.D0Sel, self.DstarSel]
        
    def printCuts(self):
        '''Print the compiled cut values'''
        print 'name', self.LineSuffix
        print 'KCut', self.KCut
        print 'PiCut', self.PiCut
        print 'slowPiCut', self.slowPiCut
        print 'D0CombCut', self.D0CombCut
        print 'D0Cut', self.D0Cut
        print 'DstarCombCut', self.DstarCombCut
        print 'DstarCut', self.DstarCut

        
    ############ Functions to make Selections #######################
        
    def __MakeD0__(self):
        """
        D0 for the selection
        Here [D0 -> K+ pi-]cc
        Which can be assosciated in this selection to:
        """
        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        
        D02Kpi = CombineParticles(self.LineSuffix+"D02Kpi")
        D02Kpi.DecayDescriptors = ["[D0 -> K- pi+]cc", "[D0 -> K+ pi-]cc"]  # includes WS (DCS) combination
        D02Kpi.DaughtersCuts = {
            "K+"  : self.KCut,
            "pi+" : self.PiCut
            } 
        D02Kpi.CombinationCut = self.D0CombCut
        D02Kpi.MotherCut = self.D0Cut
        
        D02Kpi.ParticleCombiners = {
            ''  : combiner
            } 
        
        MyStdLooseKaonsForD0 = DataOnDemand(Location = 'Phys/StdLooseKaons')
        MyStdLoosePionsForD0 = DataOnDemand(Location = 'Phys/StdLoosePions')
        
        SelD02Kpi = Selection("SelD02Kpi"+self.LineSuffix,
                              Algorithm=D02Kpi,
                              RequiredSelections = [MyStdLooseKaonsForD0,MyStdLoosePionsForD0])
        
        self.D0Sel=SelD02Kpi
    
    def __MakeDstar__(self):
        """
        Dstar for the selection 
        Here [D*(2010)+ -> pi+ D0]cc
        
        """
        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        
        Dstar = CombineParticles(self.LineSuffix+"Dstar")
        Dstar.DecayDescriptor =  "[D*(2010)+ -> pi+ D0]cc" 
        Dstar.DaughtersCuts = {
            "pi+" : self.slowPiCut
            } 
        Dstar.CombinationCut = self.DstarCombCut
        Dstar.MotherCut = self.DstarCut
        
        Dstar.ParticleCombiners = {
            ''  : combiner
            } 
        

        MyStdLoosePionsForDstar = DataOnDemand(Location = 'Phys/StdLoosePions')
        
        SelDstar = Selection("SelDstar"+self.LineSuffix,
                                    Algorithm=Dstar,
                                    RequiredSelections = [self.D0Sel,MyStdLoosePionsForDstar])
        
        self.DstarSel=SelDstar
    

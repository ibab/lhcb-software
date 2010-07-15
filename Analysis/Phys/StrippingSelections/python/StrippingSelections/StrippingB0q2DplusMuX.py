# $Id: StrippingB0q2DplusMuX.py,v 1.4 2010-07-15 18:35:06 rlambert Exp $
'''
Module for constuction of B0q->DplusMuNuX lines

B0q->DplusMuNuX0 with X0 neutrino, gamma, whatever,
Dq->KKpi, as required for Afs analysis in semileptonics.

There will be three lines:

A) Presel, sidebands in all variables, to be prescaled as rate increases
B) MC09, Full MC09-tuned offline selection
C) Data-tuned stripping selection (to be written)

The Presel is initially scaled to 10%, which should be fine up to 100 pb-1.

The MC09 selection expects 10^7 events in 1 fb-1, and is the hardest selection at the moment.

The Data-tuned-stripping selection will be written to provide 10x or 100x less rate than the Presel.

The selection cuts are stored in the dictionaries: confdict['Presel'] or 'MC09' or 'Tuned'.
The cuts are stored only if they are different between the lines, common cuts are hardcoded.

The configuration class makes all the selections and the lines, when passed a correct dictionary.

The lines look basically like this:

1) Choose a Muon
2) Make a D
3) Make a B

'''
__author__ = [ 'Rob Lambert' ]
__date__ = '2010-07-15'
__version = '$Revision: 1.4 $'

#### This is the dictionary of all tunable cuts ########
#### It is separated into the different lines   ########

confdict={
    'Presel' : { 'Prescale'    : 1.0 ,
                 'Postscale'   : 1.0 ,
                 #muon paramters
                 'MuPT'        : 500, #MeV
                 'MuPidPi'     : -5.,
                 'MuPidK'      : -10.,
                 'MuTrChi2'    : 9,
                 #kaon parameters
                 'KPT'         : 300, #MeV
                 'KPidPi'      : -5,
                 'KPidMu'      : -40,
                 'KPidP'       : -40,
                 'KTrChi2'     : 16,
                 'KIPChi2'     : 4,
                 #pion parameters
                 'PiPidK'      : -40,
                 'PiPidMu'     : -5,
                 'PiTrChi2'    : 16,
                 'PiIPChi2'    : 4,
                 #D-resonance parameters
                 'DPT'         : 1000, #MeV
                 'D_APT'       : 1000, #MeV
                 'D_VPCHI2'    : 0.000250,
                 'D_BPVVDCHI2' : 49,
                 #B-resonance parameters
                 'B_VPCHI2'    : 0.000250,
                 'B_BPVDIRA'   : 0.997
                 },
    
    'MC09'   : { 'Prescale'    : 1.0 ,
                 'Postscale'   : 1.0 ,
                 #muon paramters
                 'MuPT'        : 600, #MeV
                 'MuPidPi'     : 0.,
                 'MuPidK'      : -10,
                 'MuTrChi2'    : 2,
                 #kaon parameters
                 'KPT'         : 400, #MeV
                 'KPidPi'      : 7,
                 'KPidMu'      : 10,
                 'KPidP'       : -10,
                 'KTrChi2'     : 3,
                 'KIPChi2'     : 4,
                 #pion parameters
                 'PiPidK'      : -5,
                 'PiPidMu'     : -2,
                 'PiTrChi2'    : 9,
                 'PiIPChi2'    : 7,
                 #D-resonance parameters
                 'DPT'         : 1500, #MeV
                 'D_APT'       : 1200, #MeV
                 'D_VPCHI2'    : 0.0150,
                 'D_BPVVDCHI2' : 144,
                 #B-resonance parameters
                 'B_VPCHI2'    : 0.0150,
                 'B_BPVDIRA'   : 0.9980
                 }
    }





class B0q2DplusMuXAllLinesConf(object):
    """
    Configuration object for all B0qDplusMuX lines

    B0qDplusMuX attempts to strip (KKpi)Mu.
    Several different lines are used, and for more information call help on the module
    
    usage:
    configdict={'LineNameSuffix' : {...},
                'LineNameSuffix2': {...} }
    
    B0q2DplusMuXAllLinesConf(config, offLines=[] )
    
    To turn off lines which otherwise would be created, add the name
    of the line to offLines.
    
    To only configure/run one line, it's better to use the B0q2DplusMuXOneLineConf class.
    
    The created lines appear as a list in the Lines object
    
    To print out all the cuts, use the printCuts method
    """
    
    Lines=[]
    
    confObjects={}
    
    def __init__(self, config, offLines=[]):
        '''In the constructor we make all the lines, and configure them all'''
        for aline in config.keys():
            if aline not in offLines:
                lineconf=B0q2DplusMuXOneLineConf(aline, config[aline])
                
                self.confObjects[aline]=lineconf
                self.Lines.append(lineconf.Line)
                
    def printCuts(self):
        '''Print out all the configured cuts for the lines you asked for'''
        for aline in self.confObjects.keys():
            print '===='
            self.confObjects[aline].printCuts()
            

class B0q2DplusMuXOneLineConf(object):
    """
    Configuration object for a B0qDplusMuX line

    usage: config={...}
    B0q2DplusMuXConf(LineSuffix, config)
    Will make lines ending in LineSuffix with the config configurations
    
    The cuts are configuration parameter only if they are different between the lines,
    common cuts are hardcoded.
    
    Use conf.printCuts to check the cuts in python
    The selections are available individually as MuSel, DSel and B0Sel
    The Line object holds the configured line
    """
    
    Line=None
    Selections=[]
    TopSelectionSeq=None
    
    MuCut=''
    KCut=''
    PiCut=''
    DCut=''
    BCut=''
    
    MuSel=None
    DSel=None
    B0Sel=None
    
    LineSuffix=''
    
    __configuration_keys__=[
        'Prescale',
        'Postscale',
        #muon paramters
        'MuPT',
        'MuPidPi',
        'MuPidK',
        'MuTrChi2',
        #kaon parameters
        'KPT',
        'KPidPi',
        'KPidMu',
        'KPidP',
        'KTrChi2',
        'KIPChi2',
        #pion parameters
        'PiPidK',
        'PiPidMu',
        'PiTrChi2',
        'PiIPChi2',
        #D-resonance parameters
        'DPT',
        'D_APT',
        'D_VPCHI2',
        'D_BPVVDCHI2',
        #B-resonance parameters
        'B_VPCHI2',
        'B_BPVDIRA'
        ]
    
    def __init__(self, LineSuffix, config):
        '''The constructor of the configuration class.
        Requires a name which is added to the end of each algorithm name, LineSuffix
        and a configuration dictionary, config, which must provide all the settings
        which differ between the lines'''

        from StrippingSelections.Utils import checkConfig
        
        checkConfig(B0q2DplusMuXOneLineConf.__configuration_keys__,
                    config)
        
        self.LineSuffix=LineSuffix
        
        ### first we define the cuts from the configuration ###
        ### it's nice to see all the cuts in one place      ###
        
        self.MuCut ="((ISMUON) & (HASMUON) & (ISLONG) "\
                "& (PT >  %(MuPT)s *MeV) & (TRCHI2DOF< %(MuTrChi2)s ) "\
                "& (PIDmu-PIDpi > %(MuPidPi)s ) & (PIDmu-PIDK > %(MuPidK)s )) "\
                "& (MIPCHI2DV(PRIMARY) > 2.0) & (MIPDV(PRIMARY) > 0.01)" % config
        
        
        self.KCut= "(P > 2.0*GeV)"\
                   " & (PIDK-PIDpi >  %(KPidPi)s ) & (PIDK-PIDmu  > %(KPidMu)s)  & (PIDK-PIDp  > %(KPidP)s )"\
                   " & (PT>  %(KPT)s*MeV)  & (TRCHI2DOF < %(KTrChi2)s) &"\
                   " (MIPDV(PRIMARY) > 0.03) & (MIPCHI2DV(PRIMARY) > %(KIPChi2)s )" % config
        
        self.PiCut="(P > 2.0*GeV) & (PT>300*MeV)"\
               " & (PIDpi-PIDK >  %(PiPidK)s ) & (PIDpi-PIDmu  > %(PiPidMu)s)"\
               " & (TRCHI2DOF < %(PiTrChi2)s) &"\
               " (MIPDV(PRIMARY) > 0.01) & (MIPCHI2DV(PRIMARY) > %(PiIPChi2)s )" % config
        
        
        self.DCut="("\
                  "(VFASPF(VPCHI2) > %(D_VPCHI2)s ) "\
                  "& (VFASPF(VCHI2/VDOF) < 16.0) "\
                  "& (M > 1768.*MeV) & (M < 2068.*MeV) "\
                  "& (PT > %(DPT)s *MeV) "\
                  "& (BPVDIRA > 0.99) "\
                  "& (BPVVDZ  > 1.0*mm) "\
                  "& (BPVVDCHI2 > %(D_BPVVDCHI2)s ) "\
                  ")" % config

        self.DCombCut="(ADAMASS('D_s-')<210*MeV) & (APT>%(D_APT)s *MeV) & (AMAXDOCA('')<0.3*mm)" % config
        
        self.BCut = "("\
               "(M < 6300.*MeV) & (M > 2200.*MeV) & (VFASPF(VCHI2/VDOF)<16) & "\
               "(VFASPF(VPCHI2)> %(B_VPCHI2)s ) & "\
               "((MINTREE(('D_s+'==ABSID),VFASPF(VZ))-VFASPF(VZ))>0*mm) "\
               "& (BPVDIRA> %(B_BPVDIRA)s ) "\
               "& (BPVVDCHI2>1) "\
               ")"  % config
        
        ### Now make all the selections ###
        
        self.__MakeMuSel__()
        self.__MakeDplus__()
        self.__MakeB0__()
        
        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import SelectionSequence
        
        #SeqB0q2DplusMuX = SelectionSequence("SeqB0q2DplusMuX"+self.LineSuffix, TopSelection = self.B0Sel)
        ### Now make a stripping line ###
        B0qLine=StrippingLine("B0q2DplusMuX"+self.LineSuffix,
                              prescale = config['Prescale'],
                              postscale = config['Postscale'],
                              algos = [ self.B0Sel ]
                              )
        
        ### Collect them all together in a nice way ###
        self.Line=B0qLine
        #self.TopSelectionSeq=SeqB0q2DplusMuX
        self.Selections=[self.MuSel, self.DSel, self.B0Sel]
        
    def printCuts(self):
        '''Print the compiled cut values'''
        print 'name', self.LineSuffix
        print 'MuCut', self.MuCut
        print 'KCut', self.KCut
        print 'PiCut', self.PiCut
        print 'DCombCut', self.DCombCut
        print 'DCut', self.DCut
        print 'BCut', self.BCut
        
    ############ Functions to make Selections #######################
        
    def __MakeMuSel__(self):
        """
        the bachelor muon selection, takes some keyword arguements to make a muon selection
        """
        from Configurables import FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        MuForB0q = FilterDesktop(self.LineSuffix+"MuForB0q")
        MuForB0q.Code = self.MuCut
        MyStdMuons = DataOnDemand(Location = 'Phys/StdLooseMuons')
        SelMuForB0q = Selection("SelMuForB0q2DplusMuX"+self.LineSuffix,
                                Algorithm=MuForB0q, RequiredSelections = [MyStdMuons])
        
        self.MuSel=SelMuForB0q
        
    
    def __MakeDplus__(self):
        """
        Dplus for the selection
        Don't want to use the standard, because the decay descriptor is sub-optimal
        Here [D_s- -> K+ K- pi-]cc
        Which can be assosciated in this selection to:
        [(Meson & Charm & Strange) ==> K+ K- pi- {pi0} ]
        or
        [(Meson & Charm & Down) ==> K+ K- pi- {pi0} ]
        
        """
        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        
        Dplus2KKpiForB0q = CombineParticles(self.LineSuffix+"Dplus2KKpiForB0q")
        Dplus2KKpiForB0q.DecayDescriptor = "[D_s- -> K+ K- pi-]cc" 
        Dplus2KKpiForB0q.DaughtersCuts = {
            "K+"  : self.KCut,
            "pi+" : self.PiCut
            } 
        Dplus2KKpiForB0q.CombinationCut = self.DCombCut
        Dplus2KKpiForB0q.MotherCut = self.DCut
        
        MyStdLooseKaons = DataOnDemand(Location = 'Phys/StdLooseKaons')
        MyStdLoosePions = DataOnDemand(Location = 'Phys/StdLoosePions')
        
        SelDplus2KKpiForB0q = Selection("SelDplus2KKpiForB0q"+self.LineSuffix,
                                        Algorithm=Dplus2KKpiForB0q,
                                        RequiredSelections = [MyStdLooseKaons,MyStdLoosePions])
            
        self.DSel=SelDplus2KKpiForB0q
    
    def __MakeB0__(self):
        """
        B0q selection:
        [B_s0 -> D_s- mu+]cc, [B_s0 -> D_s+ mu+]cc
        But really this can be associated to anything of the form:
        [ (Meson & Beauty & Strange) => l+ Nu ( (Meson & Charm & Strange) ==> K+ K- pi- {pi0} ) {pi0} ]CC
        or
        [ (Meson & Beauty & Down) => l+ Nu ( (Meson & Charm & Down) ==> K+ K- pi- {pi0} ) {pi0} ]CC
        """
        from Configurables import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        CombB0q2DplusMuX = CombineParticles(self.LineSuffix+'CombB0q2DplusMuX')
        
        CombB0q2DplusMuX.DecayDescriptors = ["[B_s0 -> D_s- mu+]cc", "[B_s0 -> D_s+ mu+]cc"] #includes wrong charges
        
        CombB0q2DplusMuX.CombinationCut = "((AM > 2000.*MeV) & (AMAXDOCA('')<0.3*mm))"
        
        CombB0q2DplusMuX.MotherCut      = self.BCut
        
        SelB0q2DplusMuX = Selection("SelB0q2DplusMuX"+self.LineSuffix, Algorithm=CombB0q2DplusMuX,
                                    RequiredSelections = [self.DSel, self.MuSel])
        
        self.B0Sel=SelB0q2DplusMuX

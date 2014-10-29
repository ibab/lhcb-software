# $Id: StrippingB0q2DplusMuX.py,v 1.6 2010-09-03 00:05:35 rlambert Exp $
"""
Module for constuction of B0q->DplusMuNuX lines

==== Description of the lines ====

B0q->DplusMuNuX0 with X0 being neutrino, gamma, pi0, whatever,
Dq->KKpi, as required for Afs analysis in semileptonics.
(Ds+->KKpi, D+->KKpi)

There are three lines:

A) Presel, sidebands in all variables, to be prescaled as rate increases
B) MC09, Full MC09-tuned offline selection
C) Tuned stripping selection (tuned on real data)

The Presel is initially scaled to 10%, which should be fine up to 100 pb-1.

The MC09 selection expects 10^7 events in 1 fb-1, and is the hardest selection at the moment.

The Tuned selection is inbetween Presel and MC09, to achieve the target retention ~0.01%  and timing of <0.4 ms/event
In the first stage, this was tuned on some early preselected data, where it was found the MC09 cuts on kinematic variables could be applied.
The PID and tracking cuts were optimised on the data itself, and a slightly looser version of the optimal selection is now used here.

It was found that the 12b cuts were too tight in the Tuned selection, so now they have been reduced.

We will soon add an MC10 selection, and prescale away the MC09 selection at that time.

==== Description of the configuration ====

The selection cuts are stored in the dictionaries: confdict['Presel'] or 'MC09' or 'Tuned'.
The cuts are stored only if they are different between the lines, common cuts are hardcoded.

The configuration class makes all the selections and the lines, when passed a correct dictionary.

The lines look basically like this:

1) Choose a Muon
2) Make a D
3) Make a B

To look at all the configurable cuts, see StrippingB0q2DplusMuX.confdict

==== How to use ====

To configure all lines, just instantiate the class:

all=B0q2DplusMuXAllLinesConf('B0q2DplusMuX', confdict)

Then to print all cuts do:

all.printCuts()

You can configure one line at a time with the B0q2DplusMuXOneLineConf class:

one=B0q2DplusMuXOneLineConf('B0q2DplusMuX'+'Tuned',confdict['Tuned'])
"""
__author__ = [ 'Rob Lambert' ]
__date__ = '2010-08-11'
__version = '$Revision: 1.7 $'

#### Which VertexFitter to use? ####

#### Next is the dictionary of all tunable cuts ########
#### It is separated into the different lines   ########


confdict={
    'Presel' : { 'Prescale'    : 0.02 ,
                 'Postscale'   : 1.0 ,
                 #muon paramters
                 'MuPT'        : 500, #MeV
                 'MuPidPi'     : -5.,
                 'MuPidK'      : -10.,
                 'MuTrChi2'    : 9,
                 #kaon parameters
                 'KPT'         : 300, #MeV
                 'KIP'         : 0.03, #mm
                 'KPidPi'      : -5,
                 'KPidPi2'   : -5,
                 'KPidMu'      : -40,
                 'KPidP'       : -40,
                 'KTrChi2'     : 10, #set in the common particles
                 'KIPChi2'     : 4,
                 #pion parameters
                 'PiPidK'      : -40,
                 'PiIP'        : 0.01, #mm
                 'PiPidMu'     : -5,
                 'PiTrChi2'    : 10, #set in the common particles
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
    
    'Tuned'   : { 'Prescale'    : 1.0 ,
                 'Postscale'   : 1.0 ,
                 #muon paramters
                 'MuPT'        : 500, #MeV
                 'MuPidPi'     : 1., #reduce pion feedthrough to 1%
                 'MuPidK'      : -5,
                 'MuTrChi2'    : 5,
                 #kaon parameters
                 'KPT'         : 400, #MeV
                 'KIP'         : 0.04, #mm
                 'KPidPi'      : 5,
                 'KPidPi2'     : 10, #Kaon with same charge as pion
                 'KPidMu'      : 5,
                 'KPidP'       : -10,
                 'KTrChi2'     : 5,
                 'KIPChi2'     : 4,
                 #pion parameters
                 'PiPidK'      : -5,
                 'PiIP'        : 0.04, #mm
                 'PiPidMu'     : -5,
                 'PiTrChi2'    : 10,
                 'PiIPChi2'    : 7,
                 #D-resonance parameters
                 'DPT'         : 1500, #MeV
                 'D_APT'       : 1200, #MeV
                 'D_VPCHI2'    : 0.010,
                 'D_BPVVDCHI2' : 144,
                 #B-resonance parameters
                 'B_VPCHI2'    : 0.010,
                 'B_BPVDIRA'   : 0.9980
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
                 'KIP'         : 0.03, #mm
                 'KPidPi'      : 7,
                 'KPidPi2'     : 7, #Kaon with same charge as pion
                 'KPidMu'      : 10,
                 'KPidP'       : -10,
                 'KTrChi2'     : 3,
                 'KIPChi2'     : 4,
                 #pion parameters
                 'PiPidK'      : -5,
                 'PiIP'        : 0.01, #mm
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



from StrippingUtils.Utils import LineBuilder, MasterLineBuilder

name = "B0q2DplusMuX"

class B0q2DplusMuXAllLinesConf(MasterLineBuilder):
    """
    Configuration object for all B0qDplusMuX lines

    B0qDplusMuX attempts to strip (KKpi)Mu.
    Several different lines are used, and for more information call help on the module
    
    usage:
    configdict={'LineNameSuffix' : {...},
                'LineNameSuffix2': {...} }
    
    B0q2DplusMuXAllLinesConf('B0q2DplusMuX', config, offLines=[] )
    
    To turn off lines which otherwise would be created, add the name
    of the line to offLines.
    
    To only configure/run one line, it's better to use the B0q2DplusMuXOneLineConf class.
    
    The created lines appear as a tuple from the lines() method
    
    To print out all the cuts, use the printCuts method
    """    
    
    def __init__(self, name, config):
        '''In the constructor we make all the lines, and configure them all
        name is the initial name of the lines, i.e. B0q2DplusMuX
        config is the dictionary of {LineSuffix : configuration}
        '''
        MasterLineBuilder.__init__(self, name, config, B0q2DplusMuXOneLineConf)
        
    def printCuts(self):
        '''Print out all the configured cuts for the lines you asked for'''
        for aline in self.slaves():
            print '===='
            aline.printCuts()
            

class B0q2DplusMuXOneLineConf(LineBuilder):
    """
    Configuration object for a B0qDplusMuX line

    usage: config={...}
    B0q2DplusMuXConf(name, config)
    Will make lines called name with the config configurations
    
    The cuts are configuration parameter only if they are different between the lines,
    common cuts are hardcoded.
    
    Use conf.printCuts to check the cuts in python
    The selections are available individually as MuSel, DSel and B0Sel
    The lines method returns the configured line
    """
    
    Line=None
    Selections=[]
    TopSelectionSeq=None
    
    MuCut=''
    KCut=''
    PiCut=''
    DCombCut=''
    DCut=''
    BCombCut=''
    BCut=''
    
    MuSel=None
    DSel=None
    B0Sel=None
        
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
        'KIP',
        'KPidPi',
        'KPidPi2',
        'KPidMu',
        'KPidP',
        'KTrChi2',
        'KIPChi2',
        #pion parameters
        'PiPidK',
        'PiIP',
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
    
    def __init__(self, name, config):
        '''The constructor of the configuration class.
        Requires a name which is added to the end of each algorithm name
        and a configuration dictionary, config, which must provide all the settings
        which differ between the lines'''
        LineBuilder.__init__(self, name, config)
        
        #from StrippingSelections.Utils import checkConfig
        #
        #checkConfig(B0q2DplusMuXOneLineConf.__configuration_keys__,
        #            config)
        
        ### first we define the cuts from the configuration ###
        ### it's nice to see all the cuts in one place      ###
        
        self.MuCut ="((ISMUON) & (HASMUON) & (ISLONG) "\
                "& (PT >  %(MuPT)s *MeV) & (TRCHI2DOF< %(MuTrChi2)s ) "\
                "& (PIDmu-PIDpi > %(MuPidPi)s ) & (PIDmu-PIDK > %(MuPidK)s )) "\
                "& (MIPDV(PRIMARY) > 0.01) & (MIPCHI2DV(PRIMARY) > 2.0) " % config
        
        
        self.KCut= "(P > 2.0*GeV)"\
                   " & (PT>  %(KPT)s*MeV)  & (TRCHI2DOF < %(KTrChi2)s) "\
                   " & (PIDK-PIDpi >  %(KPidPi)s ) & (PIDK-PIDmu  > %(KPidMu)s)  & (PIDK-PIDp  > %(KPidP)s )"\
                   " & (MIPDV(PRIMARY) > %(KIP)s) & (MIPCHI2DV(PRIMARY) > %(KIPChi2)s )" % config
        
        self.PiCut="(P > 2.0*GeV) & (PT>300*MeV)"\
               " & (TRCHI2DOF < %(PiTrChi2)s) "\
               " & (PIDpi-PIDK >  %(PiPidK)s ) & (PIDpi-PIDmu  > %(PiPidMu)s)"\
               " & (MIPDV(PRIMARY) > %(PiIP)s ) & (MIPCHI2DV(PRIMARY) > %(PiIPChi2)s )" % config
        
        
        #self.DCombCut="(ADAMASS('D_s-')<210*MeV) & (APT>%(D_APT)s *MeV) & (AMAXDOCA('')<0.3*mm)" % config
        self.DCombCut="(ADAMASS('D_s-')<210*MeV) & (DAMASS('D_s-')<120*MeV) & (APT> %(D_APT)s *MeV) & (ADOCACUT( 0.3*mm , '' ))" % config # & ( ACUTDOCACHI2(20,''))" % config
        
        self.DCut="("\
                  "in_range(1768.*MeV, M, 2068.*MeV)"\
                  "& (PT > %(DPT)s *MeV) "\
                  "& (VFASPF(VPCHI2) > %(D_VPCHI2)s ) "\
                  "& (BPVDIRA > 0.99) "\
                  "& (BPVVDZ  > 1.0*mm) "\
                  "& (BPVVDCHI2 > %(D_BPVVDCHI2)s ) "\
                  "& ((CHILD(PIDK-PIDpi,2)) > %(KPidPi2)s)  "\
                  ")" % config

                  #"& (VFASPF(VCHI2/VDOF) < 16.0) "\
                  #"& (M > 1768.*MeV) & (M < 2068.*MeV) "\
        
        self.BCombCut="((AM > 2000.*MeV) & (AMAXDOCA('')<0.3*mm))"
        
        self.BCut = "("\
                    "in_range(2200.*MeV,M,6300.*MeV) & "\
                    "(VFASPF(VPCHI2)> %(B_VPCHI2)s ) & "\
                    "((CHILD(VFASPF(VZ),1)-VFASPF(VZ))>0*mm) "\
                    "& (BPVDIRA> %(B_BPVDIRA)s ) "\
                    "& (BPVVDCHI2>1) "\
                    ")"  % config
        
                    #"(M < 6300.*MeV) & (M > 2200.*MeV) & "\        
                    #" & (VFASPF(VCHI2/VDOF)<16) "\
        ### Now make all the selections ###
        
        self.__MakeMuSel__()
        self.__MakeDplus__()
        self.__MakeB0__()
        
        from StrippingConf.StrippingLine import StrippingLine
        
        ### Now make a stripping line ###
        B0qLine=StrippingLine(self._name,
                              prescale = config['Prescale'],
                              postscale = config['Postscale'],
                              algos = [ self.B0Sel ],
                              EnableFlavourTagging = True
                              )
        
        self.registerLine(B0qLine)
        ### Collect them all together in a nice way ###
        self.Line=B0qLine
        #self.TopSelectionSeq=SeqB0q2DplusMuX
        self.Selections=[self.MuSel, self.DSel, self.B0Sel]
        
    def printCuts(self):
        '''Print the compiled cut values'''
        print 'name', self._name
        print 'MuCut', self.MuCut
        print 'KCut', self.KCut
        print 'PiCut', self.PiCut
        print 'DCombCut', self.DCombCut
        print 'DCut', self.DCut
        print 'BCombCut', self.BCombCut
        print 'BCut', self.BCut
        
    ############ Functions to make Selections #######################
        
    def __MakeMuSel__(self):
        """
        the bachelor muon selection, takes some keyword arguements to make a muon selection
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection
        import StandardParticles
        if hasattr(StandardParticles,"StdAllLooseMuons"):
            from StandardParticles import StdAllLooseMuons as StdLooseMuons
        else:
            from StandardParticles import StdLooseMuons
        
        MuForB0q = FilterDesktop(Code=self.MuCut)
        SelMuForB0q = Selection("SelMuFor"+self._name,
                                Algorithm=MuForB0q, RequiredSelections = [StdLooseMuons])
        
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
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        from StandardParticles import StdLooseKaons, StdLoosePions
        
        Dplus2KKpiForB0q = CombineParticles(
            DecayDescriptor = "[D_s- -> K+ K- pi-]cc", 
            DaughtersCuts = {
            "K+"  : self.KCut,
            "pi+" : self.PiCut
            } ,
            CombinationCut = self.DCombCut,
            MotherCut = self.DCut
            
            )
        
        SelDplus2KKpiForB0q = Selection("SelDplus2KKpiFor"+self._name,
                                        Algorithm=Dplus2KKpiForB0q,
                                        RequiredSelections = [StdLooseKaons,StdLoosePions])
            
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
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection
        
        CombB0q2DplusMuX = CombineParticles(
            DecayDescriptors = ["[B_s0 -> D_s- mu+]cc", "[B_s0 -> D_s+ mu+]cc"], #includes wrong charges
            CombinationCut = self.BCombCut,
            MotherCut      = self.BCut
            )
        
        SelB0q2DplusMuX = Selection("Sel"+self._name, Algorithm=CombB0q2DplusMuX,
                                    RequiredSelections = [self.MuSel, self.DSel])
        
        self.B0Sel=SelB0q2DplusMuX

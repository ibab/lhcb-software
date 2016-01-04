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
__author__ = [ 'Stefania Vecchi, Marta Calvi','Antonio Falabella' ]
__date__ = '2011-11-08'
__version = '$Revision: 1.7 $'

#### Which VertexFitter to use? ####


#### Next is the dictionary of all tunable cuts ########
#### It is separated into the different lines   ########

default_config = {
  'Bd2DstarMuNu' : {
    'WGs'         : ['Semileptonic'],
    'BUILDERTYPE' : 'Bd2DstarMuNuAllLinesConf',
    'CONFIG'      : {
##     'LooseNOPr' : { 'Prescale'    : 1.0 ,
##                  'Postscale'   : 1.0 ,
##                  'TrGHOSTPROBCut' : 1, # loose cut   
##                  #muon paramters # loose # Tight
##                  'MuPT'        : 700, # 800  # MeV
##                  'MuTrPID'     :-20,  # -5   
##                  #kaon parameters
##                  'KPT'         : 350, # 350  # MeV
##                  'KTrPID'      : -5,  #-5    
##                  #pion parameters
##                  'PiPT'        : 350, # MeV
##                  #D0-resonance parameters
##                  'D0MassW'     : 60, # 40    # MeV
##                  'D0_BPVVDCHI2': 50,
##                  #Dstar-resonance parameters
##                  'Dstar_PT'     : 1250, # 1250 # MeV
##                  'Dstar_VCHI2'  : 25, # 20 # 
##                  'DstarMassW'   : 80, # 50 # MeV
##                  # Bd-resonance parameters
##                  'B0d_VCHI2'    : 25, # 10 # 
##                  'B0dD0_DeltaZVtx' : -10, # -2.5  # mm
##                  'B0dPV_DeltaZVtx' : -100, # 0.5  # mm
##                  },
	    'Loose' : { 'Prescale'    : 0.3 ,
			'Postscale'   : 1.0 ,
			'TrGHOSTPROBCut' : 1, # loose cut   
			#muon paramters # loose # Tight
			'MuPT'        : 700, # 800  # MeV
			'MuTrPID'     :-20,  # -5   
			#kaon parameters
			'KPT'         : 350, # 350  # MeV
			'KTrPID'      : -5,  #-5    
			#pion parameters
			'PiPT'        : 350, # MeV
			#D0-resonance parameters
			'D0MassW'     : 60, # 40    # MeV
			'D0_BPVVDCHI2': 50,
			#Dstar-resonance parameters
			'Dstar_PT'     : 1250, # 1250 # MeV
			'Dstar_VCHI2'  : 25, # 20 # 
			'DstarMassW'   : 80, # 50 # MeV
			#Bd-resonance parameters
			'B0d_VCHI2'    : 25, # 10 # 
			'B0dD0_DeltaZVtx' : -10, # -2.5  # mm
			'B0dPV_DeltaZVtx' : -100, # 0.5  # mm
			},
	    'Tight'   : {'Prescale'    : 1.0 ,
			 'Postscale'   : 1.0 ,
			 'TrGHOSTPROBCut' : 0.5, # 
			 #muon paramters # 
			 'MuPT'        : 800, # MeV
			 'MuTrPID'     :-5,
			 #kaon parameters
			 'KPT'         : 350, # MeV
			 'KTrPID'      :-5, 
			 #pion parameters
			 'PiPT'        : 350, # MeV
			 #D0-resonance parameters
			 'D0MassW'     : 60, # MeV
			 'D0_BPVVDCHI2': 50,
			 #Dstar-resonance parameters
			 'Dstar_PT'     : 1250, #  MeV
			 'Dstar_VCHI2'  : 20, 
			 'DstarMassW'   : 70, 
			 #Bd-resonance parameters
			 'B0d_VCHI2'    : 25, 
			 'B0dD0_DeltaZVtx' : -2.5, # mm
			 'B0dPV_DeltaZVtx' : 0.5 #  mm
                 }
	 },
       'STREAMS'     : ['Semileptonic']	  
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
    #D0CombCut=''
    D0Cut=''
    #DstarCombCut=''
    DstarCut=''
    totalDstarCut=''    
    B0CombCut=''
    B0Cut=''
    
    MuSel=None
    D0Sel=None
    DstarSel=None
    B0Sel=None
    
        
    __configuration_keys__=[
        'Prescale',
        'Postscale',
        'TrGHOSTPROBCut',
        #muon paramters
        'MuPT',
        'MuTrPID',
        #kaon parameters
        'KPT',
        'KTrPID',
        #pion parameters
        'PiPT',
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
        ### NB: Common particles are defined with some cuts ###
        ### Check in CommonParticles the definition         ###


        # Cuts for Mu        
        self.MuCut = " (PT >  %(MuPT)s *MeV) & (P > 2.0*GeV) & (ISMUON) & (HASMUON) & "\
                     " (TRGHOSTPROB < %(TrGHOSTPROBCut)s) & "\
                     " (PIDmu > %(MuTrPID)s) " % config

        # Cuts to refine D0 selection
        self.KCut  = "& CHILDCUT(CHILDCUT( (PT > %(KPT)s*MeV) & (P > 2.0*GeV) & "\
                     " (MIPDV(PRIMARY) > 0.04*mm) & "\
                     " (TRGHOSTPROB < %(TrGHOSTPROBCut)s) & "\
                     " (PIDK >  %(KTrPID)s ),1),2) " % config
		     
        
        self.PiCut = "& CHILDCUT(CHILDCUT( (PT> %(PiPT)s*MeV) & (P > 2.0*GeV) & "\
                     " (TRGHOSTPROB < %(TrGHOSTPROBCut)s) & "\
                     " (MIPDV(PRIMARY) > 0.04*mm) ,2),2) " % config


        self.D0Cut = "& CHILDCUT( (PT>1600*MeV) & (ADMASS('D0') < %(D0MassW)s *MeV ) &  " \
                     " (BPVVDCHI2 > %(D0_BPVVDCHI2)s) & (VFASPF(VCHI2/VDOF)<10),2)  "% config

        # Cuts to refine D* selection
        self.slowPiCut= "& CHILDCUT( (PT>110*MeV) & "\
                        " (MIPDV(PRIMARY) > 0.04*mm),1) " % config        

        self.DstarCut = " (VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2)s ) & " \
                        " (PT>%(Dstar_PT)s *MeV) & (ADMASS('D*(2010)+')< %(DstarMassW)s*MeV)" % config
#& (M-MAXTREE('D0'==ABSID,M)<165 * MeV) &"\

        # Cuts for B0         
        self.BCombCut="((AM > 3000.*MeV) )"
        
        self.BCut = " (BPVDIRA> 0.999 ) & "\
                    "  in_range(3000.*MeV,M,5280.*MeV) & "\
                    " (MINTREE(ABSID=='D0', VFASPF(VZ))-VFASPF(VZ) > %(B0dD0_DeltaZVtx)s*mm) & (BPVVDZ > %(B0dPV_DeltaZVtx)s*mm) & "\
                    " (VFASPF(VCHI2/VDOF)< %(B0d_VCHI2)s ) " % config
	
        self.totalDstarCut = self.DstarCut + self.KCut + self.PiCut + self.D0Cut + self.slowPiCut		    

        ### Now make all the selections ###
        
        self.__MakeMuSel__()
        self.__MakeDstar__()
        self.__MakeB0d__()
        
        from StrippingConf.StrippingLine import StrippingLine
        
        ### Now make a stripping line ###
        B0dLine=StrippingLine(self._name,
                              prescale = config['Prescale'],
                              postscale = config['Postscale'],
                              algos = [ self.B0Sel ],
                              EnableFlavourTagging = True
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
        #print 'D0CombCut', self.D0CombCut
        print 'D0Cut', self.D0Cut
        #print 'DstarCombCut', self.DstarCombCut
        print 'DstarCut', self.DstarCut
        print 'totalDstarCut', self.totalDstarCut
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


    def __MakeDstar__(self):
        """
        Dstar for the selection 
        Here [D*(2010)+ -> pi+ D0]cc
        
        """
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        
        DstarsForB0d = FilterDesktop(
	    Code = self.totalDstarCut
            )
        #print "\nCreating Dstar from CommonParticles\n"
        MyStdDstars = DataOnDemand(Location = 'Phys/StdLooseDstarWithD02KPi/Particles')
        
        SelDstarsForB0d = Selection("SelDstarsFor"+self._name,Algorithm=DstarsForB0d, RequiredSelections = [MyStdDstars])
        
        self.DstarSel=SelDstarsForB0d
        

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
            MotherCut      = self.BCut,
            ##ReFitPVs       = True
            )
        
        SelBd2DstarMuNu = Selection("Sel"+self._name, Algorithm=CombBd2DstarMuNu,
                                    RequiredSelections = [self.MuSel,self.DstarSel])
        
        self.B0Sel=SelBd2DstarMuNu

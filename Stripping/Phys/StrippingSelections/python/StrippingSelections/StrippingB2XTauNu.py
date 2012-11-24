# $Id: StrippingB2XTauNu.py,v 1.1 2012-8-13 14:28:54 dhill Exp $
'''
Module for constuction of:

Bd->DstarTauNu
Bd->DTauNu
Bu->D0TauNu
Bs->DsTauNu
Bc->J/psiTauNu
Bd->D**0TauNu

==== Description of the lines ====

Bd->DstarTauNu, with Dstar -> pi (D0 -> K pi)
Bd->DTauNu, with D -> K pi pi
Bs->DsTauNu, with Ds -> K K pi
Bu->D0TauNu, with D0 -> K pi
Bc->JpsiTauNu, with Jpsi -> Mu Mu
Bd->D**0TauNu, with D**0 -> D*+ pi-

Tau -> 3pi in all cases

==== Description of the configuration ====

The selecton cuts are stored in the dictionary confdict.

The configuration class makes all the selections and the lines, when passed the cut dictionary.

The lines look like this:

1) Filter charm mesons (D*, D+, D0, Ds, J/psi)
2) Filter taus from StdLooseDetachedTau3pi common particle
5) Create B from charm meson and Tau

To look at all the configurable cuts, see StrippingB2XTauNu.confdict

==== How to use ====

To configure all lines, just instantiate the class:

all=B2XTauNuAllLinesConf("B2XTauNu",confdict)

Then to print all cuts do:

all.printCuts()

'''
__author__ = [ 'Donal Hill','Conor Fitzpatrick' ]
__date__ = '2012-8-23'
__version = '$Revision: 1.1 $'

#### Next is the dictionary of all tunable cuts ########

confdict={
    'Prescale_B0d2DstarTauNu'        : 1.0,
    'Prescale_B0d2DTauNu'            : 1.0,
    'Prescale_Bu2D0TauNu'            : 1.0,
    'Prescale_B0s2DsTauNu'           : 1.0,
    'Prescale_Bc2JpsiTauNu'          : 1.0,
    'Prescale_B0d2DdoubleStarTauNu'  : 1.0,
    'Postscale'   : 1.0 ,
    #B cuts
    'B_BPVDIRA'       : 0.999,
    #B combination cuts
    'B_DeltaM_low'       : -2579.0, #MeV
    'B_DeltaM_high'      : 300.0,   #MeV
    'B_DOCAMAX'          : 0.13,  #mm
    'B_upperDeltaM_low'  : 720.0,   #MeV
    'B_upperDeltaM_high' : 1721.0,  #MeV
    #D kaon cuts
    'D_K_PT'          : 150.0,  #MeV
    'Dplus_K_PT'      : 1500.0, #MeV
    'Ds_K_PT'         : 1500.0, #MeV
    'D_K_TRCHI2DOF'   : 3.0,
    'D_K_IPCHI2'      : 4.0,
    'D_K_PIDK'        : -5.0,
    'Dplus_K_PIDK'    : 0.0,
    'Dplus_K_TRPCHI2' : 0.1,
    #D pion cuts
    'D_Pi_PT'         : 150.0, # MeV
    'D_Pi_TRCHI2'     : 3.0,
    'D_Pi_IPCHI2'     : 4.0,
    'D_Pi_PIDK'       : 8.0,
    'Dplus_Pi_TRPCHI2' : 0.1,
    #D0-resonance parameters
    'D0_MassW'        : 40.0,      #MeV, 100 in StdLooseD0 
    'D0_BPVVDCHI2'    : 50.0,      #36 in StdLooseD0
    'D0_DIRA'         : 0.999,
    'D0_PT'           : 1600.0,    #MeV, 1.2 GeV in StdLooseD02KPi
    'D0_VCHI2'        : 10.0,      #10 in StdLooseD0
    #D0 combination cut
    'D0_DOCAMAX'      : 0.1,     #mm, 0.5 mm in StdLooseD0
    #D-resonance parameters
    'D_MassW'         : 40.0,      #MeV, 100 in StdLooseDplus
    'D_BPVVDCHI2'     : 50.0,      #36 in StdLooseDplus
    'Ds_BPVVDCHI2'    : 36.0,      #shorter lifetime than Dplus 
    'D_DIRA'          : 0.999,   #0.98 in StdLooseDplus
    'D_PT'            : 1600.0,    #MeV, 1.2 in StdLooseDplus
    'D_VCHI2'         : 10.0,      #30 in StdLooseDplus
    'D_MIPCHI2'       : 10.0,
    #Dstar-resonance parameters
    'slowPi_TRCHI2DOF' : 3.0,
    'slowPi_PT'        : 110.0,    #MeV
    'Dstar_PT'         : 1250.0,   #MeV, 1250 in StdLooseD*
    'Dstar_VCHI2'      : 10.0,     #MeV, 25 in StdLooseD*WithD02KPi 
    'Dstar_MassW'      : 50.0,     #MeV, 50 MeV in StdLooseD*WithD02KPi
    'Dstar_DeltaM_low'  : 135.0,
    'Dstar_DeltaM_high' : 160.0,   #MeV, 165.5 in StdLooseD*
    #Muon and J/psi cuts
    'Muon_TRCHI2DOF'    : 3.0,
    'Muon_PT'           : 1000.0,  #MeV
    'Jpsi_MassW'        : 80.0,    #MeV, 100 in StdLooseJpsi
    'Jpsi_VCHI2'        : 9.0,   #25 in StdLooseJpsi
    'Jpsi_PT'           : 2000.0,  #MeV
     #Cuts on single pions which are used to form the D_1(2420)0 from D* and pion
    'D1_Pi_IPCHI2' : 4.0,
    'D1_Pi_TRCHI2' : 3.0,
    'D1_Pi_PIDK'   : 8.0,
    #D1(2420) and D*2(2460) cuts
    'D1_VCHI2'         : 25.0,
    'D1_PT'            : 500.0,  #MeV
    'D1_DeltaM_low'    : 350.0,  #MeV
    'D1_DeltaM_high'   : 450.0,  #MeV
    'D1_MassW'         : 300.0,  #MeV
    #Ghost Prob cut on all tracks apart from slow pion
    'TRGHP'             : 0.4,
    #Slow pion ghost prob
    'TRGHP_slowPi'      : 0.6
    
    }


from StrippingUtils.Utils import LineBuilder

default_name="B2XTauNu"

class B2XTauNuAllLinesConf(LineBuilder):
    """
    Configuration object for B2XTauNu lines

    usage: config={...}
    B2XTauNuConf(name+LineSuffix, config)
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
   
    Dstar_KCut=''
    Dstar_PiCut=''
    Dstar_slowPiCut=''
    Dstar_D0Cut=''
    DstarCut=''
    totalDstarCut=''
    D_KCut=''
    D_Pi1Cut=''
    D_Pi2Cut=''
    DplusCut=''
    totalDplusCut=''
    Ds_K1Cut=''
    Ds_K2Cut=''
    Ds_PiCut=''
    DsCut=''
    totalDsCut=''
    D0Cut=''
    totalD0Cut=''
    MuonCut=''
    JpsiCut=''
    totalJpsiCut=''
    D1_PionCut=''
    D1Cut=''
    B0CombCut=''
    BplusCombCut=''
    BsCombCut=''
    BcCombCut=''
    BCut=''


    JpsiSel=None
    DstarSel=None
    DplusSel=None
    D0Sel=None
    DsSel=None
    
    B0d2DstarTauNuSel=None
    B0d2DTauNuSel=None
    B0s2DsTauNuSel=None
    Bu2D0TauNuSel=None
    Bc2JpsiTauNuSel=None

    D12DstarPiSel=None
    
    
    __configuration_keys__=[
        'Prescale_B0d2DstarTauNu',        
        'Prescale_B0d2DTauNu',            
        'Prescale_Bu2D0TauNu',            
        'Prescale_B0s2DsTauNu',          
        'Prescale_Bc2JpsiTauNu',          
        'Prescale_B0d2DdoubleStarTauNu',
        'Postscale',
        #B cuts
        'B_BPVDIRA',
        #B combi cuts
        'B_DeltaM_low',
        'B_DeltaM_high',
        'B_DOCAMAX',
        'B_upperDeltaM_low',
        'B_upperDeltaM_high',
        #kaon parameters
        'D_K_PT',
        'Dplus_K_PT',
        'Ds_K_PT',
        'D_K_TRCHI2DOF',
        'D_K_IPCHI2',
        'D_K_PIDK',
        'Dplus_K_PIDK',
        'Dplus_K_TRPCHI2',
        #pion parameters
        'D_Pi_PT',
        'D_Pi_TRCHI2',
        'D_Pi_IPCHI2',
        'D_Pi_PIDK',
        'Dplus_Pi_TRPCHI2',
        #D0-resonance parameters
        'D0_MassW',
        'D0_BPVVDCHI2',
        'D0_DIRA',
        'D0_PT',
        'D0_VCHI2',
        #D-resonance parameters
        'D_MassW',
        'D_BPVVDCHI2',
        'Ds_BPVVDCHI2',
        'D_DIRA',
        'D_PT',
        'D_VCHI2',
        'D_MIPCHI2',
        #D0 combination cut
        'D0_DOCAMAX',
        #Dstar-resonance parameters
        'slowPi_TRCHI2DOF',
        'slowPi_PT',
        'Dstar_PT',
        'Dstar_VCHI2',
        'Dstar_MassW',
        'Dstar_DeltaM_low',
        'Dstar_DeltaM_high',
        #Muon and J/psi cuts
        'Muon_TRCHI2DOF',
        'Muon_PT',
        'Jpsi_MassW',     
        'Jpsi_VCHI2',      
        'Jpsi_PT',
        #D1(2420): single pion cuts
        'D1_Pi_IPCHI2',
        'D1_Pi_TRCHI2',
        'D1_Pi_PIDK',
        #D1(2420) cuts
        'D1_VCHI2',
        'D1_PT',
        'D1_DeltaM_low',
        'D1_DeltaM_high',
        'D1_MassW',
        #Ghost prob cut on all tracks apart from slow pion
        'TRGHP',
        #Slow pion ghost prob
        'TRGHP_slowPi'
        ]
    
    __confdict__={}
    
    def __init__(self, name, config):
        '''The constructor of the configuration class.
        Requires a name which is added to the end of each algorithm name
        and a configuration dictionary, config, which must provide all the settings
        which differ between the lines'''
        LineBuilder.__init__(self, name, config)
        self.__confdict__=config


#/\/\/\/\/\/ SELECTION CUTS /\/\/\/\/\/\/\/\/\/

        # StdLoose D* has the following decay chain:  D*+ -> pi ( D0 -> K pi )

        # Cuts for kaon from D0
        self.Dstar_KCut  = "& CHILDCUT(CHILDCUT( (PT > %(D_K_PT)s*MeV) & "\
                     " (TRCHI2DOF < %(D_K_TRCHI2DOF)s ) & "\
                     " (MIPCHI2DV(PRIMARY)> %(D_K_IPCHI2)s ) & (TRGHP < %(TRGHP)s) & "\
                     " (PIDK >  %(D_K_PIDK)s),1),2) " % config

        
        #Cuts for pion from D0
        self.Dstar_PiCut = "& CHILDCUT(CHILDCUT( (PT> %(D_Pi_PT)s*MeV) & "\
                     " (TRCHI2DOF < %(D_Pi_TRCHI2)s) & (TRGHP < %(TRGHP)s) & "\
                     " (MIPCHI2DV(PRIMARY)> %(D_Pi_IPCHI2)s) & (PIDK < %(D_Pi_PIDK)s),2),2)" % config

        #Cuts for D0 from D*
        self.Dstar_D0Cut = "& CHILDCUT( (PT>%(D0_PT)s*MeV) & (ADMASS('D0') < %(D0_MassW)s *MeV ) & (BPVDIRA > %(D0_DIRA)s) & " \
                     " (BPVVDCHI2 > %(D0_BPVVDCHI2)s) & (VFASPF(VCHI2/VDOF)<%(D0_VCHI2)s),2)  "% config

        # Cuts for Dstar        
        self.Dstar_slowPiCut= "& CHILDCUT( (PT>%(slowPi_PT)s*MeV) & "\
                        " (TRCHI2DOF < %(slowPi_TRCHI2DOF)s) & (TRGHP < %(TRGHP_slowPi)s),1)" % config       

        
        self.DstarCut = " (VFASPF(VCHI2/VDOF) < %(Dstar_VCHI2)s ) & (M-MAXTREE('D0'==ABSID,M)>%(Dstar_DeltaM_low)s*MeV) & (M-MAXTREE('D0'==ABSID,M)<%(Dstar_DeltaM_high)s*MeV) &"\
                        " (PT>%(Dstar_PT)s *MeV) & ((ADMASS('D*(2010)+')< %(Dstar_MassW)s*MeV))" % config
        

        self.totalDstarCut = self.DstarCut + self.Dstar_KCut + self.Dstar_PiCut + self.Dstar_D0Cut + self.Dstar_slowPiCut

        
        #StdLooseDPlus has the following decay chain: D+ -> K pi pi

        #Cuts for kaon from D+ (also used as cuts for D0 -> K pi kaon)
        self.D_KCut  = "& CHILDCUT( ('K+'==ABSID) & (PT > %(Dplus_K_PT)s*MeV) & "\
                           " (TRCHI2DOF < %(D_K_TRCHI2DOF)s ) & (TRPCHI2 > %(Dplus_K_TRPCHI2)s) &"\
                           " (MIPCHI2DV(PRIMARY)> %(D_K_IPCHI2)s ) & (TRGHP < %(TRGHP)s) & "\
                           " (PIDK > %(Dplus_K_PIDK)s),1) " % config 
        
        #First daughter pion (also used as cuts for D0 -> K pi pion)
        self.D_Pi1Cut = "& CHILDCUT( ('pi+'==ABSID) & (PT> %(D_Pi_PT)s*MeV) & (TRPCHI2 > %(Dplus_Pi_TRPCHI2)s) &"\
                            " (TRCHI2DOF < %(D_Pi_TRCHI2)s) & (TRGHP < %(TRGHP)s) & "\
                            " (MIPCHI2DV(PRIMARY)> %(D_Pi_IPCHI2)s ) & (PIDK < %(D_Pi_PIDK)s),2)" % config

        #Second daughter pion
        self.D_Pi2Cut = "& CHILDCUT( ('pi+'==ABSID) & (PT> %(D_Pi_PT)s*MeV) & (TRPCHI2 > %(Dplus_Pi_TRPCHI2)s) & "\
                            " (TRCHI2DOF < %(D_Pi_TRCHI2)s) & (TRGHP < %(TRGHP)s) & "\
                            " (MIPCHI2DV(PRIMARY)> %(D_Pi_IPCHI2)s) & (PIDK < %(D_Pi_PIDK)s),3)" % config
   
        #D+ cuts
        self.DplusCut = "(PT>%(D_PT)s*MeV) & (ADMASS('D+') < %(D_MassW)s *MeV ) & (BPVDIRA > %(D_DIRA)s) & " \
                     " (BPVVDCHI2 > %(D_BPVVDCHI2)s) & (VFASPF(VCHI2/VDOF)<%(D_VCHI2)s) & (MIPCHI2DV(PRIMARY)> %(D_MIPCHI2)s)"% config

        self.totalDplusCut = self.DplusCut + self.D_KCut + self.D_Pi1Cut + self.D_Pi2Cut


        #Cuts for Ds 

        #First daughter kaon
        self.Ds_K1Cut  = "& CHILDCUT( ('K+'==ABSID) & (PT > %(Ds_K_PT)s*MeV) & "\
                           " (TRCHI2DOF < %(D_K_TRCHI2DOF)s ) &"\
                           " (MIPCHI2DV(PRIMARY)> %(D_K_IPCHI2)s ) & (TRGHP < %(TRGHP)s) & "\
                           " (PIDK > %(Dplus_K_PIDK)s),1) " % config 

        #Second daughter kaon
        self.Ds_K2Cut  = "& CHILDCUT( ('K+'==ABSID) & (PT > %(Ds_K_PT)s*MeV) & "\
                           " (TRCHI2DOF < %(D_K_TRCHI2DOF)s ) &"\
                           " (MIPCHI2DV(PRIMARY)> %(D_K_IPCHI2)s ) & (TRGHP < %(TRGHP)s) & "\
                           " (PIDK > %(Dplus_K_PIDK)s),2) " % config
        
        #Daughter pion
        self.Ds_PiCut = "& CHILDCUT( ('pi+'==ABSID) & (PT> %(D_Pi_PT)s*MeV) &"\
                            " (TRCHI2DOF < %(D_Pi_TRCHI2)s) & (TRGHP < %(TRGHP)s) & "\
                            " (MIPCHI2DV(PRIMARY)> %(D_Pi_IPCHI2)s ) & (PIDK < %(D_Pi_PIDK)s),3)" % config
        
        #Ds cuts
        self.DsCut = "(PT>%(D_PT)s*MeV) & (ADMASS('D_s+') < %(D_MassW)s *MeV ) & (BPVDIRA > %(D_DIRA)s) & " \
                     " (BPVVDCHI2 > %(Ds_BPVVDCHI2)s) & (VFASPF(VCHI2/VDOF)<%(D_VCHI2)s) & (MIPCHI2DV(PRIMARY)> %(D_MIPCHI2)s)"% config

        self.totalDsCut = self.DsCut + self.Ds_K1Cut + self.Ds_K2Cut + self.Ds_PiCut
        


        #StdLooseD02HH contains the decay : D0 -> K pi 

        
        #Cuts on D0
        self.D0Cut = "(PT>%(D0_PT)s*MeV) & (ADMASS('D0') < %(D0_MassW)s *MeV ) & (BPVDIRA > %(D0_DIRA)s) & " \
                     " (BPVVDCHI2 > %(D0_BPVVDCHI2)s) & (VFASPF(VCHI2/VDOF)<%(D0_VCHI2)s) & (MIPCHI2DV(PRIMARY)> %(D_MIPCHI2)s)"% config

        #Use cuts listed above for K and pi
        self.totalD0Cut = self.D0Cut + self.D_KCut + self.D_Pi1Cut



        #J/psi cuts for Bc mode
        self.MuonCut = " & (MINTREE('mu+'==ABSID,PT) > %(Muon_PT)s *MeV) & (MAXTREE('mu+'==ABSID,TRCHI2DOF) < %(Muon_TRCHI2DOF)s)" %config
        self.JpsiCut = "(ADMASS('J/psi(1S)') < %(Jpsi_MassW)s *MeV) & (VFASPF(VCHI2PDOF)< %(Jpsi_VCHI2)s) & (PT > %(Jpsi_PT)s *MeV)" %config

        self.totalJpsiCut = self.JpsiCut + self.MuonCut



        #Cuts for pion in D_1(2420)0 -> D*(2010)+ pi- and D*_2(2460)0 -> D*(2010)+ pi-
        self.D1_PionCut = "(MIPCHI2DV(PRIMARY)> %(D1_Pi_IPCHI2)s ) & (TRCHI2DOF < %(D1_Pi_TRCHI2)s) & (PIDK < %(D1_Pi_PIDK)s) & (TRGHP < %(TRGHP_slowPi)s)" %config

        #Cuts for D_1(2420)0 -> D*(2010)+ pi- and D*_2(2460)0 -> D*(2010)+ pi-
        self.D1Cut = " (VFASPF(VCHI2/VDOF) < %(D1_VCHI2)s ) & (M-MAXTREE('D*(2010)+'==ABSID,M)>%(D1_DeltaM_low)s*MeV) & (M-MAXTREE('D*(2010)+'==ABSID,M)<%(D1_DeltaM_high)s*MeV) &"\
                        " (PT>%(D1_PT)s *MeV) & ((ADMASS('D_1(2420)0')< %(D1_MassW)s*MeV))" % config

       
        #B combination cuts
        self.B0CombCut="(((DAMASS('B0') > %(B_DeltaM_low)s*MeV) & (DAMASS('B0') < %(B_DeltaM_high)s*MeV)) or ((DAMASS('B0') > %(B_upperDeltaM_low)s*MeV) & (DAMASS('B0') < %(B_upperDeltaM_high)s*MeV))) & (AMAXDOCA('') < %(B_DOCAMAX)s*mm)" %config

        self.BplusCombCut="(((DAMASS('B+') > %(B_DeltaM_low)s*MeV) & (DAMASS('B+') < %(B_DeltaM_high)s*MeV)) or ((DAMASS('B+') > %(B_upperDeltaM_low)s*MeV) & (DAMASS('B+') < %(B_upperDeltaM_high)s*MeV))) & (AMAXDOCA('') < %(B_DOCAMAX)s*mm)" %config

        self.BcCombCut="(((DAMASS('B_c+') > %(B_DeltaM_low)s*MeV) & (DAMASS('B_c+') < %(B_DeltaM_high)s*MeV)) or ((DAMASS('B_c+') > %(B_upperDeltaM_low)s*MeV) & (DAMASS('B_c+') < %(B_upperDeltaM_high)s*MeV))) & (AMAXDOCA('') < %(B_DOCAMAX)s*mm)" %config

        self.BsCombCut="(((DAMASS('B_s0') > %(B_DeltaM_low)s*MeV) & (DAMASS('B_s0') < %(B_DeltaM_high)s*MeV)) or ((DAMASS('B_s0') > %(B_upperDeltaM_low)s*MeV) & (DAMASS('B_s0') < %(B_upperDeltaM_high)s*MeV))) & (AMAXDOCA('') < %(B_DOCAMAX)s*mm)" %config

        #B cuts
        self.BCut = " (BPVDIRA > %(B_BPVDIRA)s)" %config


        ### Now make all the selections ###

        self.__FilterDstars__()
        self.__FilterDplus__()
        self.__FilterD0__()
        self.__FilterDs__()
        self.__FilterJpsi2MuMu__()
        
        self.__MakeB0d2DstarTauNu__()
        self.__MakeB0d2DstarTauNuWS__()
        self.__MakeB0d2DstarTauNuNonPhysTau__()
        self.__MakeB0d2DTauNu__()
        self.__MakeB0d2DTauNuWS__()
        self.__MakeB0d2DTauNuNonPhysTau__()
        self.__MakeBu2D0TauNu__()
        self.__MakeBu2D0TauNuWS__()
        self.__MakeBu2D0TauNuNonPhysTau__()
        self.__MakeBc2JpsiTauNu__()
        self.__MakeBc2JpsiTauNuNonPhysTau__()
        self.__MakeB0s2DsTauNu__()
        self.__MakeB0s2DsTauNuWS__()
        self.__MakeB0s2DsTauNuNonPhysTau__()

        self.__MakeDdoubleStar2DstarPi__()
        self.__MakeB0d2DdoubleStarTauNu__()
        self.__MakeB0d2DdoubleStarTauNuWS__()
        self.__MakeB0d2DdoubleStarTauNuNonPhysTau__()
        
        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import SelectionSequence


        ### Now make B stripping lines ###
        Bd2DstarTauNuLine=StrippingLine("Bd2DstarTauNuFor"+self._name,
                                        prescale = config['Prescale_B0d2DstarTauNu'],
                                        postscale = config['Postscale'],
                                        algos = [ self.B0d2DstarTauNuSel ]
                                        )
        self.registerLine(Bd2DstarTauNuLine)
        
        Bd2DstarTauNuWSLine=StrippingLine("Bd2DstarTauNuWSFor"+self._name,
                                          prescale = config['Prescale_B0d2DstarTauNu'],
                                          postscale = config['Postscale'],
                                          algos = [ self.B0d2DstarTauNuSelWS ]
                                          )
        self.registerLine(Bd2DstarTauNuWSLine)


        Bd2DstarTauNuNonPhysTauLine=StrippingLine("Bd2DstarTauNuNonPhysTauFor"+self._name,
                                          prescale = config['Prescale_B0d2DstarTauNu'],
                                          postscale = config['Postscale'],
                                          algos = [ self.B0d2DstarTauNuSelNonPhysTau ]
                                          )
        self.registerLine(Bd2DstarTauNuNonPhysTauLine)
        
        
        Bd2DTauNuLine=StrippingLine("Bd2DTauNuFor"+self._name,
                                    prescale = config['Prescale_B0d2DTauNu'],
                                    postscale = config['Postscale'],
                                    algos = [ self.B0d2DTauNuSel ]
                                    )
        self.registerLine(Bd2DTauNuLine)
        
        
        Bd2DTauNuWSLine=StrippingLine("Bd2DTauNuWSFor"+self._name,
                                      prescale = config['Prescale_B0d2DTauNu'],
                                      postscale = config['Postscale'],
                                      algos = [ self.B0d2DTauNuSelWS ]
                                      )
        self.registerLine(Bd2DTauNuWSLine)

        Bd2DTauNuNonPhysTauLine=StrippingLine("Bd2DTauNuNonPhysTauFor"+self._name,
                                      prescale = config['Prescale_B0d2DTauNu'],
                                      postscale = config['Postscale'],
                                      algos = [ self.B0d2DTauNuSelNonPhysTau ]
                                      )
        self.registerLine(Bd2DTauNuNonPhysTauLine)

        
        Bu2D0TauNuLine=StrippingLine("Bu2D0TauNuFor"+self._name,
                                     prescale = config['Prescale_Bu2D0TauNu'],
                                     postscale = config['Postscale'],
                                     algos = [ self.Bu2D0TauNuSel ]
                                     )
        self.registerLine(Bu2D0TauNuLine)
        
        Bu2D0TauNuWSLine=StrippingLine("Bu2D0TauNuWSFor"+self._name,
                                       prescale = config['Prescale_Bu2D0TauNu'],
                                       postscale = config['Postscale'],
                                       algos = [ self.Bu2D0TauNuSelWS ]
                                       )
        self.registerLine(Bu2D0TauNuWSLine)

        Bu2D0TauNuNonPhysTauLine=StrippingLine("Bu2D0TauNuNonPhysTauFor"+self._name,
                                       prescale = config['Prescale_Bu2D0TauNu'],
                                       postscale = config['Postscale'],
                                       algos = [ self.Bu2D0TauNuSelNonPhysTau ]
                                       )
        self.registerLine(Bu2D0TauNuNonPhysTauLine)
        
        
        Bc2JpsiTauNuLine=StrippingLine("Bc2JpsiTauNuFor"+self._name,
                                       prescale = config['Prescale_Bc2JpsiTauNu'],
                                       postscale = config['Postscale'],
                                       algos = [ self.Bc2JpsiTauNuSel ]
                                       )
        self.registerLine(Bc2JpsiTauNuLine)

        Bc2JpsiTauNuNonPhysTauLine=StrippingLine("Bc2JpsiTauNuNonPhysTauFor"+self._name,
                                       prescale = config['Prescale_Bc2JpsiTauNu'],
                                       postscale = config['Postscale'],
                                       algos = [ self.Bc2JpsiTauNuSelNonPhysTau ]
                                       )
        self.registerLine(Bc2JpsiTauNuNonPhysTauLine)
        
        
        Bs2DsTauNuLine=StrippingLine("Bs2DsTauNuFor"+self._name,
                                     prescale = config['Prescale_B0s2DsTauNu'],
                                     postscale = config['Postscale'],
                                     algos = [ self.B0s2DsTauNuSel ]
                                     )
        self.registerLine(Bs2DsTauNuLine)
        
        Bs2DsTauNuWSLine=StrippingLine("Bs2DsTauNuWSFor"+self._name,
                                       prescale = config['Prescale_B0s2DsTauNu'],
                                       postscale = config['Postscale'],
                                       algos = [ self.B0s2DsTauNuSelWS ]
                                       )
        self.registerLine(Bs2DsTauNuWSLine)

        Bs2DsTauNuNonPhysTauLine=StrippingLine("Bs2DsTauNuNonPhysTauFor"+self._name,
                                       prescale = config['Prescale_B0s2DsTauNu'],
                                       postscale = config['Postscale'],
                                       algos = [ self.B0s2DsTauNuSelNonPhysTau ]
                                       )
        self.registerLine(Bs2DsTauNuNonPhysTauLine)


        Bd2DdoubleStarTauNuLine=StrippingLine("Bd2DdoubleStarTauNuFor"+self._name,
                                        prescale = config['Prescale_B0d2DdoubleStarTauNu'],
                                        postscale = config['Postscale'],
                                        algos = [ self.B0d2DdoubleStarTauNuSel ]
                                        )
        self.registerLine(Bd2DdoubleStarTauNuLine)
        
        Bd2DdoubleStarTauNuWSLine=StrippingLine("Bd2DdoubleStarTauNuWSFor"+self._name,
                                          prescale = config['Prescale_B0d2DdoubleStarTauNu'],
                                          postscale = config['Postscale'],
                                          algos = [ self.B0d2DdoubleStarTauNuSelWS ]
                                          )
        self.registerLine(Bd2DdoubleStarTauNuWSLine)

        Bd2DdoubleStarTauNuNonPhysTauLine=StrippingLine("Bd2DdoubleStarTauNuNonPhysTauFor"+self._name,
                                          prescale = config['Prescale_B0d2DdoubleStarTauNu'],
                                          postscale = config['Postscale'],
                                          algos = [ self.B0d2DdoubleStarTauNuSelNonPhysTau ]
                                          )
        self.registerLine(Bd2DdoubleStarTauNuNonPhysTauLine)
        
        
        ### Collect them all together in a nice way ###
        self.Line=Bd2DstarTauNuLine
        self.Selections=[self.DstarSel, self.B0d2DstarTauNuSel]

        self.Line=Bd2DstarTauNuWSLine
        self.Selections=[self.DstarSel, self.B0d2DstarTauNuSelWS]

        self.Line=Bd2DstarTauNuNonPhysTauLine
        self.Selections=[self.DstarSel, self.B0d2DstarTauNuSelNonPhysTau]
        
        self.Line=Bd2DTauNuLine
        self.Selections=[self.DplusSel, self.B0d2DTauNuSel]

        self.Line=Bd2DTauNuWSLine
        self.Selections=[self.DplusSel, self.B0d2DTauNuSelWS]

        self.Line=Bd2DTauNuNonPhysTauLine
        self.Selections=[self.DplusSel, self.B0d2DTauNuSelNonPhysTau]
        

        self.Line=Bu2D0TauNuLine
        self.Selections=[self.D0Sel, self.Bu2D0TauNuSel]

        self.Line=Bu2D0TauNuWSLine
        self.Selections=[self.D0Sel, self.Bu2D0TauNuSelWS]

        self.Line=Bu2D0TauNuNonPhysTauLine
        self.Selections=[self.D0Sel, self.Bu2D0TauNuSelNonPhysTau]
        

        self.Line=Bc2JpsiTauNuLine
        self.Selections=[self.JpsiSel, self.Bc2JpsiTauNuSel]

        self.Line=Bc2JpsiTauNuNonPhysTauLine
        self.Selections=[self.JpsiSel, self.Bc2JpsiTauNuSelNonPhysTau]
        

        self.Line=Bs2DsTauNuLine
        self.Selections=[self.DsSel, self.B0s2DsTauNuSel]

        self.Line=Bs2DsTauNuWSLine
        self.Selections=[self.DsSel, self.B0s2DsTauNuSelWS]

        self.Line=Bs2DsTauNuNonPhysTauLine
        self.Selections=[self.DsSel, self.B0s2DsTauNuSelNonPhysTau]


        self.Line=Bd2DdoubleStarTauNuLine
        self.Selections=[self.DdoubleStar2DstarPiSel, self.B0d2DdoubleStarTauNuSel]

        self.Line=Bd2DdoubleStarTauNuWSLine
        self.Selections=[self.DdoubleStar2DstarPiSel, self.B0d2DdoubleStarTauNuSelWS]

        self.Line=Bd2DdoubleStarTauNuNonPhysTauLine
        self.Selections=[self.DdoubleStar2DstarPiSel, self.B0d2DdoubleStarTauNuSelNonPhysTau]

        
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


    def __FilterDplus__(self):

        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        DplusForB0d = FilterDesktop(
            Code = self.totalDplusCut
            )
        MyStdDplus = DataOnDemand(Location = 'Phys/StdLooseDplus2KPiPi/Particles')
        SelDplusForB0d = Selection("SelDplusFor"+self._name,
                                   Algorithm=DplusForB0d, RequiredSelections = [MyStdDplus])
        
        self.DplusSel=SelDplusForB0d


        
    def __FilterD0__(self):

        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        D0ForBu = FilterDesktop(
            Code = self.totalD0Cut
            )
        MyStdD0 = DataOnDemand(Location = 'Phys/StdLooseD02KPi/Particles')
        SelD0ForBu = Selection("SelD0For"+self._name,
                                   Algorithm=D0ForBu, RequiredSelections = [MyStdD0])
        
        self.D0Sel=SelD0ForBu


    def __FilterDs__(self):

        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand

        DsForB0s = FilterDesktop(
            Code = self.totalDsCut
            )
        MyStdDs = DataOnDemand(Location = 'Phys/StdLooseDsplus2KKPi/Particles')
        SelDsForB0s = Selection("SelDsFor"+self._name,
                                   Algorithm=DsForB0s, RequiredSelections = [MyStdDs])
        
        self.DsSel=SelDsForB0s


    def __FilterJpsi2MuMu__(self):


        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseJpsi2MuMu

        JpsiForBc = FilterDesktop(
            Code = self.totalJpsiCut
            )
        MyStdLooseJpsi2MuMu = DataOnDemand(Location = 'Phys/StdLooseJpsi2MuMu/Particles')
        SelJpsiForBc = Selection("SelJpsiFor"+self._name,
                                   Algorithm=JpsiForBc, RequiredSelections = [MyStdLooseJpsi2MuMu])
        
        self.JpsiSel=SelJpsiForBc


    def __MakeDdoubleStar2DstarPi__(self):
        """
        [D_1(2420)0 -> D*(2010)+ pi-]cc, [D*_2(2460)0 -> D*(2010)+ pi-]cc
        useful for understanding background in B0 -> D* tau
        """

        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from StandardParticles import StdLoosePions

        
        CombDdoubleStar2DstarPi = CombineParticles(        
            DecayDescriptors = ["[D_1(2420)0 -> D*(2010)+ pi-]cc", "[D*_2(2460)0 -> D*(2010)+ pi-]cc" ],
            DaughtersCuts = { "pi-" : self.D1_PionCut },
            MotherCut      = self.D1Cut,
            )
        
        SelDdoubleStar2DstarPi = Selection("SelDdoubleStar2DstarPi", Algorithm=CombDdoubleStar2DstarPi,
                                    RequiredSelections = [self.DstarSel,StdLoosePions])
        
        self.DdoubleStar2DstarPiSel=SelDdoubleStar2DstarPi
        
 
    def __MakeB0d2DstarTauNu__(self):
        """
        B0d selection:
        [B0 -> D*- tau+]cc
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBd2DstarTauNu = CombineParticles(        
            DecayDescriptors = ["[B0 -> D*(2010)- tau+]cc"],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelBd2DstarTauNu = Selection("SelBd2DstarTauNu", Algorithm=CombBd2DstarTauNu,
                                    RequiredSelections = [self.DstarSel,MyStdLooseDetachedTau])
        
        self.B0d2DstarTauNuSel=SelBd2DstarTauNu


    def __MakeB0d2DstarTauNuWS__(self):
        """
        B0d selection:
        [B0 -> D*+ tau+]cc
        use of wrong charge combination is useful for background subtraction
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBd2DstarTauNuWS = CombineParticles(        
            DecayDescriptors = ["[B0 -> D*(2010)+ tau+]cc"],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelBd2DstarTauNuWS = Selection("SelBd2DstarTauNuWS", Algorithm=CombBd2DstarTauNuWS,
                                    RequiredSelections = [self.DstarSel,MyStdLooseDetachedTau])
        
        self.B0d2DstarTauNuSelWS=SelBd2DstarTauNuWS



    def __MakeB0d2DstarTauNuNonPhysTau__(self):
        """
        B0d selection:
        [B0 -> D*- tau+++]cc, [B0 -> D*- tau---]cc
        non-physical tau+ -> pi+ pi+ pi+ which serves as an independent background source
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombBd2DstarTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B0 -> D*(2010)- tau+]cc", "[B0 -> D*(2010)+ tau+]cc"],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelBd2DstarTauNuNonPhysTau = Selection("SelBd2DstarTauNuNonPhysTau", Algorithm=CombBd2DstarTauNuNonPhysTau,
                                    RequiredSelections = [self.DstarSel,MyStdLooseDetachedTau])
        
        self.B0d2DstarTauNuSelNonPhysTau=SelBd2DstarTauNuNonPhysTau

        
    def __MakeB0d2DTauNu__(self):
        """
        B0d selection:
        [B0 -> D- tau+]cc
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBd2DTauNu = CombineParticles(        
            DecayDescriptors = ["[B0 -> D- tau+]cc"],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.999)" },
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelBd2DTauNu = Selection("SelBd2DTauNu", Algorithm=CombBd2DTauNu,
                                    RequiredSelections = [self.DplusSel,MyStdLooseDetachedTau])
        
        self.B0d2DTauNuSel=SelBd2DTauNu


            
    def __MakeB0d2DTauNuWS__(self):
        """
        B0d selection:
        [B0 -> D+ tau+]cc
        use of wrong charge combination is useful for background subtraction
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBd2DTauNuWS = CombineParticles(        
            DecayDescriptors = ["[B0 -> D+ tau+]cc" ],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.999)" },
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelBd2DTauNuWS = Selection("SelBd2DTauNuWS", Algorithm=CombBd2DTauNuWS,
                                    RequiredSelections = [self.DplusSel,MyStdLooseDetachedTau])
        
        self.B0d2DTauNuSelWS=SelBd2DTauNuWS


    def __MakeB0d2DTauNuNonPhysTau__(self):
        """
        B0d selection:
        [B0 -> D+ tau+++]cc, [B0 -> D+ tau---]cc 
        use of non-physical tau+++ is useful as an independent background source
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombBd2DTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B0 -> D- tau+]cc", "[B0 -> D+ tau+]cc"],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.999)" },
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelBd2DTauNuNonPhysTau = Selection("SelBd2DTauNuNonPhysTau", Algorithm=CombBd2DTauNuNonPhysTau,
                                    RequiredSelections = [self.DplusSel,MyStdLooseDetachedTau])
        
        self.B0d2DTauNuSelNonPhysTau=SelBd2DTauNuNonPhysTau



    def __MakeBu2D0TauNu__(self):
        """
        Bu selection:
        [B- -> D0 tau-]cc
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBu2D0TauNu = CombineParticles(        
            DecayDescriptors = ["[B- -> D0 tau-]cc"],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.999)" },
            CombinationCut = self.BplusCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBu2D0TauNu = Selection("SelBu2D0TauNu", Algorithm=CombBu2D0TauNu,
                                    RequiredSelections = [self.D0Sel,MyStdLooseDetachedTau])
        
        self.Bu2D0TauNuSel=SelBu2D0TauNu



    def __MakeBu2D0TauNuWS__(self):
        """
        Bu selection:
        [B- -> D0 tau+]cc,
        use of wrong charge combination is useful for background subtraction
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBu2D0TauNuWS = CombineParticles(        
            DecayDescriptors = ["[B- -> D0 tau+]cc"],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.999)" },
            CombinationCut = self.BplusCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBu2D0TauNuWS = Selection("SelBu2D0TauNuWS", Algorithm=CombBu2D0TauNuWS,
                                    RequiredSelections = [self.D0Sel,MyStdLooseDetachedTau])
        
        self.Bu2D0TauNuSelWS=SelBu2D0TauNuWS


    def __MakeBu2D0TauNuNonPhysTau__(self):
        """
        Bu selection:
        [B- -> D0 tau+++]cc, [B- -> D0 tau---]cc 
        use of non-physical tau+++ is useful as an independent background source
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombBu2D0TauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B- -> D0 tau+]cc", "[B- -> D0 tau-]cc" ],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.999)" },
            CombinationCut = self.BplusCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBu2D0TauNuNonPhysTau = Selection("SelBu2D0TauNuNonPhysTau", Algorithm=CombBu2D0TauNuNonPhysTau,
                                    RequiredSelections = [self.D0Sel,MyStdLooseDetachedTau])
        
        self.Bu2D0TauNuSelNonPhysTau=SelBu2D0TauNuNonPhysTau


        
    def __MakeBc2JpsiTauNu__(self):
        """
        Bc selection:
        [B_c+ -> J/psi(1S) tau+]cc
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBc2JpsiTauNu = CombineParticles(        
            DecayDescriptors = ["[B_c+ -> J/psi(1S) tau+]cc"],
            CombinationCut = self.BcCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBc2JpsiTauNu = Selection("SelBc2JpsiTauNu", Algorithm=CombBc2JpsiTauNu,
                                    RequiredSelections = [self.JpsiSel,MyStdLooseDetachedTau])
        
        self.Bc2JpsiTauNuSel=SelBc2JpsiTauNu


    def __MakeBc2JpsiTauNuNonPhysTau__(self):
        """
        Bc selection:
        [B_c+ -> J/psi(1S) tau+++]cc, [B_c+ -> J/psi(1S) tau---]cc
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombBc2JpsiTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B_c+ -> J/psi(1S) tau+]cc", "[B_c+ -> J/psi(1S) tau-]cc"],
            CombinationCut = self.BcCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBc2JpsiTauNuNonPhysTau = Selection("SelBc2JpsiTauNuNonPhysTau", Algorithm=CombBc2JpsiTauNuNonPhysTau,
                                    RequiredSelections = [self.JpsiSel,MyStdLooseDetachedTau])
        
        self.Bc2JpsiTauNuSelNonPhysTau=SelBc2JpsiTauNuNonPhysTau


    def __MakeB0s2DsTauNu__(self):
        """
        B0s selection:
        [B_s0 -> D_s- tau+]cc
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBs2DsTauNu = CombineParticles(        
            DecayDescriptors = ["[B_s0 -> D_s- tau+]cc"],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.999)" },
            CombinationCut = self.BsCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBs2DsTauNu = Selection("SelBs2DsTauNu", Algorithm=CombBs2DsTauNu,
                                    RequiredSelections = [self.DsSel,MyStdLooseDetachedTau])
        
        self.B0s2DsTauNuSel=SelBs2DsTauNu


            
    def __MakeB0s2DsTauNuWS__(self):
        """
        B0s selection:
        [B_s0 -> D_s+ tau+]cc
        use of wrong charge combination is useful for background subtraction
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBs2DsTauNuWS = CombineParticles(        
            DecayDescriptors = ["[B_s0 -> D_s+ tau+]cc" ],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.999)" },
            CombinationCut = self.BsCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBs2DsTauNuWS = Selection("SelBs2DsTauNuWS", Algorithm=CombBs2DsTauNuWS,
                                    RequiredSelections = [self.DsSel,MyStdLooseDetachedTau])
        
        self.B0s2DsTauNuSelWS=SelBs2DsTauNuWS


    def __MakeB0s2DsTauNuNonPhysTau__(self):
        """
        B0s selection:
        [B_s0 -> D_s+ tau+++]cc,  [B_s0 -> D_s+ tau---]cc
        use of non-physical tau+++ is useful as a background source
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombBs2DsTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B_s0 -> D_s+ tau+]cc", "[B_s0 -> D_s+ tau-]cc" ],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.999)" },
            CombinationCut = self.BsCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBs2DsTauNuNonPhysTau = Selection("SelBs2DsTauNuNonPhysTau", Algorithm=CombBs2DsTauNuNonPhysTau,
                                    RequiredSelections = [self.DsSel,MyStdLooseDetachedTau])
        
        self.B0s2DsTauNuSelNonPhysTau=SelBs2DsTauNuNonPhysTau


    def __MakeB0d2DdoubleStarTauNu__(self):
        """
        B0d selection:
        [B~0 -> (D_1(2420)0 -> D*+ pi-) tau-]cc, [B~0 -> (D*_2(2460)0 -> D*+ pi-) tau-]cc
        useful for studying background in B0 -> D* tau nu when pi- from D** decay is missed
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBd2DdoubleStarTauNu = CombineParticles(        
            DecayDescriptors = ["[B~0 -> D_1(2420)0 tau-]cc", "[B~0 -> D*_2(2460)0 tau-]cc" ],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelBd2DdoubleStarTauNu = Selection("SelBd2DdoubleStarTauNu", Algorithm=CombBd2DdoubleStarTauNu,
                                    RequiredSelections = [self.DdoubleStar2DstarPiSel,MyStdLooseDetachedTau])
        
        self.B0d2DdoubleStarTauNuSel=SelBd2DdoubleStarTauNu


    def __MakeB0d2DdoubleStarTauNuWS__(self):
        """
        B0d selection:
        [B~0 -> (D_1(2420)~0 -> D*- pi+) tau-]cc, [B~0 -> (D*_2(2460)~0 -> D*- pi+) tau-]cc
        useful for studying WS background in B0 -> D* tau nu when pi+ from D** decay is missed
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBd2DdoubleStarTauNuWS = CombineParticles(        
            DecayDescriptors = ["[B~0 -> D_1(2420)~0 tau-]cc", "[B~0 -> D*_2(2460)~0 tau-]cc" ],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelBd2DdoubleStarTauNuWS = Selection("SelBd2DdoubleStarTauNuWS", Algorithm=CombBd2DdoubleStarTauNuWS,
                                    RequiredSelections = [self.DdoubleStar2DstarPiSel,MyStdLooseDetachedTau])
        
        self.B0d2DdoubleStarTauNuSelWS=SelBd2DdoubleStarTauNuWS


    def __MakeB0d2DdoubleStarTauNuNonPhysTau__(self):
        """
        B0d selection:
        [B~0 -> (D_1(2420)~0 -> D*- pi+) tau---]cc, [B~0 -> (D*_2(2460)~0 -> D*- pi+) tau---]cc,
        [B~0 -> (D_1(2420)~0 -> D*- pi+) tau+++]cc, [B~0 -> (D*_2(2460)~0 -> D*- pi+) tau+++]cc
        useful for studying WS background in B0 -> D* tau nu when pi+ from D** decay is missed
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys, StdLooseDipion

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombBd2DdoubleStarTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B~0 -> D_1(2420)~0 tau-]cc", "[B~0 -> D*_2(2460)~0 tau-]cc", "[B~0 -> D_1(2420)~0 tau+]cc", "[B~0 -> D*_2(2460)~0 tau+]cc" ],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelBd2DdoubleStarTauNuNonPhysTau = Selection("SelBd2DdoubleStarTauNuNonPhysTau", Algorithm=CombBd2DdoubleStarTauNuNonPhysTau,
                                    RequiredSelections = [self.DdoubleStar2DstarPiSel,MyStdLooseDetachedTau])
        
        self.B0d2DdoubleStarTauNuSelNonPhysTau=SelBd2DdoubleStarTauNuNonPhysTau


    


    
        

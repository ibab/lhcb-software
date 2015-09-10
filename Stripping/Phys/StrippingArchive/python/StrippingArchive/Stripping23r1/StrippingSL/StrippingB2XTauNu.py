# $Id: StrippingB2XTauNu.py,v 1.1 2014-8-15 14:28:54 guwormse Exp $
'''
Module for constuction of:

B0d->DstarTauNu
B0d->DTauNu
Bu->D0TauNu
Bs->DsTauNu
Bc->J/psiTauNu
B0d->D**0TauNu
Lb->LcTauNu
==== Description of the lines ====

B0d->DstarTauNu, with Dstar -> pi (D0 -> K pi)
B0d->DTauNu, with D -> K pi pi
Bs->DsTauNu, with Ds -> K K pi
Bu->D0TauNu, with D0 -> K pi
Bc->JpsiTauNu, with Jpsi -> Mu Mu
B0d->D**0TauNu, with D**0 -> D*+ pi-
Lb->LcTauNu, with Lc -> p K pi
Tau -> 3pi in all cases
New selection using inverted vertex cut only
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
__author__ = [ 'Donal Hill','Conor Fitzpatrick','Guy Wormser' ]
__date__ = '2014-8-18'
__version = '$Revision: 1.3 $'

#### Next is the dictionary of all tunable cuts ########

default_config = {
  'B2XTauNuAllLines' : {
    'WGs'         : ['Semileptonic'],
    'BUILDERTYPE' : 'B2XTauNuAllLinesConf',
    'CONFIG' : {
        'Prescale_B0d2DstarTauNu'        : 1.0,
        'Prescale_B0d2DTauNu'            : 1.0,
        'Prescale_Bu2D0TauNu'            : 1.0,
        'Prescale_B0s2DsTauNu'           : 1.0,
        'Prescale_Bc2JpsiTauNu'          : 1.0,
        'Prescale_Lb2LcTauNu'            : 1.0,
        'Prescale_B0d2DdoubleStarTauNu'  : 1.0,
        'Prescale_NonPhys'               : 0.1,
        'Postscale'   : 1.0 ,
        #B cuts
        'B_BPVDIRA'       : 0.995 ,
        #B combination cuts
        'B_DeltaM_low'       : -2579.0, #MeV
        'B_DeltaM_high'      : 300.0,   #MeV
        'B_DOCAMAX'          : 0.15 , #mm
        'B_upperDeltaM_low'  : 720.0,   #MeV
        'B_upperDeltaM_high' : 1721.0,  #MeV
        #D kaon cuts
        'D_K_PT'          : 150.0,  #MeV
        'Dplus_K_PT'      : 1500.0, #MeV
        'Ds_K_PT'         : 1500.0, #MeV
        'D_K_TRCHI2DOF'   : 30.0,
        'D_K_IPCHI2'      : 10.0,
        'D_K_PIDK'        : -3 ,
        'Dplus_K_PIDK'    : 3,
        'Dplus_K_TRPCHI2' : 0.1,
        #D pion cuts
        'D_Pi_PT'         : 150.0, # MeV
        'D_Pi_TRCHI2'     : 3.0,
        'D_Pi_IPCHI2'     : 10.,
        'D_Pi_PIDK'       : 50.0,
        'Dplus_Pi_TRPCHI2' : 0.1,
        #Lc kaon cuts
        'Lc_K_PT'          : 150.0,  #MeV
        'Lc_K_TRCHI2DOF'   : 3.0,
        'Lc_K_IPCHI2'      : 10.0,
        'Lc_K_PIDK'        : 3.0,    
        'Lc_K_TRPCHI2' : 0.01,
        #Lc pion cuts
        'Lc_Pi_PT'         : 150.0, # MeV
        'Lc_Pi_TRCHI2'     : 3.0,
        'Lc_Pi_IPCHI2'     : 10.0,
        'Lc_Pi_PIDK'       : 50.0,
        'Lc_Pi_TRPCHI2' : 0.01,
        #Lc proton cuts
        'Lc_p_PT'          : 150.0,  #MeV
        'Lc_p_TRCHI2DOF'   : 3.0,
        'Lc_p_IPCHI2'      : 10.0,
        'Lc_p_PIDp'        : 5.0,
        'Lc_p_TRPCHI2' :0.01,
        
        
        #D0-resonance parameters
        'D0_MassW'        : 40.0,      #MeV, 100 in StdLooseD0 
        'D0_BPVVDCHI2'    : 36.0,      #36 in StdLooseD0
        'D0_DIRA'         : 0.995,
        'D0_PT'           : 1200.0,    #MeV, 1.2 GeV in StdLooseD02KPi
        'D0_VCHI2'        : 10.0,      #10 in StdLooseD0
        #D0 combination cut
        'D0_DOCAMAX'      : 0.5,     #mm, 0.5 mm in StdLooseD0
        #D-resonance parameters
        'D_MassW'         : 40.0,      #MeV, 100 in StdLooseDplus
        'D_BPVVDCHI2'     : 50.0,      #36 in StdLooseDplus
        'Ds_BPVVDCHI2'    : 36.0,      #shorter lifetime than Dplus 
        'D_DIRA'          : 0.995,   #0.98 in StdLooseDplus
        'D_PT'            : 1600.0,    #MeV, 1.2 in StdLooseDplus
        'D_VCHI2'         : 10.0,      #30 in StdLooseDplus
        'D_MIPCHI2'       : 10.0,
        #Dstar-resonance parameters
        'slowPi_TRCHI2DOF' : 30.0,
        'slowPi_PT'        : 50.0,    #MeV
        'Dstar_PT'         : 1250.0,   #MeV, 1250 in StdLooseD*
        'Dstar_VCHI2'      : 25.0,     #MeV, 25 in StdLooseD*WithD02KPi 
        'Dstar_MassW'      : 50.0,     #MeV, 50 MeV in StdLooseD*WithD02KPi
        'Dstar_DeltaM_low'  : 135.0,  #MeV
        'Dstar_DeltaM_high' : 160.0,   #MeV, 165.5 in StdLooseD*
        
        #Lc-resonance parameters
        'Lc_MassW'         : 30.0,      #MeV, 100 in StdLooseDplus
        'Lc_BPVVDCHI2'     : 50.0,      #36 in StdLooseDplus
        'Lc_DIRA'          : 0.995,   #0.98 in StdLooseDplus
        'Lc_PT'            : 1200.0,    #MeV, 1.2 in StdLooseDplus
        'Lc_VCHI2'         : 10.0,      #30 in StdLooseDplus
        'Lc_MIPCHI2'       : 10.0,
        # Lb Inverted vertex Cuts
        'LbInvVertD'       : 1. ,         #mm
        #Dstar Inverted cut
        'DInvVertD'        : 1. ,         #mm
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
        'TRGHP_slowPi'      : 0.6,
        #HLT1 trigger
        'TisTosSpecs'       : { 'Hlt1.*Decision%TOS' : 0 }    
        },
    'STREAMS' : ['BhadronCompleteEvent']
    }
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
    Lc_KCut=''
    Lc_PiCut=''
    Lc_pCut=''
    LcCut=''
    LcLightCut=''
    Lc_pLightCut=''
    Lc_KLightCut=''
    DLightCut=''
    DLight_partCut=''
    totalLcCut=''
    LcInvVertCut=''
    totalLcInvVertCut=''
    DInvVertCut=''
    totalDInvVertCut=''
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
    LbCombCut=''
    BCut=''
    LbCombInvVertCut=''
    B0dCombInvVertCut=''
    BInvVertCut=''


    JpsiSel=None
    DstarSel=None
    DplusSel=None
    D0Sel=None
    DsSel=None
    LcSel=None
    
    B0d2DstarTauNuSel=None
    B0d2DTauNuSel=None
    B0s2DsTauNuSel=None
    Bu2D0TauNuSel=None
    Bc2JpsiTauNuSel=None
    Lb2LcTauNuSel=None
    D12DstarPiSel=None
    
    
    __configuration_keys__=[
        'Prescale_B0d2DstarTauNu',        
        'Prescale_B0d2DTauNu',            
        'Prescale_Bu2D0TauNu',            
        'Prescale_B0s2DsTauNu',          
        'Prescale_Bc2JpsiTauNu',
        'Prescale_Lb2LcTauNu',
        'Prescale_B0d2DdoubleStarTauNu',
        'Prescale_NonPhys',
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
            #Lc kaon cuts
    'Lc_K_PT'     ,
    'Lc_K_TRCHI2DOF' ,
    'Lc_K_IPCHI2'     ,
    'Lc_K_PIDK'       ,    
    'Lc_K_TRPCHI2' ,
        #Lc pion cuts
    'Lc_Pi_PT'         ,
    'Lc_Pi_TRCHI2'     ,
    'Lc_Pi_IPCHI2'     ,
    'Lc_Pi_PIDK'       ,
    'Lc_Pi_TRPCHI2' ,
      #Lc proton cuts
    'Lc_p_PT'          , 
    'Lc_p_TRCHI2DOF'   ,
    'Lc_p_IPCHI2'      ,
    'Lc_p_PIDp'        ,
    'Lc_p_TRPCHI2' ,
         #Lc-resonance parameters
    'Lc_MassW',
    'Lc_BPVVDCHI2' ,      
    'Lc_DIRA'  ,       
    'Lc_PT'    ,       
    'Lc_VCHI2'  ,      
    'Lc_MIPCHI2' ,
           # Inverted Vertex cuts
    'LbInvVertD',
    'DInvVertD',
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
        'TRGHP_slowPi',
        #HLT1 decision
        'TisTosSpecs'
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
        
        

        self.totalDstarCut = self.DstarCut + self.Dstar_KCut + self.Dstar_PiCut + self.Dstar_D0Cut+ self.Dstar_slowPiCut

        
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

#StdLooseLc has the following decay chain: Lc+ -> p K pi

        #Cuts for kaon from Lc (also used as cuts for D0 -> K pi kaon)
        self.Lc_KCut  = "& INTREE( ('K-'==ABSID) & (PT > %(Lc_K_PT)s*MeV) & "\
                           " (TRCHI2DOF < %(Lc_K_TRCHI2DOF)s ) & (TRPCHI2 > %(Lc_K_TRPCHI2)s) &"\
                           " (MIPCHI2DV(PRIMARY)> %(Lc_K_IPCHI2)s ) & (TRGHP < %(TRGHP)s) & "\
                           " (PIDK > %(Lc_K_PIDK)s)) " % config 
        
        # pion (also used as cuts for D0 -> K pi pion)
        self.Lc_PiCut = "& INTREE ( ('pi+'==ABSID) & (PT> %(Lc_Pi_PT)s*MeV) & (TRPCHI2 > %(Lc_Pi_TRPCHI2)s) &"\
                            " (TRCHI2DOF < %(Lc_Pi_TRCHI2)s) & (TRGHP < %(TRGHP)s) & "\
                            " (MIPCHI2DV(PRIMARY)> %(Lc_Pi_IPCHI2)s ) & (PIDK < %(Lc_Pi_PIDK)s))" % config

        #proton
        self.Lc_pCut = "& INTREE( ('p+'==ABSID) & (PT> %(Lc_p_PT)s*MeV) & (TRPCHI2 > %(Lc_p_TRPCHI2)s) & "\
                            " (TRCHI2DOF < %(Lc_p_TRCHI2DOF)s) & (TRGHP < %(TRGHP)s) & "\
                            " (MIPCHI2DV(PRIMARY)> %(Lc_p_IPCHI2)s) & (PIDp > %(Lc_p_PIDp)s))" % config
   
        #Lc cuts
        self.LcCut = "(PT>%(Lc_PT)s*MeV) & (ADMASS('Lambda_c+') < %(Lc_MassW)s *MeV )& (BPVDIRA > %(Lc_DIRA)s) & (BPVVDCHI2 > %(Lc_BPVVDCHI2)s) & (VFASPF(VCHI2/VDOF)<%(Lc_VCHI2)s) & (MIPCHI2DV(PRIMARY)> %(Lc_MIPCHI2)s)"% config
         
        self.totalLcCut = self.LcCut  + self.Lc_pCut + self.Lc_PiCut + self.Lc_KCut 

#Lc cuts for Invertex vertex selection
 
       
        self.LcLightCut = " (ADMASS('Lambda_c+') < %(Lc_MassW)s *MeV )&  (BPVVDCHI2 > %(Lc_BPVVDCHI2)s) & (VFASPF(VCHI2/VDOF)<%(Lc_VCHI2)s)& (MIPCHI2DV(PRIMARY)> %(Lc_MIPCHI2)s) "% config
        self.Lc_pLightcut="& INTREE( ('p+'==ABSID)& (TRCHI2DOF < %(Lc_p_TRCHI2DOF)s) & (TRGHP < %(TRGHP)s) & (PIDp > %(Lc_p_PIDp)s))" % config
        self.Lc_KLightcut = "& INTREE( ('K-'==ABSID) & (TRCHI2DOF < %(Lc_K_TRCHI2DOF)s ) & (TRGHP < %(TRGHP)s) & (PIDK > %(Lc_K_PIDK)s)) " % config
        

        self.totalLcInvVertCut=  self.LcLightCut + self.Lc_pCut+self.Lc_KCut

        #Dstar cuts for Invertex vertex selection
 
       
        self.DLightCut = "  (M-MAXTREE('D0'==ABSID,M)>%(Dstar_DeltaM_low)s*MeV) & (M-MAXTREE('D0'==ABSID,M)<%(Dstar_DeltaM_high)s*MeV) & (VFASPF(VCHI2/VDOF)<%(D0_VCHI2)s) & CHILDCUT(  (ADMASS('D0') < %(D0_MassW)s *MeV ),2)  "% config
       
        self.DLight_partCut="& INTREE( ('K-'==ABSID)& (TRCHI2DOF < %(D_K_TRCHI2DOF)s) & (TRGHP < %(TRGHP)s)) &INTREE( ('pi+'==ABSID)& (TRCHI2DOF < %(D_Pi_TRCHI2)s) & (TRGHP < %(TRGHP)s)) " % config
        self.totalDInvVertCut=  self.DLightCut +self.DLight_partCut
        
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
        self.B0CombCut="(((DAMASS('B0') > %(B_DeltaM_low)s*MeV) & (DAMASS('B0') < %(B_DeltaM_high)s*MeV)) or ((DAMASS('B0') > %(B_upperDeltaM_low)s*MeV) & (DAMASS('B0') < %(B_upperDeltaM_high)s*MeV))) & (AMAXDOCA('',0) < %(B_DOCAMAX)s*mm)" %config

        self.BplusCombCut="(((DAMASS('B+') > %(B_DeltaM_low)s*MeV) & (DAMASS('B+') < %(B_DeltaM_high)s*MeV)) or ((DAMASS('B+') > %(B_upperDeltaM_low)s*MeV) & (DAMASS('B+') < %(B_upperDeltaM_high)s*MeV))) & (AMAXDOCA('',0) < %(B_DOCAMAX)s*mm)" %config

        self.BcCombCut="(((DAMASS('B_c+') > %(B_DeltaM_low)s*MeV) & (DAMASS('B_c+') < %(B_DeltaM_high)s*MeV)) or ((DAMASS('B_c+') > %(B_upperDeltaM_low)s*MeV) & (DAMASS('B_c+') < %(B_upperDeltaM_high)s*MeV))) & (AMAXDOCA('',0) < %(B_DOCAMAX)s*mm)" %config

        self.BsCombCut="(((DAMASS('B_s0') > %(B_DeltaM_low)s*MeV) & (DAMASS('B_s0') < %(B_DeltaM_high)s*MeV)) or ((DAMASS('B_s0') > %(B_upperDeltaM_low)s*MeV) & (DAMASS('B_s0') < %(B_upperDeltaM_high)s*MeV))) & (AMAXDOCA('',0) < %(B_DOCAMAX)s*mm)" %config

        self.LbCombCut="(DAMASS('Lambda_b0') < %(B_DeltaM_high)s*MeV)  & (AMAXDOCA('') < %(B_DOCAMAX)s*mm)" %config
        self.LbCombInvVertCut="(ACHILD(VFASPF(VZ),2) - ACHILD(VFASPF(VZ),1) >  %(LbInvVertD)s *mm)& (ACHILD(VFASPF(VZ),2) - ACHILD(VFASPF(VZ),1) <50. ) & (DAMASS('Lambda_b0') < %(B_DeltaM_high)s*MeV)" %config
        self.B0dCombInvVertCut="(ACHILD(VFASPF(VZ),2) - ACHILD(VFASPF(VZ),1) >  %(DInvVertD)s *mm)&(ACHILD(VFASPF(VZ),2) - ACHILD(VFASPF(VZ),1) <50.) & (DAMASS('B0') < %(B_DeltaM_high)s*MeV)" %config
        #B cuts
        self.BCut = " (BPVDIRA > %(B_BPVDIRA)s)" %config
       
        self.BInvVertCut = " (BPVDIRA >%(B_BPVDIRA)s)" %config

        ### Now make all the selections ###

        #self.filterTisTos()
        self.__FilterDstars__()
        self.__FilterDplus__()
        self.__FilterD0__()
        self.__FilterDs__()
        self.__FilterLc__()
        self.__FilterLcInvVert__()
        self.__FilterDInvVert__()
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
        self.__MakeLb2LcTauNu__()
        self.__MakeLb2LcTauNuWS__()
        self.__MakeLb2LcTauNuNonPhysTau__()
        self.__MakeLb2LcTauNuInvVert__()
        self.__MakeB0d2DstarTauNuInvVert__()
        self.__MakeDdoubleStar2DstarPi__()
        self.__MakeB0d2DdoubleStarTauNu__()
        self.__MakeB0d2DdoubleStarTauNuWS__()
        self.__MakeB0d2DdoubleStarTauNuNonPhysTau__()
        
        from StrippingConf.StrippingLine import StrippingLine
        from PhysSelPython.Wrappers import SelectionSequence

        
        #TisTos filter
        def filterTisTos(name,Input,myTisTosSpecs):
            
            from Configurables import TisTosParticleTagger
            from PhysSelPython.Wrappers import Selection
            myTagger = TisTosParticleTagger(name + "_TisTosTagger")
            myTagger.TisTosSpecs = myTisTosSpecs
            
            # To speed it up, TisTos only with tracking system
            myTagger.ProjectTracksToCalo = False
            myTagger.CaloClustForCharged = False
            myTagger.CaloClustForNeutral = False
            myTagger.TOSFrac = { 4:0.0, 5:0.0 }
            
            return Selection(name + "_SelTisTos",
                             Algorithm = myTagger,
                             RequiredSelections = [ Input ] )


        ##TisTos definition
        self.TOSB0d2DstarTauNu = filterTisTos("TOSB0d2DstarTauNu",
                                             Input = self.B0d2DstarTauNuSel,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )

        ### Now make B stripping line ###
        B0d2DstarTauNuLine=StrippingLine("B0d2DstarTauNuFor"+self._name,
                                        prescale = config['Prescale_B0d2DstarTauNu'],
                                        postscale = config['Postscale'],
                                        algos = [ self.TOSB0d2DstarTauNu ]
                                        )
        self.registerLine(B0d2DstarTauNuLine)
        

        self.TOSB0d2DstarTauNuWS = filterTisTos("TOSB0d2DstarTauNuWS",
                                             Input = self.B0d2DstarTauNuSelWS,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        B0d2DstarTauNuWSLine=StrippingLine("B0d2DstarTauNuWSFor"+self._name,
                                          prescale = config['Prescale_B0d2DstarTauNu'],
                                          postscale = config['Postscale'],
                                          algos = [ self.TOSB0d2DstarTauNuWS ]
                                          )
        self.registerLine(B0d2DstarTauNuWSLine)

        self.TOSB0d2DstarTauNuNonPhys = filterTisTos("TOSB0d2DstarTauNuNonPhys",
                                             Input = self.B0d2DstarTauNuSelNonPhysTau,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )

        B0d2DstarTauNuNonPhysTauLine=StrippingLine("B0d2DstarTauNuNonPhysTauFor"+self._name,
                                          prescale = config['Prescale_NonPhys'],
                                          postscale = config['Postscale'],
                                          algos = [ self.TOSB0d2DstarTauNuNonPhys ]
                                          )
        self.registerLine(B0d2DstarTauNuNonPhysTauLine)

        

        self.TOSB0d2DTauNu = filterTisTos("TOSB0d2DTauNu",
                                             Input = self.B0d2DTauNuSel,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        B0d2DTauNuLine=StrippingLine("B0d2DTauNuFor"+self._name,
                                    prescale = config['Prescale_B0d2DTauNu'],
                                    postscale = config['Postscale'],
                                    algos = [ self.TOSB0d2DTauNu ]
                                    )
        self.registerLine(B0d2DTauNuLine)

        self.TOSB0d2DTauNuWS = filterTisTos("TOSB0d2DTauNuWS",
                                             Input = self.B0d2DTauNuSelWS,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        B0d2DTauNuWSLine=StrippingLine("B0d2DTauNuWSFor"+self._name,
                                      prescale = config['Prescale_B0d2DTauNu'],
                                      postscale = config['Postscale'],
                                      algos = [ self.TOSB0d2DTauNuWS ]
                                      )
        self.registerLine(B0d2DTauNuWSLine)

        self.TOSB0d2DTauNuNonPhys = filterTisTos("TOSB0d2DTauNuNonPhys",
                                             Input = self.B0d2DTauNuSelNonPhysTau,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )

        B0d2DTauNuNonPhysTauLine=StrippingLine("B0d2DTauNuNonPhysTauFor"+self._name,
                                      prescale = config['Prescale_NonPhys'],
                                      postscale = config['Postscale'],
                                      algos = [ self.TOSB0d2DTauNuNonPhys ]
                                      )
        self.registerLine(B0d2DTauNuNonPhysTauLine)

        self.TOSBu2D0TauNu = filterTisTos("TOSBu2D0TauNu",
                                             Input = self.Bu2D0TauNuSel,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        Bu2D0TauNuLine=StrippingLine("Bu2D0TauNuFor"+self._name,
                                     prescale = config['Prescale_Bu2D0TauNu'],
                                     postscale = config['Postscale'],
                                     algos = [ self.TOSBu2D0TauNu ]
                                     )
        self.registerLine(Bu2D0TauNuLine)

        self.TOSBu2D0TauNuWS = filterTisTos("TOSBu2D0TauNuWS",
                                             Input = self.Bu2D0TauNuSelWS,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        Bu2D0TauNuWSLine=StrippingLine("Bu2D0TauNuWSFor"+self._name,
                                       prescale = config['Prescale_Bu2D0TauNu'],
                                       postscale = config['Postscale'],
                                       algos = [ self.TOSBu2D0TauNuWS ]
                                       )
        self.registerLine(Bu2D0TauNuWSLine)

        self.TOSBu2D0TauNuNonPhys = filterTisTos("TOSBu2D0TauNuNonPhys",
                                             Input = self.Bu2D0TauNuSelNonPhysTau,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )

        Bu2D0TauNuNonPhysTauLine=StrippingLine("Bu2D0TauNuNonPhysTauFor"+self._name,
                                       prescale = config['Prescale_NonPhys'],
                                       postscale = config['Postscale'],
                                       algos = [ self.TOSBu2D0TauNuNonPhys ]
                                       )
        self.registerLine(Bu2D0TauNuNonPhysTauLine)

        self.TOSBc2JpsiTauNu = filterTisTos("TOSBc2JpsiTauNu",
                                             Input = self.Bc2JpsiTauNuSel,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        Bc2JpsiTauNuLine=StrippingLine("Bc2JpsiTauNuFor"+self._name,
                                       prescale = config['Prescale_Bc2JpsiTauNu'],
                                       postscale = config['Postscale'],
                                       algos = [ self.TOSBc2JpsiTauNu ]
                                       )
        self.registerLine(Bc2JpsiTauNuLine)

        self.TOSBc2JpsiTauNuNonPhys = filterTisTos("TOSBc2JpsiTauNuNonPhys",
                                             Input = self.Bc2JpsiTauNuSelNonPhysTau,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )

        Bc2JpsiTauNuNonPhysTauLine=StrippingLine("Bc2JpsiTauNuNonPhysTauFor"+self._name,
                                       prescale = config['Prescale_NonPhys'],
                                       postscale = config['Postscale'],
                                       algos = [ self.TOSBc2JpsiTauNuNonPhys ]
                                       )
        self.registerLine(Bc2JpsiTauNuNonPhysTauLine)

        self.TOSLb2LcTauNu = filterTisTos("TOSLb2LcTauNu",
                                             Input = self.Lb2LcTauNuSel,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        Lb2LcTauNuLine=StrippingLine("Lb2LcTauNuFor"+self._name,
                                     prescale = config['Prescale_Lb2LcTauNu'],
                                     postscale = config['Postscale'],
                                     algos = [ self.TOSLb2LcTauNu ]
                                     )
        self.registerLine(Lb2LcTauNuLine)

        self.TOSLb2LcTauNuWS = filterTisTos("TOSLb2LcTauNuWS",
                                             Input = self.Lb2LcTauNuSelWS,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        Lb2LcTauNuWSLine=StrippingLine("Lb2LcTauNuWSFor"+self._name,
                                       prescale = config['Prescale_Lb2LcTauNu'],
                                       postscale = config['Postscale'],
                                       algos = [ self.TOSLb2LcTauNuWS ]
                                       )
        self.registerLine(Lb2LcTauNuWSLine)

        self.TOSLb2LcTauNuNonPhys = filterTisTos("TOSLb2LcTauNuNonPhys",
                                             Input = self.Lb2LcTauNuSelNonPhysTau,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )

        Lb2LcTauNuNonPhysTauLine=StrippingLine("Lb2LcTauNuNonPhysTauFor"+self._name,
                                       prescale = config['Prescale_NonPhys'],
                                       postscale = config['Postscale'],
                                       algos = [ self.TOSLb2LcTauNuNonPhys ]
                                       )
        self.registerLine(Lb2LcTauNuNonPhysTauLine)
        
        self.TOSLb2LcTauNuInvVert = filterTisTos("TOSLb2LcTauNuInvVert",
                                             Input = self.Lb2LcTauNuSelInvVert,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        Lb2LcTauNuInvVertLine=StrippingLine("Lb2LcTauNuInvVertFor"+self._name,
                                     prescale = config['Prescale_Lb2LcTauNu'],
                                     postscale = config['Postscale'],
                                     algos = [ self.TOSLb2LcTauNuInvVert ]
                                     )
        self.registerLine(Lb2LcTauNuInvVertLine)

        self.TOSB0d2DstarTauNuInvVert = filterTisTos("TOSB0d2DstarTauNuInvVert",
                                             Input = self.B0d2DstarTauNuSelInvVert,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        B0d2DstarTauNuInvVertLine=StrippingLine("B0d2DstarTauNuInvVertFor"+self._name,
                                     prescale = config['Prescale_B0d2DstarTauNu'],
                                     postscale = config['Postscale'],
                                     algos = [ self.TOSB0d2DstarTauNuInvVert ]
                                     )
        self.registerLine(B0d2DstarTauNuInvVertLine)
        
        self.TOSBs2DsTauNu = filterTisTos("TOSBs2DsTauNu",
                                             Input = self.B0s2DsTauNuSel,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        Bs2DsTauNuLine=StrippingLine("Bs2DsTauNuFor"+self._name,
                                     prescale = config['Prescale_B0s2DsTauNu'],
                                     postscale = config['Postscale'],
                                     algos = [ self.TOSBs2DsTauNu ]
                                     )
        self.registerLine(Bs2DsTauNuLine)

        self.TOSBs2DsTauNuWS = filterTisTos("TOSBs2DsTauNuWS",
                                             Input = self.B0s2DsTauNuSelWS,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        Bs2DsTauNuWSLine=StrippingLine("Bs2DsTauNuWSFor"+self._name,
                                       prescale = config['Prescale_B0s2DsTauNu'],
                                       postscale = config['Postscale'],
                                       algos = [ self.TOSBs2DsTauNuWS ]
                                       )
        self.registerLine(Bs2DsTauNuWSLine)

        self.TOSBs2DsTauNuNonPhys = filterTisTos("TOSBs2DsTauNuNonPhys",
                                             Input = self.B0s2DsTauNuSelNonPhysTau,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )

        Bs2DsTauNuNonPhysTauLine=StrippingLine("Bs2DsTauNuNonPhysTauFor"+self._name,
                                       prescale = config['Prescale_NonPhys'],
                                       postscale = config['Postscale'],
                                       algos = [ self.TOSBs2DsTauNuNonPhys ]
                                       )
        self.registerLine(Bs2DsTauNuNonPhysTauLine)


        

        self.TOSB0d2DdoubleStarTauNu = filterTisTos("TOSB0d2DdoubleStarTauNu",
                                             Input = self.B0d2DdoubleStarTauNuSel,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )

        B0d2DdoubleStarTauNuLine=StrippingLine("B0d2DdoubleStarTauNuFor"+self._name,
                                        prescale = config['Prescale_B0d2DdoubleStarTauNu'],
                                        postscale = config['Postscale'],
                                        algos = [ self.TOSB0d2DdoubleStarTauNu ]
                                        )
        self.registerLine(B0d2DdoubleStarTauNuLine)
        
        self.TOSB0d2DdoubleStarTauNuWS = filterTisTos("TOSB0d2DdoubleStarTauNuWS",
                                             Input = self.B0d2DdoubleStarTauNuSelWS,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )
        
        B0d2DdoubleStarTauNuWSLine=StrippingLine("B0d2DdoubleStarTauNuWSFor"+self._name,
                                          prescale = config['Prescale_B0d2DdoubleStarTauNu'],
                                          postscale = config['Postscale'],
                                          algos = [ self.TOSB0d2DdoubleStarTauNuWS ]
                                          )
        self.registerLine(B0d2DdoubleStarTauNuWSLine)

        self.TOSB0d2DdoubleStarTauNuNonPhys = filterTisTos("TOSB0d2DdoubleStarTauNuNonPhys",
                                             Input = self.B0d2DdoubleStarTauNuSelNonPhysTau,
                                             myTisTosSpecs = config['TisTosSpecs']
                                             )

        B0d2DdoubleStarTauNuNonPhysTauLine=StrippingLine("B0d2DdoubleStarTauNuNonPhysTauFor"+self._name,
                                          prescale = config['Prescale_NonPhys'],
                                          postscale = config['Postscale'],
                                          algos = [ self.TOSB0d2DdoubleStarTauNuNonPhys ]
                                          )
        self.registerLine(B0d2DdoubleStarTauNuNonPhysTauLine)
        
        
        ### Collect them all together in a nice way ###
        self.Line=B0d2DstarTauNuLine
        self.Selections=[self.DstarSel, self.B0d2DstarTauNuSel]

        self.Line=B0d2DstarTauNuWSLine
        self.Selections=[self.DstarSel, self.B0d2DstarTauNuSelWS]

        self.Line=B0d2DstarTauNuNonPhysTauLine
        self.Selections=[self.DstarSel, self.B0d2DstarTauNuSelNonPhysTau]
        
        self.Line=B0d2DTauNuLine
        self.Selections=[self.DplusSel, self.B0d2DTauNuSel]

        self.Line=B0d2DTauNuWSLine
        self.Selections=[self.DplusSel, self.B0d2DTauNuSelWS]

        self.Line=B0d2DTauNuNonPhysTauLine
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

        self.Line=Lb2LcTauNuLine
        self.Selections=[self.LcSel, self.Lb2LcTauNuSel]

        self.Line=Lb2LcTauNuWSLine
        self.Selections=[self.LcSel, self.Lb2LcTauNuSelWS]

        self.Line=Lb2LcTauNuNonPhysTauLine
        self.Selections=[self.LcSel, self.Lb2LcTauNuSelNonPhysTau]

        self.Line=Lb2LcTauNuInvVertLine
        self.Selections=[self.LcSelInvVert, self.Lb2LcTauNuSelInvVert]

        self.Line=B0d2DstarTauNuInvVertLine
        self.Selections=[self.LcSelInvVert, self.B0d2DstarTauNuSelInvVert]
        


        self.Line=B0d2DdoubleStarTauNuLine
        self.Selections=[self.DdoubleStar2DstarPiSel, self.B0d2DdoubleStarTauNuSel]

        self.Line=B0d2DdoubleStarTauNuWSLine
        self.Selections=[self.DdoubleStar2DstarPiSel, self.B0d2DdoubleStarTauNuSelWS]

        self.Line=B0d2DdoubleStarTauNuNonPhysTauLine
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
        
    def __FilterLc__(self):
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand
         
        LcForLb = FilterDesktop(
            Code = self.totalLcCut
            )
        MyStdLc = DataOnDemand(Location = 'Phys/StdLooseLambdac2PKPi/Particles')
        SelLcForLb = Selection("SelLcFor"+self._name,
                                Algorithm=LcForLb, RequiredSelections = [MyStdLc])
        
        self.LcSel=SelLcForLb
        
    def __FilterLcInvVert__(self):
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand
         
        LcForLbInvVert = FilterDesktop(
            Code = self.totalLcInvVertCut
            )
        MyStdLcInvVert = DataOnDemand(Location = 'Phys/StdLooseLambdac2PKPi/Particles')
        SelLcForLbInvVert = Selection("SelLcForLbInvVert"+self._name,
                                Algorithm=LcForLbInvVert, RequiredSelections = [MyStdLcInvVert])
        
        self.LcSelInvVert=SelLcForLbInvVert

    def __FilterDInvVert__(self):
        
        from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
        from PhysSelPython.Wrappers import Selection, DataOnDemand
         
        DstarForB0dInvVert = FilterDesktop(
            Code = self.totalDInvVertCut
            )
        MyStdDstarInvVert = DataOnDemand(Location = 'Phys/StdLooseDstarWithD02KPi/Particles')
        SelDstarForB0dInvVert = Selection("SelDstarForB0dInvVert"+self._name,
                                Algorithm=DstarForB0dInvVert, RequiredSelections = [MyStdDstarInvVert])
        
        self.DstarSelInvVert=SelDstarForB0dInvVert  
        
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
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombB0d2DstarTauNu = CombineParticles(        
            DecayDescriptors = ["[B0 -> D*(2010)- tau+]cc"],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelB0d2DstarTauNu = Selection("SelB0d2DstarTauNu", Algorithm=CombB0d2DstarTauNu,
                                    RequiredSelections = [self.DstarSel,MyStdLooseDetachedTau])
        
        self.B0d2DstarTauNuSel=SelB0d2DstarTauNu


    def __MakeB0d2DstarTauNuWS__(self):
        """
        B0d selection:
        [B0 -> D*+ tau+]cc
        use of wrong charge combination is useful for background subtraction
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombB0d2DstarTauNuWS = CombineParticles(        
            DecayDescriptors = ["[B0 -> D*(2010)+ tau+]cc"],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelB0d2DstarTauNuWS = Selection("SelB0d2DstarTauNuWS", Algorithm=CombB0d2DstarTauNuWS,
                                    RequiredSelections = [self.DstarSel,MyStdLooseDetachedTau])
        
        self.B0d2DstarTauNuSelWS=SelB0d2DstarTauNuWS



    def __MakeB0d2DstarTauNuNonPhysTau__(self):
        """
        B0d selection:
        [B0 -> D*- tau+++]cc, [B0 -> D*- tau---]cc
        non-physical tau+ -> pi+ pi+ pi+ which serves as an independent background source
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombB0d2DstarTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B0 -> D*(2010)- tau+]cc"],#, "[B0 -> D*(2010)+ tau+]cc"],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelB0d2DstarTauNuNonPhysTau = Selection("SelB0d2DstarTauNuNonPhysTau", Algorithm=CombB0d2DstarTauNuNonPhysTau,
                                    RequiredSelections = [self.DstarSel,MyStdLooseDetachedTau])
        
        self.B0d2DstarTauNuSelNonPhysTau=SelB0d2DstarTauNuNonPhysTau

        
    def __MakeB0d2DTauNu__(self):
        """
        B0d selection:
        [B0 -> D- tau+]cc
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombB0d2DTauNu = CombineParticles(        
            DecayDescriptors = ["[B0 -> D- tau+]cc"],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.98)" },
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelB0d2DTauNu = Selection("SelB0d2DTauNu", Algorithm=CombB0d2DTauNu,
                                    RequiredSelections = [self.DplusSel,MyStdLooseDetachedTau])
        
        self.B0d2DTauNuSel=SelB0d2DTauNu


            
    def __MakeB0d2DTauNuWS__(self):
        """
        B0d selection:
        [B0 -> D+ tau+]cc
        use of wrong charge combination is useful for background subtraction
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombB0d2DTauNuWS = CombineParticles(        
            DecayDescriptors = ["[B0 -> D+ tau+]cc" ],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.98)" },
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelB0d2DTauNuWS = Selection("SelB0d2DTauNuWS", Algorithm=CombB0d2DTauNuWS,
                                    RequiredSelections = [self.DplusSel,MyStdLooseDetachedTau])
        
        self.B0d2DTauNuSelWS=SelB0d2DTauNuWS


    def __MakeB0d2DTauNuNonPhysTau__(self):
        """
        B0d selection:
        [B0 -> D+ tau+++]cc, [B0 -> D+ tau---]cc 
        use of non-physical tau+++ is useful as an independent background source
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombB0d2DTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B0 -> D- tau+]cc"],#, "[B0 -> D+ tau+]cc"],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.98)" },
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelB0d2DTauNuNonPhysTau = Selection("SelB0d2DTauNuNonPhysTau", Algorithm=CombB0d2DTauNuNonPhysTau,
                                    RequiredSelections = [self.DplusSel,MyStdLooseDetachedTau])
        
        self.B0d2DTauNuSelNonPhysTau=SelB0d2DTauNuNonPhysTau



    def __MakeBu2D0TauNu__(self):
        """
        Bu selection:
        [B- -> D0 tau-]cc
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBu2D0TauNu = CombineParticles(        
            DecayDescriptors = ["[B- -> D0 tau-]cc"],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.98)" },
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
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBu2D0TauNuWS = CombineParticles(        
            DecayDescriptors = ["[B- -> D0 tau+]cc"],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.98)" },
            CombinationCut = self.BplusCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBu2D0TauNuWS = Selection("SelBu2D0TauNuWS", Algorithm=CombBu2D0TauNuWS,
                                    RequiredSelections = [self.D0Sel,MyStdLooseDetachedTau])
        
        self.Bu2D0TauNuSelWS=SelBu2D0TauNuWS


    def __MakeBu2D0TauNuNonPhysTau__(self):
        """
        Bu selection:
        [B- -> D0 tau---]cc, [B- -> D~0 tau---]cc 
        use of non-physical tau+++ is useful as an independent background source
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombBu2D0TauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B- -> D0 tau-]cc"],#, "[B- -> D~0 tau-]cc" ],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.98)" },
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
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBc2JpsiTauNu = CombineParticles(        
            DecayDescriptors = ["B_c+ -> J/psi(1S) tau+", "B_c- -> J/psi(1S) tau-" ],
            CombinationCut = self.BcCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBc2JpsiTauNu = Selection("SelBc2JpsiTauNu", Algorithm=CombBc2JpsiTauNu,
                                    RequiredSelections = [self.JpsiSel,MyStdLooseDetachedTau])
        
        self.Bc2JpsiTauNuSel=SelBc2JpsiTauNu


    def __MakeBc2JpsiTauNuNonPhysTau__(self):
        """
        Bc selection:
        [B_c+ -> J/psi(1S) tau+++]cc
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombBc2JpsiTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["B_c+ -> J/psi(1S) tau+", "B_c- -> J/psi(1S) tau-" ],
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
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBs2DsTauNu = CombineParticles(        
            DecayDescriptors = ["[B_s0 -> D_s- tau+]cc"],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.98)" },
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
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombBs2DsTauNuWS = CombineParticles(        
            DecayDescriptors = ["[B_s0 -> D_s+ tau+]cc" ],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.98)" },
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
        from CommonParticles import StdLooseDetachedTauNonPhys

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombBs2DsTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B_s0 -> D_s+ tau-]cc"],#, "[B_s0 -> D_s+ tau-]cc" ],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.98)" },
            CombinationCut = self.BsCombCut,
            MotherCut      = self.BCut,
            )
        
        SelBs2DsTauNuNonPhysTau = Selection("SelBs2DsTauNuNonPhysTau", Algorithm=CombBs2DsTauNuNonPhysTau,
                                    RequiredSelections = [self.DsSel,MyStdLooseDetachedTau])
        
        self.B0s2DsTauNuSelNonPhysTau=SelBs2DsTauNuNonPhysTau
    def __MakeLb2LcTauNu__(self):
        """
        Lb selection:
        [Lambda_b0 -> Lambda_c+ tau-]cc
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombLb2LcTauNu = CombineParticles(        
            DecayDescriptors = ["[Lambda_b0 -> Lambda_c+ tau-]cc"],
            DaughtersCuts = { "tau-" : "(BPVDIRA > 0.995)" },
            CombinationCut = self.LbCombCut,
            MotherCut      = self.BCut,
           
            )
    
        SelLb2LcTauNu = Selection("SelLb2LcTauNu", Algorithm=CombLb2LcTauNu,
                                    RequiredSelections = [self.LcSel,MyStdLooseDetachedTau])
        
        self.Lb2LcTauNuSel=SelLb2LcTauNu


            
    def __MakeLb2LcTauNuWS__(self):
        """
        Lb selection:
        [Lambda_b0 -> Lambda_c+ tau+]cc
        use of wrong charge combination is useful for background subtraction
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombLb2LcTauNuWS = CombineParticles(        
            DecayDescriptors = ["[Lambda_b0 -> Lambda_c+ tau+]cc" ],
            DaughtersCuts = { "tau+" : "(BPVDIRA > 0.995)" },
            CombinationCut = self.LbCombCut,
            MotherCut      = self.BCut,
            )
        
        SelLb2LcTauNuWS = Selection("SelLb2LcTauNuWS", Algorithm=CombLb2LcTauNuWS,
                                    RequiredSelections = [self.LcSel,MyStdLooseDetachedTau])
        
        self.Lb2LcTauNuSelWS=SelLb2LcTauNuWS


    def __MakeLb2LcTauNuNonPhysTau__(self):
        """
        Lb selection:
        [Lambda_b0 -> Lambda_c+ tau+++]cc,  [Lambda_b0 -> Lambda_c+ tau---]cc
        use of non-physical tau+++ is useful as a background source
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombLb2LcTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[Lambda_b0 -> Lambda_c+ tau-]cc"],#, "[Lb -> Lc+ tau-]cc" ],
            DaughtersCuts = { "tau-" : "(BPVDIRA > 0.995)" },
            CombinationCut = self.LbCombCut,
            MotherCut      = self.BCut,
            )
        
        SelLb2LcTauNuNonPhysTau = Selection("SelLb2LcTauNuNonPhysTau", Algorithm=CombLb2LcTauNuNonPhysTau,
                                    RequiredSelections = [self.LcSel,MyStdLooseDetachedTau])
        
        self.Lb2LcTauNuSelNonPhysTau=SelLb2LcTauNuNonPhysTau

    def __MakeLb2LcTauNuInvVert__(self):
        """
        Lb selection:
        [Lambda_b0 -> Lambda_c+ tau-]cc, 
        use of inverted vertex cut only first, should boost the overall efficiency
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombLb2LcTauNuInvVert = CombineParticles(        
            DecayDescriptors = ["[Lambda_b0 -> Lambda_c+ tau-]cc"],
            CombinationCut = self.LbCombInvVertCut,
            MotherCut      = self.BInvVertCut,
            )
        
        SelLb2LcTauNuInvVert = Selection("SelLb2LcTauNuInvVert", Algorithm=CombLb2LcTauNuInvVert,
                                    RequiredSelections = [self.LcSelInvVert,MyStdLooseDetachedTau])
        
        self.Lb2LcTauNuSelInvVert=SelLb2LcTauNuInvVert

    def __MakeB0d2DstarTauNuInvVert__(self):
        """
        Dstar  selection:
        [B0 -> Dstar*- tau+]cc, 
        use of inverted vertex cut only first, should boost the overall efficiency
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombB0d2DstarTauNuInvVert = CombineParticles(                    
            DecayDescriptors = ["[B0 -> D*(2010)- tau+]cc"],
            CombinationCut = self.B0dCombInvVertCut,
            MotherCut      = self.BInvVertCut,
            )
        
        SelB0d2DstarTauNuInvVert = Selection("SelB0d2DstarTauNuInvVert", Algorithm=CombB0d2DstarTauNuInvVert,
                                    RequiredSelections = [self.DstarSelInvVert,MyStdLooseDetachedTau])
        
        self.B0d2DstarTauNuSelInvVert=SelB0d2DstarTauNuInvVert
        
    def __MakeB0d2DdoubleStarTauNu__(self):
        """
        B0d selection:
        [B~0 -> (D_1(2420)0 -> D*+ pi-) tau-]cc, [B~0 -> (D*_2(2460)0 -> D*+ pi-) tau-]cc
        useful for studying background in B0 -> D* tau nu when pi- from D** decay is missed
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombB0d2DdoubleStarTauNu = CombineParticles(        
            DecayDescriptors = ["[B~0 -> D_1(2420)0 tau-]cc", "[B~0 -> D*_2(2460)0 tau-]cc" ],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelB0d2DdoubleStarTauNu = Selection("SelB0d2DdoubleStarTauNu", Algorithm=CombB0d2DdoubleStarTauNu,
                                    RequiredSelections = [self.DdoubleStar2DstarPiSel,MyStdLooseDetachedTau])
        
        self.B0d2DdoubleStarTauNuSel=SelB0d2DdoubleStarTauNu


    def __MakeB0d2DdoubleStarTauNuWS__(self):
        """
        B0d selection:
        [B~0 -> (D_1(2420)~0 -> D*- pi+) tau-]cc, [B~0 -> (D*_2(2460)~0 -> D*- pi+) tau-]cc
        useful for studying WS background in B0 -> D* tau nu when pi+ from D** decay is missed
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTau

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3pi/Particles')
        
        CombB0d2DdoubleStarTauNuWS = CombineParticles(        
            DecayDescriptors = ["[B~0 -> D_1(2420)~0 tau-]cc", "[B~0 -> D*_2(2460)~0 tau-]cc" ],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelB0d2DdoubleStarTauNuWS = Selection("SelB0d2DdoubleStarTauNuWS", Algorithm=CombB0d2DdoubleStarTauNuWS,
                                    RequiredSelections = [self.DdoubleStar2DstarPiSel,MyStdLooseDetachedTau])
        
        self.B0d2DdoubleStarTauNuSelWS=SelB0d2DdoubleStarTauNuWS


    def __MakeB0d2DdoubleStarTauNuNonPhysTau__(self):
        """
        B0d selection:
        [B~0 -> (D_1(2420)~0 -> D*- pi+) tau---]cc, [B~0 -> (D*_2(2460)~0 -> D*- pi+) tau---]cc,
        [B~0 -> (D_1(2420)~0 -> D*- pi+) tau+++]cc, [B~0 -> (D*_2(2460)~0 -> D*- pi+) tau+++]cc
        useful for studying WS background in B0 -> D* tau nu when pi+ from D** decay is missed
        """
        from GaudiConfUtils.ConfigurableGenerators import CombineParticles
        from PhysSelPython.Wrappers import Selection, DataOnDemand
        from CommonParticles import StdLooseDetachedTauNonPhys

        MyStdLooseDetachedTau = DataOnDemand(Location = 'Phys/StdLooseDetachedTau3piNonPhys/Particles')
        
        CombB0d2DdoubleStarTauNuNonPhysTau = CombineParticles(        
            DecayDescriptors = ["[B~0 -> D_1(2420)~0 tau-]cc", "[B~0 -> D*_2(2460)~0 tau-]cc"],#, "[B~0 -> D_1(2420)~0 tau+]cc", "[B~0 -> D*_2(2460)~0 tau+]cc" ],
            CombinationCut = self.B0CombCut,
            MotherCut      = self.BCut,
            )
        
        SelB0d2DdoubleStarTauNuNonPhysTau = Selection("SelB0d2DdoubleStarTauNuNonPhysTau", Algorithm=CombB0d2DdoubleStarTauNuNonPhysTau,
                                    RequiredSelections = [self.DdoubleStar2DstarPiSel,MyStdLooseDetachedTau])
        
        self.B0d2DdoubleStarTauNuSelNonPhysTau=SelB0d2DdoubleStarTauNuNonPhysTau


    


    
        

  

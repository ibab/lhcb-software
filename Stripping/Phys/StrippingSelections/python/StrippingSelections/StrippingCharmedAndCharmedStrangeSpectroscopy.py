'''
Stripping lines for several modes to Ds and Dj spectroscopy studies. 
It provides the main class StrippingCharmedAndCharmedStrangeSpectroscopyConf which 
constructs the Selections and StrippingLines given a configuration 
dictionary.
Exported symbols are
  -CharmedAndCharmedStrangeSpectroscopyConf
  -CombineDandTrack
  -makeKS02LL
   -makeKS02DD
   -makeDstar2D0pi
   -makeDp2KmPipPip
   -makePromptD02KPi
   -makePromptTracks
It can be executed as
from StrippingSelections.StrippingCharmedAndCharmedStrangeSpectroscopy import CharmedAndCharmedStrangeSpectroscopyConf as builder
from StrippingSelections.StrippingCharmedAndCharmedStrangeSpectroscopy import config_params as config_params
lb = builder( 'CharmedAndCharmedStrangeSpectroscopy', config_params )
 '''
__author__  = [ 'Diego Milanes', 'Marco Pappagallo' ]
__date__    = '23/02/2011'
__version__ = '$Revision: 1.2 $'

__all__     = ('CharmedAndCharmedStrangeSpectroscopyConf',
               'CombineDandTrack',
               'makeKS02LL',
               'makeKS02DD',
               'makeDstar2D0pi',
               'makeDp2KmPipPip',
               'makePromptD02KPi',
               'makeDs2KKPi',
               'makeDs2KKPiList',
               'makeD02K3Pi',
               'makeDstarD02K3Pi',
               'makeDstar0D0pi0',
               'makePromptTracks',
               'makeNoPromptTracks',
               'makePromptSoftTracks'
               )

config_params = { 'DpKs_prescale'            : 1,
                  'DstarpKs_prescale'        : 1,
                  'D0K_prescale'             : 1,
                  'DpPim_prescale'           : 1,
                  'D0Pip_prescale'           : 1,
                  'DstarpPim_prescale'       : 1,
                  'DzP_prescale'             : 1,
                  'DzPWS_prescale'           : 1,
                  'DpP_prescale'             : 1,
                  'DpPp_prescale'             : 1,
                  'DsKs_prescale'            : 1,
                  'DsKm_prescale'            : 1,
                  'DstarD02K3PiKs_prescale'  : 1,
                  'Dstar0K_prescale'         : 1,
                  'Dstar0P_prescale'         : 1,
                  'Dstar0KWS_prescale'       : 1,
                  'Dstar0PWS_prescale'       : 1
                  } 

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StrippingConf.StrippingLine import StrippingLine, StrippingMember
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence, MergedSelection
from Configurables import LoKi__VoidFilter as VoidFilter
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
from StrippingUtils.Utils import LineBuilder
 
#modules =  CoreFactory('CoreFactory').Modules
#for i in [ 'LoKiTracks.decorators' ] :
#    if i not in modules : modules.append(i)

default_name = 'CharmedAndCharmedStrangeSpectroscopyConf'
class CharmedAndCharmedStrangeSpectroscopyConf( LineBuilder ):
    '''    
    Builder of pre-scaled Dsj and Dj stripping Selection and StrippingLine.
    Constructs Selections and StrippingLines from a configuration dictionary.
    Usage:
   
    The lines can be used directly to build a StrippingStream object, for instance
    -----------------------------------
    >>>from StrippingSelections import StrippingCharmedAndCharmedStrangeSpectroscopy
    >>>stream = StrippingStream("CharmedAndCharmedStrangeSpectroscopyStream")
    >>>config_params = { 'DpKs_prescale'      : 1,
                         'DstarpKs_prescale'  : 1,
                         'D0K_prescale'       : 1,
                         'DpPim_prescale'     : 1,
                         'D0Pip_prescale'     : 1,
                         'DstarpPim_prescale' : 1
                        } 
    >>>confCharmedAndCharmedStrangeSpectroscopy = StrippingCharmedAndCharmedStrangeSpectroscopy.CharmedAndCharmedStrangeSpectroscopyConf( 'CharmedAndCharmedStrangeSpectroscopy', config_params )
    >>>stream.appendLines( confCharmedAndCharmedStrangeSpectroscopy.lines() )
    --------------------------------------
    Exports as class data member:
    CharmedAndCharmedStrangeSpectroscopyConf.__configuration_keys__ : List of required configuration parameters.
    '''

    __configuration_keys__ = ('DpKs_prescale',
                              'DstarpKs_prescale',
                              'D0K_prescale',
                              'DpPim_prescale',
                              'D0Pip_prescale',
                              'DstarpPim_prescale', 
                              'DzP_prescale', 
                              'DzPWS_prescale', 
                              'DpP_prescale',
                              'DpPp_prescale',
                              'DsKs_prescale',
                              'DsKm_prescale',
                              'DstarD02K3PiKs_prescale',
                              'Dstar0K_prescale',
                              'Dstar0P_prescale',
                              'Dstar0KWS_prescale',
                              'Dstar0PWS_prescale' 
                              )

    def __init__( self, name, config ) :
        
        LineBuilder.__init__( self, name, config )

        '''
        Here we construct the selections
        '''
    
        name_DpKs_line             = name + '_DpKs'
        name_DstarpKs_line         = name + '_DstarpKs'
        name_DstarD02K3PiKs_line   = name + '_DstarD02K3PiKs'
        name_D0K_line              = name + '_D0K'
        name_DpPim_line            = name + '_DpPim'
        name_D0Pip_line            = name + '_D0Pip'
        name_DstarpPim_line        = name + '_DstarpPim'
        name_DzP_line              = name + '_DzP'
        name_DpP_line              = name + '_DpPm'
        name_DpPp_line              = name + '_DpPp'
        name_DsKm_line             = name + '_DsKm'
        name_DsKs_line             = name + '_DsKs'
        name_Ds_line               = name + '_Ds'
        name_DzPWS_line            = name + '_DzPWS'
        name_Dstar0K_line          = name + '_Dstar0K'
        name_Dstar0P_line          = name + '_Dstar0P'
        name_Dstar0KWS_line        = name + '_Dstar0KWS'
        name_Dstar0PWS_line        = name + '_Dstar0PWS'

        '''
        make particles
        '''
        from StandardParticles import StdLooseMergedPi0, StdLooseResolvedPi0

        self.sel_ResPi0        = StdLooseResolvedPi0
        self.sel_MerPi0        = StdLooseMergedPi0
        self.sel_Dp2KmPipPip   = makeDp2KmPipPip( name+"Dplus" )
        self.sel_KS02LL        = makeKS02LL( name+"KSLongLong" )
        self.sel_KS02DD        = makeKS02DD( name+"KSDownDown" )
        self.sel_KS0           = MergedSelection( name+"KS0", RequiredSelections = [ self.sel_KS02LL, self.sel_KS02DD ] )
        self.sel_pi0           = MergedSelection( name+"pi0", RequiredSelections = [ self.sel_ResPi0, self.sel_MerPi0 ] )
        self.sel_Dstarp        = makeDstar2D0pi( name+"Dstar" )
        self.sel_D0            = makePromptD02KPi( name+"D0" )
        self.sel_K             = makePromptTracks( name+"K", "Phys/StdTightKaons/Particles")
        self.sel_Pi            = makePromptTracks( name+"Pi", "Phys/StdTightPions/Particles")
        self.sel_P             = makePromptTracks( name+"P", "Phys/StdTightProtons/Particles")
        self.sel_LoosePi       = makeNoPromptTracks( name+"LoosePi", "Phys/StdLoosePions/Particles")
        self.sel_LooseK        = makeNoPromptTracks( name+"LooseK", "Phys/StdLooseKaons/Particles")
        self.sel_Ds            = makeDs2KKPi( name+"Ds", self.sel_LoosePi, self.sel_LooseK )
        self.sel_LoosePromptPi = makePromptSoftTracks( name+"LoosePromptPi", "Phys/StdAllLoosePions/Particles")
        self.sel_D02K3Pi       = makeD02K3Pi( name+"D02K3Pi", self.sel_LoosePi, self.sel_LooseK )
        self.sel_DstarD02K3Pi  = makeDstarD02K3Pi( name+"DstarD02K3Pi", self.sel_D02K3Pi, self.sel_LoosePromptPi )
        self.sel_DsList        = makeDs2KKPiList( name+"DsList" )
        self.sel_Dstar0D0pi0   = makeDstar0D0pi0( name+"Dstar0D0pi0", self.sel_D0, self.sel_pi0 )


        '''
        combine particles
        '''

        self.DpKs            = CombineDandTrack( name+"Ds2DpKs"     , "[D*_s2+ -> D+ KS0]cc", self.sel_Dp2KmPipPip, self.sel_KS0 )
        self.DstarpKs        = CombineDandTrack( name+"Ds2DstarpKs" , "[D_s1(2536)+ -> D*(2010)+ KS0]cc", self.sel_Dstarp, self.sel_KS0 )
        self.DstarD02K3PiKs  = CombineDandTrack( name+"Ds2DstarD02K3PiKs" , "[D_s1(2536)+ -> D*(2010)+ KS0]cc", self.sel_DstarD02K3Pi, self.sel_KS0 )
        self.D0K             = CombineDandTrack( name+"Ds2D0K"      , "[D_s1(2536)+ -> D0 K+]cc", self.sel_D0, self.sel_K )
        self.DpPim           = CombineDandTrack( name+"Dj2DpPim"    , "[D*_2(2460)0 -> D+ pi-]cc", self.sel_Dp2KmPipPip, self.sel_Pi )
        self.D0Pip           = CombineDandTrack( name+"Dj2D0Pip"    , "[D*_2(2460)+ -> D0 pi+]cc", self.sel_D0, self.sel_Pi )
        self.DstarpPim       = CombineDandTrack( name+"Dj2DstarpPim", "[D*_2(2460)0 -> D*(2010)+ pi-]cc", self.sel_Dstarp, self.sel_Pi )
        self.DzP             = CombineDandTrack( name+"Lam2DzP"     , "[Lambda_c(2625)+ -> D0 p+]cc", self.sel_D0, self.sel_P )
        self.DzPWS           = CombineDandTrack( name+"Lam2DzPWS"   , "[Lambda_c(2625)+ -> D0 p~-]cc", self.sel_D0, self.sel_P )
        self.DpP             = CombineDandTrack( name+"Lam2DpP"     , "[Lambda_c(2625)+ -> D+ p~-]cc", self.sel_Dp2KmPipPip, self.sel_P )
        self.DpPp            = CombineDandTrack( name+"Lam2DpPp"     , "[Lambda_c(2625)+ -> D+ p+]cc", self.sel_Dp2KmPipPip, self.sel_P )
        self.DsKs            = CombineDandTrack( name+"D2DsKs"      , "[D*_2(2460)+ -> D_s+ KS0]cc", self.sel_Ds, self.sel_KS0 )
        self.DsKm            = CombineDandTrack( name+"D2DsKm"      , "[D*_2(2460)0 -> D_s+ K-]cc", self.sel_Ds, self.sel_K )
        self.Dstar0K         = CombineDandTrack( name+"Ds2Dstar0K"     , "[D_s1(2536)+ -> D*(2007)0 K+]cc", self.sel_Dstar0D0pi0, self.sel_K )
        self.Dstar0P         = CombineDandTrack( name+"Lam2Dstar0P"     , "[Lambda_c(2625)+ -> D*(2007)0 p+]cc", self.sel_Dstar0D0pi0, self.sel_P )
        self.Dstar0KWS       = CombineDandTrack( name+"Ds2Dstar0KWS"     , "[D_s1(2536)+ -> D*(2007)0 K-]cc", self.sel_Dstar0D0pi0, self.sel_K )
        self.Dstar0PWS       = CombineDandTrack( name+"Lam2Dstar0PWS"     , "[Lambda_c(2625)+ -> D*(2007)0 p~-]cc", self.sel_Dstar0D0pi0, self.sel_P )

        '''
        Here we construct the stripping lines
        '''
        self.DpKs_line      = StrippingLine( name_DpKs_line     , prescale = config[ 'DpKs_prescale' ]     , selection = self.DpKs      )
        self.DstarpKs_line  = StrippingLine( name_DstarpKs_line , prescale = config[ 'DstarpKs_prescale' ] , selection = self.DstarpKs  )
        self.D0K_line       = StrippingLine( name_D0K_line      , prescale = config[ 'D0K_prescale' ]      , selection = self.D0K       )
        self.DpPim_line     = StrippingLine( name_DpPim_line    , prescale = config[ 'DpPim_prescale' ]    , selection = self.DpPim     )
        self.D0Pip_line     = StrippingLine( name_D0Pip_line    , prescale = config[ 'D0Pip_prescale' ]    , selection = self.D0Pip     )
        self.DstarpPim_line = StrippingLine( name_DstarpPim_line, prescale = config[ 'DstarpPim_prescale' ], selection = self.DstarpPim )
        self.DzP_line       = StrippingLine( name_DzP_line      , prescale = config[ 'DzP_prescale' ]      , selection = self.DzP       )
        self.DpP_line       = StrippingLine( name_DpP_line      , prescale = config[ 'DpP_prescale' ]      , selection = self.DpP       )
        self.DpPp_line       = StrippingLine( name_DpPp_line      , prescale = config[ 'DpPp_prescale' ]      , selection = self.DpPp       )
        self.DsKs_line      = StrippingLine( name_DsKs_line     , prescale = config[ 'DsKs_prescale' ]     , selection = self.DsKs      )
        self.DsKm_line      = StrippingLine( name_DsKm_line     , prescale = config[ 'DsKm_prescale' ]     , selection = self.DsKm      )
        self.DstarD02K3PiKs_line = StrippingLine( name_DstarD02K3PiKs_line , prescale = config[ 'DstarD02K3PiKs_prescale' ] , selection = self.DstarD02K3PiKs  )
        self.Ds_line        = StrippingLine( name_Ds_line     , prescale = 1     , selection = self.sel_DsList     ) 
        self.DzPWS_line     = StrippingLine( name_DzPWS_line  , prescale = config[ 'DzPWS_prescale' ]      , selection = self.DzPWS       )
        self.Dstar0K_line     = StrippingLine( name_Dstar0K_line  , prescale = config[ 'Dstar0K_prescale' ]      , selection = self.Dstar0K       )
        self.Dstar0P_line     = StrippingLine( name_Dstar0P_line  , prescale = config[ 'Dstar0P_prescale' ]      , selection = self.Dstar0P       )
        self.Dstar0KWS_line     = StrippingLine( name_Dstar0KWS_line  , prescale = config[ 'Dstar0KWS_prescale' ]      , selection = self.Dstar0KWS       )
        self.Dstar0PWS_line     = StrippingLine( name_Dstar0PWS_line  , prescale = config[ 'Dstar0PWS_prescale' ]      , selection = self.Dstar0PWS       )
        '''
        register stripping lines
        '''
        #self.registerLine( self.DpKs_line )
        self.registerLine( self.DstarpKs_line )
        #self.registerLine( self.D0K_line )
        #self.registerLine( self.DpPim_line )
        #self.registerLine( self.D0Pip_line )
        #self.registerLine( self.DstarpPim_line )
        self.registerLine( self.DzP_line )
        self.registerLine( self.DzPWS_line )
        self.registerLine( self.DpP_line )
        self.registerLine( self.DpPp_line )

        #self.registerLine( self.Ds_line ) #for testing only
        #self.registerLine( self.DsKm_line )
        #self.registerLine( self.DsKs_line )
        self.registerLine( self.DstarD02K3PiKs_line )
        self.registerLine( self.Dstar0K_line )
        self.registerLine( self.Dstar0P_line )
        self.registerLine( self.Dstar0KWS_line )
        self.registerLine( self.Dstar0PWS_line )


'''
we create our particles
'''


def CombineDandTrack( name, 
                      decay,
                      DSel,
                      KPiSel ):

    basic_cut = '(M > 0.0*GeV) & ~INTREE( (HASTRACK)&(THASINFO( LHCb.Track.CloneDist )) )'
    if ( name.endswith( "Ds2D0K" ) or name.endswith( "Dj2DstarpPim" ) or name.endswith( "Dj2D0Pip" ) or name.endswith( "Dj2DpPim" ) ) :
        basic_cut += ' & (LV02>0.0) & (M<3.5*GeV)'
    if ( name.endswith( "Dj2DstarpPim" )  ) :
        basic_cut += ' & (M<3.5*GeV)'
    if ( name.endswith( "Dj2DpPim" ) or name.endswith( "Dj2D0Pip" )  ) :
        basic_cut += ' & (PT>6*GeV) & (M<3.2*GeV)'
    if ( name.endswith( "Ds2D0K" )  ) :
        basic_cut += ' & (PT>4.5*GeV)'    
    if ( name.endswith( "Lam2DzP" ) or name.endswith( "Lam2DpPp" ) or name.endswith( "Lam2DpP" ) or name.endswith( "Lam2DzPWS" ) ) : 
        basic_cut += ' & (LV02>0.0) & (M<4*GeV)'    
    if ( name.endswith( "D2DsKm" ) ) : 
        basic_cut += ' & (LV02>-0.5) & (M<3.8*GeV)'    
    if ( name.endswith( "D2DsKs" ) ) : 
        basic_cut += ' & (M<3.8*GeV)'    
    if ( name.endswith( "Ds2Dstar0K" ) or name.endswith( "Ds2Dstar0KWS" ) ) :
        basic_cut += ' & (M<3.5*GeV)'
    if( name.endswith( "Lam2Dstar0P" ) or name.endswith( "Lam2Dstar0PWS" ) ):
        basic_cut += ' & (M<4*GeV)'


    D = CombineParticles( DecayDescriptor = decay, MotherCut = basic_cut )
    return Selection( name,
                      Algorithm = D,
                      RequiredSelections = [DSel, KPiSel] )

def makeDp2KmPipPip( name ):
    DplusTracks = '(PT > 250*MeV) & (P > 3*GeV) & (P < 100*GeV) & (TRPCHI2 > 0.0001) & (HASRICH)'
    cut1 = "((CHILDCUT(MIPCHI2DV(PRIMARY) > 25.0, 1)) | (CHILDCUT(MIPCHI2DV(PRIMARY) > 25.0,2)) | (CHILDCUT(MIPCHI2DV(PRIMARY) > 25.0,3)))"
    cut2 = "((CHILDCUT(PT > 0.6*GeV,1) & CHILDCUT(PT > 0.6*GeV,2)) | (CHILDCUT(PT > 0.6*GeV,3) & CHILDCUT(PT > 0.6*GeV,2)) | (CHILDCUT(PT > 0.6*GeV,3) & CHILDCUT(PT > 0.6*GeV,1)))"
    Cut_Dplus = '(BPVDIRA > 0.99999) & (MIPCHI2DV(PRIMARY) < 16) & (BPVVDCHI2 > 25) & (VFASPF(VCHI2/VDOF)<8) & (PT > 2.5*GeV) & ' + cut1 + " & " + cut2 
    code = '(' + Cut_Dplus + '& CHILDCUT(' + DplusTracks + ',1) & CHILDCUT(' + DplusTracks + ',2) & CHILDCUT(' + DplusTracks + ',3))'
    DplusFilter = FilterDesktop( Code = code )
    stdDplus = DataOnDemand( Location = "Phys/StdLooseDplus2KPiPi/Particles" )
    return Selection ( name,
                       Algorithm = DplusFilter,
                       RequiredSelections = [ stdDplus ] )

def makeKS02LL( name ):
    cut_base_ll = "(BPVVDZ > -20 *cm) & (BPVVDZ < 65 *cm) & (BPVVD > 1.5*cm) & (BPVDIRA > 0.9999) & (CHILDIP(1) < 0.5*mm) & (CHILDIP(2) < 0.5 *mm) & (VFASPF(VPCHI2) > 0.1*perCent)"
    cut_code = '(' + cut_base_ll + '& CHILDCUT((TRPCHI2 > 0.0001) & (PT > 250*MeV) & (P < 100*GeV) & (MIPDV(PRIMARY) > 0.5*mm) & (HASRICH), 1) &  CHILDCUT((TRPCHI2 > 0.0001) & (PT > 250*MeV) & (P < 100*GeV) & (MIPDV(PRIMARY) > 0.5*mm) & (HASRICH), 2) )'
    KsLLFilter = FilterDesktop( Code = cut_code )
    stdKsLL = DataOnDemand( Location = "Phys/StdLooseKsLL/Particles" )
    return Selection( name,
                      Algorithm = KsLLFilter,
                      RequiredSelections = [ stdKsLL ] )

def makeKS02DD( name ):
    cut_base_dd = "(BPVVDZ > 0.0*cm) & (BPVVDZ < 230.0*cm) & (BPVVD > 20.0*cm) & (BPVDIRA > 0.9999) & (CHILDIP(1) < 6.0*mm) & (CHILDIP(2) < 6.0 *mm)"
    cut_code = '(' + cut_base_dd + '& CHILDCUT((TRPCHI2 > 0.0001) & (PT > 250*MeV) & (P < 100*GeV) & (MIPDV(PRIMARY) > 0.5*mm) & (HASRICH), 1) &  CHILDCUT((TRPCHI2 > 0.0001) & (PT > 250*MeV) & (P < 100*GeV) & (MIPDV(PRIMARY) > 0.5*mm) & (HASRICH), 2) )'
    KsDDFilter = FilterDesktop( Code = cut_code )
    stdKsDD = DataOnDemand( Location = "Phys/StdLooseKsDD/Particles" )
    return Selection( name,
                      Algorithm = KsDDFilter,
                      RequiredSelections = [ stdKsDD ] )



def makeDstar2D0pi( name ):
    D0tracks = "(PT>250*MeV) & (P>2.5*GeV) & (P < 100*GeV) & (TRPCHI2>0.0001) & (HASRICH)"
    D0_cuts = "CHILDCUT( (BPVDIRA > 0.999) & (VFASPF(VCHI2/VDOF)<10) & (PT>2.*GeV) & (BPVVDCHI2 > 25) & (ADMASS('D0')<100*MeV), 2 )"
    D0tracks_cuts = "CHILDCUT( CHILDCUT(" + D0tracks + ",1) & CHILDCUT(" + D0tracks + ",2),2)"
    pisoft_cuts = "CHILDCUT( (PT>150*MeV) & (P>1*GeV) & (MIPCHI2DV(PRIMARY)<16.0) , 1 ) "
    Dstar_cuts = "(M-MAXTREE('D0'==ABSID,M)<160*MeV) & (PT>2.5*GeV) & (VFASPF(VCHI2/VDOF)<20)"
    DstarChain_cut = "(" + Dstar_cuts + " & " +  pisoft_cuts + " & " + D0_cuts + " & " + D0tracks_cuts + ")"
    DstarFilter = FilterDesktop( Code = DstarChain_cut )
    stdDstar = DataOnDemand( Location = "Phys/StdLooseDstarWithD02KPi/Particles" )
    return Selection( name,
                      Algorithm = DstarFilter,
                      RequiredSelections = [ stdDstar ] )

def makePromptD02KPi( name ):
    D0tracks_cuts = "(PT>250*MeV) & (P>3*GeV) & (P < 100*GeV) & (TRPCHI2>0.0001) & (HASRICH)"
    D0cuts = "(BPVDIRA > 0.99999) & (VFASPF(VCHI2/VDOF)<8) & (PT>2.*GeV) & (ADMASS('D0')<100*MeV) & (MIPCHI2DV(PRIMARY) < 16.0) & (BPVVDCHI2 > 25)"
    cut_code = "(CHILDCUT(" + D0tracks_cuts + ",1) & CHILDCUT(" + D0tracks_cuts + ",2) & " + D0cuts + ")"
    D0Filter = FilterDesktop( Code = cut_code )
    stdD0 = DataOnDemand( Location = "Phys/StdLooseD02KPi/Particles" )
    return Selection( name,
                      Algorithm = D0Filter,
                      RequiredSelections = [ stdD0 ] )

def makeDs2KKPi( name, pions, kaons ):
    combinationCuts = "(ADAMASS('D_s+')<200.0*MeV) & ((APT>1.*GeV) | (ASUM(PT)>1.*GeV))  & (ADOCAMAX('')<0.5*mm)"
    motherCuts      = "(M>1.92*GeV) & (M<2.02*GeV) & (BPVDIRA > 0.999) & (MIPCHI2DV(PRIMARY) < 25) & (BPVVDCHI2 > 25) & (VFASPF(VCHI2) < 30)" 
    DsFilter        = CombineParticles ( DecayDescriptor = "[D_s+ -> K+ K- pi+]cc",
                                         DaughtersCuts = { "K+" : "ALL", "pi+" : "ALL" },
                                         CombinationCut = combinationCuts,
                                         MotherCut = motherCuts
                                         )
    return Selection( name, Algorithm = DsFilter, RequiredSelections = [ pions, kaons ] )

def makeDs2KKPiList( name ):
    Dtracks_cuts = "(PT>250*MeV) & (P>3*GeV) & (P < 100*GeV) & (TRPCHI2>0.0001) & (HASRICH)"
    Dcuts = "(BPVDIRA > 0.99999) & (VFASPF(VCHI2/VDOF)<8) & (PT>2.*GeV) & (MIPCHI2DV(PRIMARY) < 16.0) & (BPVVDCHI2 > 25)"
    cut_code = "(CHILDCUT(" + Dtracks_cuts + ",1) & CHILDCUT(" + Dtracks_cuts + ",2) & CHILDCUT(" + Dtracks_cuts + ",3) & " + Dcuts + ")"
    DFilter = FilterDesktop( Code = cut_code )
    stdD = DataOnDemand( Location = "Phys/StdVeryTightDsplus2KKPi/Particles" )
    return Selection( name,
                      Algorithm = DFilter,
                      RequiredSelections = [ stdD ] )


def makeD02K3Pi( name, pions, kaons ):
    combinationCuts = "(ADAMASS('D0')<100.0*MeV) & (APT>800.0*MeV) & (AMAXDOCA('')<0.15*mm)"
    motherCuts = "(VFASPF(VCHI2/VDOF)<10) & (BPVVDCHI2>36) & (MIPCHI2DV(PRIMARY) < 16.0) & (BPVDIRA>0.9999) & (ADMASS('D0')<100.0*MeV) & (PT>2.0*GeV)" 
    D0Filter = CombineParticles ( DecayDescriptor = "[D0 -> K- pi+ pi- pi+]cc",
                                  DaughtersCuts = { "K+" : "ALL", "pi+" : "ALL" },
                                  CombinationCut = combinationCuts,
                                  MotherCut = motherCuts
                                  )

    return Selection( name,
                      Algorithm = D0Filter,
                      RequiredSelections=[ pions, kaons ] )


def makeDstarD02K3Pi( name, D0, pions ):
   combinationCuts = "(APT>3.0*GeV) & (AMAXDOCA('')<0.2*mm)" 
   motherCuts = "(M-MAXTREE('D0'==ABSID,M)<160*MeV) & (VFASPF(VCHI2/VDOF)<20)"
   DstarFilter = CombineParticles ( DecayDescriptor = "[D*(2010)+ -> pi+ D0]cc",
                                    CombinationCut  = combinationCuts,
                                    MotherCut       = motherCuts,
                                    DaughtersCuts   = {"D0": "ALL", "pi+" : "ALL" }
                                    )
   return Selection( name,
                     Algorithm = DstarFilter,
                     RequiredSelections = [ pions, D0 ] )


def makeDstar0D0pi0( name, D0, pions ):
   combinationCuts = "(AALL)" 
   motherCuts = "(M-MAXTREE('D0'==ABSID,M)<180*MeV)"
   #DstarFilter = CombineParticles ( ParticleCombiners = { '' : 'ParticleAdder' },
   DstarFilter = CombineParticles ( ParticleCombiners = { '' : 'LoKi::VertexFitter' },
                                    DecayDescriptor = "[D*(2007)0 -> D0 pi0]cc",
                                    CombinationCut  = combinationCuts,
                                    MotherCut       = motherCuts,
                                    DaughtersCuts   = {"D0": "ALL", "pi0" : "ALL" }
                                    )

   return Selection( name,
                     Algorithm = DstarFilter,
                     RequiredSelections = [  pions, D0 ] )




def makePromptTracks( name, particle ):
    cuts = "(ISLONG) & (PT>500*MeV) & (P>3*GeV) & (MIPCHI2DV(PRIMARY)<16.0) & (TRPCHI2>0.0001) & (HASRICH)"
    TrackFilter = FilterDesktop( Code = cuts )

    stdTightTrack = DataOnDemand( Location = particle )
    return Selection( name,
                      Algorithm = TrackFilter,
                      RequiredSelections = [ stdTightTrack ] )

def makeNoPromptTracks( name, particle ):
    cuts = "(ISLONG) & (PT>400*MeV) & (P>1*GeV) & (MIPCHI2DV(PRIMARY)>25.0) & (TRPCHI2>0.0001) & (HASRICH) & (TRCHI2DOF<3)"
    if particle.endswith( "Pions/Particles" ) :
        cuts = cuts + " & ((PIDK-PIDpi)<7)"
    if particle.endswith( "Kaons/Particles" ) :
        cuts = cuts + " & ((PIDK-PIDpi)>0)"

    TrackFilter = FilterDesktop( Code = cuts )
    stdTrack = DataOnDemand( Location = particle )
    return Selection( name,
                      Algorithm = TrackFilter,
                      RequiredSelections = [ stdTrack ] )

def makePromptSoftTracks( name, particle ):
    cuts = "(ISLONG) & (PT>250*MeV) & (P>1*GeV) & (MIPCHI2DV(PRIMARY)<16.0) & (TRPCHI2>0.0001) & (HASRICH)"
    TrackFilter = FilterDesktop( Code = cuts )
    stdTrack = DataOnDemand( Location = particle )
    return Selection( name,
                      Algorithm = TrackFilter,
                      RequiredSelections = [ stdTrack ] )


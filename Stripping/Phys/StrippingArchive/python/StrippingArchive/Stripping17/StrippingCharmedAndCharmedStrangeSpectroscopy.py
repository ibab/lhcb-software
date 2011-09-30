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
               'makePromptTracks'
               )

config_params = { 'DpKs_prescale'      : 1,
                  'DstarpKs_prescale'  : 1,
                  'D0K_prescale'       : 1,
                  'DpPim_prescale'     : 1,
                  'D0Pip_prescale'     : 1,
                  'DstarpPim_prescale' : 1
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
                              'DstarpPim_prescale'
                              )

    def __init__( self, name, config ) :
        
        LineBuilder.__init__( self, name, config )

        '''
        Here we construct the selections
        '''
    
        name_DpKs_line      = name + '_DpKs'
        name_DstarpKs_line  = name + '_DstarpKs'
        name_D0K_line       = name + '_D0K'
        name_DpPim_line     = name + '_DpPim'
        name_D0Pip_line     = name + '_D0Pip'
        name_DstarpPim_line = name + '_DstarpPim'

        '''
        make particles
        '''
        self.sel_Dp2KmPipPip = makeDp2KmPipPip( name+"Dplus" )
        self.sel_KS02LL = makeKS02LL( name+"KSLongLong" )
        self.sel_KS02DD = makeKS02DD( name+"KSDownDown" )
        self.sel_KS0 = MergedSelection( name+"KS0", RequiredSelections = [ self.sel_KS02LL, self.sel_KS02DD ] )
        self.sel_Dstarp = makeDstar2D0pi( name+"Dstar" )
        self.sel_D0 = makePromptD02KPi( name+"D0" )
        self.sel_K = makePromptTracks( name+"K", "Phys/StdTightKaons/Particles")
        self.sel_Pi = makePromptTracks( name+"Pi", "Phys/StdTightPions/Particles")

        '''
        combine particles
        '''

        self.DpKs      = CombineDandTrack( name+"Ds2DpKs"     , "[D*_s2+ -> D+ KS0]cc", self.sel_Dp2KmPipPip, self.sel_KS0 )
        self.DstarpKs  = CombineDandTrack( name+"Ds2DstarpKs" , "[D_s1(2536)+ -> D*(2010)+ KS0]cc", self.sel_Dstarp, self.sel_KS0 )
        self.D0K       = CombineDandTrack( name+"Ds2D0K"      , "[D_s1(2536)+ -> D0 K+]cc", self.sel_D0, self.sel_K )
        self.DpPim     = CombineDandTrack( name+"Dj2DpPim"    , "[D*_2(2460)0 -> D+ pi-]cc", self.sel_Dp2KmPipPip, self.sel_Pi )
        self.D0Pip     = CombineDandTrack( name+"Dj2D0Pip"    , "[D*_2(2460)+ -> D0 pi+]cc", self.sel_D0, self.sel_Pi )
        self.DstarpPim = CombineDandTrack( name+"Dj2DstarpPim", "[D*_2(2460)0 -> D*(2010)+ pi-]cc", self.sel_Dstarp, self.sel_Pi )

        '''
        Here we construct the stripping lines
        '''
        #TrkFilter = "(TrSOURCE('Rec/Track/Best', TrLONG) >> (TrSIZE < 150))"
        self.DpKs_line      = StrippingLine( name_DpKs_line     , prescale = config[ 'DpKs_prescale' ]     , selection = self.DpKs      )#, FILTER = TrkFilter )
        self.DstarpKs_line  = StrippingLine( name_DstarpKs_line , prescale = config[ 'DstarpKs_prescale' ] , selection = self.DstarpKs  )#, FILTER = TrkFilter )
        self.D0K_line       = StrippingLine( name_D0K_line      , prescale = config[ 'D0K_prescale' ]      , selection = self.D0K       )#, FILTER = TrkFilter )
        self.DpPim_line     = StrippingLine( name_DpPim_line    , prescale = config[ 'DpPim_prescale' ]    , selection = self.DpPim     )#, FILTER = TrkFilter )
        self.D0Pip_line     = StrippingLine( name_D0Pip_line    , prescale = config[ 'D0Pip_prescale' ]    , selection = self.D0Pip     )#, FILTER = TrkFilter )
        self.DstarpPim_line = StrippingLine( name_DstarpPim_line, prescale = config[ 'DstarpPim_prescale' ], selection = self.DstarpPim )#, FILTER = TrkFilter )
             
        '''
        register stripping lines
        '''
      
        self.registerLine( self.DpKs_line )
        self.registerLine( self.DstarpKs_line )
        self.registerLine( self.D0K_line )
        self.registerLine( self.DpPim_line )
        self.registerLine( self.D0Pip_line )
        self.registerLine( self.DstarpPim_line )



'''
we create our particles
'''


def CombineDandTrack( name, 
                      decay,
                      DSel,
                      KPiSel ):
    Cut_Ds = '(M > 0.0*GeV) & (M<3.5*GeV) & ~INTREE( (HASTRACK)&(THASINFO( LHCb.Track.CloneDist )) )'
    if ( name.endswith( "Ds2D0K" ) or name.endswith( "Dj2DstarpPim" ) or name.endswith( "Dj2D0Pip" ) or name.endswith( "Dj2DpPim" ) ) :
        Cut_Ds += ' & (LV02>0.0)'
    if ( name.endswith( "Dj2DstarpPim" )  ) :
        Cut_Ds += ' & (PT>5*GeV) & (M<3.2*GeV)'
    if ( name.endswith( "Dj2DpPim" ) or name.endswith( "Dj2D0Pip" )  ) :
        Cut_Ds += ' & (PT>6*GeV) & (M<3.2*GeV)'
    if ( name.endswith( "Ds2D0K" )  ) :
        Cut_Ds += ' & (PT>4.5*GeV)'

    D = CombineParticles( DecayDescriptor = decay,
                          #CombinationCut  = "AALLSAMEBPV",
                          MotherCut       = Cut_Ds )
    return Selection( name,
                      Algorithm = D,
                      RequiredSelections = [DSel, KPiSel] )

def makeDp2KmPipPip( name ):
    DplusTracks = '(PT > 250*MeV) & (P > 3*GeV) & (P < 100*GeV) & (TRPCHI2 > 0.0001)'
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
    cut_code = '(' + cut_base_ll + '& CHILDCUT((TRPCHI2 > 0.0001) & (PT > 250*MeV) & (P < 100*GeV) & (MIPDV(PRIMARY) > 0.5*mm), 1) &  CHILDCUT((TRPCHI2 > 0.0001) & (PT > 250*MeV) & (P < 100*GeV) & (MIPDV(PRIMARY) > 0.5*mm), 2) )'
    KsLLFilter = FilterDesktop( Code = cut_code )
    stdKsLL = DataOnDemand( Location = "Phys/StdLooseKsLL/Particles" )
    return Selection( name,
                      Algorithm = KsLLFilter,
                      RequiredSelections = [ stdKsLL ] )

def makeKS02DD( name ):
    cut_base_dd = "(BPVVDZ > 0.0*cm) & (BPVVDZ < 230.0*cm) & (BPVVD > 20.0*cm) & (BPVDIRA > 0.9999) & (CHILDIP(1) < 6.0*mm) & (CHILDIP(2) < 6.0 *mm)"
    cut_code = '(' + cut_base_dd + '& CHILDCUT((TRPCHI2 > 0.0001) & (PT > 250*MeV) & (P < 100*GeV) & (MIPDV(PRIMARY) > 0.5*mm), 1) &  CHILDCUT((TRPCHI2 > 0.0001) & (PT > 250*MeV) & (P < 100*GeV) & (MIPDV(PRIMARY) > 0.5*mm), 2) )'
    KsDDFilter = FilterDesktop( Code = cut_code )
    stdKsDD = DataOnDemand( Location = "Phys/StdLooseKsDD/Particles" )
    return Selection( name,
                      Algorithm = KsDDFilter,
                      RequiredSelections = [ stdKsDD ] )



def makeDstar2D0pi( name ):
    D0tracks = "(PT>250*MeV) & (P>3*GeV) & (P < 100*GeV) & (TRPCHI2>0.0001)"
    D0_cuts = "CHILDCUT( (BPVDIRA > 0.999) & (VFASPF(VCHI2/VDOF)<10) & (PT>2.*GeV) & (BPVVDCHI2 > 25) & (ADMASS('D0')<100*MeV), 2 )"
    D0tracks_cuts = "CHILDCUT( CHILDCUT(" + D0tracks + ",1) & CHILDCUT(" + D0tracks + ",2),2)"
    pisoft_cuts = "CHILDCUT( (PT>150*MeV) & (P>1*GeV) & (MIPCHI2DV(PRIMARY)<16.0) , 1 ) "
    Dstar_cuts = "(M-MAXTREE('D0'==ABSID,M)<160*MeV) & (PT>2.5*GeV) & (VFASPF(VCHI2/VDOF)<8)"
    DstarChain_cut = "(" + Dstar_cuts + " & " +  pisoft_cuts + " & " + D0_cuts + " & " + D0tracks_cuts + ")"
    DstarFilter = FilterDesktop( Code = DstarChain_cut )
    stdDstar = DataOnDemand( Location = "Phys/StdLooseDstarWithD02KPi/Particles" )
    return Selection( name,
                      Algorithm = DstarFilter,
                      RequiredSelections = [ stdDstar ] )

def makePromptD02KPi( name ):
    D0tracks_cuts = "(PT>250*MeV) & (P>3*GeV) & (P < 100*GeV) & (TRPCHI2>0.0001)"
    D0cuts = "(BPVDIRA > 0.99999) & (VFASPF(VCHI2/VDOF)<8) & (PT>2.*GeV) & (ADMASS('D0')<100*MeV) & (MIPCHI2DV(PRIMARY) < 16.0) & (BPVVDCHI2 > 25)"
    cut_code = "(CHILDCUT(" + D0tracks_cuts + ",1) & CHILDCUT(" + D0tracks_cuts + ",2) & " + D0cuts + ")"
    D0Filter = FilterDesktop( Code = cut_code )
    stdD0 = DataOnDemand( Location = "Phys/StdLooseD02KPi/Particles" )
    return Selection( name,
                      Algorithm = D0Filter,
                      RequiredSelections = [ stdD0 ] )

def makePromptTracks( name, particle ):
    cuts = "(ISLONG) & (PT>500*MeV) & (P>3*GeV) & (MIPCHI2DV(PRIMARY)<16.0) & (TRPCHI2>0.0001)"
    TrackFilter = FilterDesktop( Code = cuts )
    stdTightTrack = DataOnDemand( Location = particle )
    return Selection( name,
                      Algorithm = TrackFilter,
                      RequiredSelections = [ stdTightTrack ] )

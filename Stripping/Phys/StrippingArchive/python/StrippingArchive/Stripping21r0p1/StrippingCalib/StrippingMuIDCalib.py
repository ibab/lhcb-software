## #####################################################################
# A stripping selection for muon identification calibration trough
# the study of J/psi(1S) -> mu+ mu-
## #####################################################################

'''
This line can be invoked by doing:

from StrippingSelections.StrippingMuIDCalib import MuIDCalibConf as builder
from StrippingSelections.StrippingMuIDCalib import config_params as config_params
lb = builder( 'MuIDCalib', config_params )

'''

__author__  = [ 'S.Furcas', 'G.Lanfranchi', 'M.Palutan', 'A.Sarti', 'D.Milanes', 'MuID Team' ]
__date__    = '23/08/2011'
__version__ = '$Revision: 1.5 $'

__all__     = ( 'MuIDCalibConf',
                'makePromptSelection',
                'makeDetachedSelection',
                'makeDetachedNoMIPSelection', 
                'makeDetachedNoMIPNoPCutSelection', 
                'makeDetachedNoMIPHiPSelection', 
                'makeDetachedNoMIPKSelection',
                'makeLambdacSelection',
                'makeTISTOS',
                'default_config'
                )

default_config = {
    'NAME'        : 'MuIDCalib',
    'WGs'         : ['ALL'],
    'BUILDERTYPE' : 'MuIDCalibConf',
    'CONFIG'      : {'PromptPrescale'           : 0., #not used anymore 0.08 old prescale
                     'DetachedPrescale'         : 0.,
                     'DetachedNoMIPPrescale'    : 1, #0.3,
                     'DetachedNoMIPHiPPrescale' : 0.,
                     'DetachedNoMIPKPrescale'   : 1.,
                     'FromLambdacPrescale'      : 1.,
                     'KFromLambdacPrescale'     : 0.,
                     'PiFromLambdacPrescale'    : 0.,
                     'PFromLambdacPrescale'     : 0.,
                     'KISMUONFromLambdacPrescale' : 0.,
                     'PiISMUONFromLambdacPrescale': 0.,
                     'PISMUONFromLambdacPrescale' : 0.},
    'STREAMS'     :  { 'PID' : ['StrippingMuIDCalib_JpsiFromBNoPIDNoMip', 
                                'StrippingMuIDCalib_JpsiKFromBNoPIDNoMip', 
                                'StrippingMuIDCalib_FromLambdacDecay'] }
    }

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StrippingConf.StrippingLine import StrippingLine
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence, MergedSelection
from Configurables import LoKi__VoidFilter as VoidFilter
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsKaons, StdNoPIDsMuons, StdNoPIDsPions, StdNoPIDsProtons
from StandardParticles import StdAllNoPIDsKaons, StdAllNoPIDsMuons, StdAllNoPIDsPions, StdAllNoPIDsProtons

class MuIDCalibConf( LineBuilder ):
    __configuration_keys__ = ('PromptPrescale',           
                              'DetachedPrescale',         
                              'DetachedNoMIPPrescale',    
                              'DetachedNoMIPHiPPrescale', 
                              'DetachedNoMIPKPrescale',   
                              'FromLambdacPrescale',      
                              'KFromLambdacPrescale',     
                              'PiFromLambdacPrescale',        
                              'PFromLambdacPrescale',         
                              'KISMUONFromLambdacPrescale',   
                              'PiISMUONFromLambdacPrescale',  
                              'PISMUONFromLambdacPrescale'    
                              )

    def __init__( self, name, config ) :
        LineBuilder.__init__( self, name, config )


        self.selStdAllNoPIDMuons = StdAllNoPIDsMuons 
        self.selStdNoPIDMuons    = StdNoPIDsMuons 
        #self.selStdNoPIDMuons_L0   = makeTISTOS( name + "muons_L0"  , self.selStdNoPIDMuons   ,   "L0.*Physics.*Decision%TIS"   )
        #self.selStdNoPIDMuons_Hlt1 = makeTISTOS( name + "muons_Hlt1", self.selStdNoPIDMuons_L0,   "Hlt1.*Physics.*Decision%TIS" )
        #self.selStdNoPIDMuons_Hlt2 = makeTISTOS( name + "muons_Hlt2", self.selStdNoPIDMuons_Hlt1, "Hlt2.*Physics.*Decision%TIS" )

        self.selStdNoPIDKaons    = StdNoPIDsKaons 
        self.selStdNoPIDPions    = StdNoPIDsPions
        self.selStdNoPIDProtons  = StdNoPIDsProtons

        #Prompt line
        self.sel_Prompt  = makePromptSelection( name + "_Combine", self.selStdAllNoPIDMuons )
        self.line_Prompt = StrippingLine( name + '_JpsiNoPID', 
                                          prescale = config[ 'PromptPrescale' ], 
                                          selection = self.sel_Prompt, 
                                          RequiredRawEvents = ["Muon"],
                                          RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_Prompt.algorithm().DecayDescriptor)  ] 
                                          ) 

        #Detached
        self.sel_Detached  = makeDetachedSelection( name + "_FromBCombine", self.selStdNoPIDMuons)
        self.line_Detached = StrippingLine( name + '_JpsiFromBNoPID', 
                                            prescale = config[ 'DetachedPrescale' ], 
                                            selection = self.sel_Detached, 
                                            RequiredRawEvents = ["Muon"],
                                            RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_Detached.algorithm().DecayDescriptor)  ]
                                            ) 

        self.sel_DetachedNoMIP  = makeDetachedNoMIPSelection( name + "_FromBNoMipCombine", self.selStdNoPIDMuons )
        self.line_DetachedNoMIP = StrippingLine( name + '_JpsiFromBNoPIDNoMip', 
                                                 prescale = config[ 'DetachedNoMIPPrescale' ], 
                                                 selection = self.sel_DetachedNoMIP, 
                                                 RequiredRawEvents = ["Muon"],
                                                 RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_DetachedNoMIP.algorithm().DecayDescriptor)  ]
                                                 ) 
                 
        self.sel_DetachedNoMIPHiP  = makeDetachedNoMIPHiPSelection( name + "_FromBNoMipHiPCombine", self.selStdNoPIDMuons )
        self.line_DetachedNoMIPHiP = StrippingLine( name + '_JpsiFromBNoPIDNoMipHiP', 
                                                    prescale = config[ 'DetachedNoMIPHiPPrescale' ], 
                                                    selection = self.sel_DetachedNoMIPHiP, 
                                                    RequiredRawEvents = ["Muon"],
                                                    RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_DetachedNoMIPHiP.algorithm().DecayDescriptor)  ]
                                                    ) 

        self.sel_DetachedNoMIPNoPCut  = makeDetachedNoMIPNoPCutSelection( name + "_FromBNoMipCombineNoPCut", self.selStdNoPIDMuons )
        self.sel_DetachedNoMIPK  = makeDetachedNoMIPKSelection( name + "_FromBNoMipWithKCombine", self.sel_DetachedNoMIPNoPCut, self.selStdNoPIDKaons )
        self.line_DetachedNoMIPK = StrippingLine( name + '_JpsiKFromBNoPIDNoMip', 
                                                  prescale = config[ 'DetachedNoMIPKPrescale' ], 
                                                  selection = self.sel_DetachedNoMIPK, 
                                                  MDSTFlag = True,
                                                  RequiredRawEvents = ["Muon"],
                                                  RelatedInfoTools = [  addRelInfoMuonIsolation('[B+ ->  ( J/psi(1S) -> mu+ mu-) K+ ]CC')  ]
                                                  ) 

        #lambda_c
        self.sel_Lambdac = makeLambdacSelection( name + '_FromLambdac', "", self.selStdNoPIDPions, self.selStdNoPIDKaons, self.selStdNoPIDProtons)
        self.line_Lambdac = StrippingLine( name + '_FromLambdacDecay', 
                                           prescale = config[ 'FromLambdacPrescale' ], 
                                           selection = self.sel_Lambdac, 
                                           MDSTFlag = True,
                                           RequiredRawEvents = ["Muon"],
                                           RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_Lambdac.algorithm().DecayDescriptor)  ]
                                           ) 

        self.sel_PiLambdac = makeLambdacSelection( name + '_PiFromLambdac', "pion", self.selStdNoPIDPions, self.selStdNoPIDKaons, self.selStdNoPIDProtons)
        self.line_PiLambdac = StrippingLine( name + '_PiFromLambdacDecay', 
                                             prescale = config[ 'PiFromLambdacPrescale' ], 
                                             selection = self.sel_PiLambdac, 
                                             RequiredRawEvents = ["Muon"],
                                             RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_PiLambdac.algorithm().DecayDescriptor)  ]
                                             ) 

        self.sel_KLambdac = makeLambdacSelection( name + '_KFromLambdac', "kaon", self.selStdNoPIDPions, self.selStdNoPIDKaons, self.selStdNoPIDProtons)
        self.line_KLambdac = StrippingLine( name + '_KFromLambdacDecay', 
                                            prescale = config[ 'KFromLambdacPrescale' ], 
                                            selection = self.sel_KLambdac, 
                                            RequiredRawEvents = ["Muon"],
                                            RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_KLambdac.algorithm().DecayDescriptor)  ]
                                            ) 

        self.sel_PLambdac = makeLambdacSelection( name + '_PFromLambdac', "proton", self.selStdNoPIDPions, self.selStdNoPIDKaons, self.selStdNoPIDProtons)
        self.line_PLambdac = StrippingLine( name + '_PFromLambdacDecay', 
                                            prescale = config[ 'PFromLambdacPrescale' ], 
                                            selection = self.sel_PLambdac, 
                                            RequiredRawEvents = ["Muon"],
                                            RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_PLambdac.algorithm().DecayDescriptor)  ]
                                            ) 

        self.sel_PiISMUONLambdac = makeLambdacSelection( name + '_PiISMUONFromLambdac', "pionISMUON", 
                                                         self.selStdNoPIDPions, self.selStdNoPIDKaons, self.selStdNoPIDProtons)
        self.line_PiISMUONLambdac = StrippingLine( name + '_PiISMUONFromLambdacDecay', 
                                                   prescale = config[ 'PiISMUONFromLambdacPrescale' ], 
                                                   selection = self.sel_PiISMUONLambdac, 
                                                   RequiredRawEvents = ["Muon"],
                                                   RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_PiISMUONLambdac.algorithm().DecayDescriptor)  ]
                                                   ) 

        self.sel_KISMUONLambdac = makeLambdacSelection( name + '_KISMUONFromLambdac', "kaonISMUON", 
                                                        self.selStdNoPIDPions, self.selStdNoPIDKaons, self.selStdNoPIDProtons)
        self.line_KISMUONLambdac = StrippingLine( name + '_KISMUONFromLambdacDecay', 
                                                  prescale = config[ 'KISMUONFromLambdacPrescale' ], 
                                                  selection = self.sel_KISMUONLambdac, 
                                                  RequiredRawEvents = ["Muon"],
                                                  RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_KISMUONLambdac.algorithm().DecayDescriptor)  ]
                                                  ) 

        self.sel_PISMUONLambdac = makeLambdacSelection( name + '_PISMUONFromLambdac', "protonISMUON"
                                                        , self.selStdNoPIDPions, self.selStdNoPIDKaons, self.selStdNoPIDProtons)
        self.line_PISMUONLambdac = StrippingLine( name + '_PISMUONFromLambdacDecay', 
                                                  prescale = config[ 'PISMUONFromLambdacPrescale' ], 
                                                  selection = self.sel_PISMUONLambdac, 
                                                  RequiredRawEvents = ["Muon"],
                                                  RelatedInfoTools = [addRelInfoMuonIsolation(self.sel_PISMUONLambdac.algorithm().DecayDescriptor)  ]
                                                  ) 

        #self.registerLine( self.line_Prompt )
        #self.registerLine( self.line_Detached )
        self.registerLine( self.line_DetachedNoMIP )
        #self.registerLine( self.line_DetachedNoMIPHiP )
        self.registerLine( self.line_DetachedNoMIPK )
        self.registerLine( self.line_Lambdac )
        #self.registerLine( self.line_PiLambdac )
        #self.registerLine( self.line_KLambdac )
        #self.registerLine( self.line_PLambdac )
        #self.registerLine( self.line_PiISMUONLambdac )
        #self.registerLine( self.line_KISMUONLambdac )
        #self.registerLine( self.line_PISMUONLambdac )



'''
Selection Makers
'''
def makePromptSelection( name, muons ):
    '''
    Create Jpsi -> mumu candidates out of no pid muons
    TAG:: IsMuon and P>6Gev and Pt>1.5 GeV
    PROBE:: Ecal (-10,1000) and Hcal (1000,4000) energy filtered
    '''
    mucocut = '(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) & (P>3*GeV) & (PT>800*MeV) & (TRCHI2DOF<3) & (ISLONG)'
    tag1cuts = " (CHILDCUT(ISMUON,1)) & (CHILDCUT((P>6*GeV),1)) & (CHILDCUT((PT>1.5*GeV),1)) "
    tag2cuts = " (CHILDCUT(ISMUON,2)) & (CHILDCUT((P>6*GeV),2)) & (CHILDCUT((PT>1.5*GeV),2)) "
    probe2cuts = " ( (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<1000*MeV),2)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<4000*MeV),2)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV),2)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>1000*MeV),2)) ) "
    probe1cuts = " ( (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<1000*MeV),1)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<4000*MeV),1)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV),1)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>1000*MeV),1)) )  "
    child1cuts = tag1cuts + " & " + probe2cuts + " & " + probe1cuts
    child2cuts = tag2cuts + " & " + probe1cuts + " & " + probe2cuts

    combination = CombineParticles( DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                    DaughtersCuts = { 'mu+' : mucocut , 'mu-' : mucocut },
                                    CombinationCut = "(ADAMASS('J/psi(1S)')<200*MeV)",
                                    MotherCut = "(VFASPF(VCHI2/VDOF)<5) & ( " + child1cuts + " | " + child2cuts + " ) "
                                    )
    return Selection( name, 
                      Algorithm = combination,
                      RequiredSelections = [muons] )


def makeDetachedSelection( name, muons ):
    '''
    Create b -> Jpsi -> mumu candidates out of no pid muons
    TAG:: IsMuon and P>6Gev and Pt>1.5 GeV
    PROBE:: Ecal (-10,1000) and Hcal (1000,4000) energy filtered
    '''
    mucocut = '(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) & (P>3*GeV) & (PT>800*MeV) & (TRCHI2DOF<3) & (ISLONG) &(MIPDV(PRIMARY)>0.050) & (MIPCHI2DV(PRIMARY)>25)'
    tag1cuts = " (CHILDCUT(ISMUON,1)) & (CHILDCUT((P>6*GeV),1)) & (CHILDCUT((PT>1.5*GeV),1)) & (CHILDCUT((MIPDV(PRIMARY)>0.12),1)) "
    tag2cuts = " (CHILDCUT(ISMUON,2)) & (CHILDCUT((P>6*GeV),2)) & (CHILDCUT((PT>1.5*GeV),2)) & (CHILDCUT((MIPDV(PRIMARY)>0.12),2)) "
    probe2cuts = " ( (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<1000*MeV),2)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<4000*MeV),2)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV),2)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>1000*MeV),2)) ) "
    probe1cuts = " ( (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)<1000*MeV),1)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)<4000*MeV),1)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloEcalE,-10000)>-10*MeV),1)) & (CHILDCUT((PPINFO(LHCb.ProtoParticle.CaloHcalE,-10000)>1000*MeV),1)) )  "
    child1cuts = tag1cuts + " & " + probe2cuts 
    child2cuts = tag2cuts + " & " + probe1cuts 

    combination = CombineParticles( DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                    DaughtersCuts = { 'mu+' : mucocut , 'mu-' : mucocut },
                                    CombinationCut = "(ADAMASS('J/psi(1S)')<200*MeV)",
                                    MotherCut = "(VFASPF(VCHI2/VDOF)<5) & (BPVVDCHI2 > 225) & ( ( " + child1cuts + " ) | (" + child2cuts + " ) ) "
                                    )
    return Selection( name, 
                      Algorithm = combination,
                      RequiredSelections = [muons] )


def makeDetachedNoMIPSelection( name, muons ):
    '''
    Create b -> Jpsi -> mumu candidates out of no pid muons without mip cuts
    TAG:: IsMuon and P>6Gev and Pt>1.5 GeV & IpChi2>10
    '''
    #(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) &
    mucocut = '(P>3*GeV) & (PT>800*MeV) & (TRCHI2DOF<3) & (ISLONG) & (MIPCHI2DV(PRIMARY)>10)'
    tag1cuts = " (CHILDCUT(ISMUON,1)) & (CHILDCUT((P>6*GeV),1)) & (CHILDCUT((PT>1.5*GeV),1)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>25),1)) "
    tag2cuts = " (CHILDCUT(ISMUON,2)) & (CHILDCUT((P>6*GeV),2)) & (CHILDCUT((PT>1.5*GeV),2)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>25),2)) "

    combination = CombineParticles( DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                    DaughtersCuts = { 'mu+' : mucocut , 'mu-' : mucocut },
                                    CombinationCut = "(ADAMASS('J/psi(1S)')<200*MeV)",
                                    MotherCut = "(VFASPF(VCHI2/VDOF)<5) & (BPVVDCHI2 > 225) & ( ( " + tag1cuts + " ) | (" + tag2cuts + " ) ) "
                                    )
    return Selection( name, 
                      Algorithm = combination,
                      RequiredSelections = [muons] )

def makeDetachedNoMIPNoPCutSelection( name, muons ):
    '''
    Create b -> Jpsi -> mumu candidates out of no pid muons without mip cuts
    TAG:: IsMuon and P>6Gev and Pt>1.5 GeV & IpChi2>10
    '''
    #(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) & '(P>3*GeV) & (PT>800*MeV)
    mucocut = '(TRCHI2DOF<3) & (ISLONG) & (MIPCHI2DV(PRIMARY)>10)'
    tag1cuts = " (CHILDCUT(ISMUON,1)) & (CHILDCUT((P>6*GeV),1)) & (CHILDCUT((PT>1.5*GeV),1)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>25),1)) "
    tag2cuts = " (CHILDCUT(ISMUON,2)) & (CHILDCUT((P>6*GeV),2)) & (CHILDCUT((PT>1.5*GeV),2)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>25),2)) "

    combination = CombineParticles( DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                    DaughtersCuts = { 'mu+' : mucocut , 'mu-' : mucocut },
                                    CombinationCut = "(ADAMASS('J/psi(1S)')<200*MeV)",
                                    MotherCut = "(VFASPF(VCHI2/VDOF)<5) & (BPVVDCHI2 > 225) & ( ( " + tag1cuts + " ) | (" + tag2cuts + " ) ) "
                                    )
    return Selection( name, 
                      Algorithm = combination,
                      RequiredSelections = [muons] )

def makeDetachedNoMIPHiPSelection( name, muons ):
    '''
    Create b -> Jpsi -> mumu candidates out of no pid muons without mip cuts
    TAG:: IsMuon and P>6Gev and Pt>1.5 GeV & IpChi2>10
    '''
    #(0.5<PPINFO(LHCb.ProtoParticle.InAccMuon,-1)) &
    mucocut = '(P>3*GeV) & (PT>800*MeV) & (TRCHI2DOF<3) & (ISLONG) & (MIPCHI2DV(PRIMARY)>10)'
    tag1cuts = " (CHILDCUT(ISMUON,1)) & (CHILDCUT((P>6*GeV),1)) & (CHILDCUT((PT>1.5*GeV),1)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>25),1)) "
    tag2cuts = " (CHILDCUT(ISMUON,2)) & (CHILDCUT((P>6*GeV),2)) & (CHILDCUT((PT>1.5*GeV),2)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>25),2)) "

    probe1cuts = "(CHILDCUT(P>40*GeV,1))"
    probe2cuts = "(CHILDCUT(P>40*GeV,2))"

    child1cuts = tag1cuts + " & " + probe2cuts 
    child2cuts = tag2cuts + " & " + probe1cuts 

    combination = CombineParticles( DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                    DaughtersCuts = { 'mu+' : mucocut , 'mu-' : mucocut },
                                    CombinationCut = "(ADAMASS('J/psi(1S)')<200*MeV)",
                                    MotherCut = "(VFASPF(VCHI2/VDOF)<5) & (BPVVDCHI2 > 225) & ( ( " + child1cuts + " ) | (" + child2cuts + " ) ) "
                                    )
    return Selection( name, 
                      Algorithm = combination,
                      RequiredSelections = [muons] )


def makeDetachedNoMIPKSelection( name, jpsis, kaons ):
    jpsi_cuts = "(VFASPF(VCHI2/VDOF)<5) &  (MIPCHI2DV(PRIMARY)>25)"
    combination = CombineParticles( DecayDescriptor = '[B+ -> J/psi(1S) K+]cc',
                                    DaughtersCuts = { "K+": "(MIPCHI2DV(PRIMARY)>25)" , 'J/psi(1S)' : jpsi_cuts },
                                    CombinationCut = "ADAMASS('B+') < 500.*MeV",
                                    MotherCut = "(BPVIPCHI2()<25)"
                                    )
    return Selection( name, 
                      Algorithm = combination,
                      RequiredSelections = [ jpsis, kaons ] )


def makeLambdacSelection( name, line, pions, kaons, protons ) :
    daucuts = "( PT>250*MeV ) & ( P>2*GeV ) & ( MIPCHI2DV(PRIMARY)>8. ) & ( TRPCHI2>0.0001 )"
    kcuts   = daucuts
    picuts  = daucuts
    pcuts   = daucuts

    kPID   = " & ( (PIDK - PIDpi)>10.0 )"
    piPID  = " & ( (PIDK - PIDpi)<-5.0 )"
    pPID   = " & ( (PIDp - PIDpi)>10.0 )"
    ismuon = " & (ISMUON)"

    if( line.startswith( "pion" ) ) :
        kcuts += kPID
        pcuts += pPID
        if( line.endswith( "ISMUON" ) ) :
            picuts += ismuon
    if( line.startswith( "kaon" ) ) :
        picuts += piPID
        pcuts  += pPID
        if( line.endswith( "ISMUON" ) ) :
            kcuts += ismuon
    if( line.startswith( "proton" ) ) :
        kcuts  += kPID
        picuts += piPID
        if( line.endswith( "ISMUON" ) ) :
            pcuts += ismuon

    combcuts = "( ADAMASS('Lambda_c+')<150*MeV ) & ( APT>1.*GeV ) & ( ADOCACHI2CUT(50, '') )"
    momcuts  = "( M > 2.240*GeV ) & ( M<2.330*GeV ) & ( VFASPF(VCHI2/VDOF)<8 ) & ( BPVDIRA>0.99999 ) & ( MIPCHI2DV(PRIMARY)<4. )"
    momcuts += " & in_range( 0.85*GeV, M13, 0.95*GeV ) & ( (WM( 'K-' , 'pi+' , 'pi+' )>1.89*GeV) |  (WM( 'K-' , 'pi+' , 'pi+' )<1.80*GeV) )" #D+->Kpipi removal
    LambdaC = CombineParticles ( 
        DecayDescriptor = "[Lambda_c+ -> K- p+ pi+]cc",
        DaughtersCuts = {
            "K-"  : kcuts,
            "p+"  : pcuts,
            "pi+" : picuts
            },
        CombinationCut = combcuts,
        MotherCut = momcuts
        )

    return Selection( name, 
                      Algorithm = LambdaC,
                      RequiredSelections = [ pions, kaons, protons ] )


def makeTISTOS( name, sel, trigger ) :
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger( name + "_Tagger" )
    tisTosFilter.TisTosSpecs = { trigger : 0 }
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    #tisTosFilter.TOSFrac = { 4:0.0, 5:0.0 }
    return Selection( name,
                      Algorithm = tisTosFilter,
                      RequiredSelections = [ sel ]
                      )



def addRelInfoMuonIsolation( decdes ):    
    import re
    _DauLoc={}
    _daughters = re.match(r'(.*)->([ |\[]*)([^\]]+)(.*)', decdes)
    if _daughters:    
        _particles = _daughters.group(3).split()
        _ip=1
        _gp=1
        for _p in _particles:
            if re.match(r'(pi|p|K|e|mu)[\+|-]',_p):
                _key= _daughters.group(1)+"->"+_daughters.group(2)
                _jp=1
                for _p2 in _particles:
                    _key+=" "
                    if _jp==_ip: _key+="^"
                    _key+=_p2
                    _jp=_jp+1
                _key+=_daughters.group(4).replace("cc","CC")
                _DauLoc[_key] = "MudetIso"+str(_gp)
                _gp=_gp+1
            _ip=_ip+1
    else:
        return {}
    return {  "Type" : "RelInfoMuonIsolation", "DaughterLocations" : _DauLoc}

## #####################################################################
# A stripping selection for muon identification calibration trough
# the study of J/psi(1S) -> mu+ mu-
## #####################################################################

__author__  = [ 'S.Furcas', 'G.Lanfranchi', 'M.Palutan', 'A.Sarti', 'MuID Team' ]
__date__    = '02/14/2011'
__version__ = '$Revision: 1.2 $'
__all__     = ( 'MuIDCalibConf',
                'makePromptSelection',
                'makeDetachedSelection',
                'makeDetachedNoMIPSelection', 
                'makeDetachedNoMIPHiPSelection', 
                'makeDetachedNoMIPKSelection'
                )

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop, CombineParticles
from StrippingConf.StrippingLine import StrippingLine
from PhysSelPython.Wrappers import DataOnDemand, Selection, SelectionSequence, MergedSelection
from Configurables import LoKi__VoidFilter as VoidFilter
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory
from StrippingUtils.Utils import LineBuilder
from StandardParticles import StdNoPIDsKaons, StdNoPIDsMuons

default_name = 'MuIDCalib'
class MuIDCalibConf( LineBuilder ):
    __configuration_keys__ = ('PromptPrescale',           #0.08  #0.3 or more
                              'DetachedPrescale',         #1.    #0.
                              'DetachedNoMIPPrescale',    #0.25  #1.
                              'DetachedNoMIPHiPPrescale', #1.    #0.
                              'DetachedNoMIPKPrescale',   #1.    #0.
                              )

    def __init__( self, name, config ) :
        LineBuilder.__init__( self, name, config )


        self.selStdNoPIDMuons = StdNoPIDsMuons #DataOnDemand( Location = "Phys/StdNoPIDsMuons" )
        self.selStdNoPIDKaons = StdNoPIDsKaons #DataOnDemand( Location = "Phys/StdNoPIDsKaons" )

        #GhostLines to isolate cpu time
        #self.line_mu = StrippingLine( 'Mu', prescale = 1, selection = self.selStdNoPIDMuons ) 
        #self.line_K  = StrippingLine( 'K' , prescale = 1, selection = self.selStdNoPIDKaons ) 


        self.sel_Prompt  = makePromptSelection( name + "_Combine", self.selStdNoPIDMuons )
        self.line_Prompt = StrippingLine( name + '_JpsiNoPID', prescale = config[ 'PromptPrescale' ], selection = self.sel_Prompt ) 

        self.sel_Detached  = makeDetachedSelection( name + "_FromBCombine", self.selStdNoPIDMuons )
        self.line_Detached = StrippingLine( name + '_JpsiFromBNoPID', prescale = config[ 'DetachedPrescale' ], selection = self.sel_Detached ) 

        self.sel_DetachedNoMIP  = makeDetachedNoMIPSelection( name + "_FromBNoMipCombine", self.selStdNoPIDMuons )
        self.line_DetachedNoMIP = StrippingLine( name + '_JpsiFromBNoPIDNoMip', prescale = config[ 'DetachedNoMIPPrescale' ], selection = self.sel_DetachedNoMIP ) 
      
        self.sel_DetachedNoMIPHiP  = makeDetachedNoMIPHiPSelection( name + "_FromBNoMipHiPCombine", self.selStdNoPIDMuons )
        self.line_DetachedNoMIPHiP = StrippingLine( name + '_JpsiFromBNoPIDNoMipHiP', prescale = config[ 'DetachedNoMIPHiPPrescale' ], selection = self.sel_DetachedNoMIPHiP ) 

        self.sel_DetachedNoMIPK  = makeDetachedNoMIPKSelection( name + "_FromBNoMipWithKCombine", self.sel_DetachedNoMIP, self.selStdNoPIDKaons )
        self.line_DetachedNoMIPK = StrippingLine( name + '_JpsiKFromBNoPIDNoMip', prescale = config[ 'DetachedNoMIPKPrescale' ], selection = self.sel_DetachedNoMIPK ) 
  

        #self.registerLine( self.line_mu ) #GhostLines
        #self.registerLine( self.line_K )  #GhostLines

        self.registerLine( self.line_Prompt )
        self.registerLine( self.line_Detached )
        self.registerLine( self.line_DetachedNoMIP )
        self.registerLine( self.line_DetachedNoMIPHiP )
        self.registerLine( self.line_DetachedNoMIPK )




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
                                    CombinationCut = "(ADAMASS('J/psi(1S)')<150*MeV)",
                                    MotherCut = "(VFASPF(VCHI2/VDOF)<10) & ( " + child1cuts + " | " + child2cuts + " ) "
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
                                    CombinationCut = "(ADAMASS('J/psi(1S)')<150*MeV)",
                                    MotherCut = "(VFASPF(VCHI2/VDOF)<10) & (BPVVDCHI2 > 225) & ( ( " + child1cuts + " ) | (" + child2cuts + " ) ) "
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
    mucocut = '(P>3*GeV) & (PT>800*MeV) & (TRCHI2DOF<3) & (ISLONG) & (MIPCHI2DV(PRIMARY)>25)'
    tag1cuts = " (CHILDCUT(ISMUON,1)) & (CHILDCUT((P>6*GeV),1)) & (CHILDCUT((PT>1.5*GeV),1)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>10),1)) "
    tag2cuts = " (CHILDCUT(ISMUON,2)) & (CHILDCUT((P>6*GeV),2)) & (CHILDCUT((PT>1.5*GeV),2)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>10),2)) "

    combination = CombineParticles( DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                    DaughtersCuts = { 'mu+' : mucocut , 'mu-' : mucocut },
                                    CombinationCut = "(ADAMASS('J/psi(1S)')<200*MeV)",
                                    MotherCut = "(VFASPF(VCHI2/VDOF)<8) & (BPVVDCHI2 > 225) & ( ( " + tag1cuts + " ) | (" + tag2cuts + " ) ) "
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
    mucocut = '(P>3*GeV) & (PT>800*MeV) & (TRCHI2DOF<3) & (ISLONG) & (MIPCHI2DV(PRIMARY)>25)'
    tag1cuts = " (CHILDCUT(ISMUON,1)) & (CHILDCUT((P>6*GeV),1)) & (CHILDCUT((PT>1.5*GeV),1)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>10),1)) "
    tag2cuts = " (CHILDCUT(ISMUON,2)) & (CHILDCUT((P>6*GeV),2)) & (CHILDCUT((PT>1.5*GeV),2)) & (CHILDCUT((MIPCHI2DV(PRIMARY)>10),2)) "

    probe1cuts = "(CHILDCUT(P>40*GeV,1))"
    probe2cuts = "(CHILDCUT(P>40*GeV,2))"

    child1cuts = tag1cuts + " & " + probe2cuts 
    child2cuts = tag2cuts + " & " + probe1cuts 

    combination = CombineParticles( DecayDescriptor = 'J/psi(1S) -> mu+ mu-',
                                    DaughtersCuts = { 'mu+' : mucocut , 'mu-' : mucocut },
                                    CombinationCut = "(ADAMASS('J/psi(1S)')<200*MeV)",
                                    MotherCut = "(VFASPF(VCHI2/VDOF)<8) & (BPVVDCHI2 > 225) & ( ( " + child1cuts + " ) | (" + child2cuts + " ) ) "
                                    )
    return Selection( name, 
                      Algorithm = combination,
                      RequiredSelections = [muons] )


def makeDetachedNoMIPKSelection( name, jpsis, kaons ):
    jpsi_cuts = "(VFASPF(VCHI2/VDOF)<15) &  (MIPCHI2DV(PRIMARY)>25)"
    combination = CombineParticles( DecayDescriptor = '[B+ -> J/psi(1S) K+]cc',
                                    DaughtersCuts = { "K+": "(MIPCHI2DV(PRIMARY)>25)" , 'J/psi(1S)' : jpsi_cuts },
                                    CombinationCut = "ADAMASS('B+') < 500.*MeV",
                                    MotherCut = "(BPVIPCHI2()<25)"
                                    )
    return Selection( name, 
                      Algorithm = combination,
                      RequiredSelections = [ jpsis, kaons ] )















################################################################################
"""
    ADDING BDT TO TUPLE

    Example script to run a BDT at DaVinci level over a uDST.
    The exact BDT is used to distinguish:
        D+ -> K- pi+ pi+
    originating from a B. More details available if required.
    We use the example decay:
        [B+ -> ^(D+ -> ^pi+ ^pi+ ^K-) ^(rho(770)0 -> ^pi+ ^pi-)]CC'
    from the B2DPiPiD2HHHCFPIDBeauty2CharmLine.

    Workflow goes as the following:
        - Create selection from stripping using FilterDesktop
        - Create tuple
        - Add branch for the D to the tuple
        - Add Dict2Tuple tool (adds elements of dictionary to tuple)
        - Add TMVAClassifier to Dict2Tuple tool
            - TMVAClassifier is a DictTransform
        - Configure TMVA options
        - Add variables to TMVA, must be named the same as in the .xml
        - Will run over a few files at cern by default

    Tutorial begins at TUTORIAL

    N.B. labX notation for input BDT is used with this LoKi notation of CHILD

"""

__title__ = "mvaDfromB.py"
__author__ = ["Sam Hall", "Sebastian Neubert"]
__email__ = ["shall@cern.ch", "sneubert@cern.ch"]


################################################################################
# Imports
################################################################################
from DecayTreeTuple.Configuration import *

from Configurables import DaVinci
from Configurables import FilterDesktop

from PhysSelPython.Wrappers import Selection
from PhysSelPython.Wrappers import SelectionSequence
from PhysSelPython.Wrappers import DataOnDemand

################################################################################
# Config
simulation = False
rootInTES = '/Event/Bhadron'
DaVinci(RootInTES=rootInTES, InputType='MDST')
################################################################################

################################################################################
# Imports for LoKi functors
from LoKiPhys.decorators import *
from LoKiArrayFunctors.decorators import *
from LoKiProtoParticles.decorators import *
#from LoKiCore.decorators import *  # For debugging in ipython
################################################################################

def labvar(labX, var):
    """Quick function to return formatted lab_variable"""
    return 'lab{}_{}'.format(labX, var)


################################################################################

def get_preambulo():
    """Add shorthands to prambulo to make LoKi functors easier to read"""
    preambulo = [
        'fitVeloChi2 = TINFO(LHCb.Track.FitVeloChi2,-1)',
        'fitVeloNdof = TINFO(LHCb.Track.FitVeloNDoF,-1)',
        'fitTChi2 = TINFO(LHCb.Track.FitTNDoF,-1)',
        'fitTNdof = TINFO(LHCb.Track.FitTChi2,-1)',
    ]
    return preambulo


################################################################################

def get_bdt_vars():
    """Return all variables required for the BDT
    Variable names MUST correspond exactly to what is needed by classifier (xml)
    If they are unknown: they are in the xml, and they will be shown in stdout
    """
    labX = 2  # lab number of parent D
    ndau = 3  # number of daughters
    bdt_vars = {}
    # Variables for D and daughters; labX_ prefix added later
    vars_parent = {
        'P' : 'P',
        'PT' : 'PT',
        'ENDVERTEX_CHI2' : 'VFASPF(VCHI2)',
        'IPCHI2_OWNPV' : 'MIPCHI2DV(PRIMARY)',
        'FDCHI2_OWNPV' : 'BPVVDCHI2',
    }
    vars_daughters = {
        'P' : 'CHILD(P,{0})',
        'PT' : 'CHILD(PT,{0})',
        'PE' : 'CHILD(E,{0})',
        'PX' : 'CHILD(PX,{0})',
        'PY' : 'CHILD(PY,{0})',
        'PZ' : 'CHILD(PZ,{0})',
        'IPCHI2_OWNPV' : 'CHILD(MIPCHI2DV(PRIMARY),{0})',
        # If NDOF > 0 then CHi2/NDOF else -1
        'TRACK_VeloCHI2NDOF' : 'switch(CHILD(fitVeloNdof,{0})>0,CHILD(fitVeloChi2,{0})/CHILD(fitVeloNdof,{0}),-1)',
        'TRACK_TCHI2NDOF' : 'switch(CHILD(fitTNdof,{0})>0,CHILD(fitTChi2,{0})/CHILD(fitTNdof,{0}),-1)',
        'TRACK_MatchCHI2' : 'CHILD(TINFO(LHCb.Track.FitMatchChi2,-1.),{0})',
        'TRACK_GhostProb' : 'CHILD(TRGHOSTPROB,{0})',
        'UsedRichAerogel' : 'switch(CHILDCUT(PPCUT(PP_USEDAEROGEL),{0}),1,0)',
        'UsedRich1Gas' : 'switch(CHILDCUT(PPCUT(PP_USEDRICH1GAS),{0}),1,0)',
        'UsedRich2Gas' : 'switch(CHILDCUT(PPCUT(PP_USEDRICH2GAS),{0}),1,0)',
        'RichAbovePiThres' : 'switch(CHILDCUT(PPCUT(PP_RICHTHRES_PI),{0}),1,0)',
        'RichAboveKaThres' : 'switch(CHILDCUT(PPCUT(PP_RICHTHRES_K),{0}),1,0)',
        'RichAbovePrThres' : 'switch(CHILDCUT(PPCUT(PP_RICHTHRES_P),{0}),1,0)',
        'RichDLLe'  : 'CHILD(PPINFO(LHCb.ProtoParticle.RichDLLe,-1000),{0})',
        'RichDLLmu' : 'CHILD(PPINFO(LHCb.ProtoParticle.RichDLLmu,-1000),{0})',
        'RichDLLk'  : 'CHILD(PPINFO(LHCb.ProtoParticle.RichDLLk,-1000),{0})',
        'RichDLLp'  : 'CHILD(PPINFO(LHCb.ProtoParticle.RichDLLp,-1000),{0})',
        'RichDLLbt' : 'CHILD(PPINFO(LHCb.ProtoParticle.RichDLLbt,-1000),{0})',
        'MuonLLbg'  : 'CHILD(PPINFO(LHCb.ProtoParticle.MuonBkgLL,-1000),{0})',
        'MuonLLmu'  : 'CHILD(PPINFO(LHCb.ProtoParticle.MuonMuLL,-1000),{0})',
        'isMuon' : 'switch(CHILDCUT(ISMUON,{0}),1,0)',
        'MuonNShared' : 'CHILD(PPINFO(LHCb.ProtoParticle.MuonNShared,-1000),{0})',
        'VeloCharge' : 'CHILD(PPINFO(LHCb.ProtoParticle.VeloCharge,-1000),{0})',
    }
    # Add all parent D variables to output
    for var, loki in vars_parent.iteritems():
        bdt_vars.update({labvar(labX, var) : loki})
    # Add all daughter variables to output
    for child, lab in enumerate(range(labX + 1, labX + ndau + 1)):
        for var, loki in vars_daughters.iteritems():
            bdt_vars.update({labvar(lab, var) : loki.format(child + 1)})
    # Print out variables for sanity
    for key in sorted(bdt_vars):
        print '{:<25} : {}'.format(key, bdt_vars[key].replace(',', ', '))
    print 80 * '-'
    return bdt_vars


################################################################################

def get_selection_sequence(name):
    """Get the selection from stripping stream"""
    # Relative lesLoc for uDST, no preceeding '/'
    tesLoc = 'Phys/B2DPiPiD2HHHCFPIDBeauty2CharmLine/Particles'
    alg = FilterDesktop('SelFilterFor{}B2D'.format(name))
    alg.Code = 'ALL'
    reqSels = [DataOnDemand(Location=tesLoc)]
    sel = Selection('Sel' + name, Algorithm=alg, RequiredSelections=reqSels)
    return SelectionSequence('SelSeq' + name, TopSelection=sel)


################################################################################

def decay_tree_tuple(name, sel_seq, decay):
    """Create simple DecayTree"""
    tpl = DecayTreeTuple('{}DTTuple'.format(name))
    tpl.Inputs = [sel_seq.outputLocation()]
    tpl.ToolList = [
        'TupleToolTrigger',
        'TupleToolKinematic',
        'TupleToolPid',
        'TupleToolGeometry',
        'TupleToolPrimaries',
    ]
    # Output distributions for pions do not match unless labX == True
    tpl.UseLabXSyntax = False  # True
    tpl.RevertToPositiveID = False
    tpl.Decay = decay
    return tpl

################################################################################
# Make selection
################################################################################
name = 'b2dpipi'
decay = '[B+ -> ^(D+ -> ^K- ^pi+ ^pi+) ^(rho(770)0 -> ^pi+ ^pi-)]CC'

seq = GaudiSequencer('My{}DTTupleSeq'.format(name))
sel = get_selection_sequence(name)
seq.Members += [sel.sequence()]

# Make tuple
tpl = decay_tree_tuple(name, sel, decay)

################################################################################
# TUTORIAL
################################################################################
# Add branch of the decay where BDT is added
#  - in this case adding to (D+ -> K- pi+ pi+)
tpl.addBranches({'D' : '[B+ -> ^(D+ -> K- pi+ pi+) (rho(770)0 -> pi+ pi-)]CC'})

LoKi_D = tpl.D.addTupleTool('LoKi::Hybrid::TupleTool/LoKi_D')

# THIS IS THE LINE:
#  pass the banch,, xml, variables, name, and whether you want all variables
#  in the nTuple (default to False), add preamulo if you wish
from MVADictHelpers import *
addTMVAclassifierTuple(tpl.D, 'bdt_d2kpipi.xml', get_bdt_vars(),
                       Name='BDT', Keep=True, Preambulo=get_preambulo())

################################################################################
seq.Members += [tpl]
DaVinci().appendToMainSequence([seq])
################################################################################
# DaVinci
#  uDST config at the top
DaVinci().Lumi = True
DaVinci().DDDBtag = 'dddb-20130111'
DaVinci().CondDBtag = 'cond-20130114'
DaVinci().Simulation = simulation
DaVinci().EvtMax = -1
DaVinci().SkipEvents = 0
DaVinci().DataType = '2012'
DaVinci().PrintFreq = 1000
DaVinci().TupleFile = 'bdt_d.root'
################################################################################

################################################################################
# Run on some eos data located at CERN
################################################################################
from Configurables import EventSelector
DaVinci().EvtMax = 10000
eos = 'root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/BHADRON.MDST'
EventSelector().Input = [
    'PFN:{}/00020198/0000/00020198_00007143_1.bhadron.mdst'.format(eos),
    'PFN:{}/00020198/0000/00020198_00007352_1.bhadron.mdst'.format(eos),
    'PFN:{}/00020738/0000/00020738_00003969_1.bhadron.mdst'.format(eos),
    'PFN:{}/00020456/0000/00020456_00001811_1.bhadron.mdst'.format(eos)
]
################################################################################

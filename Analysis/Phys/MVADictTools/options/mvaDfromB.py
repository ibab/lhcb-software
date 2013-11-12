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

"""

__author__ = ["Sam Hall", "Sebastian Neubert"]
__email__ = ["shall@cern.ch", "sneubert@cernch"]


################################################################################
# Imports
################################################################################
from DecayTreeTuple.Configuration import *

from Configurables import LHCbApp
from Configurables import DaVinci
from Configurables import CombineParticles
from Configurables import FilterDesktop
from Configurables import TupleToolPid

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
#from LoKiCore.decorators import *
from LoKiPhys.decorators import *
from LoKiProtoParticles.decorators import *
from LoKiArrayFunctors.decorators import *
################################################################################

def get_preambulo():
    """Add shorthands to prambulo to make LoKi functors easier to read"""
    # No preambulo for MVATransform - yet
    preambulo = [
        "fitVeloChi2 = TINFO(LHCb.Track.FitVeloChi2,-1)",
        "fitVeloNdof = TINFO(LHCb.Track.FitVeloNDoF,-1)",
        "fitTChi2 = TINFO(LHCb.Track.FitTNDoF,-1)",
        "fitTNdof = TINFO(LHCb.Track.FitTChi2,-1)",
    ]
    return preambulo

################################################################################


def get_bdt_vars():
    """Return all variables required for the BDT
    Variable names MUST correspond exactly to what is needed by classifier (xml)
    If they are unknown, they are in the xml, and they will be shown in stdout
    """
    labX = 2
    ndau = 3
    bdt_vars = {}
    # Variables for D and daughters; labX_ prefix added later
    vars_parent = {
        "P" : "P",
        "PT" : "PT",
        "ENDVERTEX_CHI2" : "VFASPF(VCHI2)",
        "IPCHI2_OWNPV" : "MIPCHI2DV(PRIMARY)",  ## ?
        "FDCHI2_OWNPV" : "BPVVDCHI2",
    }
    vars_daughters = {
        "P"  : "CHILD(P,%d)",
        "PT" : "CHILD(PT,%d)",
        "PE" : "CHILD(E,%d)",
        "PX" : "CHILD(PX,%d)",
        "PY" : "CHILD(PY,%d)",
        "PZ" : "CHILD(PZ,%d)",
        "IPCHI2_OWNPV" : "CHILD(MIPCHI2DV(PRIMARY),%d)",
        "TRACK_VeloCHI2NDOF" : "switch(%s,%s/%s,-1)" % (
            "CHILD(TINFO(LHCb.Track.FitVeloNDoF,-1),%d)>0", # IF
            "CHILD(TINFO(LHCb.Track.FitVeloChi2,-1),%d)",   # THEN x/y
            "CHILD(TINFO(LHCb.Track.FitVeloNDoF,-1),%d)"
        ),
        "TRACK_TCHI2NDOF" : "switch(%s,%s/%s,-1)" % (
            "CHILD(TINFO(LHCb.Track.FitTNDoF,-1),%d)>0", # IF
            "CHILD(TINFO(LHCb.Track.FitTChi2,-1),%d)",   # THEN x/y
            "CHILD(TINFO(LHCb.Track.FitTNDoF,-1),%d)"
        ),
        # If Preambulo works
        #"TRACK_VeloCHI2NDOF" : "switch(CHILD(fitVeloNdof,%d)>0,CHILD(fitVeloChi2,%d)/CHILD(fitVeloNdof,%d),-1)",
        #"TRACK_TCHI2NDOF" : "switch(CHILD(fitTNdof,%d)>0, CHILD(fitTChi2,%d)/CHILD(fitTNdof,%d), -1)",
        "TRACK_MatchCHI2" : "CHILD(TINFO(LHCb.Track.FitMatchChi2,-1.),%d)",
        "TRACK_GhostProb" : "CHILD(TRGHOSTPROB,%d)",
        "UsedRichAerogel" : "switch(CHILDCUT(PPCUT(PP_USEDAEROGEL),%d),1,0)",
        "UsedRich1Gas" : "switch(CHILDCUT(PPCUT(PP_USEDRICH1GAS),%d),1,0)",
        "UsedRich2Gas" : "switch(CHILDCUT(PPCUT(PP_USEDRICH2GAS),%d),1,0)",
        "RichAbovePiThres" : "switch(CHILDCUT(PPCUT(PP_RICHTHRES_PI),%d),1,0)",
        "RichAboveKaThres" : "switch(CHILDCUT(PPCUT(PP_RICHTHRES_K),%d),1,0)",
        "RichAbovePrThres" : "switch(CHILDCUT(PPCUT(PP_RICHTHRES_P),%d),1,0)",
        "RichDLLe"  : "CHILD(PPINFO(LHCb.ProtoParticle.RichDLLe,-1000),%d)",
        "RichDLLmu" : "CHILD(PPINFO(LHCb.ProtoParticle.RichDLLmu,-1000),%d)",
        "RichDLLk"  : "CHILD(PPINFO(LHCb.ProtoParticle.RichDLLk,-1000),%d)",
        "RichDLLp"  : "CHILD(PPINFO(LHCb.ProtoParticle.RichDLLp,-1000),%d)",
        "RichDLLbt" : "CHILD(PPINFO(LHCb.ProtoParticle.RichDLLbt,-1000),%d)",
        "MuonLLbg"  : "CHILD(PPINFO(LHCb.ProtoParticle.MuonBkgLL,-1000),%d)",
        "MuonLLmu"  : "CHILD(PPINFO(LHCb.ProtoParticle.MuonMuLL,-1000),%d)",
        "isMuon" : "switch(CHILDCUT(ISMUON,%d),1,0)",
        "MuonNShared" : "CHILD(PPINFO(LHCb.ProtoParticle.MuonNShared,-1000),%d)",
        "VeloCharge" : "CHILD(PPINFO(LHCb.ProtoParticle.VeloCharge,-1000),%d)",
    }
    # Add all parent D variables to output
    lab = 'lab%d_' % labX
    for key, item in vars_parent.iteritems():
        bdt_vars.update({lab + key : item})
    # Add all daughter variables to output
    for child, lab in enumerate(range(labX + 1, labX + ndau + 1)):
        lab_temp = 'lab%d_' % lab
        for key, item in vars_daughters.iteritems():
            # Replace the child number for every instance of CHILD
            replacements = tuple(child + 1 for x in range(item.count('CHILD')))
            #replacements = tuple(child_to_decay[child + 1] for x in range(item.count('CHILD')))
            if len(replacements):
                bdt_vars.update({lab_temp + key : item % replacements})
            else:
                bdt_vars.update({lab_temp + key : item})
    # Print out variables
    for key in sorted(bdt_vars):
        print key.ljust(25), " : ", bdt_vars[key].replace(',', ', ')
    print 80 * '-'
    return bdt_vars

################################################################################


def get_selection_sequence(name):
    """Get the selection from stripping stream"""
    #tesLoc = '/Event/Bhadron/Phys/B2DPiPiD2HHHCFPIDBeauty2CharmLine/Particles'
    # Relative lesLoc for uDST, no preceeding '/'
    tesLoc = 'Phys/B2DPiPiD2HHHCFPIDBeauty2CharmLine/Particles'
    alg = FilterDesktop('SelFilterFor%sB2C' % name)
    alg.Code = "ALL"
    reqSels = [DataOnDemand(Location=tesLoc)]
    sel = Selection('Sel%s' % name, Algorithm=alg, RequiredSelections=reqSels)
    return SelectionSequence('SelSeq%s' % name, TopSelection=sel)

################################################################################


def decay_tree_tuple(name, sel_seq, decay):
    """Create simple DecayTree"""
    tpl = DecayTreeTuple("%sDTTuple" % name)
    tpl.Inputs = [sel_seq.outputLocation()]
    tpl.ToolList = [
        "TupleToolTrigger",
        #"TupleToolTISTOS",
        "TupleToolKinematic",
        "TupleToolPid",
        "TupleToolGeometry",
        "TupleToolPrimaries"
        #"TupleToolTrackInfo",
        #"TupleToolPropertime",
        #"TupleToolRICHPid",
        #"TupleToolMuonPid"
    ]
    # Output distributions for pions do not match unless labX == True
    tpl.UseLabXSyntax = False  # True
    tpl.RevertToPositiveID = False
    tpl.Decay = decay
    return tpl

################################################################################
# Make selection
################################################################################
name = "b2dpipi"
decay = '[B+ -> ^(D+ -> ^pi+ ^pi+ ^K-) ^(rho(770)0 -> ^pi+ ^pi-)]CC'

seq = GaudiSequencer("My%sDTTupleSeq" % name)
sel = get_selection_sequence(name)
seq.Members += [sel.sequence()]

# Make tuple
tpl = decay_tree_tuple(name, sel, decay)

################################################################################
# Imports for BDT
from Configurables import LoKi__Hybrid__DictOfFunctors
from Configurables import LoKi__Hybrid__Dict2Tuple
from Configurables import LoKi__Hybrid__DictTransform_TMVATransform_ as TMVAClassifier
from Configurables import LoKi__Hybrid__TupleTool
################################################################################
# Add branch of the decay where BDT is added
#  - in this case adding to (D+ -> K- pi+ pi+)
tpl.addBranches({"D" : "[B+ -> ^(D+ -> pi+ pi+ K-) (rho(770)0 -> pi+ pi-)]CC"})

LoKi_D = tpl.D.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_D")

# Write MVA responce to nTuple, all variables prefixed by D_
D_tmva = tpl.D.addTupleTool(LoKi__Hybrid__Dict2Tuple, "TMVA2Tuple")

# Add the TMVAClassifier to the Dict2Tuple
D_tmva.addTool(TMVAClassifier, "TMVA")
D_tmva.Source = "LoKi__Hybrid__DictTransform_TMVATransform_/TMVA"

# Configure the classifier (available options depend upon classifier)
D_tmva.TMVA.Options = {
    "XMLFile"    : "bdt_d2kpipi.xml", # TMVA uses an xml file to load classifier
    "Name"       : "MyBDT",     # Name of responce variable in nTuple, D_MyBDT
    "KeepVars"   : "1",         # Write out input variables 0/1
}

# Add DictOfFunctors as source of classifier
# MultiTool uses LoKiFunctors to query required variables
D_tmva.TMVA.addTool(LoKi__Hybrid__DictOfFunctors, "MVAdict")
D_tmva.TMVA.Source = "LoKi__Hybrid__DictOfFunctors/MVAdict"
D_tmva.TMVA.MVAdict.Variables = get_bdt_vars()  # See above


################################################################################
seq.Members += [tpl]
DaVinci().appendToMainSequence([seq])
################################################################################
# DaVinci
DaVinci().Lumi = True
DaVinci().DDDBtag  = "dddb-20130111"
DaVinci().CondDBtag  = "cond-20130114"
DaVinci().Simulation = simulation
DaVinci().EvtMax = -1
DaVinci().SkipEvents = 0
DaVinci().DataType = '2012'
DaVinci().PrintFreq = 1000
DaVinci().TupleFile = "bdt_d.root"
################################################################################


################################################################################
# Run on some eos data stored at CERN
################################################################################
from Configurables import EventSelector
DaVinci().EvtMax = 10000
eos = 'root://eoslhcb.cern.ch//eos/lhcb/LHCb/Collision12/BHADRON.MDST'
EventSelector().Input = [
    "PFN:%s/00020198/0000/00020198_00007143_1.bhadron.mdst" % eos,
    "PFN:%s/00020198/0000/00020198_00007352_1.bhadron.mdst" % eos,
    "PFN:%s/00020738/0000/00020738_00003969_1.bhadron.mdst" % eos,
    "PFN:%s/00020456/0000/00020456_00001811_1.bhadron.mdst" % eos
]
################################################################################

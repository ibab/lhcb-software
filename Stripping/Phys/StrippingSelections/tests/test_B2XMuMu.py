# $Id: $
# Test your line(s) of the stripping
#  
# NOTE: Please make a copy of this file for your testing, and do NOT change this one!
#

from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf

# Tighten Trk Chi2 to <3
from CommonParticles.Utils import DefaultTrackingCuts
DefaultTrackingCuts().Cuts  = { "Chi2Cut" : [ 0, 3 ],
                                "CloneDistCut" : [5000, 9e+99 ] }

from Configurables import DecayTreeTuple, FitDecayTrees, TupleToolRecoStats, TupleToolTrigger, TupleToolTISTOS, CondDB, SelDSTWriter
from DecayTreeTuple.Configuration import *
# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

# Append your line



from StrippingSelections.StrippingB2XMuMu import B2XMuMuConf
from StrippingSelections.StrippingB2XMuMu import defaultConfig


#inclusiveConfig['DIRA_HIGHQ2'] = -0.90
#inclusiveConfig['DIRA_LOWQ2'] = 0.99
#inclusiveConfig['VertexCHI2'] = 2.0
#inclusiveConfig['Muon_PIDmu'] = 2
#inclusiveConfig['Muon_MinIPCHI2'] = 16
#inclusiveConfig['Muon_PT'] = 800
#inclusiveConfig['WS'] = False
#inclusiveConfig['LOWERMASS_LOWQ2'] = 1500
#inclusiveConfig['UPPERMASS_LOWQ2'] = 2200

IncXMuMuBuilder = B2XMuMuConf( name="B2XMuMu", config=defaultConfig )
stream.appendLines( IncXMuMuBuilder.lines() )
IncXMuMuBuilder2 = B2XMuMuConf( name="B2XMuMu2", config=defaultConfig )
#stream.appendLines( IncXMuMuBuilder2.lines() )

# Standard configuration of Stripping, do NOT change them
from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents,
                    TESPrefix = 'Strip'
                    )

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

tuple = DecayTreeTuple("Jpsi_Tuple")

tuple.Inputs = ["Phys/B2XMuMu_Line/Particles"]
#tuple.Inputs = ["Phys/B2XMuMuInclusive2_InclDiMuLowQ2Line/Particles"]


tuple.ToolList =  [
      "TupleToolKinematic"
    , "TupleToolEventInfo"
    , "TupleToolRecoStats"
   # , "TupleBuKmmFit"
]


tuple.addBranches ({         
      "muplus" :  "[B+ -> (J/psi(1S) -> ^mu+ mu-) K+]CC",
      "muminus" :  "[B+ -> (J/psi(1S) -> mu+ ^mu-) K+]CC",
      "Jpsi" :  "[B+ -> ^(J/psi(1S) -> mu+ mu-) K+]CC",
      "Kplus" :  "[B+ -> J/psi(1S) ^K+]CC",
      "Bplus" : "[B+ -> J/psi(1S) K+]CC",
})
LoKi_All=tuple.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_All")
LoKi_All.Variables = {
        'MINIPCHI2' : "MIPCHI2DV(PRIMARY)", 
        'MINIP' : "MIPDV(PRIMARY)",
        'IPCHI2_OWNPV' : "BPVIPCHI2()", 
        'IP_OWNPV' : "BPVIP()"
}

LoKi_muplus=tuple.muplus.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_muplus")
LoKi_muplus.Variables = {
       'PIDmu' : "PIDmu",
       'ghost' : "TRGHP",
       'TRACK_CHI2' : "TRCHI2DOF",
       'NNK' : "PPINFO(PROBNNK)",
       'NNpi' : "PPINFO(PROBNNpi)",
       'NNmu' : "PPINFO(PROBNNmu)"
}

LoKi_Kplus=tuple.Kplus.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_Kplus")
LoKi_Kplus.Variables = {
       'PIDK' : "PIDK",
       'PIDmu' : "PIDmu",
       'ghost' : "TRGHP",
       'TRACK_CHI2' : "TRCHI2DOF",
       'NNK' : "PPINFO(PROBNNK)",
       'NNpi' : "PPINFO(PROBNNpi)",
       'NNmu' : "PPINFO(PROBNNmu)"
}

LoKi_muminus=tuple.muminus.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_Kminus")
LoKi_muminus.Variables = {
       'PIDK' : "PIDK",
       'PIDmu' : "PIDmu",
       'ghost' : "TRGHP",
       'TRACK_CHI2' : "TRCHI2DOF",
       'NNK' : "PPINFO(PROBNNK)",
       'NNpi' : "PPINFO(PROBNNpi)",
       'NNmu' : "PPINFO(PROBNNmu)"
}


LoKi_Bplus=tuple.Bplus.addTupleTool("LoKi::Hybrid::TupleTool/LoKi_Bplus")
LoKi_Bplus.Variables = {
       'DTF_CHI2' : "DTF_CHI2NDOF(True)",
       'TAU' : "BPVLTIME()",
       'DIRA_OWNPV' : "BPVDIRA",
       'FD_CHI2' : "BPVVDCHI2",
       'Mcorr' : "BPVCORRM",
       'ENDVERTEX_CHI2' : "VFASPF(VCHI2/VDOF)",
       "CONE_angle" : "RELINFO('Phys/B2XMuMu_Line/ConeIsoInfo','CONEANGLE', -1.)",
       "CONE_PT" : "RELINFO('Phys/B2XMuMu_Line/ConeIsoInfo','CONEPT', -1.)",
       "BDTIso" : "RELINFO('Phys/B2XMuMu_Line/VtxIsoBDTInfo','VTXISOBDTHARDFIRSTVALUE', -1.)",
       "NormalVtxIso" : "RELINFO('Phys/B2XMuMu_Line/VtxIsoInfo','VTXISONUMVTX', -1.)"
}


list = [
      "L0DiMuonDecision"
    , "L0MuonDecision"
    , "Hlt1TrackAllL0Decision"
    , "Hlt1TrackMuonDecision"
    , "Hlt1DiMuonLowMassDecision"
    , "Hlt1DiMuonHighMassDecision"
    , "Hlt1SingleMuonHighPTDecision"
    , "Hlt2TopoMu2BodyBBDTDecision"
    , "Hlt2TopoMu3BodyBBDTDecision"
    , "Hlt2Topo2BodyBBDTDecision"
    , "Hlt2Topo3BodyBBDTDecision"
    , "Hlt2DiMuonDetachedDecision"
    , "Hlt2SingleMuonDecision"
    , "Hlt2DiMuonDetachedHeavyDecision"
]

tuple.Decay = "[B+ -> ^(J/psi(1S) -> ^mu+ ^mu-) ^K+]CC"

#DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().EvtMax = 100000
DaVinci().PrintFreq = 2000
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ tuple ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().DataType  = "2012"
DaVinci().InputType = "DST"

# change the column size of timing table
from Configurables import TimingAuditor, SequencerTimerTool
TimingAuditor().addTool(SequencerTimerTool,name="TIMER")
TimingAuditor().TIMER.NameSize = 60
NTupleSvc().Output = ["FILE1 DATAFILE='rootfile.root' TYP='ROOT' OPT='NEW'"]
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

# database
DaVinci().DDDBtag  = "dddb-20120831"
DaVinci().CondDBtag = "cond-20121008"

# input file
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/Reco14_Run125113.py")

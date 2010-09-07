from Gaudi.Configuration import *
                       
########   B+ -> (D0 -> K3pi) K+   ########
from Configurables import DecayTreeTuple
Bu2D0K_D02K3PiTuple=DecayTreeTuple('Bu2D0K_D02K3PiTuple')
Bu2D0K_D02K3PiTuple.Decay='[B+ -> (^D0 => [^K- ^pi+ ^pi- ^pi+]cc) ^K+]cc'
Bu2D0K_D02K3PiTuple.Branches = {
  'D0' : '[B+ -> (^D0 => [K- pi+ pi- pi+]cc) K+]cc'
  ,'Kaon' : '[B+ -> (D0 => [^K- pi+ pi- pi+]cc) K+]cc'
  ,'Pion' : '[B+ -> (D0 => [K- ^pi+ ^pi- ^pi+]cc) K+]cc'
  ,'Bachelor' : '[B+ -> (D0 => [K- pi+ pi- pi+]cc) ^K+]cc'
  ,'B' : '[B+]cc : [B+ -> (D0 => [K- pi+ pi- pi+]cc) K+]cc'
}

Bu2D0K_D02K3PiTuple.TupleName='Bu2D0K_D02K3Pi'
Bu2D0K_D02K3PiTuple.Context="Offline"

from Configurables import LoKi__Hybrid__TupleTool as LoKiTupleTool

from Configurables import LoKi__Hybrid__EvtTupleTool as LoKiEvtTupleTool

# number of tracks in event
trackEvtVars={"nTracks" : "TrSOURCE('Rec/Track/Best') >> TrSIZE"
           #,"nTracks_Long" : "TrSOURCE('Rec/Track/Best') >> TrLONG >> TrSIZE"
           #,"nTracks_Up" : "TrSOURCE('Rec/Track/Best') >> TrUPSTREAM >> TrSIZE"
           #,"nTracks_Down" : "TrSOURCE('Rec/Track/Best') >> TrDOWNSTREAM >> TrSIZE"
           #,"nTracks_Back" : "TrSOURCE('Rec/Track/Best') >> TrBACKWARD >> TrSIZE"

}
# mass and momentum variables 
massvars={ "M" : "M/GeV", "MM" : "MM/GeV", "P" : "P/GeV", "PT" : "PT/GeV"
           , "PX" : "PX/GeV", "PY" : "PY/GeV", "PZ" : "PZ/GeV"}

# impact paramater variables
ipvars={ #"MINIP_PV" : "MIPDV(PRIMARY)", 
  "IP_BESTPV" : "BPVIP()" }
ipchi2vars={ #"MINIPCHI2_PV" : "MIPCHI2DV(PRIMARY)", 
  "IPCHI2_BESTPV" : "BPVIPCHI2()" }

# flight distance variables
fdvars= { #"MINFD_PV" : "MINVDDV(PRIMARY)", 
          "FD_BESTPV" : "BPVVD", 
          "FD_BESTPV_SIGNED" : "BPVVDSIGN"}
fdchi2vars = { #"MINFDCHI2_PV" : "MINVDCHI2DV(PRIMARY)",
  "FDCHI2_BESTPV" : "BPVVDCHI2" }

# vertex chi2 variables
vertexvars={ "VTXCHI2DOF" : "VFASPF(VCHI2/VDOF)", "VTXCHI2" : "VFASPF(VCHI2)"
             , "VTXDOF" : "VFASPF(VDOF)"}

# track variables
trackvars={ "TRCHI2DOF" : "TRCHI2DOF", "TRGHOSTPROB" : "TRGHP" }

# lifetime variables
lifetimevars={ 
   "LIFETIME_BESTPV" : "BPVLTIME('PropertimeFitter/ProperTime::PUBLIC')" 
   ,"LIFETIMECHI2_BESTPV" : "BPVLTCHI2('PropertimeFitter/ProperTime::PUBLIC')"
   ,"LIFETIMECHI2_BESTPV_SIGNED" : "BPVLTSIGNCHI2('PropertimeFitter/ProperTime::PUBLIC')"
   ,"LIFETIMEFITCHI2_BESTPV" : "BPVLTFITCHI2('PropertimeFitter/ProperTime::PUBLIC')"}        

# rapidity and pseudorapidity
rapidityvars={"ETA" : "ETA", "RAPIDITY" : "Y"}

from Configurables import TupleToolDecay
for branch in Bu2D0K_D02K3PiTuple.Branches.keys():
  Bu2D0K_D02K3PiTuple.addTool(TupleToolDecay, name=branch)

#### global variables ####
Bu2D0K_D02K3PiTuple.ToolList = [
  'TupleToolPrimaries'
  ,'TupleToolEventInfo'
  ]

#### Configurable variables for B+- ####
Bu2D0K_D02K3PiTuple.B.ToolList=[
  "TupleToolPid"
  ,"LoKi::Hybrid::TupleTool/LoKiTool_B"
  ]

LoKiTool_B=LoKiTupleTool('LoKiTool_B')
LoKiTool_B.Variables = {
  "Q" : "Q",
  "DMASS" : "DMASS('B+')/MeV",
  "MAXDOCA" : "DOCAMAX",
#  "SUMPT" : "((CHILD(1, PT))+(CHILD(2, PT)))/GeV",
#  "SUMP" : "((CHILD(1, P))+(CHILD(2, P)))/GeV",
  "DIRA_BESTPV" : "BPVDIRA",
  }
LoKiTool_B.Variables.update(massvars)
LoKiTool_B.Variables.update(ipvars)
LoKiTool_B.Variables.update(ipchi2vars)
LoKiTool_B.Variables.update(fdvars)
LoKiTool_B.Variables.update(fdchi2vars)
LoKiTool_B.Variables.update(vertexvars)
LoKiTool_B.Variables.update(lifetimevars)
LoKiTool_B.Variables.update(rapidityvars)

Bu2D0K_D02K3PiTuple.B.addTool(LoKiTool_B)

#### Configurable variables for D0 ####

Bu2D0K_D02K3PiTuple.D0.ToolList=[
  "TupleToolPid"
  ,"LoKi::Hybrid::TupleTool/LoKiTool_D0"
  ]

LoKiTool_D0=LoKiTupleTool("LoKiTool_D0")
LoKiTool_D0.Variables = {
#  "SUMPT" : "((CHILD(1, PT)) + (CHILD(2, PT)) + (CHILD(3, PT)) + (CHILD(4, PT)))/GeV",
#  "SUMP" : "((CHILD(1, P)) + (CHILD(2, P)) + (CHILD(3, P)) + (CHILD(4, P)))/GeV",

  "DMASS" : "DMASS('D0')/MeV",

  "MAXDOCA" : "DOCAMAX",

  "DOCA_K_PI0" : "DOCA(1, 2)",
  "DOCA_K_PI1" : "DOCA(1, 3)",
  "DOCA_K_PI2" : "DOCA(1, 4)",
  "DOCA_PI0_PI1" : "DOCA(2, 3)",
  "DOCA_PI0_PI2" : "DOCA(2, 4)",
  "DOCA_PI1_PI2" : "DOCA(3, 4)",

  "MAXPT_DAUGHTERS" : "MAXTREE(PT, TRUE)/GeV",  
  "MAXP_DAUGHTERS" : "MAXTREE(P, TRUE)/GeV",
  
  "MAXIP_BESTPV_DAUGHTERS" : "MAXTREE(BPVIP(), TRUE)",
  
  "MAXIPCHI2_BESTPV_DAUGHTERS" : "MAXTREE(BPVIPCHI2(), TRUE)",

  "MINPT_DAUGHTERS" : "MINTREE(PT, TRUE)/GeV",
  
  "MINP_DAUGHTERS" : "MINTREE(P, TRUE)/GeV",
  
  "MINIP_BESTPV_DAUGHTERS" : "MINTREE(BPVIP(), TRUE)",
  
  "MINIPCHI2_BESTPV_DAUGHTERS" : "MINTREE(BPVIPCHI2(), TRUE)",

  "DIRA_BESTPV" : "BPVDIRA",
  "DWMASS_KKPIPI" : "DWM('D0', WM('K+', 'K-', 'pi+', 'pi-'))/GeV",
  "DWMASS_4PI" : "DWM('D0', WM('pi+', 'pi-', 'pi+', 'pi-'))/GeV"
  }
LoKiTool_D0.Variables.update(massvars)
LoKiTool_D0.Variables.update(ipvars)
LoKiTool_D0.Variables.update(ipchi2vars)
LoKiTool_D0.Variables.update(fdvars)
LoKiTool_D0.Variables.update(fdchi2vars)
LoKiTool_D0.Variables.update(vertexvars)
LoKiTool_D0.Variables.update(lifetimevars)
LoKiTool_D0.Variables.update(rapidityvars)

Bu2D0K_D02K3PiTuple.D0.addTool(LoKiTool_D0)

#### Configurable variables for stable particles (bachelor and D0 daughters) ####

LoKiTool_Stable=LoKiTupleTool("LoKiTool_Stable")
LoKiTool_Stable.Variables = { "Q" : "Q" }
LoKiTool_Stable.Variables.update(massvars)
LoKiTool_Stable.Variables.update(ipvars)
LoKiTool_Stable.Variables.update(ipchi2vars)
LoKiTool_Stable.Variables.update(trackvars)

# kaons from D0
Bu2D0K_D02K3PiTuple.Kaon.ToolList=[
  "TupleToolPid",
  "LoKi::Hybrid::TupleTool/LoKiTool_Stable"
  ]
Bu2D0K_D02K3PiTuple.Kaon.addTool(LoKiTool_Stable)

# pions from D0
Bu2D0K_D02K3PiTuple.Pion.ToolList=[
  "TupleToolPid",
  "LoKi::Hybrid::TupleTool/LoKiTool_Stable"
  ]
Bu2D0K_D02K3PiTuple.Pion.addTool(LoKiTool_Stable)

# bachelor
Bu2D0K_D02K3PiTuple.addTool(TupleToolDecay,name="Bachelor")

Bu2D0K_D02K3PiTuple.Bachelor.ToolList=[
  "TupleToolPid",
  "LoKi::Hybrid::TupleTool/LoKiTool_Stable"
  ]
Bu2D0K_D02K3PiTuple.Bachelor.addTool(LoKiTool_Stable)

#### add trigger information ####
Bu2D0K_D02K3PiTuple.ToolList+=['TupleToolTrigger/Trigger']
from Configurables import TupleToolTrigger
trigger = TupleToolTrigger('Trigger')
trigger.VerboseL0=True
trigger.VerboseHlt1=True
trigger.VerboseHlt2=True
trigger.FillL0=True
trigger.FillHlt=True
trigger.TriggerList = [
  # HLT1 hadron lines
  'Hlt1SingleHadronDecision', 'Hlt1DiHadronDecision'
  # HLT2 2-body topological lines
  ,'Hlt2Topo2BodySADecision', 'Hlt2TopoTF2BodySADecision'
  ,'Hlt2TopoTF2BodyReq2YesDecision'
  ,'Hlt2TopoTF2BodyReq3YesDecision'
  ,'Hlt2TopoTF2BodyReq4YesDecision'
  ,'Hlt2TopoOSTF2BodyDecision'
  ,'Hlt2TopoRobTF2BodyDecision'
  # HLT2 3-body topological lines
  ,'Hlt2Topo3BodySADecision', 'Hlt2TopoTF3BodySADecision'
  ,'Hlt2TopoTF3BodyReq2YesDecision'
  ,'Hlt2TopoTF3BodyReq3YesDecision'
  ,'Hlt2TopoTF3BodyReq4YesDecision'
  ,'Hlt2TopoOSTF3BodyDecision', 'Hlt2TopoRobTF3BodyDecision'
  # HLT2 4-body topological lines
  ,'Hlt2Topo4BodySADecision', 'Hlt2TopoTF4BodySADecision'
  ,'Hlt2TopoTF4BodyReq2YesDecision'
  ,'Hlt2TopoTF4BodyReq3YesDecision'
  ,'Hlt2TopoTF4BodyReq4YesDecision'
  ,'Hlt2TopoOSTF4BodyDecision', 'Hlt2TopoRobTF4BodyDecision'
  ]

from Configurables import TupleToolTISTOS
tistos = TupleToolTrigger('Trigger')
tistos.VerboseL0=True
tistos.VerboseHlt1=True
tistos.VerboseHlt2=True
tistos.FillL0=True
tistos.FillHlt=True
tistos.TriggerList = [
  # HLT1 hadron lines
  'Hlt1SingleHadronDecision', 'Hlt1DiHadronDecision'
  # HLT2 2-body topological lines
  ,'Hlt2Topo2BodySADecision', 'Hlt2TopoTF2BodySADecision'
  ,'Hlt2TopoTF2BodyReq2YesDecision'
  ,'Hlt2TopoTF2BodyReq3YesDecision'
  ,'Hlt2TopoTF2BodyReq4YesDecision'
  ,'Hlt2TopoOSTF2BodyDecision'
  ,'Hlt2TopoRobTF2BodyDecision'
  # HLT2 3-body topological lines
  ,'Hlt2Topo3BodySADecision', 'Hlt2TopoTF3BodySADecision'
  ,'Hlt2TopoTF3BodyReq2YesDecision'
  ,'Hlt2TopoTF3BodyReq3YesDecision'
  ,'Hlt2TopoTF3BodyReq4YesDecision'
  ,'Hlt2TopoOSTF3BodyDecision', 'Hlt2TopoRobTF3BodyDecision'
  # HLT2 4-body topological lines
  ,'Hlt2Topo4BodySADecision', 'Hlt2TopoTF4BodySADecision'
  ,'Hlt2TopoTF4BodyReq2YesDecision'
  ,'Hlt2TopoTF4BodyReq3YesDecision'
  ,'Hlt2TopoTF4BodyReq4YesDecision'
  ,'Hlt2TopoOSTF4BodyDecision', 'Hlt2TopoRobTF4BodyDecision'
  ]

Bu2D0K_D02K3PiTuple.addTool(trigger)
Bu2D0K_D02K3PiTuple.addTool(tistos)

#### add the number of tracks in event to ntuple
Bu2D0K_D02K3PiTuple.ToolList+=["LoKi::Hybrid::EvtTupleTool/LoKiTool_Tracks"]

Bu2D0K_D02K3PiTuple.addTool(LoKiEvtTupleTool, name="LoKiTool_Tracks")
Bu2D0K_D02K3PiTuple.LoKiTool_Tracks.VOID_Variables=trackEvtVars
Bu2D0K_D02K3PiTuple.LoKiTool_Tracks.Preambulo+=["from LoKiTrigger.decorators import *"]


########   B+ -> (D0 -> KKpipi) K+   ########
Bu2D0K_D02KKPiPiTuple=Bu2D0K_D02K3PiTuple.clone("Bu2D0K_D02KKPiPiTuple")                                        
Bu2D0K_D02KKPiPiTuple.Decay='[B+ -> (^D0 => ^K- ^K+ ^pi- ^pi+) ^K+]cc'
Bu2D0K_D02KKPiPiTuple.Branches = {
  'D0' : '[B+ -> (^D0 => K- K+ pi- pi+) K+]cc'
  ,'Kaon' : '[B+ -> (D0 => ^K- ^K+ pi- pi+) K+]cc'
  ,'Pion' : '[B+ -> (D0 => K- K+ ^pi- ^pi+) K+]cc'
  ,'Bachelor' : '[B+ -> (D0 => K- K+ pi- pi+) ^K+]cc'
  ,'B' : '[B+]cc : [B+ -> (D0 => K- K+ pi- pi+) K+]cc'
}

Bu2D0K_D02KKPiPiTuple.TupleName='Bu2D0K_D02KKPiPi'

#### Configurable variables for D0 ####

Bu2D0K_D02K3PiTuple.D0.ToolList=[
  "TupleToolPid"
  ,"LoKi::Hybrid::TupleTool/LoKiTool_D0_KKPIPI"
  ]

LoKiTool_D0_KKPiPi=LoKiTupleTool("LoKiTool_D0_KKPIPI")
LoKiTool_D0_KKPiPi.Variables = {
#  "SUMPT" : "((CHILD(1, PT)) + (CHILD(2, PT)) + (CHILD(3, PT)) + (CHILD(4, PT)))/GeV",
#  "SUMP" : "((CHILD(1, P)) + (CHILD(2, P)) + (CHILD(3, P)) + (CHILD(4, P)))/GeV",

  "DMASS" : "DMASS('D0')/MeV",

  "MAXDOCA" : "DOCAMAX",

  "DOCA_K_PI0" : "DOCA(1, 2)",
  "DOCA_K_PI1" : "DOCA(1, 3)",
  "DOCA_K_PI2" : "DOCA(1, 4)",
  "DOCA_PI0_PI1" : "DOCA(2, 3)",
  "DOCA_PI0_PI2" : "DOCA(2, 4)",
  "DOCA_PI1_PI2" : "DOCA(3, 4)",

  "MAXPT_DAUGHTERS" : "MAXTREE(PT, TRUE)/GeV",  
  "MAXP_DAUGHTERS" : "MAXTREE(P, TRUE)/GeV",
  
  "MAXIP_BESTPV_DAUGHTERS" : "MAXTREE(BPVIP(), TRUE)",
  
  "MAXIPCHI2_BESTPV_DAUGHTERS" : "MAXTREE(BPVIPCHI2(), TRUE)",

  "MINPT_DAUGHTERS" : "MINTREE(PT, TRUE)/GeV",
  
  "MINP_DAUGHTERS" : "MINTREE(P, TRUE)/GeV",
  
  "MINIP_BESTPV_DAUGHTERS" : "MINTREE(BPVIP(), TRUE)",
  
  "MINIPCHI2_BESTPV_DAUGHTERS" : "MINTREE(BPVIPCHI2(), TRUE)",

  "DIRA_BESTPV" : "BPVDIRA",
  "DWMASS_K3PI" : "DWM('D0', WM('K-', 'pi+', 'pi+', 'pi-'))/GeV",
  "DWMASS_4PI" : "DWM('D0', WM('pi+', 'pi-', 'pi+', 'pi-'))/GeV"
  }
LoKiTool_D0_KKPiPi.Variables.update(massvars)
LoKiTool_D0_KKPiPi.Variables.update(ipvars)
LoKiTool_D0_KKPiPi.Variables.update(ipchi2vars)
LoKiTool_D0_KKPiPi.Variables.update(fdvars)
LoKiTool_D0_KKPiPi.Variables.update(fdchi2vars)
LoKiTool_D0_KKPiPi.Variables.update(vertexvars)
LoKiTool_D0_KKPiPi.Variables.update(lifetimevars)
LoKiTool_D0_KKPiPi.Variables.update(rapidityvars)

Bu2D0K_D02K3PiTuple.D0.addTool(LoKiTool_D0_KKPiPi)

########   B+ -> (D0 -> K3pi) K+   ########
Bu2D0pi_D02K3PiTuple=Bu2D0K_D02K3PiTuple.clone("Bu2D0pi_D02K3PiTuple")
Bu2D0pi_D02K3PiTuple.Decay='[B+ -> (^D0 => [^K- ^pi+ ^pi- ^pi+]cc) ^pi+]cc'
Bu2D0pi_D02K3PiTuple.Branches = {
  'D0' : '[B+ -> (^D0 => [K- pi+ pi- pi+]cc) pi+]cc'
  ,'Kaon' : '[B+ -> (D0 => [^K- pi+ pi- pi+]cc) pi+]cc'
  ,'Pion' : '[B+ -> (D0 => [K- ^pi+ ^pi- ^pi+]cc) pi+]cc'
  ,'Bachelor' : '[B+ -> (D0 => [K- pi+ pi- pi+]cc) ^pi+]cc'
  ,'B' : '[B+]cc : [B+ -> (D0 => [K- pi+ pi- pi+]cc) pi+]cc'
}

Bu2D0pi_D02K3PiTuple.TupleName='Bu2D0pi_D02K3Pi'

########   B+ -> (D0 -> KKpipi) pi+   ########
Bu2D0pi_D02KKPiPiTuple=Bu2D0K_D02KKPiPiTuple.clone("Bu2D0pi_D02KKPiPiTuple")                                        
Bu2D0pi_D02KKPiPiTuple.Decay='[B+ -> (^D0 => ^K- ^K+ ^pi- ^pi+) ^pi+]cc'
Bu2D0pi_D02KKPiPiTuple.Branches = {
  'D0' : '[B+ -> (^D0 => K- K+ pi- pi+) pi+]cc'
  ,'Kaon' : '[B+ -> (D0 => ^K- ^K+ pi- pi+) pi+]cc'
  ,'Pion' : '[B+ -> (D0 => K- K+ ^pi- ^pi+) pi+]cc'
  ,'Bachelor' : '[B+ -> (D0 => K- K+ pi- pi+) ^pi+]cc'
  ,'B' : '[B+]cc : [B+ -> (D0 => K- K+ pi- pi+) pi+]cc'
}

Bu2D0pi_D02KKPiPiTuple.TupleName='Bu2D0pi_D02KKPiPi'

__all__=('Bu2D0pi_D02K3PiTuple', 'Bu2D0pi_D02KKPiPiTuple'
         , 'Bu2D0K_D02K3PiTuple', 'Bu2D0K_D02KKPiPiTuple')

from Bu2D0X_D02HHHH.DecayTreeTuple.MakeTuple import *
for tuple in (Bu2D0pi_D02K3PiTuple, Bu2D0pi_D02KKPiPiTuple
              ,Bu2D0K_D02K3PiTuple, Bu2D0K_D02KKPiPiTuple):
  tuple.ToolList+=['TupleToolMCTruth/MCTruth'
                  ,"MCTupleToolInteractions"
                  ,"MCTupleToolPrimaries"]
  tuple.B.ToolList+=["TupleToolMCBackgroundInfo"]

  from Configurables import TupleToolMCTruth
  mcTruth = TupleToolMCTruth("MCTruth")
  mcTruth.ToolList = [ "MCTupleToolHierarchy"
                       ,"MCTupleToolKinematic"]
  tuple.addTool(mcTruth)

__all__=('Bu2D0pi_D02K3PiTuple', 'Bu2D0pi_D02KKPiPiTuple'
         , 'Bu2D0K_D02K3PiTuple', 'Bu2D0K_D02KKPiPiTuple')


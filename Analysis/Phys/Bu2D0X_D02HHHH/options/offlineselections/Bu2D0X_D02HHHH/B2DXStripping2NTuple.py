from Gaudi.Configuration import *
from Bu2D0X_D02HHHH.OfflineSelections.SelectB2DXStripped import *
from Bu2D0X_D02HHHH.OfflineSelections.SelectB2DXSignalStripped import *
from Bu2D0X_D02HHHH.DecayTreeTuple.MakeTuple import *

from Configurables import GaudiSequencer
SeqMakeTuples=GaudiSequencer("SeqMakeTuples")
SeqMakeTuples.Members=[]

for tuple in (Bu2D0pi_D02K3PiTuple, Bu2D0pi_D02KKPiPiTuple
              ,Bu2D0K_D02K3PiTuple, Bu2D0K_D02KKPiPiTuple):
  tuple.InputLocations=[B2DX_D2HHHHSel.outputLocation()]
  SeqMakeTuples.Members+=[tuple]
  tupleName=tuple.name()[:-5]
  signalTuple = tuple.clone("%sSignalTuple" %tupleName)
  signalTuple.TupleName="%s_Signal" %tupleName
  signalTuple.InputLocations=[B2DX_D2HHHHSignalSel.outputLocation()]
  SeqMakeTuples.Members+=[signalTuple]

from Configurables import DaVinci, LHCbApp

LHCbApp().XMLSummary="summary.xml"
DaVinci().UserAlgorithms=[SeqMakeTuples]
DaVinci().TupleFile="Bu2D0X_D02HHHH.root"
DaVinci().DataType='2010'
DaVinci().Simulation=False
DaVinci().EvtMax=-1

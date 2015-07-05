
from Gaudi.Configuration import *
from Configurables import ( Brunel, GaudiSequencer, RichPIDQCConf )

# Enabled data-on-demand
ApplicationMgr().ExtSvc += [ "DataOnDemandSvc" ]

# The sequencer to run all the PID monitoring in
seq = GaudiSequencer("PIDMoniSeq")

# Set up PID monitoring sequence
pidSeq = GaudiSequencer("RichPIDSelections")
seq.Members += [ pidSeq ]

# Set options
Brunel().setOtherProps( RichPIDQCConf(), ['OutputLevel','Context'] )
RichPIDQCConf().setProp("CalibSequencer",pidSeq)

def addMonitors():
    # Append to processing
    GaudiSequencer("PhysicsSeq").Members += [ seq ]
appendPostConfigAction(addMonitors)

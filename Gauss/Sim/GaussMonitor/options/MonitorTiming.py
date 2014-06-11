# --- Configure the tool
from Configurables import GiGa, GiGaStepActionSequence, MonitorTiming
giga = GiGa()
giga.addTool( GiGaStepActionSequence("StepSeq") , name = "StepSeq" )
giga.StepSeq.addTool( MonitorTiming )
giga.StepSeq.MonitorTiming.OutputLevel = 0
#giga.StepSeq.MonitorTiming.OutputFileName='monitorOut.log' #obsolete: remove!
giga.StepSeq.MonitorTiming.TimerFileName='Timing.log'
giga.StepSeq.MonitorTiming.Volumes=['Velo','TT', 'IT', 'OT', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Rich1', 'Rich2', 'Muon', 'Universe', 'Pipe', 'Magnet', 'Converter']
#giga.StepSeq.MonitorTiming.Volumes=['Velo','TT', 'IT', 'OT', 'Spd', 'Prs', 'Ecal', 'Hcal', 'Rich1', 'Rich2', 'Muon', 'Universe']

#Need to run as post-config option: Add other memebers to StepSeq
giga.StepSeq.Members += ['MonitorTiming']

giga.StepSeq.Members += ['RichG4StepAnalysis6/RichStepHpdRefl']
giga.StepSeq.Members += ['RichG4StepAnalysis4/RichStepAgelExit']
giga.StepSeq.Members += ['RichG4StepAnalysis5/RichStepMirrorRefl']
giga.StepSeq.Members += ['GaussStepAction/GaussStep']
giga.StepSeq.Members += ['RichG4StepAnalysis4/RichStepAgelExit']
giga.StepSeq.Members += ['RichG4StepAnalysis5/RichStepMirrorRefl']
giga.StepSeq.Members += ['RichG4StepAnalysis3/RichStep']

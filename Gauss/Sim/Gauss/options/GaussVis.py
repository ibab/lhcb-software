from Configurables import Gauss, GiGa, GiGaRunManager

### Activate the visualization
GiGa().VisManager = "GiGaVisManager/GiGaVis"

### Activate the Geant4 user interactive control (switch off to run in
### "batch", i.e. to have G4 UI prompt)
GiGa().UIsession  = "GiGaUIsession/GiGaUI"

### setting visualization attributes
GiGa().addTool( GiGaRunManager("GiGaMgr") , name="GiGaMgr" ) 
GiGa().GiGaMgr.RunTools += [ "GiGaSetVisAttributes" ]

### When using DAWN swith off some detectors
Gauss().DetectorGeo  = {'Detectors': ['Velo', 'PuVeto', 'Muon', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Magnet'] }
Gauss().DetectorSim  = {'Detectors': ['Velo', 'PuVeto', 'Muon', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Magnet'] }
Gauss().DetectorMoni = {'Detectors': ['Velo', 'PuVeto', 'Muon', 'TT', 'IT', 'OT', 'Rich1', 'Rich2', 'Magnet'] }

## When Switiching off the Rich siwth off also its physics
#Gauss().PhysicsList = {"Em":'NoCuts', "Hadron":'LHEP', "GeneralPhys":True, "LHCbPhys":False}

### To control level of drawing of track while traveling through detector
## from Configurables import GiGaStepActionSequence
## GiGa().SteppingAction =   "GiGaStepActionSequence/StepSeq"
## GiGa().addTool( GiGaStepActionSequence("StepSeq") , name = "StepSeq" )
## GiGa().StepSeq.Members  += [ "GiGaStepActionDraw/DrawStep" ]

### Switch on when running in "batch", i.e. no G4 UI
## from Configurables import GiGaEventActionSequence 
## GiGa().EventAction = "GiGaEventActionSequence/EventSeq"
## GiGa().addTool( GiGaEventActionSequence("EventSeq") , name="EventSeq" ) 
## GiGa().EventSeq.Members += [ "GiGaEventActionDraw/DrawEvent" ]
## GiGa().EventSeq.Members += [ "GiGaEventActionCommand/VisEvtCmd" ]

### Commands to execute in batch mode
## GiGa().EventSeq.addTool(GiGaEventActionCommand("VisEvtCmd"), name = "VisEvtCmd")
## GiGa().EventSeq.VisEvtCmd.EndOfEventCommands = [
##     "/vis/scene/add/volume",
##     "/vis/scene/add/trajectories",
##     "/vis/viewer/update" ]
## GiGa().EventSeq.VisEvtCmd.BeginOfEventCommands = [
##     "/vis/scene/create", 
##     "/vis/sceneHandler/create DAWNFILE",
##     "/vis/viewer/create",
##     "/vis/viewer/set/viewpointThetaPhi -90 -90",
##     "/vis/scene/add/volume", 
##     "/vis/scene/add/trajectories",
##     "/vis/scene/notifyHandlers" ]

### To dump the geometry tree on an ASCII file, comments lines above for 
### BeginOFEventCommands and uncomment those below
## GiGa().EventSeq.VisEvtCmd.BeginOfEventCommands = [
##     "/vis/ASCIITree/verbose 10",
##     "/vis/drawTree" ]


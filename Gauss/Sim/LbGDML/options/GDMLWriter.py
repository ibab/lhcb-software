##  File with options to set up specific run action to write
##  geometry in GDML ( Geant4 geometry description language)
##
##  @author G.Corti
##  @date 2012-07-12 

from Configurables import GiGa
giga = GiGa()
giga.addTool( GiGaRunActionSequence("RunSeq") , name="RunSeq" )
giga.RunSeq.Members.append( "GDMLRunAction" )

# The following lines have the default settings
#giga.RunSeq.addTool( GDMLRunAction("GDMLRunAction") , name = "GDMLRunAction" )
#giga.RunSeq.GDMLRunAction.Schema = "$G4GDMLROOT/schema/gdml.xsd";
#giga.RunSeq.GDMLRunAction.Output = "LHCb.gdml";

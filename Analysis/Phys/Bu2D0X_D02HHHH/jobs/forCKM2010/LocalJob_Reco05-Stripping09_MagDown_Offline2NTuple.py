nevents=10000
printFreq=10
inputfile="../datacards/Collision10_Reco05-Stripping09_MagDown_Hadronic_Run71805_pfn_cern.py"
outputfile="Bu2D0X_D02HHHH.root"

app=DaVinci(
  version="v25r7"
  ,optsfile=["../../options/offlineselections/Bu2D0X_D02HHHH/B2DXStripping2NTuple.py"]
  ,setupProjectOptions=" --use-grid"
  ,extraopts="""from Gaudi.Configuration import *
from Configurables import DaVinci
DaVinci().EvtMax=%d
DaVinci().PrintFreq=%d""" %(nevents, printFreq)
  )
app.optsfile.append("../../options/DV-Reco05Stripping09_NoOracleDB.py")
app.optsfile.append('../../options/DV-FilterInBeamCrossing.py')
inputdata=app.readInputData(inputfile)
backend=Local()
splitter=None
j=Job(name="Collision10_Reco05-Stripping09_MagDown_Offline2NTuple_ForCKM2010"
      ,application=app
      ,inputdata=inputdata
      ,backend=backend
      ,splitter=splitter
      ,outputsandbox=["summary.xml", outputfile]
      )
j.submit()      

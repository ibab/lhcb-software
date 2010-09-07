nevents=-1
printFreq=500
inputfile="../datacards/Collision10_Reco05-Stripping09_MagUp_Hadronic.py"
outputfile="Bu2D0X_D02HHHH.root"
filesPerJob=10

app=DaVinci(
  version="v25r7"
  ,optsfile=["../../options/offlineselections/Bu2D0X_D02HHHH/B2DXStripping2NTuple.py"]
#  ,setupProjectOptions=" --use-grid"
  ,extraopts="""from Gaudi.Configuration import *
from Configurables import DaVinci
DaVinci().EvtMax=%d
DaVinci().PrintFreq=%d""" %(nevents, printFreq)
  )
app.optsfile.append("../../options/DV-Reco05Stripping09.py")
app.optsfile.append('../../options/DV-FilterInBeamCrossing.py')
inputdata=app.readInputData(inputfile)
backend=Dirac()
splitter=DiracSplitter()
splitter.filesPerJob=filesPerJob
j=Job(name="Collision10_Reco05-Stripping09_MagUp_Offline2NTuple_ForCKM2010"
      ,application=app
      ,inputdata=inputdata
      ,backend=backend
      ,splitter=splitter
      ,outputsandbox=["summary.xml", outputfile]
      )
j.submit()      

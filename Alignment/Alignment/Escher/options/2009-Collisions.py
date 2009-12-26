from GaudiKernel.ProcessJobOptions import importOptions
importOptions("$APPCONFIGOPTS/Brunel/earlyData.py")

from Configurables import ( Escher )

# uncomment if you want to read from the CERN oracle dbase.
# is only possible from lxplus, or by using a grid certificate.
#importOptions("$APPCONFIGOPTS/UseOracle.py")

from Gaudi.Configuration import FileCatalog, EventSelector

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml" ]

from Configurables import ( Escher )
#-- Use latest 2009 database tags for real data
Escher().DataType = "2009"
Escher().SpecialData += ["veloOpen"]
Escher().InputType = "MDF"
Escher().Detectors = ["OT"]
Escher().EvtMax = 100
Escher().AlignmentLevel = "layers"
Escher().Millepede = True
Escher().Kalman = False
Escher().Incident = "GlobalMPedeFit"
Escher().DatasetName = "060784"
Escher().TrackContainer = "Rec/Track/Seed"



# add 2009 collision data files
EventSelector().Input = [
    "DATAFILE='file:/data/data/060784_0000000001.raw' SVC='LHCb::MDFSelector'"
    ]


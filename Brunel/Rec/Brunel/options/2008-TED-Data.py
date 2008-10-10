# Special options for processing TED data taken in September 2008
# Details of these datasets are at https://lbtwiki.cern.ch/bin/view/Computing/VeryFirstData

from Gaudi.Configuration import *
from GaudiConf.Configuration import *
from Brunel.Configuration import *

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]

#-- Main ('signal') event input
inputType   = Brunel().getProp("inputType").upper()

if inputType == "MDF":
    datasetName = "SeptTEDData"
    # friday night with the velo - prev2
    EventSelector().Input = [  
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32474/032474_0000081642.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32476/032476_0000081643.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32477/032477_0000081644.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32479/032479_0000081647.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32481/032481_0000081648.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32484/032484_0000081651.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32493/032493_0000081660.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32498/032498_0000081699.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32500/032500_0000081701.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32501/032501_0000081702.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32576/032576_0000081818.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32578/032578_0000081820.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32580/032580_0000081822.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32581/032581_0000081823.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='rfio:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32583/032583_0000081825.raw'  SVC='LHCb::MDFSelector'"
        ]
    # Above file requires following special options for Velo
    from Configurables import (RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg,
                               DecodeVeloRawBuffer, UpdateManagerSvc )
    DecodeVeloRawBuffer().ForceBankVersion=3
    DecodeVeloRawBuffer().RawEventLocation='Prev2/DAQ/RawEvent'
    DecodeVeloRawBuffer('DecodeVeloClusters').RawEventLocation='Prev2/DAQ/RawEvent'
    DecodeVeloRawBuffer('DecodeVeloClusters').ForceBankVersion=3
    UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-29. ; double ResolPosLA = 29. ;"]
    RawBankToSTClusterAlg('CreateTTClusters').rawEventLocation = "/Event/Prev2/DAQ/RawEvent"
    RawBankToSTLiteClusterAlg('CreateTTLiteClusters').rawEventLocation = "/Event/Prev2/DAQ/RawEvent"
else:
    print "********************************************************************************"
    print "The 2008-TED-Files.py file requires Brunel be set up to process MDF data"
    print "********************************************************************************"


# Set the property, used to build other file names
Brunel().setProp( "datasetName", datasetName )

#-- Save the monitoring histograms
HistogramPersistencySvc().OutputFile = Brunel().histosName()

#-- Possible output streams. Enabled by setting the corresponding Brunel() property
outputName = Brunel().outputName()
OutputStream("DstWriter").Output = "DATAFILE='PFN:" + outputName + "' TYP='POOL_ROOTTREE' OPT='REC'"

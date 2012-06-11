# Special options for processing TED data taken in September 2008
# Details of these datasets are at https://lbtwiki.cern.ch/bin/view/Computing/VeryFirstData

from Gaudi.Configuration import *
from Configurables import Brunel, LHCbApp

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]

#-- Main ('signal') event input
inputType   = Brunel().getProp("InputType").upper()

if inputType == "MDF":
    #-- Use latest database tags for real data
    LHCbApp().DDDBtag   = "default"
    LHCbApp().CondDBtag = "default"

    datasetName = "SeptTEDData"
    # friday night with the velo - prev2
    EventSelector().Input = [  
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32474/032474_0000081642.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32476/032476_0000081643.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32477/032477_0000081644.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32479/032479_0000081647.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32481/032481_0000081648.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32484/032484_0000081651.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32493/032493_0000081660.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32498/032498_0000081699.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32500/032500_0000081701.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32501/032501_0000081702.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32576/032576_0000081818.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32578/032578_0000081820.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32580/032580_0000081822.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32581/032581_0000081823.raw'  SVC='LHCb::MDFSelector'",
        "DATAFILE='root:/castor/cern.ch/grid/lhcb/data/2008/RAW/LHCb/BEAM//32583/032583_0000081825.raw'  SVC='LHCb::MDFSelector'"
        ]
    # Above file requires following special options for Velo
    from Configurables import (RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg,
                               DecodeVeloRawBuffer, UpdateManagerSvc )
    DecodeVeloRawBuffer().ForceBankVersion=3
    DecodeVeloRawBuffer().RawEventLocations= ['Prev2/DAQ/RawEvent']
    DecodeVeloRawBuffer('DecodeVeloClusters').RawEventLocations=['Prev2/DAQ/RawEvent']
    DecodeVeloRawBuffer('DecodeVeloClusters').ForceBankVersion=3
    UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-29. ; double ResolPosLA = 29. ;"]
    RawBankToSTClusterAlg('CreateTTClusters').RawEventLocations = ["/Event/Prev2/DAQ/RawEvent"]
    RawBankToSTLiteClusterAlg('CreateTTLiteClusters').RawEventLocations = ["/Event/Prev2/DAQ/RawEvent"]
else:
    print "********************************************************************************"
    print "The 2008-TED-Files.py file requires Brunel be set up to process MDF data"
    print "********************************************************************************"
    import sys
    sys.exit()

# Default output files names are set up using value Brunel().DatasetName property
Brunel().DatasetName = datasetName

# Redefine defaults by uncommenting one or more of options below 

# Monitoring histograms
#HistogramPersistencySvc().OutputFile = "SomeFile.root"

#-- Dst or rDst file
#OutputStream("DstWriter").Output = "DATAFILE='PFN:someFile.dst' TYP='POOL_ROOTTREE' OPT='REC'"
#OutputStream("DstWriter").Output = "DATAFILE='PFN:someFile.rdst' TYP='POOL_ROOTTREE' OPT='REC'"

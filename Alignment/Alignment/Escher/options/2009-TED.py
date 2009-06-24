# Special options for processing TED data taken in June 2009
# Details of these datasets are at https://lbtwiki.cern.ch/bin/view/VELO/TEDJune2009

from Gaudi.Configuration import *
from Configurables import LHCbApp, Escher
from Configurables import (RawBankToSTClusterAlg, RawBankToSTLiteClusterAlg,
                           DecodeVeloRawBuffer, UpdateManagerSvc )
from Configurables import CondDBAccessSvc, UpdateManagerSvc

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]

#-- Main ('signal') event input
inputType   = Escher().getProp("InputType").upper()

if inputType == "MDF":
    #-- Use latest database tags for real data
    LHCbApp().DDDBtag   = "head-20090112"
    LHCbApp().CondDBtag = "head-20090112"


    ## in Prev1 first time alignment
    ## EventSelector().Input = [  
    ##     "DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50403/050403_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50405/050405_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50407/050407_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50408/050408_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50410/050410_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50411/050411_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50413/050413_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50415/050415_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50416/050416_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50418/050418_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50419/050419_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50420/050420_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50423/050423_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50424/050424_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50425/050425_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50426/050426_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50427/050427_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50428/050428_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50429/050429_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50430/050430_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50431/050431_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50432/050432_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ]
    ## DecodeVeloRawBuffer().RawEventLocation='Prev1/DAQ/RawEvent'
    ## DecodeVeloRawBuffer('DecodeVeloClusters').RawEventLocation='Prev1/DAQ/RawEvent'
    ## DecodeVeloRawBuffer('DecodeVeloClusters').DecodeToVeloClusters=True
    ## DecodeVeloRawBuffer('DecodeVeloClusters').DecodeToVeloLiteClusters=False
    ## UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-1 ; double ResolPosLA = 1 ;"]
    ## UpdateManagerSvc().ConditionsOverride += [
    ##  "Conditions/Alignment/Velo/VeloRight :=  double_v dPosXYZ = 0 0 0;",
    ##  "Conditions/Alignment/Velo/VeloLeft  :=  double_v dPosXYZ = 0 0 0;"
    ## ]

    ## # good time alignment in Prev1
    EventSelector().Input = [  
        "DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50433/050433_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50434/050434_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50435/050435_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50436/050436_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50437/050437_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50438/050438_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50439/050439_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50440/050440_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50441/050441_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50442/050442_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50444/050444_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50445/050445_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50446/050446_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50447/050447_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50448/050448_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50449/050449_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50450/050450_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50452/050452_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50454/050454_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50457/050457_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50462/050462_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50463/050463_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50465/050465_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50468/050468_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50472/050472_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50473/050473_0000000001.raw' SVC='LHCb::MDFSelector'"
        ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50474/050474_0000000001.raw' SVC='LHCb::MDFSelector'"
        ]
    DecodeVeloRawBuffer().RawEventLocation='Prev1/DAQ/RawEvent'
    DecodeVeloRawBuffer('DecodeVeloClusters').RawEventLocation='Prev1/DAQ/RawEvent'
    DecodeVeloRawBuffer('DecodeVeloClusters').DecodeToVeloClusters=True
    DecodeVeloRawBuffer('DecodeVeloClusters').DecodeToVeloLiteClusters=False
    UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-1 ; double ResolPosLA = 1 ;"]
    UpdateManagerSvc().ConditionsOverride += [
     "Conditions/Alignment/Velo/VeloRight :=  double_v dPosXYZ = 0 0 0;",
     "Conditions/Alignment/Velo/VeloLeft  :=  double_v dPosXYZ = 0 0 0;"
    ]

    ## ##  # in Central
    ## EventSelector().Input = [  
    ##     "DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50475/050475_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50476/050476_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50477/050477_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50478/050478_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50481/050481_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ]
    ## UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-1 ; double ResolPosLA = 1 ;"]
    ## UpdateManagerSvc().ConditionsOverride += [
    ##  "Conditions/Alignment/Velo/VeloRight :=  double_v dPosXYZ = 0 0 0;",
    ##  "Conditions/Alignment/Velo/VeloLeft  :=  double_v dPosXYZ = 0 0 0;"
    ## ]


    ## ##     # very high intensity ... very slow PR
    ## EventSelector().Input = [  
    ##     "DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50479/050479_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ]
    ## UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-1 ; double ResolPosLA = 1 ;"]
    ## UpdateManagerSvc().ConditionsOverride += [
    ##  "Conditions/Alignment/Velo/VeloRight :=  double_v dPosXYZ = 0 0 0;",
    ##  "Conditions/Alignment/Velo/VeloLeft  :=  double_v dPosXYZ = 0 0 0;"
    ## ]

    ## #### new Velo position
    ## ## #in Central
    ## EventSelector().Input = [  
    ##     "DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50482/050482_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50484/050484_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50485/050485_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50486/050486_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50487/050487_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50488/050488_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50489/050489_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50490/050490_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50491/050491_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50492/050492_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50493/050493_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ]
    ## UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-1.45  ; double ResolPosLA = 1 ;"]
    ## UpdateManagerSvc().ConditionsOverride += [
    ##  "Conditions/Alignment/Velo/VeloRight :=  double_v dPosXYZ = 0 0 0;",
    ##  "Conditions/Alignment/Velo/VeloLeft  :=  double_v dPosXYZ = 0 0 0;"
    ## ]

    ## ##     # in Prev1
    ## EventSelector().Input = [  
    ##     "DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50494/050494_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ,"DATA='castor:/castor/cern.ch/grid/lhcb/data/2009/RAW/FULL/LHCb/TED/50496/050496_0000000001.raw' SVC='LHCb::MDFSelector'"
    ##     ]
    ## UpdateManagerSvc().ConditionsOverride +=  ["Conditions/Online/Velo/MotionSystem := double ResolPosRC =-1.45 ; double ResolPosLA = 1 ;"]
    ## UpdateManagerSvc().ConditionsOverride += [
    ##  "Conditions/Alignment/Velo/VeloRight :=  double_v dPosXYZ = 0 0 0;",
    ##  "Conditions/Alignment/Velo/VeloLeft  :=  double_v dPosXYZ = 0 0 0;"
    ## ]
    ## DecodeVeloRawBuffer().RawEventLocation='Prev1/DAQ/RawEvent'
    ## DecodeVeloRawBuffer('DecodeVeloClusters').RawEventLocation='Prev1/DAQ/RawEvent'
    ## DecodeVeloRawBuffer('DecodeVeloClusters').DecodeToVeloClusters=True
    ## DecodeVeloRawBuffer('DecodeVeloClusters').DecodeToVeloLiteClusters=False


else:
    print "********************************************************************************"
    print "The 2009-TED-Files.py file requires Escher be set up to process MDF data"
    print "********************************************************************************"
    import sys
    sys.exit()

# Default output files names are set up using value Escher().DatasetName property
datasetName = "TEDDataJune2009"
Escher().DatasetName = datasetName

# Redefine defaults by uncommenting one or more of options below 

# Monitoring histograms
#HistogramPersistencySvc().OutputFile = "SomeFile.root"

## #-- Dst or rDst file
## #OutputStream("DstWriter").Output = "DATAFILE='PFN:someFile.dst' TYP='POOL_ROOTTREE' OPT='REC'"
## #OutputStream("DstWriter").Output = "DATAFILE='PFN:someFile.rdst' TYP='POOL_ROOTTREE' OPT='REC'"

## $Id: DVTestReadETC.py,v 1.1 2009-06-23 13:41:38 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $
## ============================================================================
#
# Example options to read a user-defined ETC
#
from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.StrippingLine import StrippingLine, strippingLines
from StrippingConf.Configuration import StrippingConf
StrippingConf().ActiveLines = []
StrippingConf().OutputType = "NONE"                    # Can be either "ETC" or "DST"
StrippingConf().MainOptions = "$STRIPPINGSELECTIONSROOT/options/STEP09/StrippingSelections.py"

DaVinci().EvtMax = -1
DaVinci().PrintFreq  = 1
DaVinci().DataType = "2009"

DaVinci().Input   = [
    "COLLECTION='TagCreator/EventTuple' DATAFILE='etc.root' TYP='POOL_ROOT' SEL='(StrippingBs2JpsiPhiLine==1)'" ]

from Configurables import  TagCollectionSvc
ApplicationMgr().ExtSvc  += [ TagCollectionSvc("EvtTupleSvc") ]

FileCatalog().Catalogs = [ "xmlcatalog_file:$DAVINCIROOT/options/MC09-Bs2JpsiPhiDst.xml" ]
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

## $Id: DVTestReadETC.py,v 1.5 2010-01-28 07:21:10 pkoppenb Exp $
## ============================================================================
## CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $
## ============================================================================
#
# Example options to read a user-defined ETC
#
from Gaudi.Configuration import *
from Configurables import DaVinci
from StrippingConf.Configuration import StrippingConf

from StrippingSelections import StreamDimuon

sc = StrippingConf()
sc.appendStream( StreamDimuon.stream )

DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().EvtMax = -1
DaVinci().PrintFreq  = 1
DaVinci().DataType = "MC09"

DaVinci().Input   = [
    "COLLECTION='TagCreator/EventTuple' DATAFILE='etc.root' TYP='POOL_ROOT' SEL='(StrippingBs2JpsiPhiLine==1)'" ]

from Configurables import  TagCollectionSvc
ApplicationMgr().ExtSvc  += [ TagCollectionSvc("EvtTupleSvc") ]

FileCatalog().Catalogs = [ "xmlcatalog_file:$DAVINCIROOT/options/MC09-Bs2JpsiPhiDst.xml" ]
MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

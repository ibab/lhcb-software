from Gaudi.Configuration import *

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]


from Configurables import Brunel
Brunel().DataType =  "2013"
Brunel().InputType = "DST"
file = "$QMTESTFILESROOT/data/136237_10ev_bug_svn164539.dst"
EventSelector().Input.append(file)
Brunel().SplitRawEventInput=2.0  # Stripping20

# Suppress GECs for pA data
importOptions("$APPCONFIGOPTS/Brunel/pA-GECs.py")

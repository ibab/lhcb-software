from Configurables import (Escher, TAlignment, ApplicationMgr, NTupleSvc)
TAlignment().CondFilePrefix = "2009/Collisions/x/"
Escher().DatasetName = '2009-Collisions-X'
ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]
tupleFile = "$ESCHERROOT/cmt/2009/Collisions/x/2009CollisionsX.root"
tuple = "FILE1 DATAFILE='"+tupleFile+"' TYP='ROOT' OPT='NEW'"
NTupleSvc().Output = [ tuple ]


from Configurables import (Escher, TAlignment, ApplicationMgr, NTupleSvc)
TAlignment().CondFilePrefix = "2009/Collisions/x/"
Escher().DatasetName = '2009-Collisions-X'
ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]
tupleFile = "/work/work2/blouw/Alignment/Alignment_v4r6/Alignment/Escher/cmt/2009/Collisions/x/2009CollisionsX.root"
tuple = "FILE1 DATAFILE='"+tupleFile+"' TYP='ROOT' OPT='NEW'"
NTupleSvc().Output = [ tuple ]


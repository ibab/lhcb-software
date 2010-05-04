from Configurables import (Escher, TAlignment, ApplicationMgr, NTupleSvc)
TAlignment().CondFilePrefix = ""
myfile = 'MC09-it3-evts5100-skpCl-uniTr-dc__lhcbID.root'
Escher().DatasetName = myfile
ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]
tupleFile = "$ESCHERROOT/job/"+myfile
tuple = "FILE1 DATAFILE='"+tupleFile+"' TYP='ROOT' OPT='NEW'"
NTupleSvc().Output = [ tuple ]


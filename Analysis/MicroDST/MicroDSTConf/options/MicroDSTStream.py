"""
$Id: MicroDSTStream.py,v 1.1.1.1 2008-11-12 12:33:48 jpalac Exp $
Configure the MicroDST OutputStream and add it to the ApplicationMgr
@author Juan Palacios juan.palacios@nikhef.nl
"""
from Gaudi.Configuration import *
from Configurables import OutputStream
#
MicroDSTStream=OutputStream('MicroDSTStream')
MicroDSTStream.OptItemList = ["/Event/microDST#99"]
MicroDSTStream.Output = "DATAFILE='MicroDST.dst' TYP='POOL_ROOTTREE' OPT='REC'";
ApplicationMgr().OutStream.append(MicroDSTStream)
#

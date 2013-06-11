
import RawEventFormat
if len(RawEventFormat.Raw_location_db)==0:
    raise AttributeError("Nothing in the database!")

if len(RawEventFormat.Reco_dict_db)==0:
    raise AttributeError("Nothing in the database!")

from Configurables import RawEventFormatConf
RawEventFormatConf().forceLoad()

print "Pass"

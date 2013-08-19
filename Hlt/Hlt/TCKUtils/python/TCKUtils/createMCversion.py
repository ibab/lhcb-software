import GaudiPython
from Gaudi.Configuration import*
from Configurables import ConfigStackAccessSvc, ConfigDBAccessSvc, ConfigTarFileAccessSvc, ConfigFileAccessSvc, ConfigTreeEditor, PropertyConfigSvc

from TCKUtils.utils import *


def createMCversion( tck, newtck = None, cas = ConfigTarFileAccessSvc() ) :
    if not newtck : newtck = tck | 0x40000000
    props = getProperties(tck,'DeterministicPrescaler/.*','AcceptFraction')
    exceptions = { 'LumiStripper.*'     : '0',
         '.*Debug.*'          : None,
         'Hlt1ODINTechnical.*': None,
         'Hlt1Lumi.*'         : None,
         'Hlt1L0.*'           : None,
         'Hlt1MBMicroBias.*'  : None}
    update = { } 
    for n,v in props.iteritems() :
       v = v['AcceptFraction']
       if v == '1' or v == '0' : continue # do not modify these
       vorig = v
       m = 0  # count # of matching exceptions
       for (e,o) in exceptions.iteritems() :
          import re
          if re.match(e,n) : 
             if o : v = o
             m = m+1
       if m == 0 : v = '1'   # if no exception, put at 1
       if vorig == v : continue  # if not 
       update[ n ] = { 'AcceptFraction' : '%s'%v }
    from pprint import pprint
    print "==================> dump update <===================="
    pprint(update)
    label = None
    for (i,j) in getConfigurations(cas).iteritems() :
       if tck in j['TCK'] : label = j['label']
    id  = updateProperties( resolveTCK(tck,cas),update,label='%s - MC version of 0x%08x' % (label,tck), cas=cas)
    createTCKEntries({newtck:id},cas)


    print "==================> diff data / MC TCK <===================="
    diff (tck, newtck)


##
## main
##

if __name__ == "__main__":
    dataTCK = 0x760037
    print "Create MC TCK for TCK %x" % (dataTCK)
    #listHlt1Lines(dataTCK)
    createMCversion( tck = dataTCK, cas=ConfigTarFileAccessSvc(Mode='ReadWrite',CompressOnWrite=True) )

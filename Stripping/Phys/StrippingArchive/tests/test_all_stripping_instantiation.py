from StrippingArchive import strippingArchive

import commands

for astripping in strippingArchive():
    print '============================================='
    print astripping,':'
    print '============================================='
    status,stdout=commands.getstatusoutput('python -c "from StrippingArchive.Utils import buildStreams; buildStreams('+"'"+astripping+"'"+');"')
    print stdout
    if status!=0:
        print '----------FAILED----------'
        print astripping,'failed',status

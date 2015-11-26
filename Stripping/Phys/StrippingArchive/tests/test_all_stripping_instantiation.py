from StrippingArchive import strippingArchive, _relinfo_obsolete_strippings, _duplicate_strippings

import commands

print "RelInfo-obsolete strippings are"
print _relinfo_obsolete_strippings


obsoleteduplicates=[]
for i,k in _duplicate_strippings.iteritems():
    if k in _relinfo_obsolete_strippings:
        obsoleteduplicates.append(i)

strippings = [s for s in strippingArchive() if s not in _relinfo_obsolete_strippings+obsoleteduplicates]

print "looping over:"
print strippings

for astripping in strippings:
    print '============================================='
    print astripping,':'
    print '============================================='
    status,stdout=commands.getstatusoutput('python -c "from StrippingArchive.Utils import buildStreams; buildStreams('+"'"+astripping+"'"+');"')
    print stdout
    if status!=0:
        print '----------FAILED----------'
        print astripping,'failed',status

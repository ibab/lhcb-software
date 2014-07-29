from Configurables import Moore

#check that all properties have adequate documentation

missingdocs=[prop for prop in Moore.__slots__ if prop not in Moore._propertyDocDct]
extradocs=[prop for prop in Moore._propertyDocDct if prop not in Moore.__slots__]
emptydocs=[prop for prop in Moore._propertyDocDct if len(Moore._propertyDocDct[prop])<5]

if len(missingdocs):
    raise KeyError("You are missing documentation for the properties "+missingdocs.__str__())

if len(extradocs):
    raise KeyError("You have documented non-existant properties "+extra.__str__())

if len(emptydocs):
    raise KeyError("You have not provided enough documentation for a property "+emptydocss.__str__())

#check the deprecation warnings
depd=[p for p in Moore._propertyDocDct if "DEPRECATED" in Moore._propertyDocDct[p]]

if depd :
    for prop in depd:
        try:
            Moore().setProp(prop,"deprecated")
        except:
            Moore().setProp(prop,314159)

    try:
        Moore().__apply_configuration__()
        raise RuntimeError("No deprecation warnings were printed!")
    except DeprecationWarning as dw:
        pass

#search for tests and options setting deprecated properties!
import commands
found=""
for prop in depd:
    cmd="grep -e '\."+prop+"\(=\|[ \\t]\)' $MOOREROOT/options/*.{py,opts} ${MOOREROOT}/tests/*/*.{py,opts} ${MOOREROOT}/tests/qmtest/*/*/*.qmt  ${MOOREROOT}/tests/qmtest/*/*.qmt ${MOOREROOT}/tests/qmtest/*/*/*/*.qmt"
    #print cmd
    import os
    stat,out=commands.getstatusoutput(cmd)
    outlines=[prop+":-> "+line.replace(os.environ["MOOREROOT"],"$MOOREROOT").split(":")[0] for line in out.strip().split("\n") if "No such file or directory" not in line]
    if len(outlines):
        found=found+'\n'.join(outlines)+'\n'

if len(found.strip()):
    raise DeprecationWarning("A test or options file is trying to set deprecated properties:\n"+found.strip())

print "PASS"


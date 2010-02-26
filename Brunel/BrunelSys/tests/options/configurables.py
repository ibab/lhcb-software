import Configurables

if len(Configurables.__all__) <1000:
    print "Not enough configurables found, should be >1000"
    print Configurables.__all__

for conf in Configurables.__all__:
    print conf
    exec("Configurables.%s()" % conf)

print "done"


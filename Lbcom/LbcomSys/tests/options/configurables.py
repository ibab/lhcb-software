import Configurables

if len(Configurables.__all__) <500:
    print "Not enough configurables found, should be >500"
    print Configurables.__all__

for conf in Configurables.__all__:
    print conf
    exec("Configurables.%s()" % conf)

print "done"


import Configurables

if len(Configurables.__all__) <400:
    print "Not enough configurables found, should be >400"
    print Configurables.__all__

for conf in Configurables.__all__:
    print conf
    exec("Configurables.%s()" % conf)

print "done"


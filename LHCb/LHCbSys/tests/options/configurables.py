import Configurables

if len(Configurables.__all__) <400:
    print "Not enough configurables found, should be >400"

for conf in sorted(Configurables.__all__):
    print conf
    exec("Configurables.%s()" % conf)

print "done"


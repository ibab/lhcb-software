import Configurables

success_list=[]
fail_list=[]

for conf in sorted(Configurables.__all__):
    try:
        print "# " + conf
        exec("Configurables.%s()" % conf)
        success_list.append(conf)
    except:
        import sys
        import traceback
        traceback.print_exc(file=sys.stdout)
        fail_list.append(conf)

print "== successful =="
print len(success_list)
print success_list
print "== failed =="
print len(fail_list)
print fail_list
print "== done =="


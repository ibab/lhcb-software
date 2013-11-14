from Gaudi.Configuration import *
importOptions("../options/OldDecodeRawEvent.py")

from GaudiConf.Manipulations import configurableInstanceFromString

for k,v in configurableInstanceFromString("DataOnDemandSvc").AlgMap.iteritems():
    if type(v) is str:
        v=configurableInstanceFromString(v)
    print "===================="
    print k
    print "--------------------"
    print v.getFullName()
    props={}
    for prop in v.getProperties():
        props[prop]=v.getProp(prop)
    print props
    print "===================="

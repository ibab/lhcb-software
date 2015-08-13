#!/bin/python

from GaudiPython import gbl
AlMon = gbl.Alignment.AlignmentMonitoring

refDir = "/group/online/alignment"
alDir = "/group/online/AligWork/Tracker"
versions = {'ITG': {'set': 'IT/ITGlobal', 'ver':'v10'},
            'ITM': {'set': 'IT/ITModules','ver':'v7'},
            'TTG': {'set': 'TT/TTGlobal', 'ver':'v9'},
            'TTM': {'set': 'TT/TTModules', 'ver':'v9'},
            'OTG': {'set': 'OT/OTGlobal', 'ver':'v9'},
            'OTM': {'set': 'OT/OTModules', 'ver':'v9'} }
cc = {}
for key, args in versions.iteritems():
    cc[key] = AlMon.CompareConstants(args['set'], args['ver'], refDir)
    #cc[key].Compare('v13')
    #for runNo in ['run156916','run157025','run157118','run157235','run157348','run157558','run157697','run157803','run157895','run158044']:
    for runNo in ['run158517']:
        print 'compare ', args['set']+':'+args['ver'], ' with ', runNo, '-->'
        cc[key].CompareWithRun(alDir,runNo)
        cc[key].PrintWarnings(2) #1: OK, 2: WARNING, 3:SEVERE
        cc[key].SaveHists('output/hists_'+key+'_'+runNo+'.root')

#!/bin/python

from GaudiPython import gbl
AlMon = gbl.Alignment.AlignmentMonitoring

alDir = "/afs/cern.ch/user/m/mamartin/public/tests/alignment/"
versions = {'ITG': {'set': 'IT/ITGlobal/', 'ver':'v9'},
            'ITM': {'set': 'IT/ITModules/','ver':'v7'},
            'TTG': {'set': 'TT/TTGlobal/', 'ver':'v9'},
            'TTM': {'set': 'TT/TTModules/', 'ver':'v9'},
            'OTG': {'set': 'OT/OTGlobal/', 'ver':'v9'},
            'OTM': {'set': 'OT/OTModules/', 'ver':'v9'} }
cc = {}
for key, args in versions.iteritems():
    cc[key] = AlMon.CompareConstants(args['set'], args['ver'], alDir)
    cc[key].Compare('v14')
    cc[key].PrintWarnings()

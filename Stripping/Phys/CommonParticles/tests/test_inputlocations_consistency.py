#!/usr/bin/env python
#$Id: test_load_modules.py,v 1.1 2010-04-22 14:44:53 jpalac Exp $
'''
Test loading various CommonParticles in the Stripping environment. Needs to be extended.
'''
__author__ = 'Juan Palacios juan.palacios@nikhef.nl'


import sys
sys.path.append('../python')

from CommonParticles import StandardBasic, StandardIntermediate, StdLooseKs

outputLocations = list(StandardBasic.locations.keys()) + list(StandardIntermediate.locations.keys())

algs = list(StandardBasic.locations.values()) + list(StandardIntermediate.locations.values())

errors = {}

message = ""

for alg in algs :
    if not hasattr(alg, 'Inputs') : continue
    locs = alg.Inputs
    for loc in locs :
        if not loc in outputLocations :
            message+= "ERROR: Algorithm "+alg.name()+".Input "+loc+" not in output locations\n"
            errors[alg.name()]=loc


if len(errors)>0 :
    sys.stderr.write('test_inputlocations_consistency: FAIL\n')
    sys.stderr.write(message)
else :
    sys.stdout.write('test_inputlocations_consistency: PASS\n')

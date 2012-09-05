#!/usr/bin/env python
'''
Test that tools can be instantiated standalone from the tool service.
'''

tools = { 'Bd2KstarMuMuAngleCalculator' : 'IP2VVPartAngleCalculator',
          'Bs2JpsiPhiAngleCalculator' : 'IP2VVPartAngleCalculator',
          'CheckOverlap' : 'ICheckOverlap',
          'CheckSelResultsTool' : 'ICheckSelResults',
          'CheckVeloOverlap' : 'ICheckOverlap',
          'DecayFinder' : 'IDecayFinder',
          'DecodeSimpleDecayString' : 'IDecodeSimpleDecayString',
          'FindCloneTool' : 'ICheckOverlap',
          'NeutralCCChangePIDTool' : 'IChangePIDTool',
          'GenericParticle2PVRelator__p2PVWithIPChi2_OnlineDistanceCalculatorName_' : 'IRelatedPVFinder',
          'GenericParticle2PVRelator__p2PVWithIP_OnlineDistanceCalculatorName_' : 'IRelatedPVFinder',
          'GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_' : 'IRelatedPVFinder',
          'GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_' : 'IRelatedPVFinder',
          'P2VVAngleCalculator' : 'IP2VVAngleCalculator',
          'ParticleDescendants' : 'IParticleDescendants'
                            }

if __name__ == '__main__' :

    from GaudiPython.Bindings import AppMgr
    import PartProp.Service
    from Configurables import LHCbApp
    from Configurables import LHCb__ParticlePropertySvc
    LHCb__ParticlePropertySvc(ParticlePropertiesFile = '$DAVINCITOOLSROOT/tests/data/ParticleTable.txt')
    import sys

    LHCbApp()

    errors = ['======== Error Summary =========================================']
    successes = ['======== Success Summary =======================================']
    appMgr = AppMgr(outputlevel=5)
    appMgr.ExtSvc += ['LHCb::ParticlePropertySvc']
    appMgr.initialize()
    toolSvc = appMgr.toolSvc()

    import atexit
    atexit.register(appMgr.exit)


    for impl, iface in tools.iteritems() :
        try :
            print 'Trying to instantiate', impl+'/'+iface
            tool = toolSvc.create(impl, interface = iface)
            if tool == None :
                errors.append('ERROR: Failed to instantiate Tool '+ impl+'/'+iface)
            else :
                successes.append('SUCCESS: Instantiated Tool '+ impl+'/'+iface)
        except :
            errors.append('ERROR: Failed to instantiate Tool '+ impl+'/'+iface)

    errors.append('================================================================')
    successes.append('================================================================')
    if len(errors) > 2  :
        for error in errors :
            sys.stderr.write(error + '\n')

    if len(successes) > 2  :
        for success in successes :
            sys.stdout.write(success+'\n')

#!/usr/bin/env python

if __name__ == '__main__' :

    import sys
    from Gaudi.Configuration import *
    from Configurables import ( AlgorithmCorrelationsAlg,
                                Bd2KstarMuMuAngleCalculator,
                                BremAdder,
                                Bs2JpsiPhiAngleCalculator,
                                CheckOverlap,
                                CheckPV,
                                CheckSelResult,
                                CheckSelResultsTool,
                                CheckVeloOverlap,
                                CombineParticles,
                                ConjugateNeutralPID,
                                CountParticles,
                                DaVinciInit,
                                DecayFinder,
                                DecodeSimpleDecayString,
                                FilterDesktop,
                                FilterEventList,
                                FilterInTrees,
                                FindCloneTool,
                                NeutralCCChangePIDTool,
                                GenericParticle2PVRelator__p2PVWithIPChi2_OnlineDistanceCalculatorName_,
                                GenericParticle2PVRelator__p2PVWithIP_OnlineDistanceCalculatorName_,
                                OnOfflineTool,
                                GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_,
                                GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_,
                                P2VVAngleCalculator,
                                ParticleDescendants,
                                PhysDesktop,
                                PrintDecayTree,
                                PVRelatorAlg,
                                RemoveClones
                                )

    components = ( AlgorithmCorrelationsAlg,
                   Bd2KstarMuMuAngleCalculator,
                   BremAdder,
                   Bs2JpsiPhiAngleCalculator,
                   CheckOverlap,
                   CheckPV,
                   CheckSelResult,
                   CheckSelResultsTool,
                   CheckVeloOverlap,
                   CombineParticles,
                   ConjugateNeutralPID,
                   CountParticles,
                   DaVinciInit,
                   DecayFinder,
                   DecodeSimpleDecayString,
                   FilterDesktop,
                   FilterEventList,
                   FilterInTrees,
                   FindCloneTool,
                   NeutralCCChangePIDTool,
                   GenericParticle2PVRelator__p2PVWithIPChi2_OnlineDistanceCalculatorName_,
                   GenericParticle2PVRelator__p2PVWithIP_OnlineDistanceCalculatorName_,
                   OnOfflineTool,
                   GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_,
                   GenericParticle2PVRelator__p2PVWithIP_OfflineDistanceCalculatorName_,
                   P2VVAngleCalculator,
                   ParticleDescendants,
                   PhysDesktop,
                   PrintDecayTree,
                   PVRelatorAlg,
                   RemoveClones
                   )

    errors = ['======== Error Summary =========================================']
    successes = ['======== Success Summary =======================================']

    for component in components :
        try :
            c = component()
            successes.append('SUCCESS: Instantiated Configurable ' + c.name() )
        except :
            errors.append('ERROR: Failed to instantiate Configurable ' + str(component) )

    errors.append('================================================================')
    successes.append('================================================================')
    if len(errors) > 2  :
        for error in errors :
            sys.stderr.write(error + '\n')

    if len(successes) > 2  :
        for success in successes :
            sys.stdout.write(success+'\n')




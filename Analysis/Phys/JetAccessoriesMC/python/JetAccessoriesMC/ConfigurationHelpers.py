# =============================================================================
# $URL$
# =============================================================================
## @file ConfigurationHelpers
#  @author  Albert Bursche bursche@cern.ch with much input from Victor Coco
#  @date 2011-06-22
#
#  Last modification $Date: 2011-05-13 13:24:04 $
#                 by $Author: bursche $

# =============================================================================
__author__  = " Albert Bursche  bursche@cern.ch"
__date__    = " 2011-06-22 " 
__version__ = " Version $Revision: 1.0 $ "
# =============================================================================
def jetMakerConfig( name , Inputs  , R ):
    """
    to ease jet maker config
    """
    
    from Configurables         import LoKi__JetMaker, LoKi__FastJetMaker
    algo =  LoKi__JetMaker ( name )
    algo.JetMaker = 'LoKi__FastJetMaker'
    algo.addTool ( LoKi__FastJetMaker )
    algo.Inputs = Inputs
    tool = getattr ( algo , 'LoKi__FastJetMaker' )
    tool.Type = 2
    tool.RParameter = R
    tool.Recombination = 0
    return algo
from GaudiKernel.Constants import DEBUG,VERBOSE
def jetMatcherConfig( name , toolName, jetALocation,jetBLocation,outputTable ):
    """
    to ease jet association config
    """
    assert( toolName in ["LoKi__HepMCJets2HepMCJets",
                        "LoKi__HepMCJets2Jets",
                        "LoKi__HepMCJets2JetsDeltaR",
                        "LoKi__PartonicJets2HepMCJets"])
    
    from Configurables import (Jets2JetsAlg,
                               LoKi__HepMCJets2HepMCJets,
                               LoKi__HepMCJets2Jets,
                               LoKi__HepMCJets2JetsDeltaR,
                               LoKi__PartonicJets2HepMCJets)
    algo =  Jets2JetsAlg ( name )
    algo.Jets2Jets = toolName
    if toolName ==    "LoKi__HepMCJets2HepMCJets":
        algo.addTool ( LoKi__HepMCJets2HepMCJets )
    if toolName ==  "LoKi__HepMCJets2Jets":
        algo.addTool ( LoKi__HepMCJets2Jets )

        #removed to try with MCMatchObjP2MCRelator
        #from Configurables import DaVinciSmartAssociator
        #algo.LoKi__HepMCJets2Jets.addTool(DaVinciSmartAssociator)
        #algo.LoKi__HepMCJets2Jets.DaVinciSmartAssociator.OutputLevel = VERBOSE
        
    if toolName ==  "LoKi__HepMCJets2JetsDeltaR":
        algo.addTool ( LoKi__HepMCJets2JetsDeltaR )
    if toolName ==  "LoKi__PartonicJets2HepMCJets":
        algo.addTool ( LoKi__PartonicJets2HepMCJets )
        
    algo.JetsALocation =jetALocation
    algo.JetsBLocation =jetBLocation
    algo.OutputTable = outputTable
#    algo.OutputLevel = DEBUG

    return algo
    
def addRelationTuple(iDecayTreeTuple,iJets2JetsAlg,InvertTable,ReverseMatching,extraName = ''):
    namePieces = iJets2JetsAlg.JetsALocation.split('/')+['2']+iJets2JetsAlg.JetsBLocation.split('/')
    name = ''
    for np in namePieces:
        if np not in ['Phys', 'Event', 'Particles']:
            name += np
    if InvertTable:
        name += 'Inv'
    if ReverseMatching:
        name += 'Rev'
    name += extraName
    print name
    from Configurables import TupleToolJetRelations
    ##tool = TupleToolJetRelations(name)
    iDecayTreeTuple.addTool(TupleToolJetRelations(name,
                                                  InvertTable=InvertTable,
                                                  ReverseMatching=ReverseMatching,
                                                  RelationsContainer=iJets2JetsAlg.OutputTable,
                                                  ExtraName = name,
                                                  MaxMatches = 2,
                                                  Verbose = True))
    iDecayTreeTuple.ToolList+=['TupleToolJetRelations/'+name] 
    #print iDecayTreeTuple.ToolList
def ConfigureDoD(RValue = 0.5):
    from  Configurables       import DataOnDemandSvc
    dod = DataOnDemandSvc ()
    #dod.OutputLevel = VERBOSE

    #datasets
    #first MC
    from JetAccessoriesMC.StdHepMCParticles import StdHepMCParticlesConf
    ### create the partons
    import JetAccessoriesMC.PreConfiguredMCSelections 
    StdHepMCPartons = StdHepMCParticlesConf ("StdHepMCPartons",JetAccessoriesMC.PreConfiguredMCSelections.PartonConfAll)
    
    ### create all hepMC particles from Z interaction
    StdHepMCParticlesAll = StdHepMCParticlesConf ("StdHepMCParticlesAll",JetAccessoriesMC.PreConfiguredMCSelections.HepMCAllConfZJets)
    
    ### create measurable hepMC particles from Z interaction
    StdHepMCParticles = StdHepMCParticlesConf("StdHepMCParticles",JetAccessoriesMC.PreConfiguredMCSelections.HepMCConfVisibleZJets)

    for alg in  [StdHepMCPartons.Alg, StdHepMCParticlesAll.Alg, StdHepMCParticles.Alg]:
        dod.AlgMap ['Phys/'+alg.getName()+'/Particles'] = alg.getFullName() 
    
    #filter input particles MC
    import JetAccessoriesMC.MCParticles
    MCPartFilter = JetAccessoriesMC.MCParticles.MCPartFilter

    #then reco
    #selected tracks
    import JetAccessories.RecoParticles 
    SelectedRecoParticles = JetAccessories.RecoParticles.SelectedTracks
    
    #Particle Flow
#    import JetAccessories.ParticleFlow #fixme
#    SelectedPFParticles = JetAccessories.ParticleFlow.ParticleFlow

    #jets
    from JetAccessoriesMC.ConfigurationHelpers import jetMakerConfig
    ### create partonic jets
    StdPartonicJets = jetMakerConfig("StdPartonicJets",["Phys/StdHepMCPartons/Particles"],RValue)

    ### create hepMC all jets
    StdHepMCAllJets = jetMakerConfig("StdHepMCAllJets",["Phys/StdHepMCParticlesAll/Particles"],RValue)

    ### create hepMC jets
    StdHepMCJets    = jetMakerConfig("StdHepMCJets",   ["Phys/FilteredStableParticles/Particles"],RValue)

    ### create reco jets
    StdJets = jetMakerConfig("StdJets",   [  #"Phys/FilteredPions/Particles",
            "Phys/StdNoPIDsDownPions/Particles",
            "Phys/StdAllNoPIDsPions/Particles",
            "Phys/StdLoosePhotons/Particles"]
                             ,RValue)
    ### create ParticleFlow jets
#    StdPFJets = jetMakerConfig("StdPFJets",   [ SelectedPFParticles.PFOutputLocation] ,RValue)

    for alg in  [StdPartonicJets,StdHepMCAllJets,StdHepMCJets,StdJets,#StdPFJets,
                 MCPartFilter,SelectedRecoParticles#,SelectedPFParticles
                 ]:
        dod.AlgMap ['Phys/'+alg.getName()+'/Particles'] = alg.getFullName() 

    #matching tables

    # more MC matching tables (for LoKi in HepMCJets2Jets)
    from GaudiKernel.ProcessJobOptions import importOptions
    importOptions("$LOKIGENMCROOT/python/LoKiGenMC/HepMC2MC_Configuration.py")
    importOptions("$LOKIGENMCROOT/python/LoKiGenMC/MC2Collision_Configuration.py")
 
    # configure matchers
    matchers = {}
    from JetAccessoriesMC.ConfigurationHelpers import jetMatcherConfig
    for HepMCParticleJet in [StdHepMCAllJets,StdHepMCJets
#                             ,StdHepMCJetsAcc,StdHepMCAllJetsAcc todo MC jets with acceptance cuts!
                             ]: 
        name = HepMCParticleJet.getName()
        # parton jets
        Parton2HepMCParticleJet = jetMatcherConfig('Parton2'+name ,
                                                   'LoKi__PartonicJets2HepMCJets',
                                                   'Phys/StdPartonicJets/Particles','Phys/'+name+'/Particles',
                                                   'Relations/Phys/StdPartonicJetsJets2'+name)
        Parton2HepMCParticleJet.LoKi__PartonicJets2HepMCJets.StdHepMC2HepMCTable = 'Relations/Phys/StdHepMC2HepMC'
        Parton2HepMCParticleJet.LoKi__PartonicJets2HepMCJets.PartonInJet = []
        Parton2HepMCParticleJet.LoKi__PartonicJets2HepMCJets.HadronInJet = []
        Parton2HepMCParticleJet.LoKi__PartonicJets2HepMCJets.AllPartons = True
        matchers[name+'Partonic'] = Parton2HepMCParticleJet
        
        # reco jets
        for RecJet in [StdJets]:#,StdPFJets]: weird problem!
            name2 = RecJet.getName()
            print name, name2
            # use 'MCtrue' energy to calulate overlap
            HepMC2RecMC = jetMatcherConfig(name+'2'+name2+'MC' ,'LoKi__HepMCJets2Jets',
                                           'Phys/'+name+'/Particles','Phys/'+name2+'/Particles',
                                           'Relations/Phys/'+name+'2'+name2+'MCtrue')
            if name == 'StdHepMCAllJets':
                HepMC2RecMC.LoKi__HepMCJets2Jets.StdHepMC2HepMCTable = 'Relations/Phys/StdHepMC2HepMCAllZ'
            if name == 'StdHepMCJets':
                HepMC2RecMC.LoKi__HepMCJets2Jets.StdHepMC2HepMCTable = 'Relations/Phys/StdHepMC2HepMCZ'
            HepMC2RecMC.LoKi__HepMCJets2Jets.MatchingMethod = 'MCtrue'
            matchers[name+'2'+name2+"MCtrue"] = HepMC2RecMC

            #use 'measured' energy to calcluate overlap
            HepMC2RecM = jetMatcherConfig(name+'2RecM' ,'LoKi__HepMCJets2Jets',
                                     'Phys/'+name+'/Particles','Phys/'+name2+'/Particles',
                                     'Relations/Phys/'+name+'2'+name2+'Measured')
            if name == 'StdHepMCAllJets':
                HepMC2RecM.LoKi__HepMCJets2Jets.StdHepMC2HepMCTable = 'Relations/Phys/StdHepMC2HepMCAllZ'
            if name == 'StdHepMCJets':
                HepMC2RecM.LoKi__HepMCJets2Jets.StdHepMC2HepMCTable = 'Relations/Phys/StdHepMC2HepMCZ'
            HepMC2RecM.LoKi__HepMCJets2Jets.MatchingMethod = 'measured'
            matchers[name+'2'+name2+"measured"] = HepMC2RecM

            #use DeltaR to calculate distance (note: meaning of the weight variable is opposite to the other two cases)
            HepMC2RecDR = jetMatcherConfig(name+'2RecDeltaR' ,'LoKi__HepMCJets2JetsDeltaR',
                                           'Phys/'+name+'/Particles','Phys/'+name2+'/Particles',
                                           'Relations/Phys/'+name+'2'+name2+'DeltaR')
            matchers[name+'2'+name2+"DeltaR"] = HepMC2RecDR
        
    for MatchAlg in matchers.values():
        dod.AlgMap [MatchAlg.OutputTable] = MatchAlg.getFullName() 
    
    return matchers

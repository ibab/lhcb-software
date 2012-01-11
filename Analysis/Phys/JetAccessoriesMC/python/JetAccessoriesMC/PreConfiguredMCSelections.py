# =============================================================================
# $URL$
# =============================================================================
## @file PreConfiguredMCSelections
#  @author  Albert Bursche bursche@cern.ch with much input from Victor Coco
#  @date 2011-06-22
#
#  Last modification $Date: 2011-05-13 13:24:04 $
#                 by $Author: bursche $

# =============================================================================
__author__  = " Albert Bursche  bursche@cern.ch"
__date__    = " 2011-07-22 " 
__version__ = " Version $Revision: 1.0 $ "
# =============================================================================

from Configurables import  LoKi__ParticleMaker,LoKi__HepMCParticleMaker
    
PartonConfZJets = { 
    'From': ['b','c','d','s','u','g'],
    'UseSelector' : True , 
    'MotherSelectorName' : 'WZPlusJetHepMCSelector' ,
    'PartonicMode' : True ,
    'OutputTable'  : 'Relations/Phys/StdHepMCParton2HepMCZ',
    'Particles' : ['b','c','d','s','u','g'] }

PartonConfAll = { 
    'From': ['b','c','d','s','u','g'],
    'UseSelector' : False , 
    'MotherSelectorName' : 'WZPlusJetHepMCSelector' ,
    'PartonicMode' : True ,
    'OutputTable'  : 'Relations/Phys/StdHepMCParton2HepMC',
    'Particles' : ['b','c','d','s','u','g'] }

HepMCAllConfZJets = {
    'From': ['b','c','d','s','u','g'],  
    'UseSelector' : True , 
    'MotherSelectorName' : 'WZPlusJetHepMCSelector' ,
    'OutputTable'  : 'Relations/Phys/StdHepMC2HepMCAllZ', 
    'InAcceptance': False, 'PartonicMode' : False,
    'Particles' : ["gamma","e+","mu+","pi+","K+","nu_e","p+","nu_mu","nu_tau","n0", "KL0"] }

HepMCAllConfAll = {
    'From': ['b','c','d','s','u','g'],  
    'UseSelector' : False , 
    'MotherSelectorName' : 'WZPlusJetHepMCSelector' ,
    'OutputTable'  : 'Relations/Phys/StdHepMC2HepMCAll', 
    'InAcceptance': False, 
    'PartonicMode' : False,
    'Particles' : ["gamma","e+","mu+","pi+","K+","nu_e","p+","nu_mu","nu_tau","n0", "KL0"] }

HepMCConfVisibleZJets = { 
    'From': ['b','c','d','s','u','g'], 
    'UseSelector' : True,
    'MotherSelectorName' : 'WZPlusJetHepMCSelector' ,
    'OutputTable'  : 'Relations/Phys/StdHepMC2HepMCZ',
    'InAcceptance': False, 
    'PartonicMode' : False,
    'Particles' : ["gamma","e+","mu+","pi+","K+","p+"] }

HepMCConfVisible = { 
    'From': ['b','c','d','s','u','g'], 
    'UseSelector' : False,
    'MotherSelectorName' : 'WZPlusJetHepMCSelector' ,
    'OutputTable'  : 'Relations/Phys/StdHepMC2HepMC',
    'InAcceptance': False, 
    'PartonicMode' : False,
    'Particles' : ["gamma","e+","mu+","pi+","K+","p+"] }

HepMCMuonsFromZ = { 
    'From': ['Z0'], 
    'InAcceptance': False, 
    'PartonicMode' : False,
    'OutputTable'  : 'Relations/Phys/StdHepMCMuons2HepMC',
    'Particles':['mu+']
    }

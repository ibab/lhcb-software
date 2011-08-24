## @package LoKiJets
#  Base configurables for the jet reconstruction and energy correction
#  @author Victor Coco  (Victor.Coco@cern.ch), Cedric Potterat (Cedric.Potterat@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: JetMaker_Config.py,v 1.1 2010-02-04 12:37:55 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>, Cedric Potterat <Cedric.Potterat@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables import ( LoKi__JetMaker ,LoKi__FastJetMaker, LoKi__SeedConeJetMaker, LoKi__SeedFinder, LoKi__VVSeedFinder, LoKi__FastJetWithAreaMaker )

__all__   = ( 'updateDoD',
              'JetMaker_Configuration'
              )

# local storage of data-on-demand actions 
_particles = {}

# =============================================================================
# Update Data-On-Demand Service 
def updateDoD ( alg , hat = 'Phys/' ) :
    """
    Update Data-On-Demand service
    """
    _parts = { hat+alg.name() : alg } 
    _particles.update ( _parts ) 
    
    dod = DataOnDemandSvc()
    dod.AlgMap.update(
        { hat + alg.name() + '/Particles' : alg.getFullName() , 
          hat + alg.name() + '/Vertices'  : alg.getFullName() }
        )
    return _parts 
    
# ----------------------------------------------------------------------------------

# @class JetMaker_Configuration
# Configurable for the jet reconstruction
# @author Victor Coco  (Victor.Coco@cern.ch)
# @date   19.10.2009
class JetMaker_Configuration(LHCbConfigurableUser):
    ## Options
    __slots__ = { 
                   'JetsInputSequencer' : None    # The sequencer to make the jets input
                  ,'JetSeeder'  :  None
                  ,'JetMaker'   : 'anti_kt'
                  ,'RParameter' : 0.75
                  ,'Areas'      : False
                  ,'OutputContainer'  : 'StdJets'
                  ,'JetEnergyScale'   : None
                  ,'JetCalibration'   : None
                  ,'InputLocation'    : None
                  ,'ToolConf'         : ''
                  
                  }
    
   
    
    ## Configure the jet maker
    def setupJetMaker(self , name):
        ## set the algorithm
        algorithm = LoKi__JetMaker ( name )
        ## string for jet maker name
        jet_maker = ''
        ### take the jetmaker_Type from the slots
        jetmaker_Type = self.getProp('JetMaker')
        JetMakerFromFastJet = ['anti_kt','kt','cambridge','siscone']
        ### Jet Algorithm from FastJet package Phys. Lett. B641 (2006) [hep-ph/0512210]
        if jetmaker_Type in JetMakerFromFastJet :
            # select the jet maker
            if self.getProp('Areas'):
                jet_maker = 'LoKi__FastJetWithAreaMaker'
                algorithm.JetMaker = jet_maker
                algorithm.addTool ( LoKi__FastJetWithAreaMaker )
                
            else :
                jet_maker = 'LoKi__FastJetMaker'
                algorithm.JetMaker = jet_maker
                algorithm.addTool ( LoKi__FastJetMaker )
                    
        ### Jet Algorithm SeedFinder as jet
        elif jetmaker_Type == 'SeedFinder':
            jet_maker = 'LoKi__SeedFinder'
            algorithm.JetMaker = jet_maker
            algorithm.addTool ( LoKi__SeedFinder )

        ### Jet Algorithm VVSeedFinder as jet
        elif jetmaker_Type == 'VVSeedFinder':
            jet_maker = 'LoKi__VVSeedFinder'
            algorithm.JetMaker = jet_maker
            algorithm.addTool ( LoKi__VVSeedFinder )
    
             ### Jet Algorithm SeedCone
        elif jetmaker_Type == 'SeedCone':
            jet_maker = 'LoKi__SeedConeJetMaker'
            algorithm.JetMaker = jet_maker
            algorithm.addTool ( LoKi__SeedConeJetMaker )

            
        else : 
            raise RuntimeError("ERROR : the "+jetmaker+" jet algorithm does not exist")

        tool =  getattr ( algorithm , jet_maker )
        return algorithm , tool


    ## Configure the jet maker
    def ConfigJetMaker(self ,  tool , dict_config ):
        ### take the jetmaker_Type from the slots
        jetmaker_Type = self.getProp('JetMaker')
        ## Case where it is a fastjet algorithm
        if jetmaker_Type == 'kt' :
            setattr(tool,'Type',0)
        elif jetmaker_Type == 'cambridge' :
            setattr(tool,'Type',1)
        elif jetmaker_Type == 'anti_kt' :
            setattr(tool,'Type',2)
        ### Jet Algorithm seedcone
        elif jetmaker_Type == 'SeedCone':
             setattr(tool,'SeedFinder',self.getProp('JetSeeder'))
        else : 
            raise RuntimeError("ERROR : the "+jetmaker+" jet algorithm is not available yet")
        setattr(tool,'RParameter',self.getProp('RParameter'))
        
        
    ## Apply the configuration to the given sequence
    def applyConf(self):
        algorithm , tool = self.setupJetMaker(self.getProp('OutputContainer') )
        self.ConfigJetMaker(tool,self.getProp('ToolConf'))
        locations = updateDoD (algorithm)


# ----------------------------------------------------------------------------------


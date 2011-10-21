# =============================================================================
__author__  = " Victor COCO  "
__version__ = ""
# =============================================================================

# =============================================================================
"""
Configuration file for 'Standard HepMC particles'
"""

from Gaudi.Configuration   import *
from Configurables         import  LoKi__ParticleMaker,LoKi__HepMCParticleMaker
from CommonParticles.Utils import *


# =============================================================================
__all__ = (
    'StdHepMCParticlesConf'
    'confdict'
    )
# =============================================================================


confdict = {
     'Particles' : ["gamma","e+","mu+","pi+","K+","p+","n0", "KL0"]
    ,'From' : ['b']
    ,'InAcceptance' : True
    ,'UseSelector'  : False
    ,'MotherSelectorName' : 'ZPlusJetHepMCSelector'
    ,'PartonicMode' : False
    ,'OutputTable'  : 'Relations/Phys/StdHepMCRef2HepMC'
 }


class StdHepMCParticlesConf() :
    """
    Configuration for creating Particles out of HepMC Particles
    """

    __confdict__={}
        

    def __init__(self, name, config) :
        self.__confdict__= config
        if self.__confdict__.has_key("InAcceptance"):
            if not self.__confdict__['InAcceptance'] and not self.__confdict__['PartonicMode']  :        
                self.__confdict__['ForceNoAcceptanceCut'] = True
        self.name = name
        self.Alg = self.configure()

    def configure (self):
        algorithm =  LoKi__ParticleMaker ( self.name )#,OutputLevel = VERBOSE)
        algorithm.ParticleMaker = 'LoKi__HepMCParticleMaker'
        algorithm.addTool ( LoKi__HepMCParticleMaker )
        for slot in self.__confdict__.keys():
            if slot == 'InAcceptance': continue
            setattr(algorithm.LoKi__HepMCParticleMaker,slot,self.__confdict__[slot])

            ## configure Data-On-Demand service
        self.locations = updateDoD ( algorithm )
        return algorithm

## ============================================================================
if '__main__' == __name__ :
    print __doc__
    print __author__
    print __version__

# =============================================================================
# The END 
# =============================================================================




#  Base configurables for the particle makers implementing the IParticleMaker interface
#  ie. HepMCParticleMaker and MCParticleMaker
#  @author Victor Coco  (Victor.Coco@cern.ch)
#  @date   2009/10/29

__version__ = "$Id: ParticleMaker_Configuration.py,v  Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *
from Gaudi.Configuration      import *
from Configurables            import ( LoKi__ParticleMaker ,LoKi__HepMCParticleMaker ,ZPlusJetHepMCSelector )
from CommonParticles.Utils    import *

# ----------------------------------------------------------------------------------

## @class  ParticleMaker_Configuration
#  Configurable for the particle makers implementing the IParticleMaker interface
#  @author Victor Coco  (Victor.Coco@cern.ch)
#  @date   2009.10.29
class ParticleMaker_Configuration(LHCbConfigurableUser):
    ## Options
    __slots__ = { 
        'From' : 'HepMC'
        ,'OutputContainer'  :  'StdPartons'
        ,'ToolConf'  :  {'PartonicMode': True
                         ,'Particles' : ['g','u','d','s','c','b']
                         ,'From' : ['u','d','s','c','b']
                         ,'UseSelector' : True
                         ,'MotherSelectorName' : 'ZPlusJetHepMCSelector'
                         ,'OutputTable' : 'Relations/Phys/StdHepMCPartons2HepMC'}
        }
    _propertyDocDct = {
        'From'           : """ From which type of info does the LHCb::Particle are made ['HepMC,'MC']""" ,
        'OutputContainer': """ Name of the output container ex: to find it in the tes at /Event/Phys/StdHepMCPart, name is StdHepMCPart""" ,
        'ToolConf':  """ A dictionnary containing the configuratyion of the tool with the following format: {'Proterty1':Value1,...,'ProtertyN':ValueN} """ 
        }

    ## Choose the particle maker
    def setupParticleMaker(self , name ):
        ## set the algorithm
        algorithm = LoKi__ParticleMaker ( name )
        ## string for particle maker name
        particle_maker = ''
        ## set the proper particle maker tool
        if self.getProp('From') == 'HepMC':
             particle_maker = 'LoKi__HepMCParticleMaker'
             algorithm.ParticleMaker = particle_maker
             algorithm.addTool ( LoKi__HepMCParticleMaker )
             
        elif self.getProp('From') == 'MC':
             particle_maker = 'LoKi__MCParticleMaker'
             algorithm.ParticleMaker = particle_maker
             algorithm.addTool ( LoKi__MCParticleMaker )
             
        else :
            raise RuntimeError("ERROR : no ParticleMaker have been implemented to make LHCb::Particle from "+self.getProp('From'))
        
        ## getback the tool for later configuration
        tool.getattr ( algorithm , particle_maker )
        return algorithm , tool

    ## Configure the particle maker
    def ConfigParticleMaker(self ,  tool , dict_config ):
        for slot in dict_config.keys():
            setattr(tool,slot,dict_config[slot])
            
        
    ## Apply the configuration to the given sequence
    def applyConf(self):
        algorithm , tool = self.setupParticleMaker(self.getProp('OutputContainer') )
        self.ConfigParticleMaker(tool,self.getProp('ToolConf'))
        locations = updateDoD (algorithm)


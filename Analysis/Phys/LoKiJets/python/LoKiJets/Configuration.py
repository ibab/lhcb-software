
## @package LoKiJets
#  Base configurables for the jet reconstruction and energy correction
#  @author Victor Coco  (Victor.Coco@cern.ch)
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.1 2009-12-14 12:34:32 cocov Exp $"
__author__  = "Victor Coco <Victor.Coco@cern.ch>"

from LHCbKernel.Configuration import *

from Configurables            import ( LoKi__ParticleMaker ,LoKi__HepMCParticleMaker ,ZPlusJetHepMCSelector )
from Configurables            import ( MCParticleMaker )
from Configurables            import ( LoKi__JetMaker ,LoKi__FastJetMaker )
from CommonParticles.Utils    import *

__all__   = (
    'JetMaker_Configuration'     ,
    'ParticleMaker_Configuration' 
    )

# ----------------------------------------------------------------------------------

# @class JetMaker_Configuration
# Configurable for the jet reconstruction
# @author Victor Coco  (Victor.Coco@cern.ch)
# @date   19.10.2009
class JetMaker_Configuration(LHCbConfigurableUser):
    ## Options
    __slots__ = { 
                  "JetsInputSequencer" : None    # The sequencer to make the jets input
                  ,'JetSeeder'  :  None
                  ,'JetMaker'   : 'anti_kt'
                  ,'RParameter' : 0.75
                  ,'Areas'      : False
                  ,'OutputContainer'  : 'StdJets'
                  ,'JetEnergyScale'   : None
                  ,'JetCalibration'   : None
                  ,'InputLocation'    : None
                  ,'ToolConf'         : None
                  
                  }

    JetMakerFromFastJet = ['anti_kt','kt','cambridge','siscone']

    ## Configure the jet maker
    def setupJetMaker(self , name):
        ## set the algorithm
        algorithm = LoKi__JetMaker ( name )
        ## string for jet maker name
        jet_maker = ''
        ### take the jetmaker_Type from the slots
        jetmaker_Type = self.getProp('JetMaker')
        
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

        ### Jet Algorithm ala Milano Cone
        elif jetmaker_Type == 'lhcb_milano_cone':
                raise RuntimeError("ERROR : the "+jetmaker+" jet algorithm is not available yet")
        ### Jet Algorithm ala Lausanne Cone
        elif jetmaker_Type == 'lhcb_lausanne_cone':
                raise RuntimeError("ERROR : the "+jetmaker+" jet algorithm is not available yet")
        ### Jet Algorithm UNKNOWN
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
        ### Jet Algorithm ala Milano Cone
        elif jetmaker_Type == 'lhcb_milano_cone':
                raise RuntimeError("ERROR : the "+jetmaker+" jet algorithm is not available yet")
        ### Jet Algorithm ala Lausanne Cone
        elif jetmaker_Type == 'lhcb_lausanne_cone':
                raise RuntimeError("ERROR : the "+jetmaker+" jet algorithm is not available yet")
        ### Jet Algorithm UNKNOWN
        else : 
            raise RuntimeError("ERROR : the "+jetmaker+" jet algorithm is not available yet")
        setattr(tool,'RParameter',self.getProp('RParameter'))


    ## Set the slots to prefined values 
    def Predefined_Conf(self , name ):
        if name == 'AntiKt':
            """
            Configuration to get the quarks and gluons comming from q in pp->Z+q
            """
            self.setProp('From' , 'HepMC' )
            self.setProp('OutputContainer' , 'StdPartons' )
            ToolConfDict =  {'PartonicMode': True
                             ,'Particles' : ['g','u','d','s','c','b']
                             ,'From' : ['u','d','s','c','b']
                             ,'UseSelector' : True
                             ,'MotherSelectorName' : 'ZPlusJetHepMCSelector'
                             ,'OutputTable' : 'Relations/Phys/StdPartons2HepMC'}
            self.setProp( 'ToolConf'  , ToolConfDict )
            self.setProp( 'InAcceptance' , False )
             
        
        
    ## Apply the configuration to the given sequence
    def applyConf(self):
        algorithm , tool = self.setupParticleMaker(self.getProp('OutputContainer') )
        self.ConfigParticleMaker(tool,self.getProp('ToolConf'))
        locations = updateDoD (algorithm)


# ----------------------------------------------------------------------------------

## @class  ParticleMaker_Configuration
#  Configurable for the particle makers implementing the IParticleMaker interface
#  @author Victor Coco  (Victor.Coco@cern.ch)
#  @date   2009.10.29
class ParticleMaker_Configuration(LHCbConfigurableUser):
    ## Options
    __slots__ = { 
        'Predefined'        : ''
        ,'From' : 'HepMC'
        ,'OutputContainer'  :  'StdHepMC'
        ,'ToolConf'  :  {'PartonicMode': False
                         ,'Particles' : ['gamma','pi+','e+','p+','K+','mu+','pi+']
                         ,'UseSelector' : False
                         ,'OutputTable' : 'Relations/Phys/StdHepMC2HepMC'}
        ,'InAcceptance'     : False
        }
    _propertyDocDct = {
        'Predefined'     : """ Call a predefined configuration [''(default),'Partons_Zjet','HepMCRef_Zjet',...]""",
        'From'           : """ From which type of info does the LHCb::Particle are made ['HepMC,'MC']""" ,
        'OutputContainer': """ Name of the output container ex: to find it in the tes at /Event/Phys/StdHepMCPart, name is StdHepMCPart""" ,
        'ToolConf': """ A dictionnary containing the configuratyion of the tool with the following format: {'Proterty1':Value1,...,'ProtertyN':ValueN} """,
        'InAcceptance'   : """ Tells if the acceptance cut should be applied"""
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
             particle_maker = 'MCParticleMaker'
             algorithm.ParticleMaker = particle_maker
             algorithm.addTool ( MCParticleMaker )
        
        else :
            raise RuntimeError("ERROR : no ParticleMaker have been implemented to make LHCb::Particle from "+self.getProp('From'))
        
        ## getback the tool for later configuration
        tool = getattr ( algorithm , particle_maker )
        return algorithm , tool

    ## Configure the particle maker
    def ConfigParticleMaker(self ,  tool , dict_config ):
        if not self.getProp('InAcceptance') and self.getProp('From') == 'HepMC' :
            ## set the acceptance cut to null
            dict_config['MinPtGamma'] = 0.
            dict_config['MinThetaGamma'] = 0.
            dict_config['MaxThetaXGamma'] = 10.
            dict_config['MaxThetaYGamma'] = 10.
            dict_config['MinPCharged'] = 0.
            dict_config['MinPtCharged'] = 0.
            dict_config['MinThetaCharged'] = 0.
            dict_config['MaxThetaCharged'] = 10.
            dict_config['MaxZProduction'] = 0.
            dict_config['MaxRhoProduction'] = 0.
            dict_config['MinZend'] = 1000000000.
        for slot in dict_config.keys():
            setattr(tool,slot,dict_config[slot])
        
            

        ## Set the slots to prefined values 
    def Predefined_Conf(self , name ):
        if name == 'Partons_Zjet':
            """
            Configuration to get the quarks and gluons comming from q in pp->Z+q
            """
            self.setProp('From' , 'HepMC' )
            self.setProp('OutputContainer' , 'StdPartons' )
            ToolConfDict =  {'PartonicMode': True
                             ,'Particles' : ['g','u','d','s','c','b']
                             ,'From' : ['u','d','s','c','b']
                             ,'UseSelector' : True
                             ,'MotherSelectorName' : 'ZPlusJetHepMCSelector'
                             ,'OutputTable' : 'Relations/Phys/StdPartons2HepMC'}
            self.setProp( 'ToolConf'  , ToolConfDict )
            self.setProp( 'InAcceptance' , False )


        if name == 'HepMCRef_Zjet':
            """
            Configuration to get all stable particles comming from q in pp->Z+q
            """
            self.setProp('From' , 'HepMC' )
            self.setProp('OutputContainer', 'StdHepMCRef' )
            ToolConfDict =  { 'Particles' : ["gamma","e+","mu+","pi+","K+","nu","p+","nu_mu","nu_tau","n0", "KL0"]
                              ,'From' : ['u','d','s','c','b']
                              ,'UseSelector' : True
                              ,'MotherSelectorName' : 'ZPlusJetHepMCSelector'
                              ,'OutputTable' : 'Relations/Phys/StdHepMCRef2HepMC'}
            self.setProp( 'ToolConf', ToolConfDict ) 
            self.setProp( 'InAcceptance', False )

        if name == 'HepMC_Zjet':
            """
            Configuration to get all stable particles, except neutrinos, comming from q in pp->Z+q
            """
            self.setProp('From' , 'HepMC' )
            self.setProp('OutputContainer' , 'StdHepMC' )
            ToolConfDict =  { 'Particles' : ["gamma","e+","mu+","pi+","K+","p+","n0", "KL0"]
                              ,'From' : []
                              ,'UseSelector' : False
                              ,'OutputTable' : 'Relations/Phys/StdHepMC2HepMC'}
            self.setProp( 'ToolConf' , ToolConfDict )
            self.setProp( 'InAcceptance' , False )

        if name == 'HepMCAcc_Zjet':
            """
            Configuration to get all stable particles, except neutrinos, comming from q in pp->Z+q
            """
            self.setProp('From' , 'HepMC' )
            self.setProp('OutputContainer' , 'StdHepMCAcc' )
            ToolConfDict =  { 'Particles' : ["gamma","e+","mu+","pi+","K+","p+","n0", "KL0"]
                              ,'From' : []
                              ,'UseSelector' : False
                              ,'OutputTable' : 'Relations/Phys/StdHepMCAcc2HepMC'}
            self.setProp( 'ToolConf' , ToolConfDict )
            self.setProp( 'InAcceptance' , True )
            
        if name == 'Partons_HidValley':
            """
            Configuration to get the quarks and gluons comming from q in pp->Z+q
            """
            self.setProp('From' , 'HepMC' )
            self.setProp('OutputContainer' , 'StdPartons' )
            ToolConfDict =  {'PartonicMode': True
                             ,'Particles' : ['g','u','d','s','c','b']
                             ,'From' : ['H_30']
                             ,'UseSelector' : False
                             ,'OutputTable' : 'Relations/Phys/StdPartons2HepMC'}
            self.setProp( 'ToolConf'  , ToolConfDict )
            self.setProp( 'InAcceptance' , False )
             
             
        if name == 'HepMCRef_HidValley':
            """
            Configuration to get all stable particles comming from q in pp->Z+q
            """
            self.setProp('From' , 'HepMC' )
            self.setProp('OutputContainer', 'StdHepMCRef' )
            ToolConfDict =  { 'Particles' : ["gamma","e+","mu+","pi+","K+","nu","p+","nu_mu","nu_tau","n0", "KL0"]
                              ,'From' : ['H_30']
                              ,'UseSelector' : False
                              ,'OutputTable' : 'Relations/Phys/StdHepMCRef2HepMC'}
            self.setProp( 'ToolConf', ToolConfDict ) 
            self.setProp( 'InAcceptance', False )

        if name == 'HepMC_HidValley':
            """
            Configuration to get all stable particles comming from q in pp->Z+q
            """
            self.setProp('From' , 'HepMC' )
            self.setProp('OutputContainer', 'StdHepMC' )
            ToolConfDict =  { 'Particles' : ["gamma","e+","mu+","pi+","K+","p+","n0", "KL0"]
                              ,'From' : []
                              ,'UseSelector' : False
                              ,'OutputTable' : 'Relations/Phys/StdHepMC2HepMC'}
            self.setProp( 'ToolConf', ToolConfDict ) 
            self.setProp( 'InAcceptance', False )

        if name == 'HepMCAcc_HidValley':
            """
            Configuration to get all stable particles comming from q in pp->Z+q
            """
            self.setProp('From' , 'HepMC' )
            self.setProp('OutputContainer', 'StdHepMCAcc' )
            ToolConfDict =  { 'Particles' : ["gamma","e+","mu+","pi+","K+","p+","n0", "KL0"]
                              ,'From' : []
                              ,'UseSelector' : False
                              ,'OutputTable' : 'Relations/Phys/StdHepMCAcc2HepMC'}
            self.setProp( 'ToolConf', ToolConfDict ) 
            self.setProp( 'InAcceptance', True ) 
             

        if name == 'StdMCReconstructible_HidValley':
            """
            Configuration to get all reconstructible mc particles comming from Hidden Valley
            Suited to get what a jet should be with the real acceptance, if the detector was perfect
            """
            self.setProp('From' , 'MC' )
            self.setProp('OutputContainer', 'StdMCReconstructible' )
            ToolConfDict =  { 'ParticleNames' : ["gamma","e+","e-","mu+","mu-","pi+","pi-","K+","K-","p+","p-","n0","KL0"]
                              ,'SmearParticle' : False
                              ,'OnlyReconstructible' : True
                              ,'OutputTable' : 'Relations/Phys/StdMCReconstructible2MC'}
            self.setProp( 'ToolConf', ToolConfDict ) 

            
    ## Apply the configuration to the given sequence
    def applyConf(self):
        if self.getProp('Predefined') != '' : self.Predefined_Conf(self.getProp('Predefined'))
        algorithm , tool = self.setupParticleMaker(self.getProp('OutputContainer') )
        self.ConfigParticleMaker(tool,self.getProp('ToolConf'))
        locations = updateDoD (algorithm)


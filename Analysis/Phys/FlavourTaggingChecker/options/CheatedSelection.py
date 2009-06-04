########################################################################
#
# Options for CheatedSelection algorithm
#
# @author Marco Musy
# @date 2009-02-23
#
########################################################################
#
from GaudiConf.Configuration import *
from Configurables import DaVinci, CheatedSelection, CombinedParticleMaker, FilterDesktop


cheatparts = CombinedParticleMaker("Cheatparts")

tagName = 'CheatingParticles'
chParticles = FilterDesktop(tagName)
chParticles.InputLocations = [ "Cheatparts" ]
chParticles.Code = "(P>2.0*GeV)"    ## @todo Add more cuts here

hat = '/Event/Phys/'
dod = DataOnDemandSvc()
dod.AlgMap.update( { hat + 'Cheatparts/Particles' : 'CombinedParticleMaker/Cheatparts' ,
                     hat + 'Cheatparts/Vertices'  : 'CombinedParticleMaker/Cheatparts' } )

cheat = CheatedSelection("CheatedSelection")

cheat.InputLocations= [ "Cheatparts" ]

cheat.AssociatorInputData = [ "Phys/Cheatparts/Particles" ]
DaVinci().UserAlgorithms += [ cheat ]


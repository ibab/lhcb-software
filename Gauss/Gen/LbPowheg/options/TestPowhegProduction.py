# Options specific for a given job
# ie. setting of random number seed and name of output files
#

from Gauss.Configuration import *

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

#--Number of events
nEvts = 5
LHCbApp().EvtMax = nEvts

importOptions('$GAUSSOPTS/Gauss-DEV.py')
#importOptions('$GAUSSOPTS/GenStandAlone.py')


#from Gaudi.Configuration import *
#importOptions( "$DECFILESROOT/options/SwitchOffAllPythiaProcesses.py" )

from Configurables import Generation, Special, FixedNInteractions, PowhegProductionWbb, PowhegProductionWZ,PowhegProductionZZ, MinimumBias, ReadLHEfileProduction

gen = Generation()

#================================================================================================================
# Sample Generation Tool (Special  events:   events defined with  special  generator  settings) 
#================================================================================================================
gen.SampleGenerationTool = "Special"
gen.addTool( Special() )

#================================================================================================================
# Production  Tool (it  takes  care  of  the primary  pp  collision  generation -  an interface to the generator)
#================================================================================================================
gen.Special.ProductionTool = "PowhegProductionWbb"
gen.Special.addTool( PowhegProductionWbb () )
powheg=gen.Special.PowhegProductionWbb

#gen.Special.ProductionTool = "PowhegProductionWZ"
#gen.Special.addTool( PowhegProductionWZ () )
#powheg=gen.Special.PowhegProductionWZ

#gen.Special.ProductionTool = "PowhegProductionZZ"
#gen.Special.addTool( PowhegProductionZZ () )
#powheg=gen.Special.PowhegProductionZZ

#================================================================================================================
# POWHEG-BOX production parameters
#================================================================================================================
powheg.numevts = nEvts
powheg.iseed = 978
powheg.ebeam1 = 7000
powheg.ebeam2 = 7000


#================================================================================================================
# Pile-Up Tool:  
#================================================================================================================
# to  provide  the computation of the number of interactions in one event, N int .

gen.Special.ReinitializePileUpGenerator  = False

#================================================================================================================

gen.Special.CutTool = ""	# no cuts
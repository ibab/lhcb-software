# Options specific for a given job
# ie. setting of random number seed and name of output files
#

from Gauss.Configuration import *

#--Generator phase, set random numbers
GaussGen = GenInit("GaussGen")
GaussGen.FirstEventNumber = 1
GaussGen.RunNumber        = 1082

#--Number of events
nEvts = 3
LHCbApp().EvtMax = nEvts

importOptions('$GAUSSOPTS/Gauss-DEV.py')
#importOptions('$GAUSSOPTS/GenStandAlone.py')


#from Gaudi.Configuration import *
#importOptions( "$DECFILESROOT/options/SwitchOffAllPythiaProcesses.py" )

from Configurables import Generation, Special, Inclusive, FixedNInteractions, PowhegProductionZZ, MinimumBias, ReadLHEfileProduction

gen = Generation()

#================================================================================================================
# Sample Generation Tool (Special  events:   events defined with  special  generator  settings) 
#================================================================================================================
gen.SampleGenerationTool = "Special"
gen.addTool( Special() )

#================================================================================================================
# Pile-Up Tool:  
#================================================================================================================
# to  provide  the computation of the number of interactions in one event, N int .
gen.PileUpTool = "FixedNInteractions"
gen.addTool( FixedNInteractions() )
gen.FixedNInteractions.NInteractions = 1
gen.Special.CutTool = ""	# no cuts

#================================================================================================================
# Production  Tool (it  takes  care  of  the primary  pp  collision  generation -  an interface to the generator)
#================================================================================================================
gen.Special.ProductionTool = "PowhegProductionZZ"
gen.Special.addTool( PowhegProductionZZ () )
powheg=gen.Special.PowhegProductionZZ

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

#gen.Special.ReinitializePileUpGenerator  = False

#================================================================================================================

Generation().addTool( Inclusive() )
Generation().Inclusive.CutTool = "LHCbAcceptance"

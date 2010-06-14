### @file DVTest-PromptDstar.py
 #
 #  Test file for HLT on selected prompt D* events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="PromptDstar"
               , DataType = '2010'
               , Simulation = False 
               , Decay = "{[D*(2010)+ -> (D0 -> ^K- ^pi+ {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^K+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^K+ ^K- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc}"
               # Patrick Spradlin, http://www-pnp.physics.ox.ac.uk/~spradlin/CharmPeaks/D02HH/Spradlin/index.html
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/g/gligorov/OfflineDSTsForTriggerOptimization/RealData/D02HH-Spradlin-Big-MagUp.dst' TYP='POOL_ROOTTREE' OPT='READ'","DATAFILE='PFN:castor:/castor/cern.ch/user/g/gligorov/OfflineDSTsForTriggerOptimization/RealData/Dstar-MagDown-8645427.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()


### @file DVTest-Bd2D0Kstar.py
 #
 #  Test file for HLT on selected Bd2D0Kstar events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bd2D0Kstar"
               , Decay = "{[[B0]nos,[[B~0]os}  -> (D~0 -> ^K+ ^pi- {gamma,}{gamma,})) (K*0(892) -> ^K+ ^pi-^ {gamma,}{gamma,}) {gamma,}{gamma,}]cc"
               # Jacopo Nardulli, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/j/jnardull/MC09/Bd2D0Kst.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()


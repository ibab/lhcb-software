### @file DVTest-DstarWithD02KsKK.py
 #
 #  Test file for HLT on selected DstarWithD02KsKK events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="DstarWithD02KsKK"
               , Decay = "[D*(2010)+ ->  ^pi+ (D0 -> ( KS0 -> ^pi+ ^pi- {,gamma}{,gamma} ) ^K+ ^K- {,gamma}{,gamma}) {,gamma}]cc"
               # Mat Charles, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/m/mcharles/128/outputdata/dstar.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()


### @file DVTest-Bd2JpsiKs_DD.py
 #
 #  Test file for HLT on selected Bd2JpsiKs_DD events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bd2JpsiKs_DD"
               , Decay = "{[[B0]os -> (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) (KS0 -> ^pi+ ^pi-)]cc,[[B0]nos -> (J/psi(1S) -> ^mu+ ^mu- {,gamma}{,gamma}) (KS0 -> ^pi+ ^pi-)]cc}"
               # F Rodrigues, https://twiki.cern.ch/twiki/bin/view/LHCb/NewCpOfflineSelected
               , Input = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/f/frodrigu/091020_Bd2JpsiKS_DST/Bd2JpsiKS_DD.dst' TYP='POOL_ROOTTREE' OPT='READ'" ])
CS.configure()


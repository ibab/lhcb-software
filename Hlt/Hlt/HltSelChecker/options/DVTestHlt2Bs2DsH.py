### @file DVTestHlt2Bs2DsH.opts
 #
 #  Test file for HLT Bs->Ds(KKpi)(K,pi) selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( dv=DaVinci()
               , s="Bs2DsH"
               , d = "{[[B_s0]nos -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ^K+ {,gamma}{,gamma}]cc, [[B_s0]os -> (D_s+ => ^K- ^K+ ^pi+ {,gamma}{,gamma}{,gamma}) ^K- {,gamma}{,gamma}]cc, [[B_s0]nos -> (D_s- => ^K+ ^K- ^pi- {,gamma}{,gamma}{,gamma}) ^pi+ {,gamma}{,gamma}]cc, [[B_s0]os -> (D_s+ => ^K- ^K+ ^pi+ {,gamma}{,gamma}{,gamma}) ^pi- {,gamma}{,gamma}]cc}"
               , ip = [ "DATAFILE='PFN:castor:/castor/cern.ch/user/d/dijkstra/Selections-DC06/Bs2DsK-lum2-fixedL0.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]
               , dt = 'DC06')

CS.configure()


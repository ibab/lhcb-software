### @file DVTest-Bd2DstarMuNu.py
 #
 #  Test file for HLT on selected Bd2DstarMuNu events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal="Bd2DstarMuNu"
               , Decay = "{[[B0]nos -> nu_mu ^mu+ (D*(2010)- -> ^pi- (D~0 -> ^K+ ^pi-))]cc, [[B0]os -> nu_mu~ ^mu- (D*(2010)+ -> ^pi+ (D0 -> ^K- ^pi+))]cc}"
               # 
               , Input = [  ])
CS.configure()


#
# Stripping selections 
# for Minimum Bias physics.
# author: Yasmine Amhis
# date : 2009-11-11
#
 
# Begin StrippingNoBias.py
from Configurables import LoKi__VoidFilter as VoidFilter
from StrippingConf.StrippingLine import StrippingLine
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory

modules =  CoreFactory('CoreFactory').Modules
for i in [ 'LoKiTrigger.decorators' ] :
  if i not in modules : modules.append(i)


#-----------------------------------------------------------------
#The following 4 lines use Random trigger decoded with the ODIN
#-----------------------------------------------------------------
RandomBeam1Line  = StrippingLine( "RandomBeam1Line"
				  , ODIN = 'ODIN_BXTYP == LHCb.ODIN.Beam1'
                                  , checkPV = False
                                  , prescale = 1
                                  , postscale = 1)

#-----------------------------------------------------------------
RandomBeam2Line  = StrippingLine( "RandomBeam2Line"
                                   , ODIN =  'ODIN_BXTYP == LHCb.ODIN.Beam2'
                                  , checkPV = False
                                  , prescale =1
                                  , postscale = 1)

#-----------------------------------------------------------------
RandomNoBeamLine  = StrippingLine("RandomNoBeamLine"
				   , ODIN = 'ODIN_BXTYP == LHCb.ODIN.NoBeam'
                                  , checkPV = False
				  , prescale =1
                                  , postscale = 1)
#-----------------------------------------------------------------
RandomBeamCrossingLine  = StrippingLine ("RandomBeamCrossingLine"
					  ,ODIN = 'ODIN_BXTYP == LHCb.ODIN.BeamCrossing'
                                         , checkPV = False
                                         , prescale =1
                                         , postscale = 1)

# End StrippingNoBias.py


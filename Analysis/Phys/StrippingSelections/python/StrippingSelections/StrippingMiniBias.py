#
# Stripping selections 
# for Minimum Bias physics.
# author: Yasmine Amhis
# date : 2009-11-11
#
 
# Begin StrippingMinBias.py
from Configurables import LoKi__VoidFilter as VoidFilter
from StrippingConf.StrippingLine import StrippingLine
from Configurables import LoKi__Hybrid__CoreFactory as CoreFactory

modules =  CoreFactory('CoreFactory').Modules
for i in [ 'LoKiTrigger.decorators' ] :
  if i not in modules : modules.append(i)
#----------------------------------------------------------------------
#Here we require at least one  reconstructed best track in the event 

#-----------------------------------------------------------------
# Microbias line
#-----------------------------------------------------------------
MBMicroBiasLine  = StrippingLine( "MBMicroBias"
                                  , HLT = "HLT_PASS_RE('Hlt1MBMicro.*Decision')"
                                  , checkPV = False
                                  , prescale = 1
                                  , postscale = 1)

#-----------------------------------------------------------------
# No bias line
#-----------------------------------------------------------------
MBNoBiasLine  = StrippingLine( "MBNoBias"
                               , HLT = "HLT_PASS('Hlt1MBNoBiasDecision')"
                               , checkPV = False
                               , prescale = 1
                               , postscale = 1)

#-----------------------------------------------------------------
# Mini bias line
#-----------------------------------------------------------------
MBMiniBiasLine  = StrippingLine( "Hlt1L0Any"
                                 , HLT = "HLT_PASS('Hlt1MBMiniBiasDecision')"
                                 , checkPV = False
                                 , prescale = 1
                                 , postscale = 1)


# End StrippingMiniBias.py


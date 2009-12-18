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


TrackFilter = VoidFilter('TrackFilter', Code = "TrSOURCE('Rec/Track/Best') >>(TrSIZE )>1 ")

#Tracking information line
TrackLine = StrippingLine( "TrackLine" 
                                         , algos = [TrackFilter]
                                         , checkPV = False 
                                         , prescale = 1
                                         , postscale = 1 )
#----------------------------------------------------------------------
#Here we require at least one reconstructed Primary Vertex per event.
VertexFilter = VoidFilter(  'VertexFilter', Code = "RVSOURCE('Rec/Vertex/Primary') >> (RVSIZE > 0 )")
#Vertex information line
VertexLine = StrippingLine( "VertexLine" 
                                         , algos = [VertexFilter]
					 , checkPV = 1	 
                                         , prescale = 1
                                         , postscale = 1 )

#----------------------------------------------------------------------
L0MinBiasLine = StrippingLine( "L0MinBiasLine" 
                                , L0DU = "L0_CHANNEL('CALO')"
                               , checkPV = False
                               , prescale = 1
                               , postscale = 1)


#Little note: for  TCK = 0x2009
#The "CALO" channel --> [HCAL > 25 and SPD > 2]


		  
# End StrippingMiniBias.py


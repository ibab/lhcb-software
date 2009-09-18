##
## Switch on geometry upstream of Velo
##
from Gaudi.Configuration import *
from Configurables import GiGaInputStream
from GaudiKernel import SystemOfUnits

geo = GiGaInputStream('Geo')
geoList = geo.StreamItems
det = ''
for d in geoList:
    if d.find('/dd/Structure/LHCb/UpstreamRegion') != -1:
        det = d

if det != '':
    geoList.remove(det)

geo.StreamItems += [ "/dd/Structure/LHCb/UpstreamRegion" ]

## Extend stopping upstream
from Configurables import ( GiGa, GiGaRunActionSequence, TrCutsRunAction )
giga = GiGa()
giga.addTool( GiGaRunActionSequence("RunSeq") , name="RunSeq" )
giga.RunSeq.addTool( TrCutsRunAction("TrCuts") , name = "TrCuts" )
giga.RunSeq.TrCuts.MinZ = -25.0*SystemOfUnits.m

## Import here the extra magnetic field upstream
## This will need an addition to the configurable to specify which
## field to select, then here we will put the configuration

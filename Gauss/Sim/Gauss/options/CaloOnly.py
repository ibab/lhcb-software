## Options to switch off all geometry (and related simulation)
## but for that of calorimeters area.  But leave containers in output.
##
## Author: G.Corti
## Date:   2009-09-16
##

from Gaudi.Configuration import *

importOptions('$GAUSSOPTS/RICHesOff.py')

def onlyCaloGeometry():
    
    from Configurables import GiGaInputStream

    # Define only the Calo geometry
    geo = GiGaInputStream('Geo')
    geo.StreamItems  = ["/dd/Structure/LHCb/DownstreamRegion/Spd"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Converter"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Prs"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Ecal"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/Hcal"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/PipeDownstream"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/PipeSupportsDownstream"]
    geo.StreamItems += ["/dd/Structure/LHCb/DownstreamRegion/PipeBakeoutDownstream"]


appendPostConfigAction(onlyCaloGeometry)

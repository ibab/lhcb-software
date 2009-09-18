## Options to switch off all geometry (and related simulation)
## but for that of calorimeters area.  But leave containers in output.
##
## Author: G.Corti
## Date:   2009-09-16
##

from Gaudi.Configuration import *

def switchOffRICH():
    
    from Configurables import GiGaInputStream

    # Switch off the RICH geometry
    geo = GiGaInputStream('Geo')
    geo.StreamItems.remove("/dd/Structure/LHCb/BeforeMagnetRegion/Rich1")
    geo.StreamItems.remove("/dd/Geometry/BeforeMagnetRegion/Rich1/Rich1Surfaces")
    geo.StreamItems.remove("/dd/Structure/LHCb/AfterMagnetRegion/Rich2")
    geo.StreamItems.remove("/dd/Geometry/AfterMagnetRegion/Rich2/Rich2Surfaces")
    geo.StreamItems.remove("/dd/Geometry/BeforeMagnetRegion/Rich1/RichHPDSurfaces")


# Switch off RICH physics
from Configurables import ( GiGa, GiGaPhysListModular,
                            GiGaPhysConstructorOp, GiGaPhysConstructorHpd )
                           
giga = GiGa()
giga.addTool( GiGaPhysListModular("ModularPL") , name="ModularPL" )
giga.ModularPL.addTool( GiGaPhysConstructorOp, name = "GiGaPhysConstructorOp" )
giga.ModularPL.addTool( GiGaPhysConstructorHpd, name = "GiGaPhysConstructorHpd" )
giga.ModularPL.GiGaPhysConstructorOp.RichOpticalPhysicsProcessActivate = False
giga.ModularPL.GiGaPhysConstructorHpd.RichHpdPhysicsProcessActivate = False

appendPostConfigAction(switchOffRICH)

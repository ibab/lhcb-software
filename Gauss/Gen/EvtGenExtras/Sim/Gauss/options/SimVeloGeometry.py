# *
# *  IT IS NO LONGER NECESSARY TO EXPLICITELY PASS EACH DETECTOR ELEMENT
# *  WITH MISALIGNED VALUES IN CONDDB (RESPECT TO WHAT IN THE GEOMETRY) IF
# *  THE ASSOCIATED PVOLUME BELONGS TO A UNIQUE LVOLUME
# * 
# *  THIS IS THE CASE FOR THE VELO!!! Look at example in v2006-OpenVelo...
# *  
# *  File containing the list of detector element to explicitely set
# *  to have misalignement in the VELO.
# *  Should be included from SimGeometry.opts instead of line
# *    Geo.StreamItems         += {"/dd/Structure/LHCb/BeforeMagnetRegion/Velo"};  
# *
# *  The misaligned VELO requires to use XmlDDDB v26r0 or higher
# * 
# *  @author J.Palacio
# *  @author M.Tobin
# *  @date 2006-02-14
#  modified for python, T.Ruf 18.11.2008

from Gaudi.Configuration import *
from Configurables import GiGaInputStream

Geo = GiGaInputStream('Geo')
Geo.StreamItems.remove("/dd/Structure/LHCb/BeforeMagnetRegion/Velo") 

Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/ModulePU00")
Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/ModulePU02")
Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/ModulePU01")
Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/ModulePU03")

txt = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/ModuleXX"
import math
for i in range(42):                                                           
 nr = str(i)                                                                  
 if len(nr) == 1 : nr = '0'+str(i)
 temp1 = txt.replace('XX',nr)
 if math.modf(float(nr)/2.)[0] > 0.1 :  temp1 = temp1.replace('Left','Right')
 Geo.StreamItems.append(temp1)           
 
Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/DownStreamWakeFieldCone")
Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/UpStreamWakeFieldCone")
Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/UpStreamVacTank")
Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/DownStreamVacTank")
Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloRight/RFFoilRight")
Geo.StreamItems.append("/dd/Structure/LHCb/BeforeMagnetRegion/Velo/VeloLeft/RFFoilLeft")

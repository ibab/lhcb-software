from Gaudi.Configuration import *
from Configurables import ( Escher, TrackSys )
from Configurables import (VAlign,WriteAlignmentConditions)
from Configurables import Millepede
from GaudiKernel.ProcessJobOptions import importOptions

#########################
#
# JobOptions file for Velo Alignment algorithm
#
########################


# Option file for monitoring (control sample, constants,...)
importOptions( '$VELOALIGNMENTOPTS/VeloAlignmentMonitor.py' )
#
# Then we set the alignment job properties
#

#
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
# The main alignment switchboard
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\
#


##########################
#
# 1. Internal alignment with tracks
#
##########################


VAlign("VAlign").Internal_Alignment    = False   # Turn this step on

# Modules to be aligned (1=ON, 0=OFF)

VAlign("VAlign").Internal_Mod_Left  = [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]
VAlign("VAlign").Internal_Mod_Right = [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1]

VAlign("VAlign").Internal_DOF          = [1,1,1,1,1,1]                         # DOFs to look at
VAlign("VAlign").Internal_PTerms       = [0.01,0.01,0.005,0.0005,0.0005,0.0002] # ParSig values/DOFs (penalty terms)
VAlign("VAlign").Internal_EQs          = [1,1,1,1,1,1,1,1,1]                   # Constraint equations
VAlign("VAlign").Internal_Residual_Cut = [0.3,0.06]                            # Cuts on track residuals (in mm)
VAlign("VAlign").General_Startfactor   = 100.0                                  # Localfit chisquare initial cut
VAlign("VAlign").General_Maxtracks     = 100                                     # Max. number of track per event

# Option files for pattern recognition tuning and track selection
importOptions( '$VELOALIGNMENTOPTS/TRGtune.py' )
importOptions( '$VELOALIGNMENTOPTS/TrackStore.py' )


##########################
#
# 2. Box alignment
#
##########################


VAlign("VAlign").Box_Alignment        = True
VAlign("VAlign").Box_VELOopen         = False
VAlign("VAlign").Box_DOF              = [1,1,1,1,1,1]                         
VAlign("VAlign").Box_PTerms           = [10.0,10.0,10.0,0.03,0.03,0.03]
VAlign("VAlign").Box_EQs              = [1,1,1,1,1,0] 
VAlign("VAlign").Box_Residual_Cut     = [0.5,0.4]

# Specific options for primary vertices
VAlign("VAlign").Box_MinTracks_perPV  = 10
                            


##########################
#
# 4. Millepede specific options
#
##########################

VAlign("VAlign").addTool(Millepede("Millepede"))
tool=Millepede("Millepede")
tool.OutputLevel   = 3
tool.Iteration     = True
tool.ModuleFixed   = -1 # Fix the a module in Millepede (use Millepede numbering)
                                      # (put -1 if you don't want to fix anything) 
                                      #
#importOptions( '$VELOALIGNMENTOPTS/VeloMillepede.opts' )


from Configurables import (WriteAlignmentConditions)

writeGlobal=WriteAlignmentConditions("WriteGlobal")
writeModules= WriteAlignmentConditions("WriteModules")
writeDetectors= WriteAlignmentConditions("WriteDetectors")
writeGlobal.topElement = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
writeGlobal.depths = [ 0, 1 ]
writeGlobal.outputFile = "Global.xml"
writeModules.topElement = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
writeModules.depths = [ 2 ]
writeModules.outputFile = "Modules.xml"
writeDetectors.topElement = "/dd/Structure/LHCb/BeforeMagnetRegion/Velo"
writeDetectors.depths = [ 4 ]
writeDetectors.outputFile = "Detectors.xml"


# Define application, version and master package
app = DaVinci()
app.version = "v22r0p2"
app.masterpackage = "Phys/DaVinci/v22r0p2"

# Define job-options file
import os
app.optsfile = \
  [
  os.environ[ "User_release_area" ] \
  + "/DaVinci_v22r0p2/Phys/DstCalibSel/v1r2/options/Dstar2D0PiPresel.py"
  ]

# Define options to be appended to file
app.extraopts = \
'''
# Define input data (physical filenames for running locally)
#importOptions( "/data/lhcb/lhcb_data/DC06/cards/options/DC06-phys-v4-lumi2-27363000-Dst_D0pi_hh.ox.pfn.opts" )
importOptions( "/data/lhcb/lhcb_data/DC06/11463000-Bd_Dst+X_2body/DC06-phys-v4-lumi2/options/DC06-phys-v4-lumi2-11463000-Bd_Dst+X_2body.ox.pfn.opts" )

# Set maximum number of events
DaVinci().EvtMax = 100000

'''

# Choose PBS backend and specify walltime
bck = PBS()
bck.extraopts = "-l walltime=01:00:00"

# Split into subjobs, defining maximum number of input files to analyse
# and number of input files per subjob
splitter = SplitByFiles()
splitter.maxFiles = 200
splitter.filesPerJob = 10

# Define subjob files to be merged
merger = SmartMerger()
merger.files = [ "stderr", "stdout", "SigBDst2DPiNew.root" ]

# Create and submit job
j = Job( name = "DaVinci_PBS_Split", application = app, backend = bck,
  splitter = splitter, merger = merger )
j.submit()

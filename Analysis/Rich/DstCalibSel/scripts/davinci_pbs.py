# Define application, version and master package
app = DaVinci()
app.version = "v22r0p2"
app.masterpackage = "Phys/DaVinci/v22r0p2"

# Define job-options file
import os
app.optsfile = \
  [
  os.environ[ "User_release_area" ] \
  + "/DaVinci_v22r0p2/Phys/DstCalibSel/v1r1/options/D02KPi.py"
  ]

# Define options to be appended to file
app.extraopts = \
'''
# Define input data (physical filenames for running locally)
importOptions( "/data/lhcb/lhcb_data/DC06/cards/options/DC06-phys-v4-lumi2-27363000-Dst_D0pi_hh.ox.pfn.opts" )

# Set maximum number of events
DaVinci().EvtMax = 5000

'''

# Choose PBS backend and specify walltime
bck = PBS()
bck.extraopts = "-l walltime=01:00:00"

# Create and submit job
j = Job( name = "DaVinci_PBS", application = app, backend = bck )
j.submit()

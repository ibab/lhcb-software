
#==============================================================================
# Example ganga job submission file
#==============================================================================
#
# Set ganga environment first with (e.g. lxplus)
#  > GangaEnv
# Then choose your ganga version (default version is normally the best option)
#
# Submit with
#  > ganga Gauss_Ganga.py
#
#==============================================================================
#
# For more information on ganga see http://cern.ch/ganga
#
# In particular, if you think you have found a bug, or you wish to suggest
# some improvement in functionality, please report this at
# https://savannah.cern.ch/projects/ganga/
#
# For Dirac jobs, you can monitor their status at
# http://lhcb.pic.es/DIRAC/Monitoring/Analysis
#
#==============================================================================

#------------------------------------------------------------------------------
# Define the version of Gauss to use
#------------------------------------------------------------------------------
GaussVersion = 'v25r7'
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Define name for identifying the job (this name appears in the ganga job list)
#------------------------------------------------------------------------------
myJobName = 'MyGauss'
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# The application to run
#------------------------------------------------------------------------------
myApplication = Gauss()
# Application version number
myApplication.version = GaussVersion
# Define the options file to use.
# Note, if you want you can replace the PFNs in this file with LFNs directly,
# since ganga takes care of the book-keeping for you :)
myApplication.optsfile = File ( myApplication.cmt_user_path
   + '/Sim/Gauss/' + GaussVersion + '/options/v200601.opts' )
# Extra options
# Appended to the end of the main options to override default settings
#myApplication.extraopts = ''
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Define a job splitter. The "standard" job splitter acts on input data file
# and as such not suitable for Gauss.
# A dedicated splitter for Gauss making n identical jobs (but for random
# numbers) is planned for a future version of Ganga
#------------------------------------------------------------------------------
mySplitter = None

#------------------------------------------------------------------------------
# Job merging does not work as yet
# See https://savannah.cern.ch/bugs/?21054 for details
#------------------------------------------------------------------------------
myMerger = None
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Add here any special input files. Normally not needed.
#------------------------------------------------------------------------------
myInputsandbox = []
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Define where to run
#------------------------------------------------------------------------------
# Run interactively
#myBackend    = Interactive()
# Run directly on the local machine, but in the background
myBackend    = Local()
# Submit to an LSF batch system, using the 8nm queue
#myBackend    = LSF( queue = '8nm' )
# Submit to the grid. Requires a working grid certificate of course :)
#myBackend    = Dirac()
# Submit to a condor batch system
#myBackend    = Condor()
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Finally, create a Ganga job object and submit
#------------------------------------------------------------------------------
j = Job (
 name         = myJobName,
 application  = myApplication,
 splitter     = mySplitter,
 merger       = myMerger,
 inputsandbox = myInputsandbox,
 backend      = myBackend
) 
j.submit()
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Create 5 identical jobs to the one above but with different random seeds by
# changing the run number and for each the first event number,
# each job has 100 events (a pseudo-splitter)
# Also set a different name for each job.
#------------------------------------------------------------------------------
myNevtPerJob = 100;
myNJobs = 5;
for i in range(myNJobs):
  jn = j.copy()
  firstEvt = i*myNevtPerJob + 1
  jn.name = 'MyGauss_%d'%(i)
  jn.application.extraopts = 'ApplicationMgr.EvtMax = %d; \nGaussGen.RunNumber = 2; \nGaussGen.FirstEventNumber = %d;'%(myNevtPerJob,firstEvt)
  jn.submit()

#------------------------------------------------------------------------------


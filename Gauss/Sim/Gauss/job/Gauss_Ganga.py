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
GaussVersion = 'v30r3'
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
# A dedicated splitter for Gauss is available that will make n identical jobs
# (but for random numbers) 
#------------------------------------------------------------------------------
# No job splitting
#mySplitter = None
# Gauss splitter. Lauch two jobs of 5 events each
mySplitter = GaussSplitter( numberOfJobs = 2 , eventsPerJob = 5 )
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Job merging
#------------------------------------------------------------------------------
# No merging
#myMerger = None
#
# Root file merging. For more details run from the ganga prompt
#  ganga > help( RootMerger )
#myMerger = RootMerger( files = ['00001820_00000001_5.root'], ignorefailed = 1 )
#
# SmartMerger - Able to handle various file formats (including root files)
#               For more details run from the ganga prompt
#  ganga > help( SmartMerger )
myMerger = SmartMerger( files = ['00001820_00000001_5.root'], ignorefailed = 1 )
#
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

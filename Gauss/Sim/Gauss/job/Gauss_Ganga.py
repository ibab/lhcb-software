#==============================================================================
# Example ganga job submission file
#==============================================================================
#
# Set ganga environment first with (e.g. lxplus)
#  > GangaEnv
# Then choose your ganga version (default version is normally the best option)
#
# NB : This file is for the ganga 5 releases
#
# To submit you start ganga with the command "ganga" and then
# at the ganga prompt type
#
# [In] 1 : ganga Gauss_Ganga.py
#
# As an alternative you can also submit the job directly from
# the Unix prompt with the line
#
# [user@lxplus]~% ganga Gauss_Ganga.py
#
# but in this way you will have the overhead of starting Ganga
# for each job you submit.
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
# For any ganga object used below, more detailed information can be found by
# running help(XXX) at the ganga prompt. I.e.
#  > help(Dirac)
# For more information on the Dirac backend
#
#==============================================================================

#------------------------------------------------------------------------------
# Make a new job object for Gauss
#------------------------------------------------------------------------------
j = Job( application = Gauss( version = 'v37r4' ) )
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Define name for identifying the job (this name appears in the ganga job list)
#------------------------------------------------------------------------------
j.name = 'MyGauss'
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# The job configuration
#------------------------------------------------------------------------------
# Define the configuration file(s) to use
myOptsPath = ( j.application.user_release_area + '/Gauss_' +
               j.application.version + '/Sim/Gauss/' +
               j.application.version + '/options/' )
j.application.optsfile = [ File ( myOptsPath + 'Gauss-MC09.py' ),
                           File ( myOptsPath + 'Gauss-Job.py'  ) ]
# Extra options
# Appended to the end of the main options to override default settings
#j.application.extraopts = ''
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Define a job splitter (very useful to generate higher statistics)
#------------------------------------------------------------------------------
# Use a single job, no splitting
j.splitter = None

# Split Gauss jobs by making many identical jobs with different random number
# seed by setting a different 'GaussGen.FirstEventNumber' and will produce the
# amount of events specified in 'eventsPerJob'.
# The total number of generated events therefore will be
# 'eventsPerJob*numberOfJob' 
#j.splitter = GaussSplitter ( numberOfJobs = 2 , eventsPerJob = 5 )
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Job merging (merge output from sub-jobs from job splitter, if used)
#------------------------------------------------------------------------------
# No merging
j.merger = None

# Root file merging. See help( RootMerger ) for more details
#j.merger = RootMerger( files = ['histos.root'], ignorefailed = True )

# SmartMerger - Able to handle various file formats (including root files)
#               See help( SmartMerger ) for more details
#j.merger = SmartMerger( files = ['histos.root'], ignorefailed = True )
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Add here any special input files. Normally not needed.
#------------------------------------------------------------------------------
j.inputsandbox = []
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Define where to run
#------------------------------------------------------------------------------
# Run interactively on the local machine
#j.backend    = Interactive()
# Run directly on the local machine, but in the background
j.backend    = Local()
# Submit to an LSF batch system, using the 8nm queue
#j.backend    = LSF( queue = '8nm' )
# Submit to a condor batch system
#j.backend    = Condor()
# Submit to a PBS batch system
#j.backend    = PBS()
# Submit to the grid.
#j.backend    = Dirac()
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Submit the job
#------------------------------------------------------------------------------
j.submit()
#------------------------------------------------------------------------------

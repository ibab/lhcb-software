#$Id: DaVinci_Ganga.py,v 1.70 2010-09-07 15:53:14 jpalac Exp $
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
# [In] 1 : ganga DaVinci_Ganga.py
#
# As an alternative you can also submit the job directly from 
# the Unix prompt with the line
# 
# [user@lxplus]~% ganga DaVinci_Ganga.py
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
# Make a new job object for DaVinci
#------------------------------------------------------------------------------
j = Job( application = DaVinci( version = 'v37r1p2' ) )
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Define name for identifying the job (this name appears in the ganga job list)
#------------------------------------------------------------------------------
j.name = 'MyDaVinci'
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# The job configuration
#------------------------------------------------------------------------------
# Define the configuration file(s) to use
appOpts = j.application.user_release_area + '/DaVinci_' + j.application.version + '/Phys/DaVinci/options/'
j.application.optsfile = [ File ( appOpts+'DaVinci.py' ), File(appOpts+'DC06_stripped_bbincl_lumi2.py') ]
# Extra options
# Appended to the end of the main options to override default settings
#j.application.extraopts = ''
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Input data
# Can either be specified in the main configuration files (DaVinci.py etc.) or
# locally here as an LHCbDataset
#------------------------------------------------------------------------------
#j.inputdata = LHCbDataset( files = [
#'LFN:/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000004_3.dst' ,
#'LFN:/lhcb/production/DC06/v1r0/00002000/DST/0000/00002000_00000005_3.dst' ] )
#------------------------------------------------------------------------------
# Define a job splitter (very useful for many input data files)
#------------------------------------------------------------------------------
# Use a single job, no splitting
#j.splitter = None

# Split jobs into 1 input file per job. max of 5 files in total
j.splitter = SplitByFiles ( filesPerJob = 5, maxFiles = 25 )

# This Splitter will query the Logical File Catalog (LFC) to find
# at which sites a particular file is stored. Subjobs will be created
# so that all the data required for each subjob is stored in
# at least one common location. This prevents the submission of jobs that
# are unrunnable due to data availability.
# Useful when using the Dirac backend (see below)
#j.splitter = DiracSplitter ( filesPerJob = 1, maxFiles = 5 )
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Job merging (merge output from sub-jobs from job splitter, if used)
#------------------------------------------------------------------------------
# No merging
#j.merger = None

# Root file merging. See help( RootMerger ) for more details
#j.merger = RootMerger( files = ['DVNtuples.root','DVHistos.root'], ignorefailed = True )

# SmartMerger - Able to handle various file formats (including root files)
#               See help( SmartMerger ) for more details
j.merger = SmartMerger( files = ['DVNtuples.root','DVHistos.root'], ignorefailed = True )
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
# Output data
#j.outputdata = ["stdout"]
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

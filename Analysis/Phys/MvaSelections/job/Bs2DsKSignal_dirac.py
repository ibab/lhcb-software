#===============================================================================
# Example ganga job submission file
#===============================================================================
#
# Set ganga environment first with (e.g. lxplus)
#  > GangaEnv
# Then choose your ganga version (default version is normally the best option)
#
# Submit with
#  > ganga DaVinci_Ganga.py
#
#===============================================================================
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
#===============================================================================

#-------------------------------------------------------------------------------
# Define the version of DaVinci to use
#-------------------------------------------------------------------------------
DaVinciVersion = 'v25r2p3'
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Define name for identifying the job (this name appears in the ganga job list)
#-------------------------------------------------------------------------------
myJobName = 'MyDaVinci'
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# The application to run
#-------------------------------------------------------------------------------
myApplication = DaVinci()
# Application version number
myApplication.version = DaVinciVersion
# Define the options file to use.
# Note, if you want you can replace the PFNs in this file with LFNs directly,
# since ganga takes care of the book-keeping for you :)
myApplication.user_release_area = '/afs/cern.ch/user/s/seaso/cmtuser'
myApplication.masterpackage = 'Phys/BsDsHPreSel'
#myApplication.make()
myApplication.optsfile = File ('/afs/cern.ch/user/s/seaso/cmtuser/DaVinci_v25r2p3/Phys/MvaSelections/options/grid/Bs2DsK_grid.py')

#myApplication.optsfile = File ( myApplication.user_release_area +
#                                '/DaVinci_' + DaVinciVersion + '/Phys/DaVinci/'
#                                + DaVinciVersion + '/options/DaVinci.opts' )
# Extra options
# Appended to the end of the main options to override default settings
#myApplication.extraopts = ''
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Define a job splitter (very useful for many input data files)
# Does not work yet in released ganga versions for DC06 applications
# See https://savannah.cern.ch/bugs/index.php?20954 for details
# Planned to be fixed in Ganga 4.2.3
#-------------------------------------------------------------------------------
#mySplitter = SplitByFiles ( filesPerJob = 4 )
mySplitter = SplitByFiles ( filesPerJob = 3 )
#mySplitter = DiracSplitter ( filesPerJob = 4 )
# Use a single job, no splitting
#mySplitter = None
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Job merging does not work as yet
# See https://savannah.cern.ch/bugs/?21054 for details
#-------------------------------------------------------------------------------
myMerger = None
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
# Add here any special input files. Normally not needed.
#-------------------------------------------------------------------------------
myInputsandbox = []
#myOutputsandbox= []
myOutputsandbox= ['BsDsKSignal_MvaSelections_MC09.root' ]
#myOutputData=['']
#-------------------------------------------------------------------------------

#myBackend    = Interactive()
# Run directly on the local machine, but in the background
#myBackend    = Local()
# Submit to an LSF batch system, using the 8nm queue
myBackend    = LSF( queue = '8nh' )
# myBackend    = LSF( queue = '1nh' )
# Submit to the grid. Requires a working grid certificate of course :)
#myBackend    = Dirac()
# Submit to a condor batch system
#myBackend    = Condor()
#-------------------------------------------------------------------------------
list=load('/afs/cern.ch/user/s/seaso/cmtuser/DaVinci_v25r2p3/Phys/MvaSelections/datafiles/Bs2DsK/Bs2DsK_MC09_Signal_PFN.ganga')
dataPFN=list[0]  


#-------------------------------------------------------------------------------
# Finally, create a Ganga job object and submit
#-------------------------------------------------------------------------------
j = Job (
 name         = myJobName,
 application  = myApplication,
 splitter     = mySplitter,
 merger       = myMerger,
 inputsandbox = myInputsandbox,
 outputsandbox =myOutputsandbox,
 inputdata = dataPFN, 
 backend      = myBackend
 )
# inputdata = dataPFN,
# outputdata = myOutputData,

j.submit()
#-------------------------------------------------------------------------------

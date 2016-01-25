In this directory there is a small example of how to currently develop and run 
a user plugin for Rivet in the LHCb environment and using the LHCb Monte Carlo
tunes.

The files needed to run and process the output of a Rivet analysis plugin are 
the source code (.cc), a meta file (.info) which describes the analysis, 
references to publications, experiment, etc. and a file (.plot) which 
specifies the titles, axis labels, etc. for the plots produced when 
running the analysis (see http://rivet.hepforge.org/trac/wiki/WritingAnAnalysis). 
These files are automatically generated for you by 'rivet-mkanalysis' script.

In order to access the Rivet tools the user should first set up the 
environment (for bash shell):

$ source ../cmt/setup.sh

or 

$ SetupProject Gauss 

The .cc file contains the source code of the plugin which you must compile 
manually before running Gauss:

$ lbrivet-buildAM MC_LHCb_GENERIC.cc 

which will generate the plugin binary in form of a shared library 
('RivetAnalysis.so' by default). For additional options all the scripts 
included in the Rivet distribution have a help page accesible with '--help' 
command line option.

Finally to run the plugin create a Python option file similar to the one 
included in this directory and issue:

$ gaudirun.py MC_LHCb_GENERIC.py

This will create the 'myRivetGaussMC.yoda' file as output which can be further
processed with Rivet built-in scripts to obtain, for instance, a nice web
page presenting all the histograms:

$ rivet-mkhtml myRivetGaussMC.yoda
$ firefox ./plots/index.html &

The setenvRivet BASH script is provided to properly set the environment before 
running either an analysis plugin in GenTune itself (e.g. overriding plugin meta-
information) or using the auxiliary Rivet tools to further process the output 
histograms. See the Rivet manual for more specific information on these 
environment variables.

The file MC_Generic_Test.py is an option file used internally by the package 
automated tests in order to run the analysis given as example here. In case 
you modify it or the Rivet analysis module code do not expect the package
tests to work any more. 

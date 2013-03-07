In this directory there is a small example of how to currently develop and run 
a user plugin for Rivet in the LHCb environment and using the LHCb Monte Carlo
tunes.

The files needed to run and process the output of a Rivet analysis plugin are 
the source code (.cc), a meta file (.info) which describes the analysis, 
references to publications, experiment, etc. and a file (.plot) with 
specifications of titles, axis labels, etc. for the plots which the analysis 
will produce (see http://rivet.hepforge.org/trac/wiki/WritingAnAnalysis). 
These files are automatically generated for you by 'rivet-mkanalysis' script.

In order to access the Rivet tools the user should first set up the 
environment (for bash shell):

$ source ../cmt/setup.sh

or 

$ SetupProject Gauss 

The .cc file contains the source code of the plugin which you must compile 
manually before running Gauss:

$ rivet-buildplugin MC_LHCb_GENERIC.cc 

which will generate the plugin binary in form of a shared library 
('RivetAnalysis.so' by default). For additional options all the scripts 
included in the Rivet distribution have a help page accesible with '--help' 
command line option.

Finally to run the plugin create a Python option file similar to the one 
included in this directory and issue:

$ gaudirun.py MC_LHCb_GENERIC.py

This will create the 'myRivetGaussMC.aida' file as output which can be further
processed with Rivet built-in scripts to obtain, for instance, a nice web
page presenting all the histograms:

$ rivet-mkhtml myRivetGaussMC.aida
$ firefox ./plots/index.html &

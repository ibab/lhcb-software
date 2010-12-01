//#include "plot.h"

#include "TApplication.h"

#include "time.h"
#include "globals.h"
#include "taggingutils.h"

#include "Event.h"
#include "FlavourTag.h"
#include "NNTuner.h"
#include "NtpTag.h"
#include "PerformanceMonitor.h"
#include "EventViewer.h"

#include "TaggerMuonTool.h"
#include "TaggerElectronTool.h"
#include "TaggerKaonOppositeTool.h"
#include "TaggerKaonSameTool.h"
#include "TaggerPionSameTool.h"
#include "TaggerVertexChargeTool.h"
#include "TaggerFragmentationOppositeTool.h"
#include "TaggerDZeroTool.h"

#include "CombineTaggersProbability.h"
#include "CombineTaggersPID.h"
#include "CombineTaggersNN.h"

//be careful if changing the first line below!

/* ! \ mainpage FlavourTagging Analysis package

 \section intro_sec Introduction

  This small standalone package has been developed to study and 
mantain the LHCb Phys/FlavourTagging code released in DaVinci.
  It has all the functionalities of the official package plus the
possibility to access the Monte Carlo truth.

  You can also retune any particular aspect of tagging, including
the retuning of the neural networks.

 The main() is contained in tag.cxx which represents the 
 Phys/FlavourTagging/src/BTaggingTool.cpp algorthm with extended 
 functionalities (like e.g. accessing the MC truth).
 All taggers source code that are in src/Tagger*.cxx are in one-to-one
 correspondence with the released DaVinci code, to keep the 
 manteinance of the package as simple and transparent as possible.

More general information and tables of performance are found here:
 
\link https://twiki.cern.ch/twiki/bin/viewauth/LHCb/FlavourTagging \endlink

Latest version of source code is at:

\link http://svnweb.cern.ch/world/wsvn/lhcb/Analysis/trunk/Phys/FlavourTaggingChecker/root/#path_Analysis_trunk_Phys_FlavourTaggingChecker_root_
\endlink



 \section run_sec How to Compile and Run

 A Makefile is present in the top directory. There are various ways 
to run and make plots.

Most of the properties that are declared in Tools and Algorithms
can be accessed through the tag.opts file.
This file is parsed at instantiation time of any object and values
are assigned. You can also include extra file with additional definition 
of cuts for specific configurations (like for different strippings or 
data conditions). To include just use:

IncludeFile filename

The program will search first in the current area and if not present it
will look into $FLAVOURTAGGINGOPTS (useful if you are running on a lxplus node).
This is made to avoid unnecessary duplication of option configurations.

If a property is not present in this file the default value in the
.cxx code will be used. If you want to change its value add it to 
tag.opts. Keep in mind that all separation characters 
will be ignored. The last declaration is always taken.
At run time you will be informed that a property has changed from its default.

Compilation produces a tag.exe that is automatically run. Simply type:

\b make 

this compiles, runs and plots using default macro plot.C

or \b make \b run (if you don't want plots but just a performance table),

or \b make \b (if you already ran and want to draw plots).



 \subsection data What input data is needed?
 This package reads the information stored in ntuple(s) produced 
by the DaVinci algorithm Phys/FlavourTaggingChecker/src/BTaggingAnalysis.cpp
this can be run with the example python file  in 
Phys/FlavourTaggingChecker/options/DVBTaggingAnalysis.py.

 Please refer to the DaVinci pages for further information on this.

An example (default) ntuple is kept in the lxplus area: ~mgrabalo/public/analysis.root

You can also just specify a directory: in this case all root files 
will be parsed automatically.

The output of the tag.exe is a dump of the main tagging figures of performance,
a log file in output/logfile.txt and a root file output/tag.root which contains
all the filled histograms ready to be plotted.

The actual plotting is done by the root macro plot.C.
This macro contains blocks of plots already filled. Uncomment the 
goto statement that points to the block
of plots that you you wish to look at.

 \subsection debug Compile in debug mode: 
 For debugging with gdb you can type

 <b> make debug </b>

 This forces recompilation without optimization and enters gdb.
 You can then type "run" and if a core dump happens type "bt" to have a
backtrace of the exception. You can also set the DEBUG property 
in tag.opts to 2 or 1 to have additional dump information.
 

 \section info_sec How to get Information on Methods and Classes
 In this same html page you will find a tab (left above) where you can browse
 each class content and read a brief description of all the methods available.


 \section change_sec How to Change and/or Add new things
 To add a tool or any other class you can cut and paste the skeleton of some
 already existing tool, do not forget to update the Makefile adding the 
 target to compile and also the dependencies at the bottom of it.
 And write some documentation (update with 
 \a make \a doxygen the embedded html helper).

 \section nnet_sec How to Retune the Neural Networks
 In the tag.opts file you have the possibility to retune each tagger
neural net individually.

Change the option NNetTrain to either 
"muon", "ele", "kaon", "kaonS", "pionS", "vtx", or "comb" to 
redo the training of the neural net for all the taggers one by one,
the last being the retraining for the combination of the tagger's 
responses.

Type "make", and follow the instruction given by the program itself.


 \section boh_sec If you haven't found the solution here
 Contact Marco Musy (Marco.Musy@cern.ch) or Marc Grabalosa (marc.grabalosa@cern.ch).

*/


///////////////////////////////////////////////////////////////////
//some more globals:
TString NNetTrain, MLPtype, filevec_name;
int     Nmax, requireL0, requireHlt1, requireHlt2, requireTis;
bool    IsControlChannel, checkDV, removeFiles, UseModBtime;
double  m_IPPU_cut, m_thetaMin, m_distphi_cut;
std::vector<TString> filelist(0);
clock_t timestart(0);
double  timesum(0);

void taginit() {

  declareProperty( "DBGLEVEL",  DBGLEVEL = 3 );

  declareProperty( "IsControlChannel",  IsControlChannel = 0 );
  declareProperty( "EventListFileName", filevec_name     = "none" );

  declareProperty( "RemoveCorruptedFiles", removeFiles  = 1 );
  declareProperty( "NNetTrain",   NNetTrain   = "none" );
  declareProperty( "MLPtype",     MLPtype     = "MultiLayerPerceptron" );
  declareProperty( "requireL0",   requireL0   = 1 );
  declareProperty( "requireHlt1", requireHlt1 = 0 );
  declareProperty( "requireHlt2", requireHlt2 = 0 );
  declareProperty( "requireTis",  requireTis  = 0 );

  declareProperty( "Nmax",    Nmax    = -1 ); 
  declareProperty( "checkDV", checkDV =  0 );

  declareProperty( "IPPU_cut",     m_IPPU_cut    = 3.0 );
  declareProperty( "thetaMin_cut", m_thetaMin    = 0.012 );
  declareProperty( "distphi_cut",  m_distphi_cut = 0.005 );

  declareProperty( "UseModBtime",  UseModBtime = 0 );


  //INPUT DATA >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  TString      froot = readString("datafile");
  const string astr  = (const string)froot.Data();
  filelist = getFiles(astr);
  if(filelist.size()==0) filelist.push_back(froot);
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

  if(Nmax==-1) Nmax=10000000;

  if(checkDV) cout<<ROJO<<flush
		<<"Comparing results of DaVinci src/FlavourTagging!\n"<<ENDC;
  cout<<VERDE<<"Using "<<MLPtype
      <<", Triggers required: "<<ENDC<<SUBVERDE
      <<(requireL0?" L0 ":"")
      <<(requireHlt1?" Hlt1 ":"")
      <<(requireHlt2?" Hlt2 ":"")
      <<( (!requireL0 && !requireHlt1 && !requireHlt2)?" none ":"")
      <<( (requireL0&&requireTis) ? " Tis ":"")
      <<ENDC<<endl;

  timestart= clock();

}

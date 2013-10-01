#ifndef MONITOR_FITTER_HH
#define MONITOR_FITTER_HH

// C++ INCLUDES
#include <string>
using std::string;
#include <vector>
using std::vector;

// ROOFIT INCLUDES
#include <RooAbsPdf.h>
#include <RooArgList.h>
#include <RooCategory.h>
#include <RooCmdArg.h>
#include <RooDataHist.h>
#include <RooLinkedList.h>
#include <RooRealVar.h>
using namespace RooFit;

struct binValsAndErrs 
{
  double* vals;
  double* errs;
};

class SplitFitter : public TNamed {
public :

  // UNSPLIT CONSTRUCTOR
  SplitFitter(RooRealVar *mass, // quantity whose PDF is being fit
              RooAbsPdf *model, // model PDF for quantity
              RooCmdArg *range,
              RooArgList *vars, // vars to save values and errors of
              string directory = "tmp");
  
  // SPLIT CONSTRUCTOR
  SplitFitter(RooRealVar *mass, // quantity whose PDF is being fit
              RooAbsPdf *model, // model PDF for quantity
              RooCmdArg *range,
              RooArgList *vars, // vars to save values and errors of
              RooCategory *SplitCat, // category to be split over during fit (i.e. phid, momdiff, mom, eta)
              string directory = "tmp");
  
  // OVERRIDABLE DESTRUCTOR
  virtual ~SplitFitter(){}

  // FIT TYPE
  bool split;

  // ROOFIT VARIABLES
  RooRealVar* _mass;
  RooCategory* _SplitCat;
  RooAbsPdf* _model;
  RooArgList* _vars;
  RooCmdArg* _range;
  RooLinkedList fitpath;

  // STORED ORIGINAL PARAMETERS
  vector<double> originalParams;

  // OUTPUT DIRECTORY
  string outputDirectoryName;

  // CLASS METHODS

  // Initialization method, for stuff common to all constructors
  void initialize();

  // Smart fit function; chooses _fitAll or _fitSplit based on value of boolean "split"
  vector<binValsAndErrs> fit(RooDataHist*,
                             const char* fname,
                             const bool justPlot = false);
  
private:

  // Reset parameters
  void resetParams();

  // _fitAll ...
  // This method fits the combined data
  vector<binValsAndErrs> _fitAll(RooDataHist*, // Data total histogram data
                                 const char* fname, // name to give to output file
                                 const bool justPlot = false); // if true, don't fit; just plot
  
  // _fitSplit ...
  /* This does the same thing as _fitAll, but makes separate fits for each bin of the category
     Especially since the method assumes what the names of components of simPdf are
     It saves fits for each bin and the overall fit
  */
  vector<binValsAndErrs> _fitSplit(RooDataHist*, // Total histogram data
                                   const char *fname = 0, // name to give to file, if "writePlot" is true, and also workspace output
                                   const bool justPlot = false); // if true, don't fit; just plot
protected:
  
  //  ClassDef(SplitFitter,0);  
  
};

#endif

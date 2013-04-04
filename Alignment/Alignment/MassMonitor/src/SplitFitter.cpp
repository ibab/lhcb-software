// PROJECT INCLUDES
#include "MassMonitor/SplitFitter.hh"

// C++ INCLUDES
//#include <fstream> // perhaps I'll fix to use this

// ROOT INCLUDES
#include <TFile.h> 

// ROOFIT INCLUDES
#include <RooPlot.h>
#include <RooFitResult.h>
#include <RooArgSet.h> 
#include <RooMsgService.h>
#include <RooWorkspace.h>

// SETUP ENVIRONMENT
// ClassImp(SplitFitter) // ROOT macro to add class to shared environment

namespace {

  const string FitExt = ".fitresult";
  const string RootExt = ".root";

  // getPullHist makes a RooPlot of the pull of another RooPlot
  RooPlot* getPullHist(RooPlot* plot, // plot to make pull of
                       const char* dataName, // name of data
                       const char* pdfName, // name of pdf
                       const char* name) { // optional name of returned plot, otherwise pdfName_resid
    
    RooAbsRealLValue* variable = plot->getPlotVar();
    RooHist* graph = plot->pullHist(dataName,pdfName);
    RooPlot *pulls = variable->frame(variable->getBins()); 
    pulls->addObject((TObject*) graph,"XP"); // the "XP" makes these be points, without a connecting line
    pulls->SetYTitle("Normalized Residuals"); // i.e. pulls
    string resname;
    if (name)
      resname = name;
    else
      resname = string(pdfName) + "_resid";
    pulls->SetName(resname.c_str());
    pulls->SetMinimum(-5);
    pulls->SetMaximum(+5); 
    
    // RETURN
    return pulls;
  }
}

  
// UNSPLIT CONSTRUCTOR
SplitFitter::SplitFitter(RooRealVar *mass, //
                         RooAbsPdf *model, //
                         RooArgList *vars,
                         string directory)
  : _mass(mass),
    _model(model),
    _vars(vars),
    outputDirectoryName(directory)
{

  // Set split/unsplit bool
  split = false;  

  // Initialize
  initialize();
}

// SPLIT CONSTRUCTOR
SplitFitter::SplitFitter(RooRealVar *mass, //
                         RooAbsPdf *model, //
                         RooArgList *vars,
                         RooCategory *SplitCat, //
                         string directory)
  : _mass(mass),
    _model(model),
    _vars(vars),
    _SplitCat(SplitCat),
    outputDirectoryName(directory)
{

  // Set split/unsplit bool
  split = true;

  // Initialize
  initialize();
}

void SplitFitter::initialize() 
{
  // ROOMSGSERVICE SETTINGS
  RooMsgService::instance().setStreamStatus(1,false);
  RooMsgService::instance().setStreamStatus(0,false);

  // VERBOSITY
  this->fitpath.Add(new RooCmdArg(PrintLevel(1))); // MINUIT print level
  this->fitpath.Add(new RooCmdArg(Verbose(false))); // no verbose output
  this->fitpath.Add(new RooCmdArg(Warnings(false))); // no warnings

  // FIT PATH
  this->fitpath.Add(new RooCmdArg(Hesse(true))); // HESSE run after MIGRAD
  this->fitpath.Add(new RooCmdArg(SumW2Error(true)));
  this->fitpath.Add(new RooCmdArg(Strategy(2))); // sets MINUIT strategy to 2
  this->fitpath.Add(new RooCmdArg(Minos(true))); // MINOS run after HESSE
  this->fitpath.Add(new RooCmdArg(InitialHesse(false))); // HESSE run before MIGRAD

  // RETURN ROOFITRESULT
  this->fitpath.Add(new RooCmdArg(Save(true)));

  // SET RANGE
  //  range = new RooCmdArg(Range(
  //  this->fitpath.Add(new RooCmdArg(Range("all")));
}

// fit either does _fitAll or _fitSplit, depending on whether split category has been provided
vector<binValsAndErrs> SplitFitter::fit(RooDataHist *hist, const char* fname, const bool justPlot)
{
  if (split)
    return _fitSplit(hist,fname,justPlot);
  else
    return _fitAll(hist,fname,justPlot);    
}

// fitAll fits the combined data
vector<binValsAndErrs> SplitFitter::_fitAll(RooDataHist *hist,
                                            const char* fname,
                                            const bool justPlot) 
{
  
  // CREATE ROO WORKSPACE
  RooWorkspace FitWS("fitAll");

  // CREATE RETURN VECTOR
  int num = _vars->getSize();
  vector<binValsAndErrs> data;
  for (int i = 0; i < num; i++) {
    double* values = new double[0];
    double* errors = new double[0];
    binValsAndErrs bve;
    bve.vals = values;
    bve.errs = errors;
    data.push_back(bve);
  }

  // FIT TOTALPDF TO TOTAL #1
  RooFitResult *fitresult = 0;
  if (not justPlot) {
    
    // FIT _MODEL TO DATA
    fitresult = _model->fitTo(*hist,fitpath);
    fitresult->Print("v");
    FitWS.import(*fitresult);

    // WRITE FITRESULT TO FILE
    RooArgSet* allVars = _model->getVariables();
    string fit_filename = outputDirectoryName + "/" + fname + FitExt;
    allVars->writeToFile(fit_filename.c_str());
  }

  // SAVE VALS AND ERRS
  for (int i = 0; i < num; i++) {
    RooRealVar* var = (RooRealVar*)_vars->at(i);
    data[i].vals[0] = var->getValV();
    data[i].errs[0] = var->getError();
  }

  // CREATE OUTPUT FILE
  string root_filename = outputDirectoryName + "/" + fname + RootExt;
  TFile *fout = new TFile(root_filename.c_str(),"recreate");
  assert(fout);
  
  // DEFINE PLOT
  RooPlot* frame = _mass->frame(_mass->getBins());
  frame->SetName("all");
  
  // PLOT DATA
  hist->plotOn(frame,
               Range("all"),
               DataError(RooAbsData::Poisson),
               RooFit::Name("hist_all"));
  
  // PLOT FIT
  _model->plotOn(frame,
                 RooFit::Range("all"),
                 RooFit::Name("fit_all"));
  _model->plotOn(frame,
                 RooFit::Range("all"),
                 Components("signal"),
                 LineColor(kRed),
                 RooFit::Name("signal_all"));
  _model->plotOn(frame,
                 RooFit::Range("all"),
                 Components("bkg"),
                 LineColor(kMagenta),
                 RooFit::Name("bkg_all"));
  
  // PLOT RESIDUAL
  RooPlot *pulls = getPullHist(frame,"hist_all","curve_all","pulls_all");
  
  //   // MAKE CANVAS
  //   TCanvas canvas("Fit","Fit");
  //   canvas.cd();
  //   TPad* HistogramPad = new TPad("HistogramPad","HistogramPad",0.0,0.5,1.0,1.0);
  //   HistogramPad->Draw();
  //   TPad* ResidualPad = new TPad("ResidualPad","ResidualPad",0.0,0.0,1.0,0.5);
  //   ResidualPad->Draw();
  //   ResidualPad->SetTopMargin(0.1);
  //   HistogramPad->cd();
  //   frame->Draw();
  //   ResidualPad->cd();
  //   pulls->Draw();
  
  // SAVE TO FILE
  fout->cd();
  //  canvas.Write();
  frame->Write();
  pulls->Write();
  //   FitWS.import(*frame);
  //   FitWS.import(*pulls);
  //   FitWS.Write();
  
  // CLOSE FILE
  fout->Close();

  // RETURN VALUE AND ERRORx
  return data;
}

// fitSplit ...
// This does the same thing as _fitAll, but makes separate fits for each bin of splitting variable
vector<binValsAndErrs> SplitFitter::_fitSplit(RooDataHist *hist, // Data total histogram data
                                              const char* fname, // name to give to file
                                              const bool justPlot) { // if true, don't fit; just plot
  
  RooWorkspace SplitWS("fitSplit");

  // CREATE OUTPUT FILE
  TFile* fout;
  if (fname) {
    string filename = outputDirectoryName + "/" + fname + RootExt;
    fout = new TFile(filename.c_str(),"recreate");
    assert(fout);
  }
  
  // GET CATEGORY INFO
  int nTypes = _SplitCat->numTypes();
  string catName = _SplitCat->GetName();

  // CREATE ARRAYS OF FIT PARAMETERS TO BE MONITORED
  int num = _vars->getSize();
  vector<binValsAndErrs> data;
  for (int i = 0; i < num; i++) {
    double* values = new double[nTypes];
    double* errors = new double[nTypes];
    binValsAndErrs bve;
    bve.vals = values;
    bve.errs = errors;
    data.push_back(bve);
  }
  
  // LOOP OVER CATEGORY TAG
  for (int iType = 0; iType < nTypes; iType++) {
    // SET CATEGORY TAG
    _SplitCat->setIndex(iType);      
    
    // GET DATA SUBSET
    string tagcut = catName + "==" + catName + "::" + _SplitCat->getLabel();
    RooDataHist* subhist = (RooDataHist*)hist->reduce(RooFit::Cut(tagcut.c_str()))->reduce(RooArgSet(*_mass));
    subhist->Print("v");

    // FIT TO DATA SUBSET
    RooAbsPdf* fitter = (RooAbsPdf*)_model->Clone();
    RooFitResult* fitresult;
    if (not justPlot) {
      RooFitResult* fitresult = fitter->fitTo(*subhist,fitpath);
      fitresult->Print("v");
      SplitWS.import(*fitresult);
    }

    // WRITE FITRESULT TO FILE
    if (fname and (not justPlot)) {
      RooArgSet* allVars = fitter->getVariables();
      string fit_filename = outputDirectoryName + "/" + fname + FitExt;
      allVars->writeToFile(fit_filename.c_str());
    }
      
    // SAVE VALS AND ERRS
    for (int i = 0; i < num; i++) {
      RooRealVar* var = (RooRealVar*)_vars->at(i);
      data[i].vals[iType] = var->getValV();
      data[i].errs[iType] = var->getError();
    }

    if (fname) {
      
      // DEFINE PLOT
      RooPlot* frame = _mass->frame(_mass->getBins());
      string name = string("bin_") + _SplitCat->getLabel();
      string title = string(frame->GetTitle()) + " - " + name;
      frame->SetName(name.c_str());
      frame->SetTitle(title.c_str());
      
      // PLOT DATA
      string histName = string("hist_") + _SplitCat->getLabel();
      subhist->plotOn(frame,
                      RooFit::Range("all"),
                      DataError(RooAbsData::Poisson),
                      RooFit::Name(histName.c_str()));
      
      // PLOT FIT COMPONENTS
      string fitName = string("fit_") + _SplitCat->getLabel();
      fitter->plotOn(frame,
                     RooFit::Range("all"),
                     RooFit::Name(fitName.c_str()));
      string signalName = string("signal_") + _SplitCat->getLabel();
      fitter->plotOn(frame,
                     RooFit::Range("all"),
                     Components("signal"),
                     LineColor(kRed),
                     RooFit::Name(signalName.c_str()));
      string bkgName = string("bkg_") + _SplitCat->getLabel();
      fitter->plotOn(frame,
                     RooFit::Range("all"),
                     Components("bkg"),
                     LineColor(kMagenta),
                     RooFit::Name(bkgName.c_str()));
      
      // PLOT RESIDUAL
      string pulls_title = string("pulls_") + _SplitCat->getLabel();
      RooPlot* pulls = getPullHist(frame,histName.c_str(),fitName.c_str(),pulls_title.c_str());
      
      // SAVE TO FILE
      fout->cd();
      frame->Write();
      pulls->Write();
      //      canvas.Write();
    }
  }

  if (fname) {
    
    // CLOSE FILE
    SplitWS.Write();
    fout->Close();
  }
  
  // RETURN FIT RESULT
  return data;
}

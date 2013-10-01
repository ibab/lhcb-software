// PROJECT INCLUDES
#include "MassMonitor/SplitFitter.hh"

// C++ INCLUDES
#include <iostream>
#include <fstream>

// ROOT INCLUDES
#include <TFile.h> 
#include <TCanvas.h>
#include <TPad.h>

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
    pulls->SetTitle(resname.c_str());
    pulls->SetMinimum(-5);
    pulls->SetMaximum(+5); 
    
    // RETURN
    return pulls;
  }
}

  
// UNSPLIT CONSTRUCTOR
SplitFitter::SplitFitter(RooRealVar *mass, //
                         RooAbsPdf *model, //
                         RooCmdArg* range,
                         RooArgList *vars,
                         string directory)
  : _mass(mass),
    _model(model),
    _range(range),
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
                         RooCmdArg *range,
                         RooArgList *vars,
                         RooCategory *SplitCat, //
                         string directory)
  : _mass(mass),
    _model(model),
    _range(range),
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
  if (_range != 0)
    this->fitpath.Add(_range);

  // SAVE ORIGINAL PARAMETERS SO THEY CAN BE RESET
  RooArgSet varset(*_mass);
  RooArgSet* params = _model->getParameters(&varset);
  TIterator* iter = params->createIterator();
  RooRealVar *var = (RooRealVar*)iter->Next();
  
  while (var != NULL) {
    std::cout << "Default value: " << var->getValV() << std::endl;
    originalParams.push_back(var->getValV());
    var = (RooRealVar*)iter->Next();
  }
  
}

// RESET PARAMETERS
void SplitFitter::resetParams() 
{
  RooArgSet varset(*_mass);
  RooArgSet* params = _model->getParameters(&varset);
  TIterator* iter = params->createIterator();
  int i = 0;
  RooRealVar *var = (RooRealVar*)iter->Next();
  var = (RooRealVar*)iter->Next();
  while (var != 0) {
    var->setVal(originalParams[i]);
    var = (RooRealVar*)iter->Next();
    i++;
  }  
}


// fit either does _fitAll or _fitSplit, depending on whether split category has been provided
vector<binValsAndErrs> SplitFitter::fit(RooDataHist *hist, const char* fname, const bool justPlot)
{
  if (fname == 0)
    std::cerr << "ERROR: Must supply base file name for output" << std::endl;
  
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

  assert(fname != 0);

  // CREATE ARRAYS OF FIT PARAMETERS TO BE MONITORED
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

  // FIT TO DATA SET
  RooFitResult *fitresult = 0;
  string fit_filename = outputDirectoryName + "/" + fname + FitExt;
  std::ofstream fit_stream(fit_filename.c_str(), std::ofstream::out);
  
  if (not justPlot) {
    fitresult = _model->fitTo(*hist,fitpath);
    fitresult->Print("v");
    _model->getVariables()->writeToStream(fit_stream,false);
  }
  fit_stream.close();

  // SAVE VALS AND ERRS
  for (int i = 0; i < num; i++) {
    RooRealVar* var = (RooRealVar*)_vars->at(i);
    data[i].vals[0] = var->getValV();
    data[i].errs[0] = var->getError();
  }

  // DEFINE PLOT
  RooPlot* frame = _mass->frame(_mass->getBins());
  frame->SetName("all");
  
  // PLOT DATA
  hist->plotOn(frame,
               *_range,
               DataError(RooAbsData::Poisson),
               RooFit::Name("hist_all"));
  
  // PLOT FIT COMPONENTS
  _model->plotOn(frame,
                 *_range,
                 RooFit::Name("fit_all"));
  _model->plotOn(frame,
                 *_range,
                 Components("signal*"),
                 LineColor(kRed),
                 RooFit::Name("signal_all"));
  _model->plotOn(frame,
                 *_range,
                 Components("bkg*"),
                 LineColor(kMagenta),
                 RooFit::Name("bkg_all"));
  
  // PLOT RESIDUAL
  RooPlot *pulls = getPullHist(frame,"hist_all","fit_all","pulls_all");
  
  // MAKE CANVAS
  TCanvas canvas("Fit","Fit");
  canvas.cd();
  TPad* HistogramPad = new TPad("HistogramPad","HistogramPad",0.0,0.5,1.0,1.0);
  HistogramPad->Draw();
  TPad* ResidualPad = new TPad("ResidualPad","ResidualPad",0.0,0.0,1.0,0.5);
  ResidualPad->Draw();
  ResidualPad->SetTopMargin(0.1);
  HistogramPad->cd();
  frame->Draw();
  HistogramPad->Update();
  ResidualPad->cd();
  pulls->Draw();
  ResidualPad->Update();
  canvas.Update();
  
  // SAVE TO ROOT FILE
  string root_filename = outputDirectoryName + "/" + fname + RootExt;
  TFile *fout = new TFile(root_filename.c_str(),"recreate");
  assert(fout);
  fout->cd();
  canvas.Write();
  //  frame->Write();
  //  pulls->Write();
  fout->Close();

  // RETURN VALUE AND ERRORx
  return data;
}

// fitSplit ...
// This does the same thing as _fitAll, but makes separate fits for each bin of splitting variable
vector<binValsAndErrs> SplitFitter::_fitSplit(RooDataHist *hist, // Data total histogram data
                                              const char* fname, // name to give to file
                                              const bool justPlot) { // if true, don't fit; just plot
  
  assert(fname != 0);

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

  // FIT RESULTS FILE
  string fit_filename = outputDirectoryName + "/" + fname + FitExt;
  std::ofstream fit_stream(fit_filename.c_str(), std::ofstream::out);

  // ROOTFILE
  string root_filename = outputDirectoryName + "/" + fname + RootExt;
  TFile* fout = new TFile(root_filename.c_str(),"recreate");
  assert(fout);

  // LOOP OVER CATEGORY TAG
  for (int iType = 0; iType < nTypes; iType++) {
    _SplitCat->setIndex(iType);      
    
    // GET DATA SUBSET
    string tagcut = catName + "==" + catName + "::" + _SplitCat->getLabel();
    RooDataHist* subhist = (RooDataHist*)hist->reduce(RooFit::Cut(tagcut.c_str()))->reduce(RooArgSet(*_mass));
    subhist->Print("v");

    // FIT TO DATA SUBSET
    RooFitResult* fitresult = 0;
    RooAbsPdf* fitter = (RooAbsPdf*)_model->Clone();
    if (not justPlot) {
      if (subhist->sum(false) > 0) {
        std::cout << "Enough entries to fit:" << std::endl;
        fitresult = fitter->fitTo(*subhist,fitpath);
        fitresult->Print("v");
        fitter->getVariables()->writeToStream(fit_stream,false);
      } else {
        std::cout << "Not enough entries; skipping bin ..." << std::endl;
      }
    }

    // SAVE VALS AND ERRS
    for (int i = 0; i < num; i++) {
      RooRealVar* var = (RooRealVar*)_vars->at(i);
      data[i].vals[iType] = var->getValV();
      data[i].errs[iType] = var->getError();
    }

    // DEFINE PLOT
    RooPlot* frame = _mass->frame(_mass->getBins());
    string name = string("bin_") + _SplitCat->getLabel();
    string title = name;
    frame->SetName(name.c_str());
    frame->SetTitle(title.c_str());
    
    // PLOT DATA
    string histName = string("hist_") + _SplitCat->getLabel();
    subhist->plotOn(frame,
                    *_range,
                    DataError(RooAbsData::Poisson),
                    RooFit::Name(histName.c_str()));
    
    // PLOT FIT COMPONENTS
    string fitName = string("fit_") + _SplitCat->getLabel();
    fitter->plotOn(frame,
                   *_range,
                     RooFit::Name(fitName.c_str()));
    string signalName = string("signal_") + _SplitCat->getLabel();
    fitter->plotOn(frame,
                   *_range,
                   Components("signal*"),
                   LineColor(kRed),
                   RooFit::Name(signalName.c_str()));
    string bkgName = string("bkg_") + _SplitCat->getLabel();
    fitter->plotOn(frame,
                   *_range,
                   Components("bkg*"),
                   LineColor(kMagenta),
                   RooFit::Name(bkgName.c_str()));
    
    // PLOT RESIDUAL
    string pulls_name = string("pulls_") + _SplitCat->getLabel();
    RooPlot* pulls = getPullHist(frame,histName.c_str(),fitName.c_str(),pulls_name.c_str());
    
    // MAKE CANVAS
    string canvas_name = string("Fit_") + _SplitCat->getLabel();
    string canvas_title = canvas_name;
    TCanvas canvas(canvas_name.c_str(),canvas_title.c_str());
    canvas.cd();
    TPad* HistogramPad = new TPad("HistogramPad","HistogramPad",0.0,0.5,1.0,1.0);
    HistogramPad->Draw();
    TPad* ResidualPad = new TPad("ResidualPad","ResidualPad",0.0,0.0,1.0,0.5);
    ResidualPad->Draw();
    ResidualPad->SetTopMargin(0.1);
    HistogramPad->cd();
    frame->Draw();
    ResidualPad->cd();
    pulls->Draw();
  
    // SAVE TO ROOT FILE
    fout->cd();
    canvas.Write();
  }
  
  // CLOSE FILES
  fit_stream.close();
  fout->Close();
  
  // RETURN FIT RESULT
  return data;
}

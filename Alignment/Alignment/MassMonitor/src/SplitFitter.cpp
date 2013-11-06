// PROJECT INCLUDES
#include "MassMonitor/SplitFitter.hh"

// C++ INCLUDES
#include <iostream>
#include <fstream>

// ROOT INCLUDES
#include <TFile.h> 
#include <TCanvas.h>
#include <TPad.h>
#include <TMath.h>

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
    pulls->SetYTitle("Normalized Residuals");
    string resname;
    if (name)
      resname = name;
    else
      resname = string(pdfName) + "_resid";
    pulls->SetName(resname.c_str());
    pulls->SetTitle(resname.c_str());
    pulls->SetMinimum(-15);
    pulls->SetMaximum(+15); 
    
    // RETURN
    return pulls;
  }
}


// UNSPLIT CONSTRUCTOR
SplitFitter::SplitFitter(RooRealVar *mass, //
                         RooAbsPdf *model, //
                         RooCmdArg *range,
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
  this->fitpath.Add(new RooCmdArg(PrintLevel(-1))); // MINUIT print level
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
  // also going to add ln(P(chi^2)), calculated two ways
  vector<binValsAndErrs> data;
  for (int i = 0; i < num + 2; i++) {
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
    _model->getVariables()->writeToStream(fit_stream,false);
    fitresult = _model->fitTo(*hist,fitpath);
    fitresult->Print("v");
    _model->getVariables()->writeToStream(fit_stream,false);
  }
  fit_stream.close();
  
  // SAVE VALS AND ERRS
  for (int i = 0; i < num; i++) {
    RooAbsReal* var = (RooAbsReal*)_vars->at(i);
    data[i].vals[0] = var->getValV();
    if (fitresult != 0)
      data[i].errs[0] = var->getPropagatedError(*fitresult);
    else
      data[i].errs[0] = 0;
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
                 Components("*sig"),
                 LineColor(kRed),
                 RooFit::Name("signal_all"));
  _model->plotOn(frame,
                 *_range,
                 Components("*bkg"),
                 LineColor(kMagenta),
                 RooFit::Name("bkg_all"));

  // SAVE LOG(P(CHI2))
  frame->Draw();
  Double_t chi2 = frame->chiSquare("fit_all","hist_all");
  Double_t pchi2 = TMath::Prob(chi2,1);
  data[num].vals[0] = log(pchi2);
  data[num].errs[0] = 0;
//   Int_t num_params = _model->getParameters(hist)->selectByAttrib("Constant",kFALSE)->getSize();
//   Int_t ndof = hist->numEntries() - num_params - 1;
//   Double_t pchi2_b = TMath::Prob(chi2*ndof,ndof);
//   data[num+1].vals[0] = log(pchi2_b);
//   data[num+1].errs[0] = 0;

  
  // PLOT RESIDUAL
  RooPlot *pulls = getPullHist(frame,"hist_all","fit_all","pulls_all");
  
  // MAKE CANVAS
  TCanvas canvas("Fit","Fit");
  canvas.cd();
  TPad* HistogramPad = new TPad("HistogramPad","HistogramPad",0.0,0.25,1.0,1.0);
  HistogramPad->Draw();
  HistogramPad->SetTopMargin(0.1);
  TPad* ResidualPad = new TPad("ResidualPad","ResidualPad",0.0,0.0,1.0,0.25);
  ResidualPad->Draw();
  ResidualPad->SetTopMargin(0.1);
  HistogramPad->cd();
  frame->SetTitleFont(63,"XY");
  frame->SetTitleSize(18,"ZY");
  frame->SetTitleOffset(2,"XY");
  frame->SetLabelFont(63,"XY");
  frame->SetLabelSize(14,"XY");
  frame->Draw();
  HistogramPad->Update();
  ResidualPad->cd();
  pulls->SetTitleFont(63,"XY");
  pulls->SetTitleSize(18,"XY");
  pulls->SetTitleOffset(2,"XY");
  pulls->SetLabelFont(63,"XY");
  pulls->SetLabelSize(14,"XY");
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
  for (int i = 0; i < num + 2; i++) {
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
        //        fitresult->Print("v");
        fitter->getVariables()->writeToStream(fit_stream,false);
      } else {
        std::cout << "Not enough entries; skipping bin ..." << std::endl;
      }
    }

    // SAVE VALS AND ERRS
    for (int i = 0; i < num; i++) {
      RooAbsReal* var = (RooAbsReal*)_vars->at(i);
      data[i].vals[iType] = var->getValV();
      if (fitresult != 0)
        data[i].errs[iType] = var->getPropagatedError(*fitresult);
      else
        data[i].errs[iType] = 0;
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
                   Components("*sig"),
                   LineColor(kRed),
                   RooFit::Name(signalName.c_str()));
    string bkgName = string("bkg_") + _SplitCat->getLabel();
    fitter->plotOn(frame,
                   *_range,
                   Components("*bkg"),
                   LineColor(kMagenta),
                   RooFit::Name(bkgName.c_str()));
    
    // SAVE LOG(P(CHI2))
    Double_t chi2 = frame->chiSquare(fitName.c_str(),histName.c_str());
    Double_t pchi2 = TMath::Prob(chi2,1);
    data[num].vals[iType] = log(pchi2);
    data[num].errs[iType] = 0;
//     Int_t num_params = _model->getParameters(hist)->selectByAttrib("Constant",kFALSE)->getSize();
//     Int_t ndof = hist->numEntries() - num_params - 1;
//     Double_t pchi2_b = TMath::Prob(chi2*ndof,ndof);
//     data[num+1].vals[iType] = log(pchi2_b);
//     data[num+1].errs[iType] = 0;

    // PLOT RESIDUAL
    string pulls_name = string("pulls_") + _SplitCat->getLabel();
    RooPlot* pulls = getPullHist(frame,histName.c_str(),fitName.c_str(),pulls_name.c_str());
    
    // MAKE CANVAS
    string canvas_name = string("Fit_") + _SplitCat->getLabel();
    string canvas_title = canvas_name;
    TCanvas canvas(canvas_name.c_str(),canvas_title.c_str());
    canvas.cd();
    TPad* HistogramPad = new TPad("HistogramPad","HistogramPad",0.0,0.25,1.0,1.0);
    HistogramPad->Draw();
    HistogramPad->SetTopMargin(0.1);
    TPad* ResidualPad = new TPad("ResidualPad","ResidualPad",0.0,0.0,1.0,0.25);
    ResidualPad->Draw();
    ResidualPad->SetTopMargin(0.1);
    HistogramPad->cd();
    frame->SetTitleFont(63,"XY");
    frame->SetTitleSize(18,"XY");
    frame->SetTitleOffset(2,"XY");
    frame->SetLabelFont(63,"XY");
    frame->SetLabelSize(14,"XY");
    frame->Draw();
    HistogramPad->Update();
    ResidualPad->cd();
    pulls->SetTitleFont(63,"XY");
    pulls->SetTitleSize(18,"XY");
    pulls->SetTitleOffset(2,"XY");
    pulls->SetLabelFont(63,"XY");
    pulls->SetLabelSize(14,"XY");    
    pulls->Draw();
    ResidualPad->Update();
    canvas.Update();
  
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

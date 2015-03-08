#include "include.h"
//================= Methods

RooHist* myResidHist(const RooHist& data ,const RooCurve& curve, bool normalize, int method, RooAddPdf pdf, RooRealVar M){
  

// Create and return RooHist containing  residuals w.r.t to given curve.
  // If normalize is true, the residuals are normalized by the histogram
  // errors creating a RooHist with pull values


  // Copy all non-content properties from hist1
  RooHist* hist = new RooHist(data._nominalBinWidth) ;
  if (normalize) {
    hist->SetName(Form("pull_%s_%s",data.GetName(),curve.GetName())) ;
    hist->SetTitle(Form("Pull of %s and %s",data.GetTitle(),curve.GetTitle())) ;  
  } else {
    hist->SetName(Form("resid_%s_%s",data.GetName(),curve.GetName())) ;
    hist->SetTitle(Form("Residual of %s and %s",data.GetTitle(),curve.GetTitle())) ;  
  }

  // Determine range of curve 
  Double_t xstart,xstop,y ;
  curve.GetPoint(0,xstart,y) ;
  curve.GetPoint(curve.GetN()-1,xstop,y) ;
  
  // Add histograms, calculate Poisson confidence interval on sum value
  double m=0;
  double m2=0;
  double n=0;

  //
  double sum = 0;
  if( method == 3){
    const RooArgList& coefs = pdf.coefList();
    RooLinkedListIter it = coefs.iterator();
    RooRealVar* coef;
    while ( (coef = (RooRealVar*) it.Next())) {
      sum += coef->getVal();
    }
  }
  
  //
  for(Int_t i=0 ; i<data.GetN() ; i++) {    
    Double_t x,point;
    data.GetPoint(i,x,point) ;

    // Only calculate pull for bins inside curve range
    if (x<xstart || x>xstop) continue ;
    
    Double_t yy ;
    if ( method==1) {
      Double_t exl = 0.5*data.getNominalBinWidth();
      Double_t exh = 0.5*data.getNominalBinWidth();
      yy = point - curve.average(x-exl,x+exh) ;      
    } else if(method==2){
      yy = point - curve.interpolate(x) ;
      //std::cout << " " << i << " " << x << " " << point << " " << curve.interpolate(x) << std::endl;
    } else if(method==3){
      Double_t exl = 0.5*data.getNominalBinWidth();
      Double_t exh = 0.5*data.getNominalBinWidth();
      M.setRange("bin",x-exl,x+exh);
      RooAbsReal* integ = pdf.createIntegral(M,RooFit::NormSet(M),RooFit::Range("bin"));
      //std::cout << " Integral " << integ->getVal() << " * " << sum << std::endl;
      yy = point - integ->getVal()*sum;
    } else{
      std::cout << "unknown method " << std::endl;
      return NULL;
    }
    
   
   

    Double_t dyl = data.GetErrorYlow(i) ;
    Double_t dyh = data.GetErrorYhigh(i) ;

    //std::cout << " --> " << x << " " << point << " " << yy  << " " << dyl << " " << dyh << std::endl;


    if (normalize) {
        Double_t norm = (yy>0?dyl:dyh);
        if (norm==0.) {
          //          std::cout << "RooHist::makeResisHist(" << data.GetName() << ") WARNING: point " << i << " has zero error, setting residual to zero" << std::endl ;
          yy=0 ;
          dyh=0 ;
          dyl=0 ;
        } else {
          yy   /= norm;
          dyh /= norm;
          dyl /= norm;
        }

        m  += yy;
        m2 += yy*yy;
        n  += 1.;
    }
    hist->addBinWithError(x,yy,dyl,dyh);
  }
    if( n>0){
      double mean = m/n;
      double rms  = sqrt( m2/n - m*m/n/n);
      double emean = mean/sqrt(n);
      std::cout << " PULL <mu>= " << mean << " ± " << emean   << " - rms = " << rms  << " -   chi2 = " << m2 << std::endl;
    }
  
  return hist ;
}


//============================================================
int prepareCanvas(TCanvas* canvas, int addResiduals) {
  if ( addResiduals == 1 ) {
    // Prepare canvas
    canvas->Divide( 1, 2) ;
    TPad* padHisto = (TPad*) canvas->GetListOfPrimitives()->At( 0 );
    TPad* padResid = (TPad*) canvas->GetListOfPrimitives()->At( 1 );
    double small = 0.05;
    double r  = 0.;
    if ( padResid )
      r = .2;
    padHisto->SetPad( 0., r , 1., 1. );
    padHisto->SetBottomMargin( small );
    if ( padResid ) {
      padHisto->SetBottomMargin( 1.0 );
      padResid->SetPad( 0., 0., 1., r  );
      padResid->SetBottomMargin( 0.25  );
      padResid->SetTopMargin   ( small/*+0.02*/ );
    }
  }
  return 0 ;
}
//============================================================
void plotResiduals(RooRealVar M, RooPlot& frame, TString flag, RooAddPdf pdf){
  using namespace RooFit;
  using namespace RooStats ;
  TString sdata=flag+"Data";
  TString scurve=flag+"Curve";

  const RooCurve* curve=frame.getCurve(scurve);
  const RooHist*  data=frame.getHist(sdata);
  RooHist* residuals = myResidHist(*data,*curve,true,2,pdf, M);
  double xMin = frame.GetXaxis()->GetXmin();
  double xMax = frame.GetXaxis()->GetXmax();
  RooPlot* fpull = M.frame(Title("Pull Distribution"),Range(xMin,xMax)) ;
  fpull->addPlotable(residuals,"E3") ;
  fpull->SetMinimum    ( -5.   );
  fpull->SetMaximum    (  5.   );
  residuals->SetFillColor( 13);
  //residuals->SetMarkerColor( kBlue);
  //residuals->SetMarkerStyle( 20    );
  //residuals->SetMarkerSize ( .8    );
  TAxis* xAxis = fpull->GetXaxis();
  xAxis->SetTickLength ( 5 * xAxis->GetTickLength() );
  xAxis->SetLabelSize  ( 5 * 0.03 /*xAxis->GetLabelSize()*/  );
  xAxis->SetTitleSize  ( 5 * 0.04 /*xAxis->GetTitleSize()*/  );
  xAxis->SetLabelOffset( 5 * xAxis->GetLabelOffset() );
  xAxis->SetTitle( "" ) ;    
  TLine *midLine = new TLine( xMin,  0., xMax,  0. );
  TLine *uppLine = new TLine( xMin,  2., xMax,  2. );
  TLine *lowLine = new TLine( xMin, -2., xMax, -2. );
  uppLine->SetLineColor( kRed );
  lowLine->SetLineColor( kRed );
  TAxis* yAxis = fpull->GetYaxis();
  yAxis->SetNdivisions ( 504                          );
  yAxis->SetLabelSize  ( 5 * 0.03 /*yAxis->GetLabelSize()*/   );      
  fpull->Draw();
  uppLine->Draw("same");
  midLine->Draw("same");
  lowLine->Draw("same");

  TAxis* axis = frame.GetXaxis() ;
  frame.GetYaxis()->SetTitleSize(0.05) ;
  frame.GetYaxis()->SetLabelSize(0.04) ;
  TString myTitle = axis->GetTitle();
  axis->SetTitleOffset(0.5) ;
  axis->SetLabelSize(0) ;
  axis->SetTitleSize(0.05) ;
  axis->CenterTitle() ;
}

double rooDisplay(RooRealVar x,RooDataHist* dataH, RooPlot* frame, TCanvas* c, RooAddPdf pdf, RooFitResult* result,std::string  out="",
                  RooArgList pars=RooArgList(),                  
                  bool logY=false,
                  std::string dataError = "SumW2",
                  const RooCmdArg& range=RooCmdArg(),
                  const RooCmdArg& cmd1 = RooFit::Layout(0.15,0.5,0.9)
                  ){
  using namespace RooFit;
  TPad* padHisto = NULL;
  TPad* padResid = NULL;
  if( c->GetListOfPrimitives()->LastIndex() > 0){
    padResid = (TPad*) c->GetListOfPrimitives()->At( 1 );
    padHisto = (TPad*) c->GetListOfPrimitives()->At( 0 );
    std::cout << "Prepare for residuals" << endl;
    padHisto->cd() ;
  }

  const static int ncol=10;
  const static Color_t    cols[ncol]={kRed,kGreen,kMagenta,kCyan,kBlue,kYellow,kPink,kViolet,kTeal,kSpring};
  const static int nsty=10;
  const static Style_t    stys[nsty]={1,2,3,4,5,6,7,8,9,10};
  if( dataError == "Poisson" )dataH->plotOn(frame,Name("Data"),DataError(RooAbsData::Poisson),range);
  else {
    if(dataError != "SumW2")std::cout << "Unknown data error type - assume SumW2" << endl;
    dataH->plotOn(frame,Name("Data"),DataError(RooAbsData::SumW2),range);
  }
  
  // display total pdf 
  pdf.plotOn(frame,VisualizeError( *result ),FillColor(kBlue),range); // band
  pdf.plotOn(frame,LineColor(kBlue+2),Name("Curve"),range);   // curve
  
  
  // display pdf component(s)
  printf("Display components \n");
  const RooArgList& pdfs  = pdf.pdfList();
  const RooArgList& coefs = pdf.coefList();
  RooLinkedListIter it = pdfs.iterator();
  RooAbsPdf* component; 
  int k=0;
  const RooAbsPdf& sigPdf=(RooAbsPdf&)pdfs[0];   // Warning : assume signal PDF is the 1st entry in RooAddPdf
  const RooRealVar& coef  =(RooRealVar&)coefs[0];
  while ( (component = (RooAbsPdf *)it.Next())) {
    Color_t col = (k < ncol) ? cols[k] : cols[ncol-1];
    Style_t sty = (k < nsty) ? stys[k] : stys[nsty-1];
    pdf.plotOn(frame,Components(*component),VisualizeError( *result),FillColor(col),range);
    pdf.plotOn(frame,Components(*component),LineStyle(sty)          ,LineColor(col+2),range);
    
    k++;
  }
  if( dataError == "Poisson" ){
    RooMsgService::instance().getStream(1).removeTopic(RooFit::Plotting);
    dataH->plotOn(frame,DataError(RooAbsData::Poisson));
  }else  dataH->plotOn(frame,DataError(RooAbsData::SumW2));
  
  if( logY ){
    frame->SetMinimum(0.2);
    if(padHisto)
      padHisto->SetLogy() ;
    else 
      c->SetLogy();
  }
  else
    pdf.paramOn(frame,RooFit::Parameters(pars),cmd1);
  
  printf("Draw main frame \n");
  frame->Draw();
  c->Update();
  
  if( padResid ){
    padResid->cd() ;
    printf("Draw residuals \n");
    plotResiduals(x,*frame, "",pdf);
  }
  if( out != "" ){
    c->Print(std::string(out+".C").c_str());
    c->Print(std::string(out+".pdf").c_str());
  }
  

  // S/B value
  printf("Compute S/B \n");
  if( pars.getSize() >= 2){
    const RooRealVar& mu  =(RooRealVar&)pars[0];  // Assume mean is the 1st parameters
    const RooRealVar& sig  =(RooRealVar&)pars[1]; // Assume sigma is the 2nd parameter
    double xmin=mu.getVal()-3.*sig.getVal();
    double xmax=mu.getVal()+3.*sig.getVal();
    x.setRange("xRange",xmin,xmax);
    RooAbsReal* isig = sigPdf.createIntegral(x,NormSet(x),RooFit::Range("xRange"));
    RooAbsReal* iall = pdf.createIntegral(x,NormSet(x),RooFit::Range("xRange")) ;
    double S= isig->getVal()*coef.getVal();
    double B=iall->getVal()*pdf.expectedEvents(RooArgSet(x))-S;
    double SoB=(B!=0) ? S/B : 0;
    std::cout << " ====== S/B[mean+/-3*sigma] = S/B[" << xmin<<","<<xmax<<"]  = " << S << "/"<< B << " = " << SoB << std::endl;
  }  
  // approximate chi2
  int npar = result->floatParsFinal().getSize();
  int nbin = dataH->numEntries();
  double chi2= frame->chiSquare("Curve","Data",npar)*(nbin-npar);
  return chi2;

  return 0;
  
}

//============================================================
int sPlotter(RooDataSet* dataS, TTree* tree, RooAbsPdf* pdf, const RooArgList& varlist,std::string outFile="",int offset=0){

  TFile* file = NULL;
  if( outFile != ""){
    std::cout << " ... create new root file " <<  outFile << std::endl;
    file = new TFile(outFile.c_str(),"recreate");
  }
  

  std::cout <<  "---- produce sWeights ----" << std::endl;
  std::cout <<  "... set non-yield parameters to constant ..." << std::endl;
  RooArgSet* set = pdf->getObservables(dataS);
  RooArgSet* params = pdf->getParameters(set) ;
  RooLinkedListIter ip = params->iterator();
  RooRealVar* par; 
  while ( (par = (RooRealVar *)ip.Next())) {
    RooLinkedListIter it = varlist.iterator();
    RooRealVar* var; 
    bool ok=false;
    std::string pname = std::string(par->GetName());
    while ( (var = (RooRealVar *)it.Next())) {
      std::string vname = std::string(var->GetName());
      if( pname == vname ){
        ok=true;
        break;
      }
    } 
    if( !ok )par->setConstant();
    else std::cout <<  "Parameter '" << pname << "' kept floating (yield)" << std::endl;
  }
  std::cout <<  "... create sPlot ..." << std::endl;
  RooStats::SPlot* sPlot = new RooStats::SPlot("sPlot","sPlot",*dataS, pdf,  varlist);
  std::cout << " tree    : " << tree->GetEntries() << " entries " << std::endl;
  std::cout << " dataset : " << dataS->sumEntries() << " entries " << std::endl;
  std::cout << " offset  : " << offset << std::endl;

  if( tree->GetEntries()+offset < dataS->sumEntries() )
    std::cout << "Warning : the Tree is a subsample of the rooDataSet" << std::endl;
  else if( tree->GetEntries()+offset == dataS->sumEntries() )
    std::cout << "Info : this Tree is completing the rooDataSet" << std::endl;
  else{
    std::cout << "Error : Trees are not aligned with the rooDataSet" << std::endl;
    return 0;
  }

  std::cout << " ... create sWeights TBranches... " << std::endl;
  RooLinkedListIter it = varlist.iterator();
  RooRealVar* var; 
  while ( (var = (RooRealVar *)it.Next())) {
    std::string name = std::string(var->GetTitle()) + "Weight";
    double weight;
    TBranch* branch = tree->Branch(name.c_str(),&weight,std::string(name+"/D").c_str());
    std::stringstream varName("");
    varName << var->GetName() << "_sw";    
    std::cout << " ... fill TBranch  '" << name << "' with sWeights " << std::endl;        
    for(int i = 0 ; i < tree->GetEntries();i++){
        tree->GetEntry(i);
        const RooArgSet* row = dataS->get(i+offset);
        weight  = row->getRealValue(varName.str().c_str());
        branch->Fill();
    }
  }
  if( outFile != ""){
    std::cout << " ... store new tree " <<  outFile << std::endl;
    //TFile* file = new TFile(outFile.c_str(),"recreate");
    tree->Write();
    file->Purge();
    file->Close();    
  }
  delete sPlot; // clean
  return tree->GetEntries();
}

//=========================================================================================================================
RooAddPdf RooAddEffProd(std::string name,std::string title,RooArgList pdfs, RooArgList coefs, RooAbsPdf& prod,int mask=-1){
  RooLinkedListIter it = pdfs.iterator();
  RooArgList pdfEffs(name.c_str());
  RooAbsPdf* pdf;
  std::cout << " bulding rooAddPdf '" << name <<"'"<< std::endl;
  int k=0;
  while ( (pdf = (RooAbsPdf *)it.Next())) {
    std::stringstream pdfname("");
    std::stringstream pdftitle("");
    if( mask < 0 || ((1 << k) & mask)){
      pdfname << pdf->GetName() << "(x)" << prod.GetName();
      pdftitle << pdf->GetTitle() << "(x)" << prod.GetTitle();
      std::cout << " ... adding " << pdfname.str() <<std::endl;
      RooEffProd* effprod = new RooEffProd(pdfname.str().c_str(),pdftitle.str().c_str(),*pdf,prod);
      pdfEffs.add(*effprod) ;
    }
    else{
      pdfEffs.add(*pdf);
      std::cout << " ... keeping " << pdf->GetName() <<std::endl;
    } 
    k++;
  }
  RooAddPdf p(name.c_str(),title.c_str(),pdfEffs,coefs);  
  return p;
}




/// Example of how to fit the Beetle pulse shape to a scan over time
/// The starting point is MPV fitted versus time
/// The fit is to the Zurich function used in the burn in with some changes
///     1. The range is restricted so that the fit is always +
///      2. A parameter is added for the noise MPV size.
/// The fit parameters are: 'constant' [proportional to MPV], t0, risetime, noise MPV

//---define fitfunction---
Double_t f2(Double_t *x, Double_t *par)
{
  return par[0] * ((x[0]-par[1])*(x[0]-par[1])/(2*par[2]*par[2]) -  TMath::Power
((x[0]-par[1]),3)/(6*TMath::Power(par[2],3))) * TMath::Exp(-(x[0]-par[1])/par[2]
);
}

unsigned int maxValue(Double_t *x, int size){

  unsigned int imax = 0; double max = 0;
  for (unsigned i = 0; i < size; ++i ){
    if (x[i] > max) {
      max = x[i];
      imax =  i;
    }
  }
  return imax;
}

unsigned int minValue(Double_t *x, int size){

  unsigned int imin = 0; double min = 2000;
  for (unsigned i = 0; i < size; ++i ){
    if (x[i] < min) {
      min = x[i];
      imin=  i;
    }
  }
  return imin;
}



//---define fitfunction---
Double_t f3(Double_t *x, Double_t *par)
{

  double value = 0;

  if (x[0] > par[1]){
  value = par[0] * ((x[0]-par[1])*(x[0]-par[1])/(2*par[2]*par[2]) -  
  TMath::Power((x[0]-par[1]),3)/(6*TMath::Power(par[2],3))) * 
  TMath::Exp(-(x[0]-par[1])/par[2]);
  if (value < 0) value = 0;
 }
 return value + par[3];
}

void beetleFuncFit() {

  double times[9] = {0.0, 0.25, 0.5 , 0.75 , 
                     1.0, 1.25, 1.5, 1.75, 2. };

  for (unsigned int i = 0; i < 9; ++i){
    times[i] *= 25;
  }

  double values[9] = {10.1, 18.5, 26., 27.4,  
                      23.8, 17., 10.7, 9.6, 9.8 };


  TGraph* graph  = new TGraph(9, times , values);
  graph->SetMarkerStyle(20);
  graph->SetTitle("");
  graph->Draw("AP");
  TAxis* xachse = graph->GetXaxis(); TAxis* yachse =  graph->GetYaxis();
  // mean.plotOn(xframe);
  xachse->SetTitleFont (132);
  yachse->SetTitleFont (132);
  xachse->SetLabelFont (132);
  yachse->SetLabelFont (132); 
  xachse->SetTitle("t/ns");


  TF1* fun = new TF1("f3",f3, 0., 100, 4);
  fun->SetLineColor(2);
  //fun->SetLineStyle(2);

  unsigned int imax =  maxValue(values,9);
  unsigned int imin = minValue(values,9); 


  fun->SetParameters(values[imax], times[imax] - 20 ,20., values[imin]) ;
  graph->Fit(fun);

 
 
}








// height t0 tau


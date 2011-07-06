// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:05 GMT
Double_t eff[9][9];
Double_t eeff[9][9];

void accfit()
{

  gROOT->LoadMacro("~/macros/rootlogon.C");
  rootlogon();
  gStyle->SetOptStat(0);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);  
  
  Int_t modelacc[9][9] = 
    {
      0,   0,   1,  25,  32,  22,  33, 141,   9,
      0,   6, 242, 178,  55,  25, 100, 124,  16,
      0, 190,  27,   7,   0,  41,  36,   9,   0,
      11, 181,  29,   3,   1,  28,   4,   0,   0,
      22,  64,   1,  18,  12,   3,   0,   0,   0, 
      30,  15,  23,  25,   0,   0,   0,   0,   0,
      20,  88,  48,   2,   0,   0,   0,   0,   0,
      127, 108,   9,   0,   0,   0,   0,   0,   0,
      23,  17,   0,   0,   0,   0,   0,   0,   0
    };
  
  
  Int_t modelgen[9][9] = 
    {

         0,     0,    23,  5652, 10594, 11335, 16793, 55388, 10815, 
         0,  2180, 67035, 63069, 20151,  6130, 38166, 70945, 10914,
         5, 62580, 12291,  5871,  2770, 11044, 15737,  2957,     0,
      4022, 61356,  7137,  2061,  3525,  9691,  2228,     0,     0,
      7720, 20627,  3349,  2872,  4042,  1984,     0,     0,     0,
     10418,  6031, 10200,  7019,  1390,     0,     0,     0,     0,
     16642, 38750, 16554,  2078,     0,     0,     0,     0,     0,  
     49957, 79986,  3788,     0,     0,     0,     0,     0,     0,
      8377, 10876,     0,     0,     0,     0,     0,     0,     0
         
    };
  
  
  Int_t flatacc[9][9] = {
       
       0,  0,  0, 16, 44, 55, 63, 52, 22,
       0,  7, 51, 74, 59, 57, 62, 47,  7,
       0, 62, 85, 72, 57, 62, 47,  3,  0,
      14, 73, 75, 71, 54, 65,  8,  0,  0,
      41, 81, 63, 61, 43,  6,  0,  0,  0, 
      47, 66, 52, 51,  8,  0,  0,  0,  0,
      61, 41, 57, 12,  0,  0,  0,  0,  0,
      71, 45,  5,  0,  0,  0,  0,  0,  0,
      14,  7,  0,  0,  0,  0,  0,  0,  0
};


  Int_t flatgen[9][9] = {
        0,     0,     5,  6126, 14582, 20430, 24024, 26097, 11845,
        0,  1656, 19381, 26817, 26040, 25524, 26136, 23603,  2118,
        9, 19427, 26045, 25740, 25706, 25822, 23448,  3754,     0,
     6238, 26968, 25857, 25962, 26046, 24157,  4596,     0,     0,
    14679, 26076, 25638, 25882, 23823,  4827,     0,     0,     0,
    20332, 25655, 25630, 24039,  4814,     0,     0,     0,     0,
    23875, 26197, 23644,  4579,     0,     0,     0,     0,     0,
    25563, 24031,  3716,     0,     0,     0,     0,     0,     0,   
    11675,  2103,     0,     0,     0,     0,     0,     0,     0
};

  Int_t combacc[9][9];
  Int_t combgen[9][9];
  for (Int_t ix=0;ix<9;ix++){
    for (Int_t iy=0;iy<9;iy++){
      combacc[ix][iy] = flatacc[ix][iy]+modelacc[ix][iy];
      combgen[ix][iy] = flatgen[ix][iy]+modelgen[ix][iy];
      
    }
  }
  


  TH2F* acceptance = new TH2F("acceptance","acceptance",9, 0.4, 3.1, 9, 0.4, 3.1);
  TH2F* fitacceptance = new TH2F("fitacceptance","fitacceptance",9, 0.4, 3.1, 9, 0.4, 3.1);

  TH2F* genmoddiff = new TH2F("genmoddiff","genmoddiff",9, 0.4, 3.1, 9, 0.4, 3.1);
  
  for (Int_t ix = 0; ix< acceptance->GetNbinsX(); ix++){
    for (Int_t iy = 0; iy<acceptance->GetNbinsY(); iy++){
      eff[ix][iy] = 0.0;
      eeff[ix][iy] = 1.0;
      Double_t sig_diff = 0.0;
      
      if (flatacc[ix][iy] != 0){
        eff[ix][iy] = Double_t(flatacc[ix][iy])/Double_t(flatgen[ix][iy]);
        eeff[ix][iy] = sqrt(eff[ix][iy]*(1-eff[ix][iy])/Double_t(flatgen[ix][iy]));
        if (flatacc[ix][iy]==0) eeff[ix][iy] = 1.5/Double_t(flatgen[ix][iy]);

        Double_t meff = Double_t(modelacc[ix][iy])/Double_t(modelgen[ix][iy]); 
        Double_t meeff = sqrt(meff*(1-meff)/Double_t(modelgen[ix][iy]));
        if (modelacc[ix][iy]==0) meeff = 1.5/Double_t(modelgen[ix][iy]);

        sig_diff = (meff-eff[ix][iy])/sqrt(meeff*meeff+eeff[ix][iy]*eeff[ix][iy]);
        

      }
      acceptance->SetBinContent(ix+1,iy+1,eff[ix][iy]);
      acceptance->SetBinError(ix+1,iy+1,eeff[ix][iy]);
      genmoddiff->SetBinContent(ix+1,iy+1,sig_diff);
      
    }
  }

  TCanvas* c1 = new TCanvas("c1","c1",600,300);
  
  c1->Divide(2,1);
  
  c1->cd(1);
  

  TText *label = new TText(2.5,2.5,"a)");
  label->SetTextSize(0.075);
  

  acceptance->GetYaxis()->SetTitleOffset(1.5);
  
  acceptance->SetXTitle("m^{2}_{+} (GeV^{2})");
  acceptance->SetTitle("");
  acceptance->SetYTitle("m^{2}_{-} (GeV^{2})");

  acceptance->SetMaximum(0.00264);
  acceptance->SetMinimum(0.00202);
   
  acceptance->Draw("box");
  label->Draw();
  

  TMinuit *gMinuit = new TMinuit(5);  //initialize TMinuit with a maximum of 5 params
  gMinuit->SetFCN(fcn);

  Double_t arglist[10];
  Int_t ierflg = 0;

  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);

// Set starting values and step sizes for parameters
  Double_t vstart[4] = {0.001,-10.0,0.01, 0.0};
  Double_t step[4] = {0.0001 , 0.1 , 0.00 , 0.00};
   gMinuit->mnparm(0, "a1", vstart[0], step[0], 0,0,ierflg);
   gMinuit->mnparm(1, "a2", vstart[1], step[1], 0,0,ierflg);
   gMinuit->mnparm(2, "a3", vstart[2], step[2], 0,0,ierflg);
   gMinuit->mnparm(3, "a4", vstart[3], step[3], 0,0,ierflg);

// Now ready for minimization step
   arglist[0] = 500;
   arglist[1] = 1.;
   gMinuit->mnexcm("MIGRAD", arglist ,2,ierflg);

// Print results
   Double_t amin,edm,errdef;
   Int_t nvpar,nparx,icstat;
   gMinuit->mnstat(amin,edm,errdef,nvpar,nparx,icstat);
   gMinuit->mnprin(3,amin);
   c1->cd(2);
   Double_t chisq = 0.0;
   Int_t ndof = -2;

   Double_t dummy;
   
   Double_t fitpar[4];
   gMinuit->GetParameter(0,fitpar[0],dummy);
   gMinuit->GetParameter(1,fitpar[1],dummy);
   gMinuit->GetParameter(2,fitpar[2],dummy);
   gMinuit->GetParameter(3,fitpar[3],dummy);

   for (Int_t ix = 0; ix< fitacceptance->GetNbinsX(); ix++){
    for (Int_t iy = 0; iy<fitacceptance->GetNbinsY(); iy++){
      Double_t x_mid = 0.4+0.15*Double_t(ix+1);
      
      Double_t y_mid = 0.4+0.15*Double_t(iy+1);
      
      Double_t feff = 0.0;
      if (flatacc[ix][iy] != 0){
        feff = func(x_mid,y_mid,fitpar);
        
        Double_t effm = acceptance->GetBinContent(ix+1,iy+1);
        
        Double_t eeffm = acceptance->GetBinError(ix+1,iy+1);
        ndof++;
        cout << pow((feff-effm)/eeffm,2)<< " " << ix+1 << " " << iy+1 << endl;
        
        chisq += pow((feff-effm)/eeffm,2);


      }
      fitacceptance->SetBinContent(ix+1,iy+1,feff);
    }
    
   }
   fitacceptance->GetYaxis()->SetTitleOffset(1.5);

   fitacceptance->SetMaximum(0.00264);
   fitacceptance->SetMinimum(0.00202);
   
   
  fitacceptance->SetXTitle("m^{2}_{+} (GeV^{2})");
  fitacceptance->SetYTitle("m^{2}_{-} (GeV^{2})");
  fitacceptance->SetTitle("");
  
  acceptance->Draw("box");
   
  fitacceptance->Draw("box");
  label->DrawText(2.5,2.5,"b)");
  
  
  
  cout << "chi*2 = " << chisq << "/" << ndof << endl;
  cout << TMath::Prob(chisq,ndof) << endl;

  return;
}


void fcn(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
   const Int_t nbins = 9;
   Int_t i,j;

//calculate chisquare
   Double_t chisq = 0;
   Double_t delta;
   for (i=0;i<nbins; i++) {
     for (j=0;j<nbins; j++) {
      Double_t x_mid = 0.4+0.15*Double_t(i+1);
      
      Double_t y_mid = 0.4+0.15*Double_t(i+1);

      
      
      delta  = (eff[i][j]-func(x_mid,y_mid,par))/eeff[i][j];
 

      
     
      if (eff[i][j] < 0.000001) delta = 0.0;
      
      
      chisq += delta*delta;
     }
   }
   
       
   f = chisq;
}

//______________________________________________________________________________
Double_t func(float x,float y,Double_t *par)
{
  Double_t value=par[0]*(1.0+par[1]*(x+y));
 
 return value;
}


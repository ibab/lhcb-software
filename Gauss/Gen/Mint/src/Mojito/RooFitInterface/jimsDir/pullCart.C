// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:18:09 GMT
  typedef struct
  {
    Float_t xp,exp,yp,eyp,rhop,xm,exm,ym,eym,rhom;
  } toyresult;

void pullCart()
{
  gROOT->Reset();
  
  gROOT->LoadMacro("~/macros/rootlogon.C");
  rootlogon();
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadLeftMargin(0.15);
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadBottomMargin(0.15);


  TChain alltoys("alltoys");
  

  char treename[128];

  

  for (int iexp=1001;iexp<=1030;iexp++){
    if (iexp !=1026 && iexp !=1014){
      
      sprintf(treename,"cartesianwcs_%d/KpipiResults.root/results",iexp);
      alltoys.Add(treename);
    }
    
  }


  TH1F* hxp = new TH1F("hxp","",20,-0.19,0.01);
  hxp->SetFillColor(5);
  hxp->SetXTitle("x_{+}");
  hxp->SetYTitle("Entries/0.005");
  hxp->SetTitleSize(0.06,"X");
  hxp->SetTitleSize(0.06,"Y");

  TH1F* hyp = new TH1F("hyp","",20,-0.11,0.09);
  hyp->SetFillColor(5);
  hyp->SetXTitle("y_{+}");
  hyp->SetYTitle("Entries/0.005");
  hyp->SetTitleSize(0.06,"X");
  hyp->SetTitleSize(0.06,"Y");

  TH1F* hxm = new TH1F("hxm","",20,-0.06,0.14);
  hxm->SetFillColor(5);
  hxm->SetXTitle("x_{-}");
  hxm->SetYTitle("Entries/0.005");
  hxm->SetTitleSize(0.06,"X");
  hxm->SetTitleSize(0.06,"Y");

  TH1F* hym = new TH1F("hym","",20,-0.01,0.19);
  hym->SetFillColor(5);
  hym->SetXTitle("y_{-}");
  hym->SetYTitle("Entries/0.005");
  hym->SetTitleSize(0.06,"X");
  hym->SetTitleSize(0.06,"Y");

  TH1F* hpxp = new TH1F("hpxp","",14,-3.5,3.5);
  hpxp->SetFillColor(5);
  hpxp->SetXTitle("x_{+}");
  hpxp->SetYTitle("Entries/0.005");
  hpxp->SetTitleSize(0.06,"X");
  hpxp->SetTitleSize(0.06,"Y");


  TH1F* hpyp = new TH1F("hpyp","",14,-3.5,3.5);
  hpyp->SetFillColor(5);
  hpyp->SetXTitle("y_{+}");
  hpyp->SetYTitle("Entries/0.005");
  hpyp->SetTitleSize(0.06,"X");
  hpyp->SetTitleSize(0.06,"Y");

  TH1F* hpxm = new TH1F("hpxm","",14,-3.5,3.5);
  hpxm->SetFillColor(5);
  hpxm->SetXTitle("x_{-}");
  hpxm->SetYTitle("Entries/0.005");
  hpxm->SetTitleSize(0.06,"X");
  hpxm->SetTitleSize(0.06,"Y");

  TH1F* hpym = new TH1F("hpym","",20,-3.5,3.5);
  hpym->SetFillColor(5);
  hpym->SetXTitle("y_{-}");
  hpym->SetYTitle("Entries/0.005");
  hpym->SetTitleSize(0.06,"X");
  hpym->SetTitleSize(0.06,"Y");


  TH1F* hresxp = new TH1F("hresxp","",20,-0.10,0.10);
  hresxp->SetFillColor(5);
  hresxp->SetXTitle("x_{+}-x_{+}^{true}");
  hresxp->SetYTitle("Entries/0.005");
  hresxp->SetTitleSize(0.06,"X");
  hresxp->SetTitleSize(0.06,"Y");

  TH1F* hresyp = new TH1F("hresyp","",20,-0.10,0.10);
  hresyp->SetFillColor(5);
  hresyp->SetXTitle("y_{+}-y_{+}^{true}");
  hresyp->SetYTitle("Entries/0.005");
  hresyp->SetTitleSize(0.06,"X");
  hresyp->SetTitleSize(0.06,"Y");

  TH1F* hresxm = new TH1F("hresxm","",20,-0.10,0.10);
  hresxm->SetFillColor(5);
  hresxm->SetXTitle("x_{-}-x_{-}^{true}");
  hresxm->SetYTitle("Entries/0.005");
  hresxm->SetTitleSize(0.06,"X");
  hresxm->SetTitleSize(0.06,"Y");

  TH1F* hresym = new TH1F("hresym","",20,-0.10,0.10);
  hresym->SetFillColor(5);
  hresym->SetXTitle("y_{-}-y_{-}^{true}");
  hresym->SetYTitle("Entries/0.005");
  hresym->SetTitleSize(0.06,"X");
  hresym->SetTitleSize(0.06,"Y");

  gStyle->SetOptFit(1111);
  gStyle->SetOptStat(1110);



  
  toyresult _toy;

   // Set branch addresses.
   TBranch *branch = alltoys.GetBranch("toyresult");
   alltoys.SetBranchAddress("toyresult",&_toy);
   

   Long64_t nentries = alltoys.GetEntries();

   cout << nentries << endl;


  TH1F* hgam = new TH1F("hgam","",16,20.0,100.0);
  hgam->SetFillColor(5);
  hgam->SetXTitle("#gamma (#circ)");
  hgam->SetYTitle("Entries/5#circ");
  hgam->SetTitleSize(0.06,"X");
  hgam->SetTitleSize(0.06,"Y");

  TH1F* hegam = new TH1F("hegam","",25,0.0,25.0);
  hegam->SetFillColor(5);
  hegam->SetXTitle("#sigma_{#gamma} (#circ)");
  hegam->SetYTitle("Entries/1#circ");
  hegam->SetTitleSize(0.06,"X");
  hegam->SetTitleSize(0.06,"Y");

  TH1F* hpgam = new TH1F("hpgam","",14,-3.5,3.5);
  hpgam->SetFillColor(5);
  hpgam->SetXTitle("#gamma pull");
  hpgam->SetYTitle("Entries/0.5");
  hpgam->SetTitleSize(0.06,"X");
  hpgam->SetTitleSize(0.06,"Y");
   

  TH1F* hdel = new TH1F("hdel","",16,90.0,170.0);
  hdel->SetFillColor(5);
  hdel->SetXTitle("#delta_{B} (#circ)");
  hdel->SetYTitle("Entries/5#circ");
  hdel->SetTitleSize(0.06,"X");
  hdel->SetTitleSize(0.06,"Y");

  TH1F* hedel = new TH1F("hedel","",25,0.0,25.0);
  hedel->SetFillColor(5);
  hedel->SetXTitle("#sigma_{#delta_{B}} (#circ)");
  hedel->SetYTitle("Entries/1#circ");
  hedel->SetTitleSize(0.06,"X");
  hedel->SetTitleSize(0.06,"Y");

  TH1F* hpdel = new TH1F("hpdel","",14,-3.5,3.5);
  hpdel->SetFillColor(5);
  hpdel->SetXTitle("#delta_{B} pull");
  hpdel->SetYTitle("Entries/0.5");
  hpdel->SetTitleSize(0.06,"X");
  hpdel->SetTitleSize(0.06,"Y");


  TH1F* hrb = new TH1F("hrb","",20,0.08,0.12);
  hrb->SetFillColor(5);
  hrb->SetXTitle("r_{B} (#times10^{2})");
  hrb->SetYTitle("Entries/0.025");
  hrb->SetTitleSize(0.06,"X");
  hrb->SetTitleSize(0.06,"Y");


  TH1F* herb = new TH1F("herb","",40,0.5,2.5);
  herb->SetFillColor(5);
  herb->SetXTitle("#delta_{r_{B}} (#times10^{2})");
  herb->SetYTitle("Entries/0.05");
  herb->SetTitleSize(0.06,"X");
  herb->SetTitleSize(0.06,"Y");

  TH1F* hprb = new TH1F("hprb","",14,-3.5,3.5);
  hprb->SetFillColor(5);
  hprb->SetXTitle("r_{B} pull");
  hprb->SetYTitle("Entries/0.5");
  hprb->SetTitleSize(0.06,"X");
  hprb->SetTitleSize(0.06,"Y");


   Long64_t nbytes = 0;

   for (Long64_t i=0; i<nentries;i++) {

     alltoys.GetEntry(i);

     float rbp = sqrt(pow(_toy.xp,2)+pow(_toy.yp,2));
     float rbm = sqrt(pow(_toy.xm,2)+pow(_toy.ym,2));
     float erbp = sqrt(pow(_toy.xp*_toy.exp,2)
                +  pow(_toy.yp*_toy.eyp,2)
                +  2.0*_toy.rhop*_toy.xp*_toy.yp*_toy.exp*_toy.eyp)
                /sqrt(pow(_toy.xp,2)+pow(_toy.yp,2));
     float erbm = sqrt(pow(_toy.xm*_toy.exm,2)
                +  pow(_toy.ym*_toy.eym,2)
                +  2.0*_toy.rhom*_toy.xm*_toy.ym*_toy.exm*_toy.eym)
                /sqrt(pow(_toy.xm,2)+pow(_toy.ym,2));

     float erb = 1.0/sqrt(pow(erbp,-2)+pow(erbm,-2));

     float rb = (rbp/pow(erbp,2)+rbm/pow(erbm,2))*pow(erb,2);
     
     TComplex zp(_toy.xp,_toy.yp);
     TComplex zm(_toy.xm,_toy.ym);
     Double_t xip = zp.Theta();
     if (xip<0.0) xip+=2.0*TMath::Pi();
     Double_t xim = zm.Theta();
     if (xim<0.0) xim+=2.0*TMath::Pi();

     float exip = sqrt(pow((_toy.yp*_toy.exp)/pow(_toy.xp,2),2)+
                       pow(_toy.eyp/_toy.xp,2)-
                       (2.0*_toy.rhop*_toy.yp*_toy.exp*_toy.eyp)/pow(_toy.xp,3))/
       (1+pow(_toy.yp/_toy.xp,2));
     float exim = sqrt(pow((_toy.ym*_toy.exm)/pow(_toy.xm,2),2)+
                       pow(_toy.eym/_toy.xm,2)-
                       (2.0*_toy.rhom*_toy.ym*_toy.exm*_toy.eym)/pow(_toy.xm,3))/
       (1+pow(_toy.ym/_toy.xm,2));
     float egamma = 0.5*sqrt(pow(exip,2)+pow(exim,2))*180/TMath::Pi();
     float edeltab = egamma;
     



     Double_t gamma = 0.5*(xip - xim)*180/TMath::Pi();
     Double_t deltab = 0.5*(xip + xim)*180/TMath::Pi();

     hrb->Fill(rb);
     hgam->Fill(gamma);
     hdel->Fill(deltab);
     hprb->Fill((rb-0.1)/erb);
     hpgam->Fill((gamma-60.0)/egamma);
     hpdel->Fill((deltab-130.0)/edeltab);

     hresxp->Fill(_toy.xp+0.098481);
     hresyp->Fill(_toy.yp+0.017365);
     hresxm->Fill(_toy.xm-0.034202);
     hresym->Fill(_toy.ym-0.093969);

     hpxp->Fill((_toy.xp+0.098481)/_toy.exp);
     hpyp->Fill((_toy.yp+0.017365)/_toy.eyp);
     hpxm->Fill((_toy.xm-0.034202)/_toy.exm);
     hpym->Fill((_toy.ym-0.093969)/_toy.eym);

   }

   TCanvas* c1 = new TCanvas("c1","c1",600,600);
   c1->Divide(2,2);

   c1->cd(1);
   
   hpgam->Draw();
   c1->cd(2);
   
   hprb->Draw();
   
   c1->cd(3);
   
   hpdel->Draw();
   
   
   c1->cd(4);
   //   hym->Draw();

   TCanvas* c2 = new TCanvas("c2","c2",600,600);
   c2->Divide(2,2);

   c2->cd(1);
   
   hpxp->Draw();
   c2->cd(2);
   
   hpyp->Draw();
   
   c2->cd(3);
   
   hpxm->Draw();
   
   
   c2->cd(4);
   hpym->Draw();
   
   TCanvas* c3 = new TCanvas("c3","c3",600,400);
   c3->Divide(2,1);
   c3->cd(1);
   alltoys.Draw("rhom");
   c3->cd(2);
   alltoys.Draw("rhop");
   

  return;
  
}

//
// This is just an example of how to access
// event-averaged occupancies of different OT modules.
// You may set the variable "path"
// and provide the list of files
// which e.g. correspond to different luminosities.
// The histograms will be plotted on top of each other
// 

string path("/work/holubyev/srcommit/Brunel_v35r7/jobs/");
//string path("./");
string files[] = {
  "dummy.root"
  //"",
  //"",
};

const int nfiles = 1;


occup() {

  gStyle->SetOptStat(0);
  gStyle->SetFillColor(kWhite);


 bool spillover = 0;
 TCanvas* cModOcc = new TCanvas("cModOcc", "cModOcc", 800, 400);
 leg = new TLegend(0.4, 0.6, 0.89, 0.89);

 for (int ifile=0; ifile < nfiles; ifile++) {
   
    TFile *nextfile = new TFile((path+files[ifile]).c_str());
    TDirectory *dir = (TDirectory*)nextfile->Get("Track/OccupancyCheck");
    
    TH1D* modOcc = new TH1D("modOcc", "", 9, 1., 10.);
    modOcc->SetMaximum(.5);
    for (int imod=1; imod < 10; imod++){
      char name[10];
      sprintf(name, "occMod%i", imod);
      if (spillover) sprintf(name, "occModSpill%i", imod);
      TH1D* mod = (TH1D*)dir->Get(name);
      modOcc->SetBinContent(imod, mod->GetMean());
    }
    modOcc->SetLineColor(ifile+1);
    modOcc->SetLineWidth(2);
    0==ifile ? 
      modOcc->Draw();
      modOcc->GetXaxis()->SetTitle("OT module");
      char name[40];
      sprintf(name, "Occupancy"); 
      if (spillover) sprintf(name, "Occupancy (spillover)");
      modOcc->GetYaxis()->SetTitle(name);
        : 
      modOcc->Draw("same");
    char buf[100];
    sprintf(buf, "%s", files[ifile].c_str());
    leg->AddEntry(modOcc, buf, "l");
 } // files

 leg->Draw();

}


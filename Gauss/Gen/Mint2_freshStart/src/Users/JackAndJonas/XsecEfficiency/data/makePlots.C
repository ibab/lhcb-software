
int makePlots(int which=0){
  std::string fname="";
  std::string ntpname="";
  
  if(0== which){
    fname = "mctruth_k3pi_tagged_reweighted_April2012.root";
    ntpname = "mctruth_k3pi_tagged";
  }else if(1==which){
    fname = "mctruth_k3pi_reweighted_April2012.root";
    ntpname = "mctruth_k3pi";
  }else if(2==which){
    fname = "mc_k3pi_reweighted_April2012.root";
    ntpname = "mc_k3pi";
  }else if(3==which){
    fname = "mc_k3pi_tagged_reweighted_April2012.root";
    ntpname = "mc_k3pi_tagged";
  }else if(4==which){
    fname = "mc_k3pi_taggedSel_reweighted_April2012.root";
    ntpname = "mc_k3pi";
  }else if(5==which){
    fname = "TomsMidiNtp_mc_k3pi_reweighted_April2012.root";
    ntpname = "mc_k3pi";
  }else if(6==which){
    fname = "TomsMidiNtp_mc_k3pi_tagged_reweighted_April2012.root";
    ntpname = "mc_k3pi_tagged";
  }else if(7==which){
    fname = "GenK3PiTuple_542_tagDown_reweighted_April2012.root";
    ntpname = "GenK3PiTuple";
  }else if(8==which){
    fname = "GenK3PiTuple_544_tagUp_reweighted_April2012.root";
    ntpname = "GenK3PiTuple";
  }else if(9==which){
    fname = "GenK3PiTuple_548_untUp_reweighted_April2012.root";
    ntpname = "GenK3PiTuple";
  }else if(10==which){
    fname = "GenK3PiTuple_549_untDown_reweighted_April2012.root";
    ntpname = "GenK3PiTuple";
  }else{
    cout << "uknown option " << which << endl;
    return -999;
  }

  TFile* file = new TFile(fname.c_str());
  if(0 == file){
    cout << "can't open file" << endl;
    return -888;
  }
  
  TTree* ntp = (TTree*) file->Get(ntpname.c_str());
  if(0 == ntp){
    cout << "cannot find ntuple " << ntpname 
	 << " in file " << fname << endl;
    return -777;
  }

  double s12min = 0.3, s12max=2.6;
  TH1F* s12_12 = new TH1F("s12_12", "s12_12", 100, s12min, s12max);
  TH1F* s12_13 = new TH1F("s12_13", "s12_13", 100, s12min, s12max);
  TH1F* s12_13 = new TH1F("s12_14", "s12_14", 100, s12min, s12max);

  char* s12 = "((K_e + Pi0_e)^2 - (K_px + Pi0_px)^2 - (K_py + Pi0_py)^2 - (K_pz + Pi0_pz)^2)*1.e-6 >> s12_12";
  char* s13 = "((K_e + Pi1_e)^2 - (K_px + Pi1_px)^2 - (K_py + Pi1_py)^2 - (K_pz + Pi1_pz)^2)*1.e-6 >> s12_13";
  char* s14 = "((K_e + Pi2_e)^2 - (K_px + Pi2_px)^2 - (K_py + Pi2_py)^2 - (K_pz + Pi2_pz)^2)*1.e-6 >> s12_14";
  ntp->Draw(s12, "(w>0)*(Pi0_pdg > 0)");
  ntp->Draw(s13, "(w>0)*(Pi1_pdg > 0)");
  ntp->Draw(s14, "(w>0)*(Pi2_pdg > 0)");
  
  s12_12->Add(s12_13);
  s12_12->Add(s12_14);
  s12_12->Draw();

  //ntp->Draw(s12, "w1*(w1 > 0)");
}
//

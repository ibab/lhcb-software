#include "NNTuner.h"

NNTuner::NNTuner(TString& NNetTrain){

  name = NNetTrain;
  if(name=="none") return;

  //create tree of nnet variables:
    TString pref="output/nnet_", suff=".root";
    TString fname  = (pref+NNetTrain)+suff;
    fnnet = new TFile(fname, "RECREATE" );
    cout<<"Booking root file: "<< fname <<endl;

    nntrain = new TTree("nntrain", fname);

    nntrain->Branch("iscorrect",&iscorrect, "iscorrect/I");
    nntrain->Branch("tagger", &tagger,  "tagger/I");
    nntrain->Branch("mult",   &mult,    "mult/F");
    nntrain->Branch("ptB",    &ptB,     "ptB/F");
    nntrain->Branch("partP",  &partP,   "partP/F");
    nntrain->Branch("partPt", &partPt,  "partPt/F");
    nntrain->Branch("IPPV",   &IPPV,    "IPPV/F");
    nntrain->Branch("nndeta", &nndeta,  "nndeta/F");
    nntrain->Branch("nndphi", &nndphi,  "nndphi/F");
    nntrain->Branch("nndq",   &nndq,    "nndq/F");

}

/////////////////////////////////////////////////////////////////
void NNTuner::Fill(Event& event, Taggers& taggers){

  if(name=="none") return;

  Tagger* tag=NULL; 
  if(name == "ele")        tag=taggers.at(0);
  else if(name == "muon")  tag=taggers.at(1);
  else if(name == "kaon")  tag=taggers.at(2);
  else if(name == "kaonS") tag=taggers.at(3);
  else if(name == "pionS") tag=taggers.at(3);
  else err()<<"in NNTuner: unknown tagger: "<<name<<endmsg;

  if(!tag) return;

  if(tag->decision()) {
    Particle* BSpart = event.signalB();
    Particle* tagpart= tag->taggerparts().at(0);
    iscorrect = (tag->decision() == event.TrueTag()) ? 1 : 0 ;

    std::vector<double> NNinputs(8);
    NNinputs.at(0) = event.particles().size();
    NNinputs.at(1) = BSpart->momentum().Vect().Perp();
    NNinputs.at(2) = tagpart->p();
    NNinputs.at(3) = tagpart->pt();
    NNinputs.at(4) = tagpart->IPs();
    NNinputs.at(5) = log(tan(BSpart->Theta()/2.))
      -log(tan(tagpart->Theta()/2.)); //deta
    NNinputs.at(6) = min(fabs(tagpart->Phi()-BSpart->Phi()),
			 float(6.283)-fabs(tagpart->Phi()-BSpart->Phi()));
    double dQ = (BSpart->momentum() + tagpart->momentum()).M() 
      - BSpart->momentum().M();
    NNinputs.at(7) = dQ;
      
    // if(name      == "ele")  { TaggerElectronTool n;  n.normalise( NNinputs ); }
    // else if(name == "muon") { TaggerMuonTool n;      n.normalise( NNinputs ); }
    // else if(name == "kaon") { TaggerKaonOppositeTool n; n.normalise(NNinputs);}
    // else if(name == "kaonS"){ TaggerKaonSameTool n;  n.normalise( NNinputs ); }
    // else if(name == "pionS"){ TaggerPionSameTool n;  n.normalise( NNinputs ); }

    mult   = NNinputs.at(0);
    ptB    = NNinputs.at(1);
    partP  = NNinputs.at(2);
    partPt = NNinputs.at(3);
    IPPV   = NNinputs.at(4);
    nndeta = NNinputs.at(5);
    nndphi = NNinputs.at(6);
    nndq   = NNinputs.at(7);

    nntrain->Fill();// <-- fills branch
    //cout << mult<<" "<< ptB<<" "<< partP<<" "<< partPt<<" "
    //     <<IPPV <<" "<< nndeta<<" " <<nndphi<<" " <<nndq <<endl;
  }

}

//////////////////////////////////////////////////////////////////////////////////
void NNTuner::TrainNNet() {

  if(name=="none") return;

  //**************************************************** NNet for e,mu,K:
  if(name=="ele" || name=="muon" || name=="kaon" ) {
    cout<<"\nWill train NNet for tagger: "<<name<<endl;
    Int_t ntrain = 50;
    TMultiLayerPerceptron *mlp =
      new TMultiLayerPerceptron("mult,partP,partPt,IPPV:6:@iscorrect",
                                "1.",nntrain,"Entry$%2","(Entry$+1)%2");
    mlp->Train(ntrain, "text,graph,update=5");
    TCanvas* mlpa_canvas = new TCanvas("mlpa_canvas",
                                       "Network analysis",1200,0,300,600);
    mlpa_canvas->Divide(1,3);
    TMLPAnalyzer ana(mlp);
    ana.GatherInformations(); //Initialisation
    ana.CheckNetwork(); //output to the console
    mlpa_canvas->cd(1); 
    mlp->Draw(); //shows the network structure
    mlpa_canvas->cd(2); 
    ana.DrawDInputs(); //shows how works each variable
    mlpa_canvas->cd(3);
    ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); //draws the result
    //write out code
    if(name=="muon") mlp->Export("NNmuon");
    if(name=="ele")  mlp->Export("NNele");
    if(name=="kaon") mlp->Export("NNkaon");
  }

 //**************************************************** NNet for KS,piS:
  if(name=="kaonS" || name=="pionS") {
    cout<<"\nWill train NNet for tagger: "<<name<<endl;
    Int_t ntrain = 50;
    TMultiLayerPerceptron *mlp =
      new TMultiLayerPerceptron(
          "mult,partP,partPt,IPPV,nndeta,nndphi,nndq:7:@iscorrect",
          "1.",nntrain,"Entry$%2","(Entry$+1)%2");
    mlp->Train(ntrain, "text,graph,update=5");
    TCanvas* mlpa_canvas= new TCanvas("mlpa_canvas",
                                      "Network analysis",1200,0,300,600);
    mlpa_canvas->Divide(1,3);
    TMLPAnalyzer ana(mlp);
    ana.GatherInformations(); //Initialisation
    ana.CheckNetwork(); //output to the console
    mlpa_canvas->cd(1); 
    mlp->Draw(); //shows the network structure
    mlpa_canvas->cd(2); 
    ana.DrawDInputs(); //shows how works each variable
    mlpa_canvas->cd(3);
    ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); //draws the result
    //write out code
    if(name == "kaonS") mlp->Export("NNkaonS");
    if(name == "pionS") mlp->Export("NNpionS");
  }
//   if(name!="") {
//     cout<<"\n* 1. Now quit, set NNetTrain=\"\" and rerun on the same data!"<<endl;
//     cout<<"* 2. Then update the fitted values into pn[i]."<<endl;
//     cout<<"* 3. Finally rerun and look at your combined performance\n"<<endl;
//     return;
//   }

}

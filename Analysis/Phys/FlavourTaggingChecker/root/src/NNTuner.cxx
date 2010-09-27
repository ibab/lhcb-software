#include "NNTuner.h"

NNTuner::NNTuner(TString& NNetTrain){

  name = NNetTrain;
  if(name=="none") return;

  //create tree of nnet variables:
    TString pref="output/nnet_", suff=".root";
    TString fname  = (pref+NNetTrain)+suff;
    fnnet = new TFile(fname, "RECREATE" );
    info()<<"Booking root file: "<< fname <<endmsg;

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
    nntrain->Branch("nnkrec", &nnkrec,  "nnkrec/F");

    nntrain->Branch("vflag",  &vflag,   "vflag/F");
    nntrain->Branch("ptmin",  &ptmin,   "ptmin/F");
    nntrain->Branch("ipsmin", &ipsmin,  "ipsmin/F");
    nntrain->Branch("docamax",&docamax, "docamax/F");
    nntrain->Branch("maxprobf", &maxprobf,"maxprobf/F");
    nntrain->Branch("vratio", &vratio,  "vratio/F");
    nntrain->Branch("vcharge",&vcharge, "vcharge/F");

    nntrain->Branch("om_muon",&om_muon, "om_muon/F");
    nntrain->Branch("om_ele", &om_ele,  "om_ele/F");
    nntrain->Branch("om_kaon",&om_kaon, "om_kaon/F");
    nntrain->Branch("om_same",&om_same, "om_same/F");
    nntrain->Branch("om_vtx", &om_vtx,  "om_vtx/F");
 
}

/////////////////////////////////////////////////////////////////
void NNTuner::Fill(Event& event, FlavourTag* thetag){

  if(name=="none") return;

  Taggers taggers = thetag->taggers();
  Tagger* tag=NULL; 

  if(     name == "muon")  tag=taggers.at(0);
  else if(name == "ele" )  tag=taggers.at(1);
  else if(name == "kaon")  tag=taggers.at(2);
  else if(name == "kaonS") tag=taggers.at(3);
  else if(name == "pionS") tag=taggers.at(3);
  else if(name == "vtx")   tag=taggers.at(4);
  else if(name == "comb"){
    iscorrect = (thetag->decision() == event.TrueTag()) ? 0 : 1 ; //???
    int sum = abs(taggers.at(0)->decision());
    sum    += abs(taggers.at(1)->decision());
    sum    += abs(taggers.at(2)->decision());
    sum    += abs(taggers.at(3)->decision());
    sum    += abs(taggers.at(4)->decision());
    if(sum<2) return;
    om_muon= taggers.at(0)->omega();
    om_ele = taggers.at(1)->omega();
    om_kaon= taggers.at(2)->omega();
    om_same= taggers.at(3)->omega();
    om_vtx = taggers.at(4)->omega();
    nntrain->Fill();// <-- fills branch
    return;
  }
  else err()<<"in NNTuner: unknown tagger: "<<name<<endmsg;

  if(!tag) return;

  if(tag->decision()) {
    Particle* BSpart = event.signalB();
    Particle* tagpart= tag->taggerparts().at(0);
    iscorrect = (tag->decision() == event.TrueTag()) ? 1 : 0 ;

    mult    = event.particles().size();
    ptB     = BSpart->momentum().Vect().Perp();
    partP   = tagpart->p();
    partPt  = tagpart->pt();
    IPPV    = tagpart->IPs();
    nndeta  = log(tan(BSpart->Theta()/2.))
             -log(tan(tagpart->Theta()/2.)); //deta
    nndphi  = min(fabs(tagpart->Phi()-BSpart->Phi()),
			        float(6.283)-fabs(tagpart->Phi()-BSpart->Phi()));
    nndq    = (BSpart->momentum() + tagpart->momentum()).M() 
             - BSpart->momentum().M();
    nnkrec  = event.pileup();
    
    Vertices mySVs = event.getSecondaryVertices();
    if(mySVs.empty()) return;
    Vertex* mySV = mySVs.at(0);

    vflag   = mySV->getVflagged();
    ptmin   = mySV->getVptmin();
    ipsmin  = mySV->getVipsmin();
    docamax = mySV->getVdocamax();
    maxprobf= mySV->likelihood();
    vratio  = mySV->getVratio();
    vcharge = mySV->getVCharge();
    
    nntrain->Fill();// <-- fills branch
    //cout << mult<<" "<< ptB<<" "<< partP<<" "<< partPt<<" "
    //     <<IPPV <<" "<< nndeta<<" " <<nndphi<<" " <<nndq <<" " <<nnkrec<<endl;
  }

}

//////////////////////////////////////////////////////////////////////////////////
void NNTuner::TrainNNet() {

  if(name=="none") return;

  //**************************************************** NNet for e,mu,K:
  if(name=="muon" || name=="ele" || name=="kaon" ) {
    cout<<"\nWill train NNet for tagger: "<<name<<endl;
    Int_t ntrain = 50;
    TMultiLayerPerceptron *mlp =
      new TMultiLayerPerceptron("mult,partP,partPt,IPPV,nnkrec,ptB:10:@iscorrect",
                                "1.",nntrain,"Entry$%2","(Entry$+1)%2");
    mlp->Train(ntrain, "text,graph,update=5");
    TCanvas* mlpc = new TCanvas("mlpc","Network analysis",1200,0,300,600);
    mlpc->Divide(1,3);
    TMLPAnalyzer ana(mlp);
    ana.GatherInformations(); //Initialisation
    ana.CheckNetwork(); //output to the console
    mlpc->cd(1); mlp->Draw(); //shows the network structure
    mlpc->cd(2); ana.DrawDInputs(); //shows how works each variable
    mlpc->cd(3); ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); 
    //write out code
    if(name=="muon") mlp->Export("NNmuon");
    if(name=="ele")  mlp->Export("NNele");
    if(name=="kaon") mlp->Export("NNkaon");
    exittuner();
  }

  //**************************************************** NNet for KS,piS:
  if(name=="kaonS" || name=="pionS") {
    cout<<"\nWill train NNet for tagger: "<<name<<endl;
    Int_t ntrain = 100;
    TMultiLayerPerceptron *mlp =
      new TMultiLayerPerceptron("mult,partP,partPt,IPPV,nndeta,nndphi,nndq,nnkrec,ptB:10:@iscorrect","1.",
                                nntrain,"Entry$%2","(Entry$+1)%2");
    mlp->Train(ntrain, "text,graph,update=5");
    TCanvas* mlpc= new TCanvas("mlpc","Network analysis",1200,0,300,600);
    mlpc->Divide(1,3);
    TMLPAnalyzer ana(mlp);
    ana.GatherInformations(); //Initialisation
    ana.CheckNetwork(); //output to the console
    mlpc->cd(1); mlp->Draw(); //shows the network structure
    mlpc->cd(2); ana.DrawDInputs(); //shows how works each variable
    mlpc->cd(3); ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); 
    //write out code
    if(name == "kaonS") mlp->Export("NNkaonS");
    if(name == "pionS") mlp->Export("NNpionS");
    exittuner();
  }

  //**************************************************** NNet for vtx:
  if(name=="vtx" ) {
    cout<<"\nWill train NNet for tagger: "<<name<<endl;
    Int_t ntrain = 100;

    TMultiLayerPerceptron *mlp =
      new TMultiLayerPerceptron("mult,nnkrec,ptB,vflag,ptmin,ipsmin,docamax,maxprobf,vratio,vcharge:11:@iscorrect",
                                "1.",nntrain,"Entry$%2","(Entry$+1)%2");
    mlp->Train(ntrain, "text,graph,update=5");
    TCanvas* mlpc = new TCanvas("mlpc","Network analysis",1200,0,300,600);
    mlpc->Divide(1,3);
    TMLPAnalyzer ana(mlp);
    ana.GatherInformations(); //Initialisation
    ana.CheckNetwork(); //output to the console
    mlpc->cd(1); 
    mlp->Draw(); //shows the network structure
    mlpc->cd(2); 
    ana.DrawDInputs(); //shows how works each variable
    mlpc->cd(3);
    ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); //draws the result
    //write out code
    mlp->Export("NNvtx");
    exittuner();
  }


  //**************************************************** NNet for combination
  if(name=="comb") {
    cout<<"\nWill train NNet for taggers combination "<<endl;
    Int_t ntrain = 50;
    TMultiLayerPerceptron *mlp =
      new TMultiLayerPerceptron("om_muon,om_ele,om_kaon,om_same,om_vtx:5:@iscorrect",
                                "1.",nntrain,"Entry$%2","(Entry$+1)%2");
    mlp->Train(ntrain, "text,graph,update=5");
    TCanvas* mlpc= new TCanvas("mlpc","Network analysis",1200,0,300,600);
    mlpc->Divide(1,3);
    TMLPAnalyzer ana(mlp);
    ana.GatherInformations(); //Initialisation
    ana.CheckNetwork(); //output to the console
    mlpc->cd(1); mlp->Draw(); //shows the network structure
    mlpc->cd(2); ana.DrawDInputs(); //shows how works each variable
    mlpc->cd(3); ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); 
    //write out code
    mlp->Export("NNcomb");
    exittuner();
  }

}

void NNTuner::exittuner() {
  if(name!="") {
    cout<<system("rm -v output/tag.root"); //to avoid confusion

    info()<<BOLD<<SUBAMARILLO<<"\n    INSTRUCTIONS on how to proceed:";
    info()<<ROJO<<"\n 1. Quit root, set NNetTrain=none in tag.opts "
          <<"and rerun on the same data \n     to make nnet/NN"<<name
          <<".cxx become active. \n     Ignore any error message."<<endl;
    info()<<VIOLETA<<" 2. Update the fit values of the polynomials p0, p1"
          <<" into src/NNetTool_MLP.cxx"<<endl;
    info()<<AZUL<<" 3. Finally rerun and look at the new combined performance."
          <<ENDC<<endl<<endl;
    return;
  }
}

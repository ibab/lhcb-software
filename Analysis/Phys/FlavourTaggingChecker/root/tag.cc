///////////////////////////////////////////////////////////////
//The complete program to redo tagging, tune neural nets etc..
//starting from the ntuple of the BTaggingAnalysis algorithm
//
// Usage:
// root -l
// .x tag.cc+("filename.root")
//
#include "tag.h"

void tag( TString froot="../job/analysis.root", 
          float param1=0.0, float param2=0.0, float param3=0.0 ) {
	     
  float Nmax=100000000;
  std::vector<TString> filelist;
  cout<<"\nUsing parameters = "<<param1<<"  "<<param2<<"  "<<param3<<endl;

  //STEERING >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
  filelist.push_back( froot );

  std::ofstream afile("outputAll.ascii");

  //NNet commands
  std::string NNetTrain = "";// NNet taggers: "mu" "e" "k" "kS" "pS" "vtx"

  bool IsControlChannel = 0; //look for the true flavour as the reconstructed B
  bool cheatID = 0;     // Perfect ID with MC truth
  bool cheatP  = 0;     // Perfect  P and Pt of tracks
  bool useDV   = 0;     // Read only outcome from C++ decision
  bool redoPID = 1;     // Redo PID
  bool SAME    = 1;     // 0 no same side, 1 use also same side tag
  bool DBG     = 0;     // debug printoout mode
 
  //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> 
  //Taggers common selection cuts 
  float IPPU_cut= 3.0, IP_cut= 2.0, distPhi_cut= 0.005;
  //dilution binning range for omega combination
  float  ProbMin    = 0.55;
  double omegascale = 0.06, omegamaxbin = 0.36;

  //muons -------------
  float Muon_Pt_cut= 1.1, Muon_P_cut= 0.0;
  float lcs_m_cut  = 4.0;
  float PID_m_cut  = 0.0;

  //elect -------------
  float Ele_Pt_cut   = 1.1, Ele_P_cut= 4.0;
  float VeloChargeMin= 0.0, VeloChargeMax= 1.7, Ele_EoverP_cut = 0.90;
  float lcs_e_cut  = 3.0; 
  float Ele_ghost_cut = -25.0;
  float PID_e_cut  = 5.0;

  //OS kaons -------------
  float Kaon_Pt_cut   = 0.6, Kaon_P_cut= 4.0, Kaon_IP_cut= 3.5;
  float lcs_k_ups_cut = 2.0, lcs_k_long_cut = 2.5, Kaon_ghost_prob_cut = -20;
  float PID_k_cut     = 8.0, PID_kp_cut = -1.0;
  float P0_k = 9.839528e-01, P1_k = -9.846077e-01;

  //cuts x Same Side tagging Kaons -------------
  float KaonSame_Pt_cut = 0.4, KaonSame_P_cut  = 4.0, KaonSame_IP_cut = 3.0;
  float KaonSame_Phi_cut= 1.1, KaonSame_Eta_cut= 1.0, KaonSame_dQ_cut = 1.4;
  float lcs_kS_cut = 4.0;
  float PID_kS_cut = 8.0, PID_kpS_cut = -4.0;

  //cuts x Same Side tagging Pions -------------
  float PionSame_Pt_cut = 0.2, PionSame_P_cut  = 2.0, PionSame_IP_cut= 3.0;
  float PionSame_dQ_cut = 3.0, PionSame_dQ2_cut= 1.5;
  float PionSame_PIDNoK_cut = 5.0, PionSame_PIDNoP_cut = 10.0;
  float Pion_ghost_prob_cut = -25;
  float lcs_pS_cut  = 3.0;
  float ProbMin_pion= 0.55;

  //weight of Vch in tagging decision -------------
  float VchPowerK= 0.35, MinimumVCharge= 0.15;
  float m_P0 = 5.259631e-01, m_P1 = -3.256825e-01, m_Gt075 = 0.35;
  float wSameSign2 = 0.420701, wSameSignMoreThan2 = 0.352641;
  float ProbMinVtx = 0.52;
  //>>>>>>>>>>>>>>>>>>>>>>>>

  int n2trackR=0, n2trackW=0, ntrackR=0, ntrackW=0;
  gROOT->Reset();

  //create tree of nnet variables:
  TTree *nntrain = new TTree("nnet", "tree nnet");
  Int_t iscorrect,tagger;
  Float_t mult,ptB,partP,partPt,IPPV,nndeta,nndphi,nndq;
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

  //create  histograms
  TFile fhisto("tag.root","RECREATE");
  TH1F* h5    = new TH1F("h5","BSmass ",100, 5.2, 5.5);
  TH1F* h6    = new TH1F("h6","B pT ",100, 0.0, 40.);
  TH1F* h513  = new TH1F("h513","NNmu ", 40, 0.3, 1.);
  TH1F* h613  = new TH1F("h613","NNmu ", 40, 0.3, 1.);
  TH1F* h511  = new TH1F("h511","NNe  ", 40, 0.3, 1.);
  TH1F* h611  = new TH1F("h611","NNe  ", 40, 0.3, 1.);
  TH1F* h5321 = new TH1F("h5321","NNk ", 40, 0.3, 1.);
  TH1F* h6321 = new TH1F("h6321","NNk ", 40, 0.3, 1.);
  TH1F* h53210= new TH1F("h53210","NNks/pi",40, 0.3, 1.);
  TH1F* h63210= new TH1F("h63210","NNks/pi",40, 0.3, 1.);
  TH1F* h540  = new TH1F("h540","vert charge ", 70, .1, .8);
  TH1F* h640  = new TH1F("h640","vert charge ", 70, .1, .8);
  TH1F* h100  = new TH1F("h100","PNetSum ", 60, 0.4, 1.);
  TH1F* h102  = new TH1F("h102","PNetSum ", 60, 0.4, 1.);
  TH1F* h103  = new TH1F("h103","1-PNetSum",60, 0.0, 0.6);
  TH1F* h104  = new TH1F("h104","1-PNetSum",60, 0.0, 0.6);
  TH1F* h1000 = new TH1F("h1000","de  ", 50, -25.0, 25.);
  TH1F* h1001 = new TH1F("h1001","dmu ", 50, -30.0, 30.);
  TH1F* h1002 = new TH1F("h1002","dk  ", 50, -20.0, 80.);
  TH1F* h1003 = new TH1F("h1003","dkS  ", 50,-20.0, 80.);
  TH1F* h1004 = new TH1F("h1004","dkS-dp",50,-20.0, 80.);
  TH1F* h1005 = new TH1F("h1005","dk-dp", 50,-20.0, 80.);
  TH1F* h1010 = new TH1F("h1010","de  ", 50, -25.0, 25.);
  TH1F* h1011 = new TH1F("h1011","dmu ", 50, -30.0, 30.);
  TH1F* h1012 = new TH1F("h1012","dk  ", 50, -20.0, 80.);
  TH1F* h1013 = new TH1F("h1013","dkS ", 50, -20.0, 80.);
  TH1F* h1014 = new TH1F("h1014","dkS-dp",50,-20.0, 80.);
  TH1F* h1015 = new TH1F("h1015","dk-dp", 50,-20.0, 80.);
  TH1F* h1020 = new TH1F("h1020","de  ", 50, -25.0, 25.);
  TH1F* h1021 = new TH1F("h1021","dmu ", 50, -30.0, 30.);
  TH1F* h1022 = new TH1F("h1022","dk  ", 50, -20.0, 80.);
  TH1F* h1023 = new TH1F("h1023","dkS ", 50, -20.0, 80.);
  TH1F* h1024 = new TH1F("h1024","dkS-dp",50,-20.0, 80.);
  TH1F* h1025 = new TH1F("h1025","dk-dp", 50,-20.0, 80.);
  TH1F* h301  = new TH1F("h301","P B Opposite",50, 0, 500.);
  TH1F* h401  = new TH1F("h401","P B",50, 0, 500.);
  TH1F* h302  = new TH1F("h302","theta B",50, 0, 300.);
  TH1F* h311  = new TH1F("h311","mult",50, 0, 100.);
  TH1F* h312  = new TH1F("h312","mult",50, 0, 100.);
 
  TH1F* h2011   = new TH1F("h2011","Mu tag B time (ps) no osc.", 1000,0,10);
  TH1F* h2012   = new TH1F("h2012","Mu tag B time (ps) osc.",    1000,0,10);
  TH1F* h2021   = new TH1F("h2021","Ele tag B time (ps) no osc.",1000,0,10);
  TH1F* h2022   = new TH1F("h2022","Ele tag B time (ps) osc.",   1000,0,10);
  TH1F* h2031   = new TH1F("h2031","KO tag B time (ps) no osc.", 1000,0,10);
  TH1F* h2032   = new TH1F("h2032","KO tag B time (ps) osc.",    1000,0,10);
  TH1F* h2041   = new TH1F("h2041","SS tag B time (ps) no osc.", 1000,0,10);
  TH1F* h2042   = new TH1F("h2042","SS tag B time (ps) osc.",    1000,0,10);
  TH1F* h2051   = new TH1F("h2051","Vtx tag B time (ps) no osc.",1000,0,10);
  TH1F* h2052   = new TH1F("h2052","Vtx tag B time (ps) osc.",   1000,0,10);
  TH1F* h3011   = new TH1F("h3011","cat1 tag B time (ps) no osc.", 1000,0,10);
  TH1F* h3012   = new TH1F("h3012","cat1 tag B time (ps) osc.",    1000,0,10);
  TH1F* h3021   = new TH1F("h3021","cat2 tag B time (ps) no osc.", 1000,0,10);
  TH1F* h3022   = new TH1F("h3022","cat2 tag B time (ps) osc.",    1000,0,10);
  TH1F* h3031   = new TH1F("h3031","cat3 tag B time (ps) no osc.", 1000,0,10);
  TH1F* h3032   = new TH1F("h3032","cat3 tag B time (ps) osc.",    1000,0,10);
  TH1F* h3041   = new TH1F("h3041","cat4 tag B time (ps) no osc.", 1000,0,10);
  TH1F* h3042   = new TH1F("h3042","cat4 tag B time (ps) osc.",    1000,0,10);
  TH1F* h3051   = new TH1F("h3051","cat5 tag B time (ps) no osc.", 1000,0,10);
  TH1F* h3052   = new TH1F("h3052","cat5 tag B time (ps) osc.",    1000,0,10);
  TH1F* h3061   = new TH1F("h3061","ALL tag B time (ps) no osc.",  1000,0,10);
  TH1F* h3062   = new TH1F("h3062","ALL tag B time (ps) osc.",     1000,0,10);

  TH1F* h5000 = new TH1F("h5000","omega comb",50, 0., 1.);
  TH1F* h5001 = new TH1F("h5001","nnet mu",50, 0., 1.);
  TH1F* h5002 = new TH1F("h5002","nnet ele",50, 0., 1.);
  TH1F* h5003 = new TH1F("h5003","nnet kO",50, 0., 1.);
  TH1F* h5005 = new TH1F("h5005","nnet Vtx",100, 0., 1.);
  TH1F* h5006 = new TH1F("h5006","nnet SameS",50, 0., 1.);

  TH1F* h5100 = new TH1F("h5100","omega comb",50, 0., 1.);
  TH1F* h5101 = new TH1F("h5101","nnet mu",50, 0., 1.);
  TH1F* h5102 = new TH1F("h5102","nnet ele",50, 0., 1.);
  TH1F* h5103 = new TH1F("h5103","nnet kO",50, 0., 1.);
  TH1F* h5105 = new TH1F("h5105","nnet Vtx",100, 0., 1.);
  TH1F* h5106 = new TH1F("h5106","nnet SameS",50, 0., 1.);

  TH1F* h501 = new TH1F("h501","signal kaon pT",100, 0., 10.);

  float x1=0.0, x2=20. ; //marco
  TH1F* hright = new TH1F("hright","right tags", 50, x1, x2);
  TH1F* hwrong = new TH1F("hwrong","wrong tags", 50, x1, x2);
  TH1F* homega = new TH1F("homega","omega",      50, x1, x2);
  TH1F* heffec = new TH1F("heffec","effective efficiency", 50, x1, x2);
  TString direction="left2right";

  //FILE LOOP>>>>>>>>>>>>>>>>>>>>>>>>+++++++++++++++++++++++++++++++++++++++
  Float_t nB531=0, nB521=0, nB511=0, nBother=0, nBanti=0, ntrig=0;
  std::vector<TString>::iterator ifile; int nfile = 0;
  for(ifile=filelist.begin(); ifile!=filelist.end(); ++ifile, ++nfile ){

    TFile* f = new TFile(*ifile); if(!f) return;
    TTree* tagging = (TTree*)gDirectory->Get("tagging");
    #include "tag.inc.h"

    //inits
    nsele=0;
    nlm=nllm=nle=nlle=nlk=nlkS=nllk=nllkS=nidm=nide=nidk=nidkS=
      nghost_m=nghost_e=nghost_k=nghost_kS=0; 
    nrL0tis=nrL0tos=nrL0tob=nrL1tis=nrL1tos=nrL1tob=0;
    int ntruemu=0, ntrueel=0, ntruekO=0, ntruekS=0;
    int ntruemutag=0, ntrueeltag=0, ntruekOtag=0, ntruekStag=0;
    int ndirect_mu=0,ndirect_el=0,nbc_mu=0,nbc_el=0,nbc_kO=0,nbc_kS=0;
    for(int i=0; i<20; ++i) {nrt[i]=0; nwt[i]=0; nrtag[i]=0; nwtag[i]=0;}


    // EVENT LOOP //////////////////////////////////////////////////
    float Nevt = std::min( (float)tagging->GetEntries(), Nmax);
    cout<<"Reading "<<(int)Nevt<<" entries from: "<<(*ifile)<<endl;
    for (int ievt=0; ievt<Nevt; ievt++){
      tagging->GetEntry(ievt); //fill ntp branches
      PrintAdvance(ievt,Nevt);

      int Btag=1, BSID = 0;
      double BMas= 5.279, Btime= -999 ;
      TLorentzVector BSpart(0.0);

      for (int ii=0; ii<M; ii++){

        int bid = abs(sID[ii]);
        //FORCE CORRECT RECO
        //if(bid<500) if(fabs(sP[ii]-sMCP[ii])/sMCP[ii]>0.10) {BSID=0; break;}

        if(bid==531 || bid==511 || bid==521|| bid==0) {
          if( IsControlChannel ) TrueTag = sID[ii]>0? 1 : -1 ;///////TrueTag REDEF!!!
          BSID = sID[ii];
          if(abs(BSID)==531) BMas = 5.367;
          BSpart = build4V( sP[ii], sPt[ii], sPhi[ii], sMass[ii] );
          if( BSID<0 ) Btag =-1;
          if( BSosc  ) Btag*=-1;
          TVector3 Bdir(sVx[ii]-RVx, sVy[ii]-RVy, sVz[ii]-RVz);
          Btime = Bdir.Mag()*BMas/sP[ii]/0.299792458; //proper time in ps
          //break;
        }
        if(bid == 321) h501-> Fill( sPt[ii] );
      }	
      if(!BSID) {
        //cout<<"B does not exist! Skip."<< endl; 
        continue;
      }

      bool isBs = false; if(abs(BSID)==531)  isBs = true;
      bool isBd = false; if(abs(BSID)==511)  isBd = true;
      bool isBu = false; if(abs(BSID)==521)  isBu = true;
      
      //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      // GENERAL CUTS TO SELECT EVENTS FOR STUDY PURPOSES
      if(trig != -1) if( trig == 0 ) continue;     //at least L0 trigger
      //if( bkgCat != 0 && bkgCat != 50 ) continue;//ONLY SIGNAL
      //if( fabs( BSpart.M()-BMas )> 0.050 ) continue;
      //if( BSpart.P()<144) continue;
      //if( BOP<144) continue;
      //if( K > 1 ) continue; 
      //if( krec== 1 ) continue;
      //if( kType != 68 ) continue;
      //if( TrueTag == -1 ) continue;
      //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      int evtW = 1;
      nsele += evtW;

      if(abs(BOID)==511)      ++nB511;
      else if(abs(BOID)==521) ++nB521;
      else if(abs(BOID)==531) ++nB531;
      else  ++nBother;
      if(BSID<0)  ++nBanti;
      if(trig!=0) ++ntrig;

      if(DBG) cout<< froot <<" ----------- "
                  << Run <<" "<< Event <<" "<< trig  <<endl;

      //************************
      //  fill event histos    *
      //************************
      h5->Fill(BSpart.M());
      h6->Fill(BSpart.Pt());
      if(nfile==0) h301->Fill(BOP);
      if(nfile==0) h401->Fill(BSpart.P());
      if(nfile==1) h302->Fill(BSpart.Theta()*1000);
      if(nfile==0) h311->Fill(N);
      if(nfile==1) h312->Fill(N);
      //************************

      //inits
      int imuon,iele,ikaon,ikaonS,ipionS, vflagged=0;
      imuon=iele=ikaon=ikaonS=ipionS=0;
      bool bmuon,bele,bkaon,bkaonS,bpionS;
      bmuon=bele=bkaon=bkaonS=bpionS=false;
      float ptmaxm,ptmaxe,ptmaxk,ptmaxkS,ptmaxpS;
      ptmaxm=ptmaxe=ptmaxk=ptmaxkS=ptmaxpS=-999;
      double WeigVch=0, norm=0;

      //- loop over N : select candidate m, e, K ---------------------------->
      for (int i=0; i<N; i++) {
    
        //Plots of PID//
        int ismu, muNSH, inHcal, iflag_e, iflag_m, iflag_k;
        decode(PIDfl[i], ismu, muNSH, inHcal, iflag_e, iflag_m, iflag_k );

        //OVERWRITE ID
        if ( redoPID ){
          ID[i] = 211 * ch[i];
          if( iflag_e==1 && PIDe[i] > PID_e_cut ) ID[i] = -11 * ch[i];
          if( iflag_k==1 && PIDk[i] > PID_k_cut 
              &&  PIDk[i]-PIDp[i]   > PID_kp_cut) ID[i] = 321 * ch[i];
          if( iflag_m==1 && PIDm[i] > PID_m_cut ) ID[i] = -13 * ch[i];
        }

        //CHEATING:
        if( cheatID  ) { 
          ID[i]=MCID[i];
          PIDe[i]=PIDm[i]=PIDk[i]=PIDp[i]=-1000.;
          if(abs(MCID[i])==11)  PIDe[i]=1000.;
          if(abs(MCID[i])==13)  PIDm[i]=1000.;
          if(abs(MCID[i])==321) PIDk[i]=1000.;
          if(abs(MCID[i])==2212)PIDp[i]=1000.;
        }
        if( cheatP   ) {
          P[i] = MCP[i];
          Pt[i]= MCPt[i];
        }

        //calc ID efficiencies in total sample of P > 5GeV
        if(P[i]>5.0) {
          if(abs(MCID[i])==13) {nlm++; if(MCID[i]==ID[i]) nllm++;} 
          if(abs(MCID[i])==11) {nle++; if(MCID[i]==ID[i]) nlle++;} 
          if(abs(MCID[i])==321){nlk++; if(MCID[i]==ID[i]) nllk++;}
          if(abs(MCID[i])==321){nlkS++; if(MCID[i]==ID[i]) nllkS++;}
        }
    
        //reweight Vertex Charge with product pt
        if(vFlag[i]==1) {
          double a = pow(Pt[i], VchPowerK);
          WeigVch += ch[i] * a;
          norm+= a;
          vflagged++;
        }

        //-------------------------------------------------------
        //select among tag candidates take the highest Pt of each
        double IPsig = fabs(ip[i]/iperr[i]);
        double deta= fabs(log(tan(BSpart.Theta()/2.))-log(tan(asin(Pt[i]/P[i])/2.)));
        double dphi= fabs(phi[i]-BSpart.Phi()); if(dphi>3.1416) dphi=6.283-dphi;
        double dQ_asKaon = calc_dQ(BSpart, build4V(P[i],Pt[i],phi[i], 321) );
        double dQ_asPion = calc_dQ(BSpart, build4V(P[i],Pt[i],phi[i], 211) );

        // DEBUG:
        if(DBG) {
          cout<< "--- trtyp="<<trtyp[i] << " ID="<<ID[i]<<setprecision(4)
              << " P="<<P[i] << " Pt="<<Pt[i] << " lcs="<<lcs[i] <<endl;
          cout<< "    deta="<<deta<< " dphi="<<dphi 
              << " dQ="<< (isBs? dQ_asKaon:dQ_asPion) <<endl;
          cout<<"    IPs="<<fabs(ip[i]/iperr[i])<<" IPPU="<<ipPU[i]<<" NSH="<<muNSH<<endl;
          cout<<"    vFlag="<<vFlag[i]<<" bFlag="<<bFlag[i]<<" MCID="<<MCID[i]<<endl;
          cout<<"   PIDm,e,k "<< PIDm[i]<<" "<<PIDe[i]<<" "<<PIDk[i]<<endl;
          cout<<"   E/P="<< EOverP[i]<<" veloch="<<veloch[i]<<endl;
        }

        //if(MCID[i]==0) continue;

        if(ipPU[i] < IPPU_cut) continue;   // all taggers far from PU vertices
        if(distPhi[i]<distPhi_cut) continue;
        if(bFlag[i]==-1) { 
          //cout<<"*** Warning: part from signal! Cheating. Skip."<<ID[i]<<endl;
          // continue; //cheating: kill all taggers from signal
        }

        if(bFlag[i]==1 ) {
          if(abs(MCID[i])==13)  ntruemu++;
          if(abs(MCID[i])==11)  ntrueel++;
          if(abs(MCID[i])==321) ntruekO++;
        } 
        if(abs(MCID[i])==321&& bFlag[i]==0&& xFlag[i]!=0) ntruekS++;

        //INCLUSIVE PID mode
        //if( abs(MCID[i])==13 )
        if( muNSH==0 && ismu==1 && PIDm[i] > PID_m_cut ) 
          if(Pt[i] > Muon_Pt_cut) 
            if(P[i] > Muon_P_cut) 
              if( ( trtyp[i] == 3 || trtyp[i] == 7 ) && lcs[i]<lcs_m_cut) 
                if( Pt [i]> ptmaxm ) { //Pt ordering
                  bmuon = true;
                  imuon = i;
                  ptmaxm = Pt[i];
                  if(DBG) cout<<"accepted muoncand pt="<<ptmaxm<<endl;
                }

        // selects OS electron tag      
        //if( abs(MCID[i])==11 )
        if( iflag_e==1 && PIDe[i] > PID_e_cut )
          if(Pt[i] > Ele_Pt_cut) 
            if(P[i] > Ele_P_cut) 
              if(tsal[i]>Ele_ghost_cut)
                if( ( trtyp[i] == 3 || trtyp[i] == 7 ) && lcs[i]<lcs_e_cut ) 
                  if( EOverP[i] > Ele_EoverP_cut || EOverP[i]<-100 ) 
                    if( veloch[i] > VeloChargeMin && veloch[i] < VeloChargeMax ) 
                      if( Pt[i] > ptmaxe ) { //Pt ordering
                        bele = true;
                        iele = i;
                        ptmaxe = Pt[i];
                        if(DBG) cout<<"accepted elecand pt="<<ptmaxe<<endl;
                      }

        // selects OS kaon tag
        //if( abs(MCID[i])==321 )
        if( PIDk[i]!=0 && PIDk[i]>PID_k_cut && PIDk[i]-PIDp[i]>PID_kp_cut ) 
          if(Pt[i] > Kaon_Pt_cut)
            if(P[i]  > Kaon_P_cut)
              if(IPsig > Kaon_IP_cut)
                if(fabs(ip[i])< IP_cut) 
                  if(tsal[i]>Kaon_ghost_prob_cut)
                    if(( trtyp[i]==4 && lcs[i]<lcs_k_ups_cut  ) ||
                       ( (trtyp[i]==3 || trtyp[i]==7 ) && lcs[i]<lcs_k_long_cut ) )
                      if( Pt[i] > ptmaxk ) { //Pt ordering
                        bkaon = true;
                        ikaon = i;
                        ptmaxk = Pt[i]; 
                        if(DBG) cout<<"accepted kcand pt="<<ptmaxk<<endl;
                      }
	
        // selects SS kaon tag
        if( isBs ) { 
          //if( abs(MCID[i])==321 )
          if( PIDk[i]!=0 && PIDk[i]> PID_kS_cut && PIDk[i]-PIDp[i]> PID_kpS_cut )
            if(Pt[i] > KaonSame_Pt_cut) 
              if(P[i]> KaonSame_P_cut) 
                if(IPsig < KaonSame_IP_cut)
                  if(dphi < KaonSame_Phi_cut)
                    if(deta < KaonSame_Eta_cut)
                      if(dQ_asKaon < KaonSame_dQ_cut)
                        if( lcs[i] < lcs_kS_cut ) 
                          if( Pt[i] > ptmaxkS ) { //Pt ordering
                            bkaonS = true;
                            ikaonS = i;
                            ptmaxkS = Pt[i];
                            if(DBG) cout<<"accepted kScand pt="<<ptmaxkS<<endl;
                          }
        }	
        // selects SS pion tag
        if( isBd || isBu ) {
          bool pidpass=false;
          if( PIDk[i]==0 ) pidpass=true;
          if( PIDk[i]!=0 ) if(PIDk[i] < PionSame_PIDNoK_cut &&
                              PIDp[i] < PionSame_PIDNoP_cut) pidpass=true;
          if( pidpass ) 
            if( abs(ID[i])==211 )
              if(tsal[i]>Pion_ghost_prob_cut)
                if(Pt[i] > PionSame_Pt_cut)
                  if(P[i]> PionSame_P_cut)
                    if(IPsig < PionSame_IP_cut)
                      if(dQ_asPion < PionSame_dQ_cut ) 
                        if((trtyp[i]==3 || trtyp[i]==7) && lcs[i]<lcs_pS_cut)
                          if( Pt[i] > ptmaxpS ) { //Pt ordering
                            bpionS = true;
                            ipionS = i;
                            ptmaxpS = Pt[i];
                            if(DBG) cout<<"accepted pScand pt="<<ptmaxpS<<endl;
                          }
        }
      } //end of loop on parts <---

      //extra cut for pions SS candidates
      if(bpionS) {
        double dQp= calc_dQ(BSpart,build4V(P[ipionS],Pt[ipionS],phi[ipionS],211));
        if( dQp > PionSame_dQ2_cut ) {bpionS=false; ipionS=0;}
      }

      //vertex charge calculation
      //       for (int i=0; i<N; i++) { //Kill if uses a tagger
      // 	if(vFlag[i]==1) {
      // 	  if(i==imuon) { WeigVch = 0; break; }
      // 	  if(i==iele ) { WeigVch = 0; break; }
      // 	  if(i==ikaon) { WeigVch = 0; break; }
      // 	}
      //       }
      if(norm) WeigVch = WeigVch / norm;
      if(fabs(WeigVch) < MinimumVCharge ) WeigVch = 0;

      //--- itag is the individual B-flavour guess of each separate tagger:
      std::vector<int> itag;
      for( int j=0; j<6; ++j ) itag.push_back(0);
      if( bmuon ) itag[1]= -ch[imuon];
      if( bele  ) itag[2]= -ch[iele];
      if( bkaon ) itag[3]= -ch[ikaon];
      if( bkaonS) itag[4]=  ch[ikaonS]; // SS kaon type
      if( bpionS) itag[4]=  ch[ipionS]; // SS pion type
      if( isBu )  itag[4]= -itag[4];    //swap same-side if Bu
      if(WeigVch) itag[5]=  WeigVch>0? -1 : 1;


      //************************************
      //  fill NNet Branch and NNinputs    *
      //************************************
      if( NNetTrain != "" ) {
        int tag=0, j=0;
        if(NNetTrain == "mu")  { j=imuon;  tag=itag[1]; }
        if(NNetTrain == "e")   { j=iele;   tag=itag[2]; }
        if(NNetTrain == "k")   { j=ikaon;  tag=itag[3]; }
        if(NNetTrain == "kS")  { j=ikaonS; tag=itag[4]; }
        if(NNetTrain == "pS")  { j=ipionS; tag=itag[4]; }
        if(tag) {
          iscorrect = (tag == TrueTag) ? 1 : 0 ; 
          std::vector<double> NNinputs(8);
          NNinputs.at(0) = N;
          NNinputs.at(1) = BSpart.Vect().Perp();
          NNinputs.at(2) = P[j];
          NNinputs.at(3) = Pt[j];
          NNinputs.at(4) = ip[j]/iperr[j];
          NNinputs.at(5) = log(tan(BSpart.Theta()/2.))
            -log(tan(asin(Pt[j]/P[j])/2.)); //deta
          NNinputs.at(6) = min(fabs(phi[j]-BSpart.Phi()), 
                               float(6.283)-fabs(phi[j]-BSpart.Phi()));
          NNinputs.at(7) = calc_dQ(BSpart, build4V(P[j],Pt[j],phi[j], 
                                                   NNetTrain=="kS"? 321:211) );

          normalise( NNinputs ); //otherwise NNet doesnt like them

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

      //**********************************************************************
      //  fill tagger histos   *
      //************************
      if(bele) {
        h1010->Fill(PIDe[iele]); 
        if( abs(MCID[iele])== 11 ) h1000->Fill(PIDe[iele]);
        if( abs(MCID[iele])== 0  ) h1020->Fill(PIDe[iele]);
      }
      if(bmuon) {
        h1011->Fill(PIDm[imuon]); 
        if( abs(MCID[imuon])== 13 ) h1001->Fill(PIDm[imuon]);
        if( abs(MCID[imuon])== 0  ) h1021->Fill(PIDm[imuon]);
      }
      if(bkaon) {
        h1012->Fill(PIDk[ikaon]); h1015->Fill(PIDk[ikaon]-PIDp[ikaon]);
        if( abs(MCID[ikaon])== 321) h1002->Fill(PIDk[ikaon]);
        if( abs(MCID[ikaon])== 321) h1005->Fill(PIDk[ikaon]-PIDp[ikaon]);
        if( abs(MCID[ikaon])== 0)   h1022->Fill(PIDk[ikaon]);
        if( abs(MCID[ikaon])== 0)   h1025->Fill(PIDk[ikaon]-PIDp[ikaon]);
      }
      if(bkaonS) {
        h1013->Fill(PIDk[ikaonS]); h1014->Fill(PIDk[ikaonS]-PIDp[ikaonS]); 
        if( abs(MCID[ikaonS])== 321 ) h1003->Fill(PIDk[ikaonS]);
        if( abs(MCID[ikaonS])== 321 ) h1004->Fill(PIDk[ikaonS]-PIDp[ikaonS]);
        if( abs(MCID[ikaonS])== 0 )   h1023->Fill(PIDk[ikaonS]);
        if( abs(MCID[ikaonS])== 0 )   h1024->Fill(PIDk[ikaonS]-PIDp[ikaonS]);
      }

      /////////////////////////marco
      //       if(bkaonS) {
      // 	direction="right2left";
      //  	double dphi= fabs(phi[ikaonS]-BSpart.Phi()); if(dphi>3.1416) dphi=6.283-dphi;
      // // 	double deta= fabs(log(tan(BSpart.Theta()/2.))
      // // 			  -log(tan(asin(Pt[ikaonS]/P[ikaonS])/2.)));//[0,3]
      // 	double dQ_asKaon = calc_dQ(BSpart, 
      // 				   build4V(P[ikaonS],Pt[ikaonS],phi[ikaonS], 321) );
      // 	double var = dQ_asKaon;
      //  	 if( itag[ 4 ]==TrueTag ) hright->Fill(var); else hwrong->Fill(var);
      //       }
      //       if(bele) { 
      // 	//direction="right2left";
      // 	double var = EOverP[iele];
      //  	 if( itag[ 2 ]==TrueTag ) hright->Fill(var); else hwrong->Fill(var);
      //       }
  
      //--- Combine tags: -----------------------------------------
      double pnsum_a, pnsum_b, rnet=0;
      std::vector<double> NNinputs(8),pn;
      NNinputs.at(0) = N;
      NNinputs.at(1) = BSpart.Vect().Perp();
      for( int j=0; j!=6; j++ ) pn.push_back(0.50);

      if(bmuon) {      
        NNinputs.at(2) = P[imuon];
        NNinputs.at(3) = Pt[imuon];
        NNinputs.at(4) = ip[imuon]/iperr[imuon];
        rnet = MLP_muon( NNinputs );
        pn[1] = 1.0-pol(rnet, 9.084315e-01, -8.764311e-01);
        if(itag[1] == TrueTag) h613->Fill(rnet); else h513->Fill(rnet);
      }

      if(bele) {
        NNinputs.at(2) = P[iele];
        NNinputs.at(3) = Pt[iele];
        NNinputs.at(4) = ip[iele]/iperr[iele];
        rnet = MLP_ele( NNinputs );
        pn[2] = 1.0-pol(rnet, 7.937505e-01, -7.197138e-01);
        //	pn[2] = 1.0 - (9.210542e-01 - 8.787684e-01*rnet) ;
        if(itag[2] == TrueTag) h611->Fill(rnet); else h511->Fill(rnet);
      }

      if(bkaon) {
        NNinputs.at(2) = P[ikaon];
        NNinputs.at(3) = Pt[ikaon];
        NNinputs.at(4) = ip[ikaon]/iperr[ikaon];
        rnet = MLP_kaon( NNinputs );
        pn[3]= 1.0-pol(rnet, P0_k, P1_k );
        if(itag[3] == TrueTag) h6321->Fill(rnet); else h5321->Fill(rnet);
      }
      
      if(bkaonS) {
        double ang = asin(Pt[ikaonS]/P[ikaonS]);
        double deta= log(tan(BSpart.Theta()/2.))-log(tan(ang/2.));
        double dphi= fabs(phi[ikaonS]-BSpart.Phi()); if(dphi>3.1415)dphi=6.283-dphi;
        NNinputs.at(2) = P[ikaonS];
        NNinputs.at(3) = Pt[ikaonS];
        NNinputs.at(4) = ip[ikaonS]/iperr[ikaonS];
        NNinputs.at(5) = deta;
        NNinputs.at(6) = dphi;
        NNinputs.at(7) = calc_dQ(BSpart, 
                                 build4V(P[ikaonS],Pt[ikaonS],phi[ikaonS],321) );
        rnet = MLP_kaonS( NNinputs );
        pn[4] = 1.0-pol(rnet, 1.004943, -1.017668);
        if(itag[4] == TrueTag) h53210->Fill(rnet); else h63210->Fill(rnet);
      }

      if(bpionS) {
        double ang = asin((Pt[ipionS])/(P[ipionS]));
        double deta= log(tan(BSpart.Theta()/2.))-log(tan(ang/2.));
        double dphi= fabs(phi[ipionS]-BSpart.Phi()); if(dphi>3.1415)dphi=6.283-dphi;
        NNinputs.at(2) = P[ipionS];
        NNinputs.at(3) = Pt[ipionS];
        NNinputs.at(4) = ip[ipionS]/iperr[ipionS];
        NNinputs.at(5) = deta;
        NNinputs.at(6) = dphi;
        NNinputs.at(7) = calc_dQ(BSpart, 
                                 build4V(P[ipionS],Pt[ipionS],phi[ipionS],211) );
        rnet = MLP_pionS( NNinputs );
        pn[4]= 1.0-pol(rnet, 1.147873, -1.241866 );//1-omega
        if(pn[4]<ProbMin_pion) { ipionS=0; itag[4]=0; pn[4]=0.50; }
        if(itag[4])
          if(itag[4] == TrueTag) h53210->Fill(rnet); else h63210->Fill(rnet);
      }

      if( WeigVch ) {   //---------
        double omega=0.50;
        if( fabs(WeigVch)<0.75 ) omega = pol(fabs(WeigVch), m_P0, m_P1);
        if( fabs(WeigVch)>0.75 ) omega = m_Gt075;
        if( fabs(WeigVch)>0.99 ) { // tracks have all the same charge
          if(vflagged==2) omega = wSameSign2;
          if(vflagged >2) omega = wSameSignMoreThan2;
          if(vflagged==2) {if(itag[5]==TrueTag) n2trackR++; else n2trackW++;}
          if(vflagged >2) {if(itag[5]==TrueTag) ntrackR++;  else ntrackW++;}
        }
        if(itag[5] == TrueTag) h540->Fill(fabs(WeigVch)); 
        else h640->Fill(fabs(WeigVch));

        if( 1-omega < ProbMinVtx || omega > ProbMinVtx ) {
          itag[5]=0;
          omega = 0.50;
        }

        pn[5] = 1.0 - omega;           //Vertex charge tagger
      }

      //make decision
      int tagdecision=0, catt=0;
      pnsum_a= 0.50;             //hypothesis of truetag=+1
      pnsum_b= 0.50;             //hypothesis of truetag=-1
      for( int i = 1; i != 6; i++ ) {
        //		if(  i == 4) {
        if( SAME || i != 4) {
          // se non voglio i same side tag salto i==4
          double mtag = itag[i];
          pnsum_a *= ((1-mtag)/2+mtag* pn[i] ); // p
          pnsum_b *= ((1+mtag)/2-mtag* pn[i] ); //(1-p)
          if(DBG) cout<< "i="<<i<<" itag="<<mtag
                      <<" pn="<<pn[i]<<endl;
        }		
      }      
      if(pnsum_a > pnsum_b) tagdecision = +1;
      if(pnsum_a < pnsum_b) tagdecision = -1;
      double pnsum = max(pnsum_a,pnsum_b) /(pnsum_a + pnsum_b);

      //throw away poorly significant tags
      if(pnsum < ProbMin) {
        pnsum = 0.50;
        tagdecision = 0;
      }

      if(tagdecision) {
        double var = BSpart.Pt();
        if( tagdecision==TrueTag ) hright->Fill(var); else hwrong->Fill(var);
      }

      //sort into categories
      double omega = fabs(1-pnsum); 
      if(      omega > omegamaxbin              ) catt=1;
      else if( omega > omegamaxbin-omegascale   ) catt=2;
      else if( omega > omegamaxbin-omegascale*2 ) catt=3;
      else if( omega > omegamaxbin-omegascale*3 ) catt=4;
      else                                        catt=5;
      if(tagdecision==0) catt=0;
      //       cout<< " omega " << omega<< " catt" << catt<< endl;


      //more histos:
      if(TrueTag== tagdecision) h100->Fill(pnsum); 
      if(TrueTag==-tagdecision) h102->Fill(pnsum); 
      if(TrueTag== tagdecision) h103->Fill(1-pnsum); 
      if(TrueTag==-tagdecision) h104->Fill(1-pnsum); 
      if(DBG) if(tagdecision) cout<<"pnsum="<<pnsum<<endl;
      //oscillation plots:
      if(itag[1])if(Btag*itag[1]>0) h2011->Fill(Btime); else h2012->Fill(Btime);
      if(itag[2])if(Btag*itag[2]>0) h2021->Fill(Btime); else h2022->Fill(Btime);
      if(itag[3])if(Btag*itag[3]>0) h2031->Fill(Btime); else h2032->Fill(Btime);
      if(itag[4])if(Btag*itag[4]>0) h2041->Fill(Btime); else h2042->Fill(Btime);
      if(itag[5])if(Btag*itag[5]>0) h2051->Fill(Btime); else h2052->Fill(Btime);
      int iosc = Btag*tagdecision;
      if(catt==1)if(iosc>0) h3011->Fill(Btime);else h3012->Fill(Btime);
      if(catt==2)if(iosc>0) h3021->Fill(Btime);else h3022->Fill(Btime);
      if(catt==3)if(iosc>0) h3031->Fill(Btime);else h3032->Fill(Btime);
      if(catt==4)if(iosc>0) h3041->Fill(Btime);else h3042->Fill(Btime);
      if(catt==5)if(iosc>0) h3051->Fill(Btime);else h3052->Fill(Btime);
      if(catt!=0)if(iosc>0) h3061->Fill(Btime);else h3062->Fill(Btime);
      
      //counting
      if(bmuon>0  && abs(MCID[imuon])==13)  nidm++; //pid purity
      if(bele>0   && abs(MCID[iele]) ==11)  nide++;
      if(bkaon>0  && abs(MCID[ikaon])==321) nidk++;
      if(bkaonS>0 && abs(MCID[ikaonS])==321)nidkS++;
      if(bmuon>0  && MCID[imuon]==0)  nghost_m++;  //ghosts
      if(bele >0  && MCID[iele] ==0)  nghost_e++;
      if(bkaon>0  && MCID[ikaon]==0)  nghost_k++;
      if(bkaonS>0 && MCID[ikaonS]==0) nghost_kS++;

      if(bmuon>0  && abs(MCID[imuon])== 13 && bFlag[imuon]==1) ntruemutag++;
      if(bele>0   && abs(MCID[iele]) == 11 && bFlag[iele] ==1) ntrueeltag++;
      if(bkaon>0  && abs(MCID[ikaon])==321 && bFlag[ikaon]==1) ntruekOtag++;
      if(bkaonS>0 && abs(MCID[ikaonS])==321
         &&bFlag[ikaonS]==0&& xFlag[ikaonS]!=0) ntruekStag++;

      //direct b->mu,e,K
      if(bmuon>0 && abs(MCID[imuon])== 13 
         && bFlag[imuon]==1 && mothID[imuon]==ancID[imuon]) ndirect_mu++;
      if(bele>0 && abs(MCID[iele])  == 11
         && bFlag[iele]==1 && mothID[iele]==ancID[iele]) ndirect_el++;
      //decay b -> c -> mu,e,K
      if(bmuon>0 && abs(MCID[imuon])== 13 
         && bFlag[imuon]==1 && isD(mothID[imuon])) nbc_mu++;
      if(bele>0 && abs(MCID[iele])  == 11
         && bFlag[iele]==1 && isD(mothID[iele])) nbc_el++;
      if(bkaon>0 && abs(MCID[ikaon]) == 321 && bFlag[ikaon] ==1 ) nbc_kO++;
      if(bkaonS>0&& abs(MCID[ikaonS])== 321 && xFlag[ikaonS]!=0 ) nbc_kS++;


      if(useDV) {
        tagdecision=Tag; catt=TagCat;
      } 

      //::::::::::::::::::::::::::::::::::::::::::::::::::::::::
      // Tagging results for signal/not signal, save to txt file
      if ( bkgCat == 0 || bkgCat == 50) {
  
        Int_t rightorwrongComb=0;
        std::vector<int> rightorwrong(6);

        Float_t truetag= TrueTag;
        // avendo precedentemente assegnato:
        // if( IsControlChannel ) TrueTag = sID[ii]>0? 1 : -1 ;//

        for (int it=1; it<6; it++) {
          rightorwrong[it] == 0;
          if( itag[it] !=0) rightorwrong[it] = truetag ==itag[it]? -1:+1;
        }
        if(tagdecision) rightorwrongComb= truetag==tagdecision ? -1:+1;
        if(rightorwrongComb==-1) h5000->Fill(omega);  // combined
        if(rightorwrongComb==1)  h5100->Fill(omega);  // combined
  
        if(rightorwrong[1]==-1) h5001->Fill(1-pn[1]);  // mu
        if(rightorwrong[2]==-1) h5002->Fill(1-pn[2]);  // ele
        if(rightorwrong[3]==-1) h5003->Fill(1-pn[3]);  //Ko
        if(rightorwrong[4]==-1) h5006->Fill(1-pn[4]);  //same
        if(rightorwrong[5]==-1) h5005->Fill(1-pn[5]);  //Vtx

        if(rightorwrong[1]==1) h5101->Fill(1-pn[1]);  // mu
        if(rightorwrong[2]==1) h5102->Fill(1-pn[2]);  // ele
        if(rightorwrong[3]==1) h5103->Fill(1-pn[3]);  //Ko
        if(rightorwrong[4]==1) h5106->Fill(1-pn[4]);  //same
        if(rightorwrong[5]==1) h5105->Fill(1-pn[5]);  //Vtx

        // printout of events for fits
        Real_t time  = tau;    // propertime after PV refit
        Real_t bmass = BSpart.M();

        //    if( pn[1]<0.5 ) cout<< pn[1] <<endl;
        afile<<bkgCat << "  "<<
          time <<"  "<<bmass <<"  "<<pn[1]<<"  "<<rightorwrong[1]<<
          "  "<<pn[2]<<"  "<<rightorwrong[2]<<
          "  "<<pn[3]<<"  "<<rightorwrong[3]<<
          "  "<<pn[5]<<"  "<<rightorwrong[5]<<
          "  "<<pn[4]<<"  "<<rightorwrong[4]<< 
          "  "<< omega <<"  "<<rightorwrongComb << endl;
      } // fine scrittura file
      //::::::::::::::::::::::::::::::::::::::::::::::::::::::::

      if(tagdecision) 
        if(tagdecision==TrueTag) nrt[catt]+=evtW; else nwt[catt]+=evtW;
      for(int k=1; k<6; ++k)
        if(itag[k]) { 
          if( itag[k] == TrueTag ) nrtag[k]+=evtW; else nwtag[k]+=evtW; 
        }

      if(DBG) {
        cout <<"--------------- Taggers Summary"<<endl;
        if(bmuon) cout <<"*mu  P="<<P[imuon]<<endl;
        if(bele)  cout <<"*ele P="<<P[iele]<<endl;
        if(bkaon) cout <<"*K   P="<<P[ikaon]<<endl;
        if(bkaonS)cout <<"*KS  P="<<P[ikaonS]<<endl;
        if(bpionS)cout <<"*pS  P="<<P[ipionS]<<endl;
        if(Tag!=tagdecision || TagCat!=catt) 
          cout << "TAG-Warning: official is " << Tag 
               <<" "<< TagCat<<std::setw(9)<< tagdecision<<" "<<catt<<endl;
        cout << "TAG  "<< Run 
             << " " << Event
          // 	     << std::setw(4) << trig
             << std::setw(5) << tagdecision
             << std::setw(3) << TrueTag
             << std::setw(3) << catt  
             << std::setw(5) << itag[1]
             << std::setw(3) << itag[2]
             << std::setw(3) << itag[3]
             << std::setw(3) << itag[4]
             << std::setw(3) << itag[5] << endl;
      }//debug

    }//end of loop on events

    cout<<"\nFraction in event sample of true particles from B:"<<endl;
    cout<<"mu= "<<float(ntruemu)/nsele*100
        << "%\t (sel. as tagger:"<<float(ntruemutag)/nsele*100<<"%)"<<endl;
    cout<<"el= "<<float(ntrueel)/nsele*100
        << "%\t (sel. as tagger:"<<float(ntrueeltag)/nsele*100<<"%)"<<endl;
    cout<<"kO= "<<float(ntruekO)/nsele*100
        << "%\t (sel. as tagger:"<<float(ntruekOtag)/nsele*100<<"%)"<<endl;
    cout<<"kS= "<<float(ntruekS)/nsele*100
        << "%\t (sel. as tagger:"<<float(ntruekStag)/nsele*100<<"%)"<<endl;
    cout<<"~~~~~~~~~ fractions in tagger sample of:\n";
    cout<<"b->mu= "<<int(float(ndirect_mu)/(nrtag[1]+nwtag[1])*100)<<"%\t";
    cout<<"b->c->mu= "<<int(float(nbc_mu)/(nrtag[1]+nwtag[1])*100)<<"%\n";
    cout<<"b->e = "<<int(float(ndirect_el)/(nrtag[2]+nwtag[2])*100)<<"%\t";
    cout<<"b->c->e = "<<int(float(nbc_el)/(nrtag[2]+nwtag[2])*100)<<"%\n";
    cout<<"b->Ko= "<<int(float(nbc_kO)/(nrtag[3]+nwtag[3])*100)<<"%\t";
    cout<<"b->Ks= "<<int(float(nbc_kS)/(nrtag[4]+nwtag[4])*100)<<"%\n";
    cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~";

    f->Close();
  }//end of files
 
  //*********Fill some event histos:
  calculateOmega (hright, hwrong, homega);
  calculateEffEff(nsele, hright, hwrong, heffec, direction);

  //*********Dump things:
  float tp=0; 
  if(NNetTrain=="") {
    tp = PrintPerformance();
  }
  if(useDV) cout<<"*** Warning: result from DaVinci is shown only!"<<endl;

  cout<<" Bs "<<nB531<<" " << (nB531/ntrig)*100 << endl;
  cout<<" B+ "<<nB521<<" " << (nB521/ntrig)*100 << endl;
  cout<<" B0 "<<nB511<<" " << (nB511/ntrig)*100 << endl;
  cout<<" other "<<nBother<<" " << (nBother/ntrig)*100 << endl;
  cout<< " ntrig "<<ntrig<< endl;


  //**************************************************** NNet for e,mu,K:
  if(NNetTrain=="e" || NNetTrain=="mu" || NNetTrain=="k" ) {
    cout<<"\nWill train NNet for tagger: "<<NNetTrain<<endl;
    Int_t ntrain = 200;
    TMultiLayerPerceptron *mlp = 
      new TMultiLayerPerceptron("mult,partP,partPt,IPPV:12:@iscorrect",
                                "1.",nntrain,"Entry$%2","(Entry$+1)%2");
    mlp->Train(ntrain, "text,graph,update=5");
    TCanvas* mlpa_canvas = new TCanvas("mlpa_canvas",
                                       "Network analysis",1200,0,300,600);
    mlpa_canvas->Divide(1,3);
    TMLPAnalyzer ana(mlp);
    ana.GatherInformations(); //Initialisation
    ana.CheckNetwork(); //output to the console
    mlpa_canvas->cd(1); mlp->Draw(); //shows the network structure
    mlpa_canvas->cd(2); ana.DrawDInputs(); //shows how works each variable 
    mlpa_canvas->cd(3);
    ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); //draws the result
    //write out code
    if(NNetTrain=="mu") mlp->Export("NNmuon");
    if(NNetTrain=="e")  mlp->Export("NNele");
    if(NNetTrain=="k")  mlp->Export("NNkaon");
  }
  //**************************************************** NNet for KS,piS:
  if(NNetTrain=="kS" || NNetTrain=="pS") {
    cout<<"\nWill train NNet for tagger: "<<NNetTrain<<endl;
    Int_t ntrain = 80;
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
    mlpa_canvas->cd(1); mlp->Draw(); //shows the network structure
    mlpa_canvas->cd(2); ana.DrawDInputs(); //shows how works each variable 
    mlpa_canvas->cd(3);
    ana.DrawNetwork(0,"iscorrect==1","iscorrect==0"); //draws the result
    //write out code
    if(NNetTrain == "kS") mlp->Export("NNkaonS");
    if(NNetTrain == "pS") mlp->Export("NNpionS");
  }
  if(NNetTrain=="vtx") {
    cout<<"\n\n\n***** Vertex charge omega:\n"
        <<"  for =2-tracks, omega= "<<1-float(n2trackR)/(n2trackR+n2trackW)<<"\n"
        <<"  for >2 tracks, omega= "<<1-float(ntrackR)/(ntrackR+ntrackW)<<"\n";
  }
  if(NNetTrain!="") {
    cout<<"\n* 1. Now quit, set NNetTrain=\"\" and rerun on the same data!"<<endl;
    cout<<"* 2. Then update the fitted values into pn[i]."<<endl;
    cout<<"* 3. Finally rerun and look at your combined performance\n"<<endl;
    return;
  }
  ///////////////////////////////////////////////////////////////////////
  fhisto.Write(); fhisto.Close();
  gROOT->cd(); gROOT->ProcessLine(".x tag.C");

  cout<<endl;
  return;
}////////// THE END

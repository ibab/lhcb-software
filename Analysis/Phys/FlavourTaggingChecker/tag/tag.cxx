#include "tag.h"

int main () {

#include "tag_histo.h" //define here histograms to fill.

  TApplication appKey("appKey",0,0);
  
  TaggerMuonTool            mutool ; //tagging tools to be used
  TaggerElectronTool        eletool;
  TaggerKaonOppositeTool    ktool  ;
  TaggerKaonSameTool        kStool ;
  TaggerPionSameTool        pStool ;
  TaggerVertexChargeTool    vtxtool;

  CombineTaggersPID         combPIDtool ;
  CombineTaggersNN          combNNtool;
  CombineTaggersProbability combProbtool;

  EventViewer viewer;
  NNTuner nnetune(NNetTrain);
  PerformanceMonitor perfmon, perfmonNN;

  /*
  //for B2JpsiK  
  std::ofstream afile("output_Data2011.ascii");
  //for B2JpsiK*
  std::ofstream Bdfile("output_BdData2011.txt");
  std::ofstream BdOSfile("output_BdOSData2011.txt");
  std::ofstream BdMuonfile("output_BdMuonData2011.txt");
  std::ofstream BdElectronfile("output_BdElectronData2011.txt");
  std::ofstream BdKaonfile("output_BdKaonData2011.txt");
  std::ofstream BdPionfile("output_BdPionData2011.txt");
  std::ofstream BdVtxfile("output_BdVtxData2011.txt");
  //for B2DstarMuNu  
  std::ofstream dstarfile("output_DstarData2011.ascii");
  */

  //FILE LOOP ////////////////////////////////////////////////////////////////
  for( std::vector<TString>::iterator ifile=filelist.begin(); 
       ifile!=filelist.end(); ++ifile ) {
    if(Nmax!=-1 && perfmon.N()>=Nmax) break;
    NtpTag tree(*ifile); //interface between ntuple and event class
    int nentries = tree.size();
        
    //EVENT LOOP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    for( int i=0; i<nentries; ++i ) {

      Event event; 
      if(! tree.execute(i, event) ) continue; //load event
      if(Nmax!=-1 && perfmon.N()>=Nmax) break;
      PrintAdvance( i, nentries, *ifile );

      //      if (i%2!=0) continue; //mod0            

      //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      // CUTS TO SELECT EVENT
      if(!isinTextFile(event.runNr(),event.eventNr())) continue;  //read file
      //      PrintAdvance( i, nentries, *ifile );
      if( requireL0   && event.L0()  ==0 )   continue;
      if( requireHlt1 && event.Hlt1()==0 )   continue;
      if( requireHlt2 && event.Hlt2()==0 )   continue;
      if( requireTis  && event.L0TisTos()&2) continue;
      //      if( event.backgroundCat() != 0) continue;  //for MC
      //if( event.pileup() == 1 )   continue;
      //if( event.multiplicity()<30) continue;
      //if( event.TrueTag()== -1 ) continue;
      //      if( event.Btau()>=2.2 ) continue; 
      //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      //if ( event.signalB()->pt()<5000 ) continue;

      debug()<<"============================ Process Event"<<endl;
      Particles parts = event.particles();//candidate particles

      Particle* BSpart = event.signalB();
      Particle* BOpart = event.oppositeB();
      //fatal()<<"BSID: "<<BSpart->ID()<<", BSMCID: "<<BSpart->MCID()<<endreq;
      //if (BSpart->ID()==-531) continue; //only B+/B-
      //if (BOpart->ID()<0) continue; //only B+/B- Bs

      //Individual tagger's response:
      Tagger* tmuon = mutool .tag(event);
      Tagger* tele  = eletool.tag(event);
      Tagger* tkaon = ktool  .tag(event);
      Tagger* tsame = event.isBs()? kStool.tag(event):pStool.tag(event);
      //Tagger* tsame = new Tagger;   //Combination without ss tagger     
      Tagger* tvtx  = vtxtool.tag(event);
   
      //Combination of tagger's decisions:
      Taggers taggers, OStaggers; 
      taggers.push_back(tmuon); 
      taggers.push_back(tele); 
      taggers.push_back(tkaon); 
      taggers.push_back(tsame); 
      taggers.push_back(tvtx); 
      OStaggers.push_back(tmuon); 
      OStaggers.push_back(tele); 
      OStaggers.push_back(tkaon); 
      OStaggers.push_back(tvtx); 
      //Tagger combination technique
      FlavourTag* theTag = combProbtool.combineTaggers( taggers );
      //FlavourTag* theTag = combNNtool.combineTaggers( taggers );
      //FlavourTag* theTag = combPIDtool.combineTaggers( taggers );
      debug()<<"omega: "<<theTag->omega()<<endreq;      
      debug()<<"omegaOS: "<<theTag->omegaOS()<<endreq;      

      int TrueTag = event.TrueTag();
      theTag->setTrueTag(TrueTag);

      nnetune.Fill(event, theTag);//fill info for retuning, set by NNetTrain
      perfmon.increment(event, theTag);//update counters for this event
 

      //************************************************************************
      //************************************************************************
      //  FROM THIS POINT JUST HISTOGRAM FILLING!                              *
      //************************************************************************
      //************************************************************************

      //shortcuts
      double     Btime = event.BProperTime(); //in ps
      bool        isBs = event.isBs();
      bool       isTis = event.L0TisTos()&2;
      bool       isTos = event.L0TisTos()&1;
      int     category = theTag->category();
      int  tagdecision = theTag->decision();
      double     omega = theTag->omega();
      int  tagdecisionOS = theTag->decisionOS();
      double     omegaOS = theTag->omegaOS();
      int       nparts = parts.size();
      int         krec = event.pileup();
      //get the particle used by tagger:
      Particle*  imuon = tmuon->decision()? tmuon->taggerparts().at(0) : NULL;
      Particle*   iele = tele ->decision()? tele ->taggerparts().at(0) : NULL;
      Particle*  ikaon = tkaon->decision()? tkaon->taggerparts().at(0) : NULL;
      Particle*  isame = tsame->decision()? tsame->taggerparts().at(0) : NULL;

      //store first events to visualize
      if(tagdecision && i<10) viewer.storeClone( event );

      Int_t sign=TrueTag;
      
      //**********************//
      //******Bd2DstarMu******//
      //**********************//
      if (UseModBtime>0) {
	//for DstarMuNu channel
	Particle* Dstar = event.Dstar();
	Particle* D0    = event.D0();
	Particle* Mu    = event.Mu();
        TLorentzVector DstarMu = Dstar->momentum() + Mu->momentum();
        double ChiP = DstarMu.P();
        double ChiM = DstarMu.M();
        double newChiP = 0;
        //Marc
        if (UseModBtime==1) newChiP = ChiP/(0.283+ChiM*0.1283);
        //Stefania
        if (UseModBtime==2){
          //double kFactor = (0.9947-0.197*event.Bmass()+0.0375*pow(event.Bmass(),2));
          double kFactor = (0.9947-0.197*ChiM+0.0375*pow(ChiM,2));
          double BPcor = ChiP/kFactor;
          double BinvPcor = 1./BPcor;
          double kBias = 0.067416 - 0.000564778*BPcor/1000 + 9.55013e-07*pow(BPcor/1000.,2);
          BinvPcor =  (1 - kBias)*BinvPcor;
          newChiP = 1./BinvPcor;
        }
        //Sara
        if (UseModBtime==3){
	  double kFactor= (0.9654-0.1685*ChiM+0.0327*pow(ChiM,2));//sara 15 giugno
          double BPcor = ChiP/kFactor;
          double BinvPcor = 1./BPcor;
	  double kBias = 0.07611 - 0.0008899*BPcor + 4.2e-06*pow(BPcor,2)- 1.007e-08*pow(BPcor,3)+7.6e-12*pow(BPcor,4);//sara 17 giugno
          BinvPcor =  (1 - kBias)*BinvPcor;
          newChiP = 1./BinvPcor;
	}
        TVector3 BSvtx  = event.BSVertex();
        TVector3 Recvtx = event.RecVertex();
        double Bdir = (BSvtx-Recvtx).Mag();
        double MassB  = 5.279; 
        double NewBtime = Bdir*MassB/newChiP/0.299792458;
        Btime = NewBtime;
	//***
	/*
	//write ascii file for B2DstarMuNu
	double DeltaM  = Dstar->M()-D0->M();
	//one tagger
	//Real_t mix = -(tvtx->decision()*Mu->ID())/13;
	// combined
	Real_t mix = -(theTag->decision()*Mu->ID())/13;
	dstarfile<<Btime<<"  "<<DeltaM*1000<<"    "<<mix<<" "<<(D0->M())*1000 << endl;
	*/
      }

      //*******************************//
      //write ascii files
      //*******************************//
      //***
      /*
      //write ascii files for B2JpsiK
      afile<<(BSpart->M())*1000<<" "<<Btime<<" "<<0.02<<" "<<
        -(tagdecision*sign)<<" "<<omega<<" "<<
        -(tagdecisionOS*sign)<<" "<<omegaOS<<" "<<
        -tmuon->decision()*sign<<" "<<tmuon->omega()<<" "<<
        -tele->decision()*sign<<" "<<tele->omega()<<" "<<
        -tkaon->decision()*sign<<" "<<tkaon->omega()<<" "<<
        -tsame->decision()*sign<<" "<<tsame->omega()<<" "<<
        -tvtx->decision()*sign<<" "<<tvtx->omega()<<" "<<endl;
      //  sign<<" "<<int(my_nPV)<<" "<<my_nTracks<<" "<<int(my_Trig)<<" "<<endl;
      //***
      // write ascii files for BdJpsiK*
      double Bid=BSpart->ID();
      double mixState = 0, mixStateOS = 0; 
      double tmuonmixState = 0, telemixState = 0, tkaonmixState = 0, tsamemixState = 0, tvtxmixState = 0;
      if((tagdecision== +1 && Bid == 511) || (tagdecision== -1 && Bid == -511 ))
        mixState = 1;
      if((tagdecision== +1 && Bid == -511) || (tagdecision== -1 && Bid == 511 ))
        mixState = -1;
      if((tagdecisionOS== +1 && Bid == 511) || (tagdecisionOS== -1 && Bid == -511 ))
        mixStateOS = 1;
      if((tagdecisionOS== +1 && Bid == -511) || (tagdecisionOS== -1 && Bid == 511 ))
        mixStateOS = -1;
      if((tmuon->decision()== +1 && Bid == 511) || (tmuon->decision()== -1 && Bid == -511 ))
        tmuonmixState = 1;
      if((tmuon->decision()== +1 && Bid == -511) || (tmuon->decision()== -1 && Bid == 511 ))
        tmuonmixState = -1;
      if((tele->decision()== +1 && Bid == 511) || (tele->decision()== -1 && Bid == -511 ))
        telemixState = 1;
      if((tele->decision()== +1 && Bid == -511) || (tele->decision()== -1 && Bid == 511 ))
        telemixState = -1;
      if((tkaon->decision()== +1 && Bid == 511) || (tkaon->decision()== -1 && Bid == -511 ))
        tkaonmixState = 1;
      if((tkaon->decision()== +1 && Bid == -511) || (tkaon->decision()== -1 && Bid == 511 ))
        tkaonmixState = -1;
      if((tsame->decision()== +1 && Bid == 511) || (tsame->decision()== -1 && Bid == -511 ))
        tsamemixState = 1;
      if((tsame->decision()== +1 && Bid == -511) || (tsame->decision()== -1 && Bid == 511 ))
        tsamemixState = -1;
      if((tvtx->decision()== +1 && Bid == 511) || (tvtx->decision()== -1 && Bid == -511 ))
        tvtxmixState = 1;
      if((tvtx->decision()== +1 && Bid == -511) || (tvtx->decision()== -1 && Bid == 511 ))
        tvtxmixState = -1;
      double     Btestmass = BSpart->M()*1000.; //in ps
      Bdfile<<Btestmass<<" "<<Btime<<" "<< tagdecision<<" "<<mixState <<" "<<category <<" "<<omega <<endl;
      BdOSfile<<Btestmass<<" "<<Btime<<" "<< tagdecisionOS<<" "<<mixStateOS <<endl;
      BdMuonfile<<Btestmass<<" "<<Btime<<" "<<tmuon->decision()<<" "<<tmuonmixState <<" "<<endl;
      BdElectronfile<<Btestmass<<" "<<Btime<<" "<<tele->decision()<<" "<<telemixState <<" "<< endl;
      BdKaonfile<<Btestmass<<" "<<Btime<<" "<<tkaon->decision()<<" "<<tkaonmixState <<" "<< endl;
      BdPionfile<<Btestmass<<" "<<Btime<<" "<<tsame->decision()<<" "<<tsamemixState <<" "<< endl;
      BdVtxfile<<Btestmass<<" "<<Btime<<" "<<tvtx->decision()<<" "<<tvtxmixState <<" "<< endl;
      */
    
      //plots omega
      if(theTag->decision()) homega->Fill(theTag->omega());
      //if(tvtx->decision())
	//homega->Fill(tvtx->omega());
	//homega->Fill(tele->omega());
	//homega->Fill(tmuon->omega());
	//homega->Fill(tkaon->omega());
      //if(tsame->decision()) homega->Fill(tsame->omega());
      Int_t FlTag = BOpart->ID()/511;
      if((theTag->decision()*FlTag)>0) homega2->Fill(theTag->omega());
      //if((tvtx->decision()*FlTag)>0)	
	//homega2->Fill(tvtx->omega());
	//homega2->Fill(tele->omega());
	//homega2->Fill(tmuon->omega());
	//homega2->Fill(tkaon->omega());
      //if((tsame->decision()*FlTag)>0) homega2->Fill(tsame->omega());

      //****************************************
      //event histos
      h1->Fill(BSpart->M());//do not remove this.
      h2->Fill(krec);
      h3->Fill(BOpart->pt());
      h4->Fill(event.multiplicity());

      //**********************//
      //******SV studies******//
      //**********************//
      Vertices secvtx = event.getSecondaryVertices();
      Double_t   sz=0;
      bool containskaon=false; 
      if( !secvtx.empty()) {
        int nsecpart = secvtx.at(0)->outgoingParticles().size();
        sz = secvtx.at(0)->position().z();
        TVector3 BoppDir = secvtx.at(0)->position();
        Particles secparts = secvtx.at(0)->outgoingParticles();
        TVector3 BoppMom;
        TLorentzVector SVmomentum;
        //look for kaons or leptons in SV
        int kaoncounter = 0;
        int leptoncounter = 0;
        int tagpartcounter = 0;
        for (int ip=0; ip<nsecpart; ip++) {
          BoppMom += secparts.at(ip)->momentum().Vect();
          SVmomentum += secparts.at(ip)->momentum();
          if (secparts.at(ip)->absID()==321) { containskaon = true; kaoncounter+=1; }
          if (secparts.at(ip)->absID()==13 || secparts.at(ip)->absID()==11) leptoncounter+=1;
          if (secparts.at(ip)->absID()==321 || secparts.at(ip)->absID()==13 || secparts.at(ip)->absID()==11) tagpartcounter+=1;
        }
        hsv100->Fill(kaoncounter);
        hsv101->Fill(leptoncounter);
        hsv102->Fill(tagpartcounter);
        //pointing variables
        double BDphiDir = BoppDir.DeltaPhi(SVmomentum.Vect());
        double BDTheDir = BoppDir.Theta()-SVmomentum.Vect().Theta();	
        hsv200->Fill(BDphiDir);
        hsv201->Fill(BDTheDir);
        //reconstruction of BO tau
        double SVP = SVmomentum.P();
        double SVM = SVmomentum.M();
        double SVGP = SVP/(0.16*SVM+0.12);
        double SVtau = BoppDir.Mag()*5.28/SVGP/0.299792458;
        hsv300->Fill(SVtau);
        //Check MC SV
        double BMCP = BOpart->p();
        double BMCM = BOpart->M();
        TVector3 truePV = event.MCVertex();
        double BMCsz = BOpart->endVertexPosition().z()-event.MCVertex().z();
        double BMCtau = (BOpart->endVertexPosition()-truePV).Mag()*5.28/BMCP/0.299792458;
        double BMCDphiDir = BoppDir.DeltaPhi(BOpart->momentum().Vect());
        double BMCDTheDir = BoppDir.Theta()-BOpart->momentum().Vect().Theta();
        double BMCDphiVec = BoppMom.DeltaPhi(BOpart->momentum().Vect());
        double distfromtrue = ((BOpart->endVertexPosition() - truePV)- secvtx.at(0)->position() ).Mag();
        hsv9->Fill(distfromtrue);
        //resolution
        hsv301->Fill(BMCP-SVGP);
        hsv302->Fill(BMCtau-SVtau);
        hsv303->Fill(BMCsz-sz);
        if(distfromtrue<1.0) { //consider a good SV if SV closer to BO in less than 1 mm
          //pointing
          hsv200_g->Fill(BMCDphiDir);
          hsv201_g->Fill(BMCDTheDir);
          hsv202_g->Fill(BMCDphiVec);
          //for tau
          hsv301_g->Fill(BMCP-SVGP);
          hsv302_g->Fill(BMCtau-SVtau);
          hsv303_g->Fill(BMCsz-sz);
          hsv310_g->Fill(SVP, BMCP);
          hsv311_g->Fill(SVP/BMCP);
          hsv312_g->Fill(SVM, SVP/BMCP);
          hsv313_g->Fill(SVM/BMCM);
        } else {
          //pointing
          hsv200_b->Fill(BMCDphiDir);
          hsv201_b->Fill(BMCDTheDir);
          hsv202_b->Fill(BMCDphiVec);
          //for tau
          hsv310_b->Fill(SVP, BMCP);
          hsv311_b->Fill(SVP/BMCP);
          hsv312_b->Fill(SVM, SVP/BMCP);
          hsv313_b->Fill(SVM/BMCM);
        }//distfromtrue
        if (tvtx->decision()){
          if(tvtx->decision()==TrueTag) {
            hsv100_r->Fill(kaoncounter);
            hsv101_r->Fill(leptoncounter);
            hsv102_r->Fill(tagpartcounter);
            hsv200_r->Fill(BDphiDir);
            hsv201_r->Fill(BDTheDir);
            hsv300_r->Fill(SVtau);
            if (!containskaon) hsv320_r->Fill(SVtau);
            if (fabs(BOpart->ID())==531) hsv321_r->Fill(SVtau);
            if (fabs(BOpart->ID())==511) hsv322_r->Fill(SVtau);
            if (fabs(BOpart->ID())==521) hsv323_r->Fill(SVtau);
          } else {
            hsv100_w->Fill(kaoncounter);
            hsv101_w->Fill(leptoncounter);
            hsv102_w->Fill(tagpartcounter);
            hsv200_w->Fill(BDphiDir);
            hsv201_w->Fill(BDTheDir);
            hsv300_w->Fill(SVtau);
            if (!containskaon) hsv320_w->Fill(SVtau);
            if (fabs(BOpart->ID())==531) hsv321_w->Fill(SVtau);
            if (fabs(BOpart->ID())==511) hsv322_w->Fill(SVtau);
            if (fabs(BOpart->ID())==521) hsv323_w->Fill(SVtau);
          }//truetag
        }
        //** SV vs PU **//
        if (krec>=1 && tvtx->decision()){
          //PileUp
          double zposPU1 = secparts.at(0)->zpos();
          double zposPU2 = secparts.at(0)->zpos2();
          double zposPU3 = secparts.at(0)->zpos3();
          int PUcounter=0;
          if (zposPU1!=100000) PUcounter+=1;
          if (zposPU2!=100000) PUcounter+=1;
          if (zposPU3!=100000) PUcounter+=1;
          hsv500->Fill(PUcounter);
          double tracksPU1 = secparts.at(0)->tracks();
          double tracksPU2 = secparts.at(0)->tracks2();
          double tracksPU3 = secparts.at(0)->tracks3();
          double totaltracks = tracksPU1+tracksPU2+tracksPU3;
          if (totaltracks == 0) totaltracks = -1;
          hsv501->Fill(tracksPU1); hsv501->Fill(tracksPU2); hsv501->Fill(tracksPU2);
          hsv502->Fill(zposPU1); hsv502->Fill(zposPU2); hsv502->Fill(zposPU3);
          double PUMag = sqrt(zposPU1*zposPU1+secparts.at(0)->xpos()*secparts.at(0)->xpos()+secparts.at(0)->ypos()*secparts.at(0)->ypos());
          hsv522->Fill(PUMag);
          if (zposPU1!=100000) hsv503->Fill(tracksPU1/totaltracks);//PU according to activity
          if (zposPU2!=100000) hsv503->Fill(tracksPU2/totaltracks);
          if (zposPU3!=100000) hsv503->Fill(tracksPU3/totaltracks);
          //distance SV versus PV and PU
          TVector3 BoppDirnocor = BoppDir+event.RecVertex();//SV z pos wrt (0,0)
          hsv401->Fill(BoppDir.z());//SV z pos wrt PV
          hsv421->Fill(BoppDir.Mag());//SV mag wrt PV
          TVector3 SVUnit = SVmomentum.Vect().Unit();
          TVector3 PUpos;
          PUpos.SetX(secparts.at(0)->xpos());
          PUpos.SetY(secparts.at(0)->ypos());
          PUpos.SetZ(secparts.at(0)->zpos());
          TVector3 k;//distance SV - PU
          k.SetX(BoppDirnocor.x()-PUpos.x());
          k.SetY(BoppDirnocor.y()-PUpos.y());
          k.SetZ(BoppDirnocor.z()-PUpos.z());
          double cte = SVUnit.Dot(k)/k.Mag();
          double distance = k.Mag() * sqrt(1-cte*cte);//calculate distance SVmomentum and PU
          hsv405->Fill(distance);
          double difzposSVPU = BoppDirnocor.z()-zposPU1;//only first PU
          double difSVPU = (BoppDirnocor-PUpos).Mag();
          hsv402->Fill(difzposSVPU);
          hsv422->Fill(difSVPU);
          double PUPV = fabs(difzposSVPU)-fabs(BoppDir.z());//SV closer to PV or to PU
          double PUPVmag = fabs(difSVPU)-fabs(BoppDir.Mag());
          hsv403->Fill(PUPV);
          hsv423->Fill(PUPVmag);
          if(tvtx->decision()==TrueTag) {
            hsv401_r->Fill(BoppDir.z());
            hsv402_r->Fill(difzposSVPU);
            hsv403_r->Fill(PUPV);
            hsv404_r->Fill(BoppDir.x(),BoppDir.y());//xy plane for SV
            hsv405_r->Fill(distance);
            hsv500_r->Fill(PUcounter);
            hsv501_r->Fill(tracksPU1); hsv501_r->Fill(tracksPU2); hsv501_r->Fill(tracksPU3);
            hsv502_r->Fill(zposPU1); hsv502_r->Fill(zposPU2); hsv502_r->Fill(zposPU3);
            if (zposPU1!=100000) hsv503_r->Fill(tracksPU1/totaltracks);
            if (zposPU2!=100000) hsv503_r->Fill(tracksPU2/totaltracks);
            if (zposPU3!=100000) hsv503_r->Fill(tracksPU3/totaltracks);
          } else {
            hsv401_w->Fill(BoppDir.z());
            hsv402_w->Fill(difzposSVPU);
            hsv403_w->Fill(PUPV);
            hsv404_w->Fill(BoppDir.x(),BoppDir.y());//xy plane for SV
            hsv405_w->Fill(distance);
            hsv500_w->Fill(PUcounter);
            hsv501_w->Fill(tracksPU1); hsv501_w->Fill(tracksPU2); hsv501_w->Fill(tracksPU3);
            hsv502_w->Fill(zposPU1); hsv502_w->Fill(zposPU2); hsv502_w->Fill(zposPU3);
            if (zposPU1!=100000) hsv503_w->Fill(tracksPU1/totaltracks);
            if (zposPU2!=100000) hsv503_w->Fill(tracksPU2/totaltracks);
            if (zposPU3!=100000) hsv503_w->Fill(tracksPU3/totaltracks);
          }//truetag
        }//krec>=1
      }//sec not empty
      //**********************//
      //**********************//

      //***********************//
      //**Multiple PU studies**//
      Particle*  iparticle = ikaon;
      Tagger* tparticle = tkaon;
      if ((iparticle) and (krec>1)) {
        double gippu=sqrt(iparticle->nippuerr());
        double ipmeandif= (iparticle->IP()-iparticle->ipmean());
        double ipdif=(iparticle->IP()-iparticle->nippu());
        double zposdif=(iparticle->trackzfirst()-iparticle->zpos());
        double zposabsdif=fabs(iparticle->trackzfirst()-iparticle->zpos());
        if (tparticle->decision()==TrueTag) {
          //if (iparticle->fromB()==1) {
          hpv_gippu_r->Fill(gippu);
          hpv_difip_r->Fill(ipdif);
          hpv_ipmeandif_r->Fill(ipmeandif);
          hpv_zposdif_r->Fill(zposdif);
          hpv_zposabsdif_r->Fill(zposabsdif);
        } else {
          hpv_gippu_w->Fill(gippu);
          hpv_difip_w->Fill(ipdif);
          hpv_ipmeandif_w->Fill(ipmeandif);
          hpv_zposdif_w->Fill(zposdif);
          hpv_zposabsdif_w->Fill(zposabsdif);
        }
      }
      //**********************//
      //**********************//

      //plots
      double var = BSpart->pt();
      h6->Fill(var);
      if(tagdecision && tagdecision==TrueTag) h6r->Fill(var); else h6w->Fill(var);
      if(isame){
        if(tsame->decision()==TrueTag) h6rSS->Fill(var); else h6wSS->Fill(var);
      }
      if(tagdecision && isTis){
        if(tagdecision==TrueTag) h6rtis->Fill(var); else h6wtis->Fill(var);
      }
      if(tagdecision && isTos){
        if(tagdecision==TrueTag) h6rtos->Fill(var); else h6wtos->Fill(var);
      }
      if(tagdecision && isTis && isTos){
        if(tagdecision==TrueTag) h6rtistos->Fill(var); else h6wtistos->Fill(var);
      }

      //tagger histos
      if(iele) {
        h1010->Fill( iele->PIDe() );
        if( iele->absMCID() == 11 ) h1000->Fill(iele->PIDe());
        if( iele->MCID() == 0  )    h1020->Fill(iele->PIDe());
      }
      if(imuon) {
        h1011->Fill(imuon->PIDm());
        if( imuon->absMCID()== 13 ) h1001->Fill(imuon->PIDm());
        if( imuon->MCID()== 0  )    h1021->Fill(imuon->MCID());
      }
      if(ikaon) {
        h1012->Fill(ikaon->PIDk()); h1015->Fill(ikaon->PIDk()-ikaon->PIDp());
        if( ikaon->absMCID()== 321) h1002->Fill(ikaon->PIDk());
        if( ikaon->absMCID()== 321) h1005->Fill(ikaon->PIDk()-ikaon->PIDp());
        if( ikaon->MCID()== 0)      h1022->Fill(ikaon->PIDk());
        if( ikaon->MCID()== 0)      h1025->Fill(ikaon->PIDk()-ikaon->PIDp());

        bool okk = (tkaon->decision() == TrueTag);
        bool k3  = (krec==3);
        bool k1  = (krec==1);
        h70->Fill(ikaon->pt());if(okk&&k1)   h80->Fill(ikaon->pt());
        if(okk&&k3) h80_3->Fill(ikaon->pt()); 
        h71->Fill(ikaon->p()); if(okk&&k1)   h81->Fill(ikaon->p()); 
        if(okk&&k3) h81_3->Fill(ikaon->p()); h72->Fill(ikaon->IPs()); 
        if(okk&&k1) h82->Fill(ikaon->IPs());
        if(okk&&k3) h82_3->Fill(ikaon->IPs());
        h73->Fill(ikaon->LCS());  if(okk&&k1) h83->Fill(ikaon->LCS()); 
        if(okk&&k3) h83_3->Fill(ikaon->LCS()); 
        h74->Fill(ikaon->likelihood());if(okk&&k1) h84->Fill(ikaon->likelihood());
        if(okk&&k3) h84_3->Fill(ikaon->likelihood()); 
        h75->Fill(ikaon->PIDk());  if(okk&&k1) h85->Fill(ikaon->PIDk()); 
        if(okk&&k3) h85_3->Fill(ikaon->PIDk()); 
        h76->Fill(ikaon->PIDk()-ikaon->PIDp());  
        if(okk&&k1) h86->Fill(ikaon->PIDk()-ikaon->PIDp()); 
        if(okk&&k3) h86_3->Fill(ikaon->PIDk()-ikaon->PIDp()); 
        h77->Fill(ikaon->distPhi());  
        if(okk&&k1) h87->Fill(ikaon->distPhi()); 
        if(okk&&k3) h87_3->Fill(ikaon->distPhi()); 
      }
      if(isBs && isame) {
        h1013->Fill(isame->PIDk());  h1014->Fill(isame->PIDk()-isame->PIDp());
        if( isame->absMCID()== 321 ) 
        { h1003->Fill(isame->PIDk()); h1004->Fill(isame->PIDk()-isame->PIDp()); }
        if( isame->absMCID()== 0   ) 
        { h1023->Fill(isame->PIDk()); h1024->Fill(isame->PIDk()-isame->PIDp()); }
      }
      
      /////////////////////////
      if(imuon) { 
	if (tagdecisionOS==TrueTag) hr_mu_probComb->Fill(1-tmuon->omega());
	if (tagdecisionOS==-TrueTag) hw_mu_probComb->Fill(1-tmuon->omega());
        if(tmuon->decision()==TrueTag) { 
          hr_mu_p->Fill(imuon->p()); 
          hr_mu_pt->Fill(imuon->pt()); 
          hr_mu_ips->Fill(imuon->IPs()); 
          hr_mu_ipssign->Fill(imuon->IPssign()); 
          if(krec>1) hr_mu_ippu->Fill(imuon->IPPU());
          hr_mu_lcs->Fill(imuon->LCS()); 
          hr_mu_pid->Fill(imuon->PIDm()); 
          hr_mu_tsal->Fill(imuon->likelihood()); 
          hr_mu_mult->Fill(nparts);         
          homphi_mu_r->Fill(imuon->distPhi());
          hr_mu_theta->Fill(acos(imuon->pt()/imuon->p())); 
          hr_mu_prob->Fill(1-tmuon->omega());
        } else {
          hw_mu_p->Fill(imuon->p()); 
          hw_mu_pt->Fill(imuon->pt());
          hw_mu_ips->Fill(imuon->IPs()); 
          hw_mu_ipssign->Fill(imuon->IPssign()); 
          if(krec>1) hw_mu_ippu->Fill(imuon->IPPU());
          hw_mu_lcs->Fill(imuon->LCS()); 
          hw_mu_pid->Fill(imuon->PIDm()); 
          hw_mu_tsal->Fill(imuon->likelihood()); 
          hw_mu_mult->Fill(nparts); 
          homphi_mu_w->Fill(imuon->distPhi());
          hw_mu_theta->Fill(acos(imuon->pt()/imuon->p())); 
          hw_mu_prob->Fill(1-tmuon->omega());
         }
	if(iele)  eta_corr[0][1]->Fill(tmuon->omega(),tele->omega());        
        if(ikaon) eta_corr[0][2]->Fill(tmuon->omega(),tkaon->omega());        
        if(tvtx) eta_corr[0][3]->Fill(tmuon->omega(),tvtx->omega());        
        if(isame) eta_corr[0][4]->Fill(tmuon->omega(),tsame->omega());        
      }
      if(iele) { 
	if (tagdecisionOS==TrueTag) hr_ele_probComb->Fill(1-tele->omega());
	if (tagdecisionOS==-TrueTag) hw_ele_probComb->Fill(1-tele->omega());
        if(tele->decision()==TrueTag) { 
          hr_ele_p->Fill(iele->p()); 
          hr_ele_pt->Fill(iele->pt()); 
          hr_ele_ips->Fill(iele->IPs()); 
          if (iele->IPssign()>0) hr_ele_ipssign->Fill(iele->IPssign()); else hr_ele_ipssign1->Fill(iele->IPssign()); 
          if(krec>1) hr_ele_ippu->Fill(iele->IPPU());
          hr_ele_lcs->Fill(iele->LCS()); 
          hr_ele_pid->Fill(iele->PIDe()); 
          hr_ele_tsal->Fill(iele->likelihood()); 
          hr_ele_mult->Fill(nparts); 
          hr_ele_veloch->Fill(iele->VeloCharge()); 
          hr_ele_eOverP->Fill(iele->eOverP()); 
          homphi_ele_r->Fill(iele->distPhi());
          hr_ele_theta->Fill(acos(iele->pt()/iele->p())); 
          hr_ele_prob->Fill(1-tele->omega());
        } else {
          hw_ele_p->Fill(iele->p()); 
          hw_ele_pt->Fill(iele->pt());
          hw_ele_ips->Fill(iele->IPs()); 
          if (iele->IPssign()>0) hw_ele_ipssign->Fill(iele->IPssign()); else hw_ele_ipssign1->Fill(iele->IPssign()); 
          if(krec>1) hw_ele_ippu->Fill(iele->IPPU());
          hw_ele_lcs->Fill(iele->LCS()); 
          hw_ele_pid->Fill(iele->PIDe()); 
          hw_ele_tsal->Fill(iele->likelihood()); 
          hw_ele_mult->Fill(nparts); 
          hw_ele_veloch->Fill(iele->VeloCharge()); 
          hw_ele_eOverP->Fill(iele->eOverP()); 
          homphi_ele_w->Fill(iele->distPhi());
          hw_ele_theta->Fill(acos(iele->pt()/iele->p())); 
          hw_ele_prob->Fill(1-tele->omega());
        }
	if(ikaon) eta_corr[1][2]->Fill(tele->omega(),tkaon->omega());        
        if(tvtx) eta_corr[1][3]->Fill(tele->omega(),tvtx->omega());        
        if(isame) eta_corr[1][4]->Fill(tele->omega(),tsame->omega());        
      }
      if(ikaon) { 
	if (tagdecisionOS==TrueTag) hr_k_probComb->Fill(1-tkaon->omega());
	if (tagdecisionOS==-TrueTag) hw_k_probComb->Fill(1-tkaon->omega());
        if(tkaon->decision()==TrueTag) { 
          hr_k_p->Fill(ikaon->p()); 
          hr_k_pt->Fill(ikaon->pt()); 
          hr_k_ips->Fill(ikaon->IPs()); 
          hr_k_ip->Fill(ikaon->IP()); 
          hr_k_ipssign->Fill(ikaon->IPssign()); 
          if (ikaon->IPssign()>0) hr_k_ipssign->Fill(ikaon->IPssign()); else hr_k_ipssign1->Fill(ikaon->IPssign());        
          if(krec>1) hr_k_ippu->Fill(ikaon->IPPU());
          hr_k_lcs->Fill(ikaon->LCS()); 
          hr_k_pid->Fill(ikaon->PIDk()); 
          hr_k_pidkp->Fill(ikaon->PIDk()-ikaon->PIDp()); 
          hr_k_tsal->Fill(ikaon->likelihood()); 
          hr_k_mult->Fill(nparts); 
          homphi_kO_r->Fill(ikaon->distPhi());
          hr_k_theta->Fill(acos(ikaon->pt()/ikaon->p()));        
          hr_k_prob->Fill(1-tkaon->omega());
        } else {
          hw_k_p->Fill(ikaon->p()); 
          hw_k_pt->Fill(ikaon->pt());
          hw_k_ips->Fill(ikaon->IPs());
          hw_k_ip->Fill(ikaon->IP()); 
          hw_k_ipssign->Fill(ikaon->IPssign()); 
          if (ikaon->IPssign()>0) hw_k_ipssign->Fill(ikaon->IPssign()); else hw_k_ipssign1->Fill(ikaon->IPssign());
          if(krec>1) hw_k_ippu->Fill(ikaon->IPPU());
          hw_k_lcs->Fill(ikaon->LCS()); 
          hw_k_pid->Fill(ikaon->PIDk()); 
          hw_k_pidkp->Fill(ikaon->PIDk()-ikaon->PIDp()); 
          hw_k_tsal->Fill(ikaon->likelihood()); 
          hw_k_mult->Fill(nparts); 
          homphi_kO_w->Fill(ikaon->distPhi());
          hw_k_theta->Fill(acos(ikaon->pt()/ikaon->p()));        
          hw_k_prob->Fill(1-tkaon->omega());
        }
	if(tvtx) eta_corr[2][3]->Fill(tkaon->omega(),tvtx->omega());        
        if(isame) eta_corr[2][4]->Fill(tkaon->omega(),tsame->omega());        
       }
      if(isame) { 
	if (tagdecisionOS==TrueTag) hr_kS_probComb->Fill(1-tsame->omega());
	if (tagdecisionOS==-TrueTag) hw_kS_probComb->Fill(1-tsame->omega());
        TLorentzVector ptotB = BSpart->momentum();
        double B0the= ptotB.Theta();
        double B0phi= ptotB.Phi();
        double ang = asin((isame->pt())/(isame->p()));
        double deta= log(tan(B0the/2.))-log(tan(ang/2.));
        double dphi= std::min(fabs(isame->momentum().Phi()-B0phi),
                              6.283-fabs(isame->momentum().Phi()-B0phi));
        double dR = sqrt(deta*deta+dphi*dphi);
        double B0mass = ptotB.M();
        double dQ  = ((ptotB + isame->momentum() ).M() - B0mass);
        if(tsame->decision()==TrueTag) { 
          hr_kS_p->Fill(isame->p()); 
          hr_kS_pt->Fill(isame->pt()); 
          hr_kS_ips->Fill(isame->IPs()); 
          hr_kS_ipssign->Fill(isame->IPssign());        
          hr_kS_ipsign->Fill(isame->IPsign());        
          if (isame->IPssign()>0) hr_kS_ipssign->Fill(isame->IPssign()); else hr_kS_ipssign1->Fill(isame->IPssign());
          if(krec>1) hr_kS_ippu->Fill(isame->IPPU());
          hr_kS_lcs->Fill(isame->LCS()); 
          hr_kS_pid->Fill(isame->PIDk()); 
          hr_kS_pidp->Fill(isame->PIDp()); 
          hr_kS_dq->Fill(dQ); 
          hr_kS_deta->Fill(deta); 
          hr_kS_fabsdeta->Fill(fabs(deta)); 
          hr_kS_dphi->Fill(dphi); 
          hr_kS_dr->Fill(dR); 
          hr_kS_tsal->Fill(isame->likelihood()); 
          hr_kS_mult->Fill(nparts); 
      	  homphi_kS_r->Fill(isame->distPhi());
          hr_kS_theta->Fill(acos(isame->pt()/isame->p()));        
          hr_kS_prob->Fill(1-tsame->omega());
        }
        if(tsame->decision()==-TrueTag) { 
          hw_kS_p->Fill(isame->p()); 
          hw_kS_pt->Fill(isame->pt());
          hw_kS_ips->Fill(isame->IPs()); 
          hw_kS_ipssign->Fill(isame->IPssign());        
          hw_kS_ipsign->Fill(isame->IPsign());        
          if (isame->IPssign()>0) hw_kS_ipssign->Fill(isame->IPssign()); else hw_kS_ipssign1->Fill(isame->IPssign());
          if(krec>1) hw_kS_ippu->Fill(isame->IPPU());
          hw_kS_lcs->Fill(isame->LCS()); 
          hw_kS_pid->Fill(isame->PIDk()); 
          hw_kS_pidp->Fill(isame->PIDp()); 
          hw_kS_dq->Fill(dQ); 
          hw_kS_deta->Fill(deta); 
          hw_kS_fabsdeta->Fill(fabs(deta)); 
          hw_kS_dphi->Fill(dphi); 
          hw_kS_dr->Fill(dR); 
          hw_kS_tsal->Fill(isame->likelihood()); 
          hw_kS_mult->Fill(nparts); 
          homphi_kS_w->Fill(isame->distPhi());
          hw_kS_theta->Fill(acos(isame->pt()/isame->p()));        
          hw_kS_prob->Fill(1-tsame->omega());
        }
      }
      if(tvtx){
        double VtxSumpt=0, VtxSumips=0, VtxSumdoca=0, VtxM=0, VtxP=0, VtxLike=0, VtxDphi=0, VtxMag=0;
	if( !secvtx.empty()) {
	  if (tagdecisionOS==TrueTag) hr_vtx_probComb->Fill(1-tvtx->omega());
	  if (tagdecisionOS==-TrueTag) hw_vtx_probComb->Fill(1-tvtx->omega());
	  TVector3 VtxPos = secvtx.at(0)->position();
	  int nsecpart = secvtx.at(0)->outgoingParticles().size();
	  VtxLike = secvtx.at(0)->likelihood();
	  Particles secparts = secvtx.at(0)->outgoingParticles();
	  TLorentzVector Vtxmomentum;
	  for (int ip=0; ip<nsecpart; ip++) {
	    Vtxmomentum += secparts.at(ip)->momentum();
	    VtxSumpt += secparts.at(ip)->pt();
	    VtxSumips += secparts.at(ip)->IPs();
	    VtxSumdoca += secparts.at(ip)->DOCA();
	  }
	  VtxM = Vtxmomentum.M();
	  VtxP = Vtxmomentum.P();
	  VtxMag = Vtxmomentum.Mag();
	  VtxDphi = VtxPos.DeltaPhi(Vtxmomentum.Vect());
	}
        debug()<<"Vdec: "<<tvtx->decision()<<"Vch: "<<tvtx->charge()<<endreq;
        if(tvtx->decision()==TrueTag) {
          hr_vtx_ch->Fill(fabs(tvtx->charge())); //abs(Vch)
          hr_vtx_prob->Fill(1-tvtx->omega());
          hr_vtx_like->Fill(VtxLike);
	  hr_vtx_dphidir->Fill(VtxDphi);
          hr_vtx_sumpt->Fill(VtxSumpt);
          hr_vtx_sumips->Fill(VtxSumips);
          hr_vtx_sumdoca->Fill(VtxSumdoca);
          hr_vtx_M->Fill(VtxM);
          hr_vtx_P->Fill(VtxP);
          hr_vtx_Mag->Fill(VtxMag);
        }
        if(tvtx->decision()==-1*TrueTag) {
          hw_vtx_ch->Fill(fabs(tvtx->charge())); 
          hw_vtx_prob->Fill(1-tvtx->omega());
          hw_vtx_like->Fill(VtxLike);
	  hw_vtx_dphidir->Fill(VtxDphi);
          hw_vtx_sumpt->Fill(VtxSumpt);
          hw_vtx_sumips->Fill(VtxSumips);
          hw_vtx_sumdoca->Fill(VtxSumdoca);
          hw_vtx_M->Fill(VtxM);
          hw_vtx_P->Fill(VtxP);
          hw_vtx_Mag->Fill(VtxMag);
        }
        if(tvtx->decision()==0) hu_vtx_ch->Fill(fabs(tvtx->charge())); 
	if(isame) eta_corr[3][4]->Fill(tvtx->omega(),tsame->omega());        
      }

      //more histos:
      if(TrueTag== tagdecision) {h100->Fill(1-omega); h103->Fill(omega);}
      if(TrueTag==-tagdecision) {h102->Fill(1-omega); h104->Fill(omega);}
      
      if(imuon){
        if(tmuon->decision()==TrueTag) h613->Fill(tmuon->RawNNetOutput()); 
        else h513->Fill(tmuon->RawNNetOutput());
        if(tmuon->decision()==TrueTag) h613p->Fill(tmuon->omega()); 
        else h513p->Fill(tmuon->omega());
      }
      if(iele){
        if(tele->decision()==TrueTag) h611->Fill(tele->RawNNetOutput());
        else h511->Fill(tele->RawNNetOutput());
        if(tele->decision()==TrueTag) h611p->Fill(tele->omega());
        else h511p->Fill(tele->omega());
      }
      if(ikaon){
        if(tkaon->decision()==TrueTag) h6321->Fill(tkaon->RawNNetOutput()); 
        else h5321->Fill(tkaon->RawNNetOutput());
        if(tkaon->decision()==TrueTag) h6321p->Fill(tkaon->omega()); 
        else h5321p->Fill(tkaon->omega());
      }
      if(isame){
        if(tsame->decision()==TrueTag) h535->Fill(tsame->RawNNetOutput()); 
        else h635->Fill(tsame->RawNNetOutput());
        if(tsame->decision()==TrueTag) h535p->Fill(tsame->omega()); 
        else h635p->Fill(tsame->omega());	
      }
      if(tvtx->decision()){
        if(tvtx->decision()==TrueTag) h540->Fill(tvtx->RawNNetOutput()); 
        else h640->Fill(tvtx->RawNNetOutput());
        if(tvtx->decision()==TrueTag) h540p->Fill(tvtx->omega()); 
        else h640p->Fill(tvtx->omega());	
      }

      //omega plot for taggers
      if (tagdecisionOS==TrueTag)  hr_probComb_OS->Fill(1-omegaOS);
      if (tagdecisionOS==-TrueTag) hw_probComb_OS->Fill(1-omegaOS);
      if (tagdecision==TrueTag)  hr_probComb->Fill(1-omega);
      if (tagdecision==-TrueTag) hw_probComb->Fill(1-omega);
      if (imuon) hom_mu->Fill(tmuon->omega());
      if (iele) hom_e->Fill(tele->omega());
      if (ikaon) hom_k->Fill(tkaon->omega());
      if (isame) hom_ss->Fill(tsame->omega());
      if (tvtx->decision()) hom_vtx->Fill(tvtx->omega());
      if (theTag->decisionOS()) hom_combOS->Fill(theTag->omegaOS());
      if (theTag->decision()) hom_combOSSS->Fill(theTag->omega());

      //oscillation plots:
      int Btag = TrueTag;
      if(imuon) { if(Btag*tmuon->decision()>0)
          h2011->Fill(Btime); else h2012->Fill(Btime); }
      if(iele) { if(Btag*tele->decision()>0) 
          h2021->Fill(Btime); else h2022->Fill(Btime); }
      if(ikaon) { if(Btag*tkaon->decision()>0) 
          h2031->Fill(Btime); else h2032->Fill(Btime); }
      if(isame) { if(Btag*tsame->decision()>0) 
          h2041->Fill(Btime); else h2042->Fill(Btime); }
      if(tvtx->decision())  { if(Btag*tvtx->decision()>0) 
          h2051->Fill(Btime); else h2052->Fill(Btime); }

      int iosc = TrueTag*tagdecision;
      if     (category==1) {if(iosc>0) h3011->Fill(Btime);else h3012->Fill(Btime);}
      else if(category==2) {if(iosc>0) h3021->Fill(Btime);else h3022->Fill(Btime);}
      else if(category==3) {if(iosc>0) h3031->Fill(Btime);else h3032->Fill(Btime);}
      else if(category==4) {if(iosc>0) h3041->Fill(Btime);else h3042->Fill(Btime);}
      else if(category==5) {if(iosc>0) h3051->Fill(Btime);else h3052->Fill(Btime);}
      if     (category!=0) {if(iosc>0) h3061->Fill(Btime);else h3062->Fill(Btime);}
      
      if(tree.enableMC) {
        if( BSpart->hasOscillated() ) iosc = -iosc;
        if( BSpart->hasOscillated() ) iosc = -1*TrueTag; else iosc = 1*TrueTag;
        if(iosc>0) h4061->Fill(Btime); else h4062->Fill(Btime);
      }

      //dump some info if in debug mode////////////////////////
      if(msgLevel(MSG::DEBUG)  //&& tagdecision && event.multiplicity()<15
         ) {
        event.Print(); theTag->Print();
        Particles::iterator ipa;
        int npcount=1;
        for(ipa=parts.begin(); ipa!=parts.end(); ipa++) {
          debug()<< npcount++; (*ipa)->Print();
        }
      }
    } //end of loop on event

  }//end of loop on files

  //////////////////////////////////////////////////////////////////////////

  //dump full list of declared properties:
  propertyMap::const_iterator ipro;
  for( ipro=property.begin(); ipro!=property.end(); ++ipro ) {
    if(msgLevel(MSG::DEBUG)) 
      debug()<<"Property "<<(*ipro).first<<" = "<<(*ipro).second<<endreq;
  }

  if(NNetTrain!="none") {
    nnetune.Write();
    nnetune.TrainNNet();
  } else {
    perfmon.printStats();
  }
 
  fhisto.Write(); fhisto.Close();

  //viewer.DrawLHCb(); //draw openGL view of detector
  if(property["ViewOptionLHCb"]>=0)
    viewer.DrawStoredEvents(); //also draw tracks on top of it

  timesum += (clock()-timestart)/(double)CLOCKS_PER_SEC;
  info()<<timesum<<" seconds elapsed time ("
        <<timesum*1000./((double)perfmon.N()+1)<<" ms/evt)."<<endmsg;

  return 0;
} 

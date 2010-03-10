#include "tag.h"
#include "tag.tuner.h"

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> HOW TO RUN:
  SetupProject Gaudi ROOT
  alias compile_tag "g++ tag.cc -o tag.exe `root-config --libs` -lMLP -lTMVA -lGpad -I`root-config --incdir`"
  alias run_tag="compile_tag && ./tag.exe"

  compile_tag && ./tag.exe
  root -l tag.C

  tag.cuts.txt contains the input directory of data and tagging cuts
  tag.exe fills histograms in tag.root, then tag.C reads them.
  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/


int main(){

  //load initial values of cuts and init variables
  //cout<<CLEARSCREEN;
  #include "tag.hdef.h"

  //FILE LOOP>>>>>>>>>>>>>>>>>>>>>>>>+++++++++++++++++++++++++++++++++++++++
  for(ifile=filelist.begin(); ifile!=filelist.end() && nsele<Nmax; ++ifile,++nfile ){
    TFile* f = new TFile(*ifile); if(!f) return 0;
    TTree* mytagging = (TTree*) gDirectory->Get("BTaggingAnalysis/mytagging");
    if(!mytagging) {
      if(!f) return 0;
      cout<<FAINT<<"\nNo tree found for "<<(*ifile)<<" skip!\n"<<ENDC;
      system("rm -v "+(*ifile));//REMOVE physical file!
      continue;
    }
    #include "tag.inc.h" // define variables in entuple

    // EVENT LOOP //////////////////////////////////////////////////
    float Nevt = mytagging->GetEntries();
    cout<<DEL1L<<(int)Nevt<<" evts in "<<AMARILLO<<(*ifile)<<ENDC<<flush;
    for (int ievt=0; ievt<Nevt && nsele<Nmax; ievt++){
      mytagging->GetEntry(ievt); //fill ntp branches
      PrintAdvance(ievt,Nevt);
      decode(trig, trigHlt2, trigHlt1, trigL0); 
      
      //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      // GENERAL CUTS TO SELECT EVENTS FOR STUDY PURPOSES
      if(askL0) if( trigL0 == 0 ) continue;
      if(askHlt1) if( trigHlt1 == 0 ) continue;
      if(askHlt2) if( trigHlt2 == 0 ) continue;
      if( bkgCat!=0 && bkgCat!=50 && bkgCat!=10 ) continue;
      //if(BOP<144) continue;
      //if(BSID>0) continue; //flavour dependence
      //if( kType != 68 ) continue;
      //if( krec <3 ) continue;
      //if( TrueTag == -1 ) continue;
      //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

      int Btag=1, BSID = 0;
      double BMas= 5.279, Btime= -999 ;
      TLorentzVector BSpart(0.0);

      for (int ii=0; ii<M; ii++){
        int bid = (int) abs(sID[ii]);
        if(bid==531 || bid==511 || bid==521) {
          if( IsControlChannel ) TrueTag = sID[ii]>0? 1 : -1 ;//TrueTag REDEF!!!
          BSID = (int) sID[ii];
          if(abs(BSID)==531) BMas = 5.367;
          BSpart = build4V( sP[ii], sPt[ii], sPhi[ii], sMass[ii] );
          if( BSID<0 ) Btag =-1;
          if( BSosc  ) Btag*=-1;
          TVector3 Bdir(sVx[ii]-RVx, sVy[ii]-RVy, sVz[ii]-RVz);
          Btime = Bdir.Mag()*BMas/sP[ii]/0.299792458; //proper time in ps
        }
      }	
      if(!BSID) {
        cout<<FAINT<<"Signal B does not exist! Skip event.\n\n"<<ENDC; 
        continue;
      }
      nsele++;

      isBs = false; if(abs(BSID)==531)  isBs = true;
      bool isBd = false; if(abs(BSID)==511)  isBd = true;
      bool isBu = false; if(abs(BSID)==521)  isBu = true;

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
      h301->Fill(BOP);
      h401->Fill(BSpart.P());
      h302->Fill(BSpart.Theta()*1000);
      h311->Fill(N);
      h88->Fill(krec);
      //************************

      //event inits ----------------------------
      int imuon,iele,ikaon,ikaonS,ipionS, vflagged=0;
      imuon=iele=ikaon=ikaonS=ipionS=0;
      bool bmuon,bele,bkaon,bkaonS,bpionS;
      bmuon=bele=bkaon=bkaonS=bpionS=false;
      float ptmaxm,ptmaxe,ptmaxk,ptmaxkS,ptmaxpS;
      ptmaxm=ptmaxe=ptmaxk=ptmaxkS=ptmaxpS=-999;
      double WeigVch=0, norm=0;
      float ncand_mu=0, ncand_ele=0, ncand_k=0, ncand_kS=0, ncand_pS=0; 
      int tagdecision=0, catt=0, Ntracks=0;
      double pnsum=0.5, pnsum_a=0.5, pnsum_b=0.5, rnet=0;
      std::vector<double> NNinputs(10),pn;
      std::vector<int> itag;
      for( int j=0; j<6; ++j ) {pn.push_back(0.50); itag.push_back(0);}
      if(useDV) N=0; //skip useless calculation
      for (int i=0; i<N; i++) { 
        bool dup=false;
        for (int j=i+1; j<N; j++) if(P[i]==P[j]) {dup=true; break;}
        if (!dup) if(ipPU[i]>IPPU_cut) if(distPhi[i]>distPhi_cut) ++Ntracks; 
      }


      //- loop over N : select candidate m, e, K ---------------------------->
      for ( int i=0; i<N; i++ ) {

        //Plots of PID//
        int ismu, muNSH, inHcal, iflag_e, iflag_m, iflag_k;
        decode((int)PIDfl[i], ismu, muNSH, inHcal, iflag_e, iflag_m, iflag_k );

        //OVERWRITE ID
        ID[i] = 211 * ch[i];
        if( iflag_e==1 && PIDe[i] > PID_e_cut ) ID[i] = -11 * ch[i];
        if( iflag_k==1 && PIDk[i] > PID_k_cut 
            &&  PIDk[i]-PIDp[i]   > PID_kp_cut) ID[i] = 321 * ch[i];
        if( iflag_m==1 && PIDm[i] > PID_m_cut ) ID[i] = -13 * ch[i];
	
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
    
        //Vertex charge ----
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
        double deta= fabs(log(tan(BSpart.Theta()/2.))
                          -log(tan(asin(Pt[i]/P[i])/2.)));
        double dphi= fabs(phi[i]-BSpart.Phi()); if(dphi>3.1416) dphi=6.283-dphi;
        double dQ_asKaon = calc_dQ(BSpart, build4V(P[i],Pt[i],phi[i], 321) );
        double dQ_asPion = calc_dQ(BSpart, build4V(P[i],Pt[i],phi[i], 211) );

        if(DBG) {
          cout<<BOLD<< "--- trtyp="<<trtyp[i] << " ID="<<ID[i]<<setprecision(4)
              << " P="<<P[i] << " Pt="<<Pt[i] << " lcs="<<lcs[i] <<ENDC<<endl;
          cout<< "    deta="<<deta<< " dphi="<<dphi 
              << " dQ="<< (isBs? dQ_asKaon:dQ_asPion) <<endl;
          cout<<"    IPs="<<IPsig<<" IPPU="<<" NSH="<<muNSH<<endl;
          cout<<"    vFlag="<<vFlag[i]<<" bFlag="<<bFlag[i]<<" MCID="<<MCID[i]<<endl;
          cout<<"   PIDm,e,k,p "<< PIDm[i]<<" "<<PIDe[i]<<" "
              <<PIDk[i]<<" "<<PIDp[i]<<endl;
          cout<<"   E/P="<< EOverP[i]<<" veloch="<<veloch[i]<<endl;
          if(vFlag[i])cout<<"USED IN VTX"<<endl;
        }

        //all taggers far from PU vertices
        if(ipPU[i]<IPPU_cut) continue;
	
        if(distPhi[i]<distPhi_cut) continue;

        //if(MCID[i]==0) continue;
        //if(bFlag[i]==-1) { 
        //cout<<"*** Warning: part from signal! Skip."<<ID[i]<<endl;
        // continue; //cheating: kill all taggers from signal
        //}

        if(bFlag[i]==1 ) {
          if(abs(MCID[i])==13)  ntruemu++;
          if(abs(MCID[i])==11)  ntrueel++;
          if(abs(MCID[i])==321) ntruekO++;
        } 
        if(abs(MCID[i])==321&& bFlag[i]==0&& xFlag[i]!=0) ntruekS++;

        //INCLUSIVE PID mode
        if( iflag_m && muNSH==0 && PIDm[i] > PID_m_cut ) 
          if(Pt[i] > Muon_Pt_cut) 
            if(P[i] > Muon_P_cut) 
              if( ( trtyp[i] == 3 || trtyp[i] == 7 ) && lcs[i]<lcs_m_cut) {
                ncand_mu++;
                if(DBG) cout<<"accepting muoncand "<<endl;
                if( Pt [i]> ptmaxm ) { //Pt ordering
                  bmuon = true;
                  imuon = i;
                  ptmaxm = Pt[i];
                }
              }
        // selects OS electron tag      
        if( inHcal && PIDe[i] > PID_e_cut )
          if(Pt[i] > Ele_Pt_cut) 
            if(P[i] > Ele_P_cut) 
              if(tsal[i]>Ele_ghost_cut)
                if( ( trtyp[i] == 3 || trtyp[i] == 7 ) && lcs[i]<lcs_e_cut ) 
                  if( EOverP[i] > Ele_EoverP_cut || EOverP[i]<-100 ) 
                    if( veloch[i]>VeloChargeMin && veloch[i]<VeloChargeMax ){ 
                      ncand_ele++;
                      if(DBG) cout<<"accepting elecand "<<endl;
                      if( Pt[i] > ptmaxe ) { //Pt ordering
                        bele = true;
                        iele = i;
                        ptmaxe = Pt[i];
                      }
                    }

        // selects OS kaon tag
        if( PIDk[i]!=0 && PIDk[i]>PID_k_cut && 
            PIDk[i]-PIDp[i]>PID_kp_cut )  
          if(Pt[i] > Kaon_Pt_cut)
            if(P[i]  > Kaon_P_cut) {
              //hrefit1->Fill( ip_r[i]/iperr_r[i] - IPsig );
              if(IPsig > Kaon_IP_cut) 
                if(fabs(ip[i])< Kaon_IPRAW_cut) 
                  if(tsal[i]>Kaon_ghost_prob_cut)
                    if(( trtyp[i]==4 && lcs[i]<lcs_k_ups_cut  ) ||
                       ((trtyp[i]==3 || trtyp[i]==7 ) && lcs[i]<lcs_k_long_cut)){
                      ncand_k++;
                      if(DBG) cout<<"accepting kcand "<<endl;
                      if( Pt[i] > ptmaxk ) { //Pt ordering
                        bkaon = true;
                        ikaon = i;
                        ptmaxk = Pt[i]; 
                      }
                    }
            }
        // selects SS kaon tag
        if( isBs ) { 
          if( PIDk[i]!=0 && PIDk[i]> PID_kS_cut && 
              PIDk[i]-PIDp[i]> PID_kpS_cut )
            if(Pt[i] > KaonSame_Pt_cut) 
              if(P[i]> KaonSame_P_cut) 
                if(dphi < KaonSame_Phi_cut)
                  if(deta < KaonSame_Eta_cut)
                    if(dQ_asKaon < KaonSame_dQ_cut) {
                      //hrefit2->Fill( ip_r[i]/iperr_r[i] - IPsig );
                      if(IPsig < KaonSame_IP_cut)
                        if( lcs[i] < lcs_kS_cut ) {
                          ncand_kS++;
                          if(DBG) cout<<"accepting kScand"<<endl;
                          if( Pt[i] > ptmaxkS ) { //Pt ordering
                            bkaonS = true;
                            ikaonS = i;
                            ptmaxkS = Pt[i];
                          }
                        }
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
                        if((trtyp[i]==3 || trtyp[i]==7) && lcs[i]<lcs_pS_cut){
                          ncand_pS++;
                          if(DBG) cout<<"accepting pScand "<<endl;
                          if( Pt[i] > ptmaxpS ) { //Pt ordering
                            bpionS = true;
                            ipionS = i;
                            ptmaxpS = Pt[i];
                          }
                        }
        }
      } //end of loop on parts <---
      //extra cut for pions SS candidates
      if(bpionS) {
        double dQp=calc_dQ(BSpart,build4V(P[ipionS],Pt[ipionS],phi[ipionS],211));
        if( dQp > PionSame_dQ2_cut ) {bpionS=false; ipionS=0;}
      }

   
      if(norm) WeigVch = WeigVch / norm;
      if(fabs(WeigVch) < MinimumVCharge ) WeigVch = 0;

      //--- itag is the individual B-flavour guess of each separate tagger:
      if( bmuon ) itag[1]= (int) -ch[imuon];
      if( bele  ) itag[2]= (int) -ch[iele];
      if( bkaon ) itag[3]= (int) -ch[ikaon];
      if( bkaonS) itag[4]= (int)  ch[ikaonS]; // SS kaon type
      if( bpionS) itag[4]= (int)  ch[ipionS]; // SS pion type
      if( isBu )  itag[4]= -itag[4];    //swap same-side if Bu
      if(WeigVch) itag[5]=  WeigVch>0? -1 : 1;


      //************************************
      //  fill NNet Branch and NNinputs    *
      //************************************
      if( NNetTrain != "none" ) {
        int tag=0, j=0, thisncand=0;
        if(NNetTrain == "mu") { j=imuon;  tag=itag[1]; thisncand= (int)ncand_mu; }
        if(NNetTrain == "e")  { j=iele;   tag=itag[2]; thisncand= (int)ncand_ele;}
        if(NNetTrain == "k")  { j=ikaon;  tag=itag[3]; thisncand= (int)ncand_k;  }
        if(NNetTrain == "kS") { j=ikaonS; tag=itag[4]; thisncand= (int)ncand_kS; }
        if(NNetTrain == "pS") { j=ipionS; tag=itag[4]; thisncand= (int)ncand_pS; }
        if(tag) {
          iscorrect = (tag == TrueTag) ? 1 : 0 ; 
          std::vector<double> NNinputs(10);
          NNinputs.at(0) = Ntracks;
          NNinputs.at(1) = BSpart.Vect().Perp();
          NNinputs.at(2) = P[j];
          NNinputs.at(3) = Pt[j];
          NNinputs.at(4) = ip[j]/iperr[j];
          NNinputs.at(5) = log(tan(BSpart.Theta()/2.))
            -log(tan(asin(Pt[j]/P[j])/2.)); //deta
          NNinputs.at(6) = min(fabs(phi[j]-BSpart.Phi()), 
                               float(6.283)-fabs(phi[j]-BSpart.Phi()));
          NNinputs.at(7) = calc_dQ(BSpart, build4V(P[j],Pt[j],phi[j], 
                                                   NNetTrain=="kS"? 321:211));
          NNinputs.at(8) = krec;
          NNinputs.at(9) = thisncand;

          normalise( NNinputs ); //otherwise NNet doesnt like them
          mult   = NNinputs.at(0);
          ptB    = NNinputs.at(1);
          partP  = NNinputs.at(2);
          partPt = NNinputs.at(3);
          IPPV   = NNinputs.at(4);
          nndeta = NNinputs.at(5);
          nndphi = NNinputs.at(6);
          nndq   = NNinputs.at(7);
          nkrec  = NNinputs.at(8);
          ncand  = NNinputs.at(9);

          nntrain->Fill();// <-- fills branch
        }      
      }

  
      //--- Combine tags: -----------------------------------------
      if(bmuon) {      
        NNinputs.at(0) = Ntracks;
        NNinputs.at(1) = BSpart.Vect().Perp();
        NNinputs.at(2) = P[imuon];
        NNinputs.at(3) = Pt[imuon];
        NNinputs.at(4) = ip[imuon]/iperr[imuon];
        NNinputs.at(8) = krec;
        NNinputs.at(9) = ncand_mu;
        pn[1] = Classify( mlptype, "mu", NNinputs, rnet );
        if(itag[1] == TrueTag) h613->Fill(rnet); else h513->Fill(rnet);
        if(itag[1] == TrueTag) h613p->Fill(1-pn[1]); else h513p->Fill(1-pn[1]);
        if(itag[1] == TrueTag) homphi_mu_r->Fill(distPhi[imuon]);
        else homphi_mu_w->Fill(distPhi[imuon]);
      }

      if(bele) {
        NNinputs.at(0) = Ntracks;
        NNinputs.at(1) = BSpart.Vect().Perp();
        NNinputs.at(2) = P[iele];
        NNinputs.at(3) = Pt[iele];
        NNinputs.at(4) = ip[iele]/iperr[iele];
        NNinputs.at(8) = krec;
        NNinputs.at(9) = ncand_ele;
        pn[2] = Classify( mlptype, "e", NNinputs, rnet );
        if(itag[2] == TrueTag) h611->Fill(rnet); else h511->Fill(rnet);
        if(itag[2] == TrueTag) h611p->Fill(1-pn[2]); else h511p->Fill(1-pn[2]);
      }

      if(bkaon) {
        NNinputs.at(0) = Ntracks;
        NNinputs.at(1) = BSpart.Vect().Perp();
        NNinputs.at(2) = P[ikaon];
        NNinputs.at(3) = Pt[ikaon];
        NNinputs.at(4) = ip[ikaon]/iperr[ikaon];
        NNinputs.at(8) = krec;
        NNinputs.at(9) = ncand_k;
        pn[3] = Classify( mlptype, "k", NNinputs, rnet );
        if(itag[3] == TrueTag) h6321->Fill(rnet); else h5321->Fill(rnet);
        if(itag[3] == TrueTag) h6321p->Fill(1-pn[3]); else h5321p->Fill(1-pn[3]);
        if(itag[3] == TrueTag) homphi_kO_r->Fill(distPhi[ikaon]);
        else homphi_kO_w->Fill(distPhi[ikaon]);
      }
      
      if(bkaonS) {
        double ang = asin(Pt[ikaonS]/P[ikaonS]);
        double deta= log(tan(BSpart.Theta()/2.))-log(tan(ang/2.));
        double dphi= fabs(phi[ikaonS]-BSpart.Phi()); if(dphi>3.1415)dphi=6.283-dphi;
        NNinputs.at(0) = Ntracks;
        NNinputs.at(1) = BSpart.Vect().Perp();
        NNinputs.at(2) = P[ikaonS];
        NNinputs.at(3) = Pt[ikaonS];
        NNinputs.at(4) = ip[ikaonS]/iperr[ikaonS];
        NNinputs.at(5) = deta;
        NNinputs.at(6) = dphi;
        NNinputs.at(7) = calc_dQ(BSpart, 
                                 build4V(P[ikaonS],Pt[ikaonS],phi[ikaonS],321) );
        NNinputs.at(8) = krec;
        NNinputs.at(9) = ncand_kS;
        pn[4] = Classify( mlptype, "kS", NNinputs, rnet );
        if(itag[4] == TrueTag) h53210->Fill(rnet); else h63210->Fill(rnet);
        if(itag[4] == TrueTag) h53210p->Fill(1-pn[4]); else h63210p->Fill(1-pn[4]);
        if(itag[4] == TrueTag) homphi_kS_r->Fill(distPhi[ikaonS]);
        else homphi_kS_w->Fill(distPhi[ikaonS]);
      }

      if(bpionS) {
        double ang = asin((Pt[ipionS])/(P[ipionS]));
        double deta= log(tan(BSpart.Theta()/2.))-log(tan(ang/2.));
        double dphi= fabs(phi[ipionS]-BSpart.Phi()); if(dphi>3.1415)dphi=6.283-dphi;
        NNinputs.at(0) = Ntracks;
        NNinputs.at(1) = BSpart.Vect().Perp();
        NNinputs.at(2) = P[ipionS];
        NNinputs.at(3) = Pt[ipionS];
        NNinputs.at(4) = ip[ipionS]/iperr[ipionS];
        NNinputs.at(5) = deta;
        NNinputs.at(6) = dphi;
        NNinputs.at(7) = calc_dQ(BSpart, 
                                 build4V(P[ipionS],Pt[ipionS],phi[ipionS],211) );
        NNinputs.at(8) = krec;
        NNinputs.at(9) = ncand_pS;
        pn[4] = Classify( mlptype, "pS", NNinputs, rnet );
        if(pn[4]<ProbMin_pion) { ipionS=0; itag[4]=0; pn[4]=0.50; }
        if(itag[4] == TrueTag) h53210->Fill(rnet); 
        else h63210->Fill(rnet);
        if(itag[4] == TrueTag) h53210p->Fill(1-pn[4]); 
        else h63210p->Fill(1-pn[4]);
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
        if( 1-omega < ProbMinVtx || omega > ProbMinVtx ) {
          itag[5]=0;
          omega = 0.50;
        }
        pn[5] = 1.0 - omega;           //Vertex charge tagger
        if(itag[5]==TrueTag)h540->Fill(fabs(WeigVch));else h640->Fill(fabs(WeigVch));
        if(itag[5]==TrueTag)h540p->Fill(1-pn[5]);else h640p->Fill(1-pn[5]);
      }

      //make decision
      for( int i = 1; i != 6; i++ ) {
        double mtag = itag[i];
        pnsum_a *= ((1-mtag)/2+mtag* pn[i] ); // p
        pnsum_b *= ((1+mtag)/2-mtag* pn[i] ); //(1-p)
        if(DBG) cout<< "i="<<i<<" itag="<<mtag<<" pn="<<pn[i]<<endl;
      }      
      if(pnsum_a > pnsum_b) tagdecision = +1;
      if(pnsum_a < pnsum_b) tagdecision = -1;
      pnsum = max(pnsum_a,pnsum_b) /(pnsum_a + pnsum_b);

      //throw away poorly significant tags
      if(pnsum < ProbMin) {
        pnsum = 0.50;
        tagdecision = 0;
      }

      //sort into categories
      double omega = fabs(1-pnsum); 
      if(      omega > omegamaxbin              ) catt=1;
      else if( omega > omegamaxbin-omegascale   ) catt=2;
      else if( omega > omegamaxbin-omegascale*2 ) catt=3;
      else if( omega > omegamaxbin-omegascale*3 ) catt=4;
      else                                        catt=5;
      if(tagdecision==0) catt=0;

     
      //****************************************
      //Fill standard control plots and counters
      #include "tag.hfill.h"
      
    }//end of loop on events
 
    f->Close();
  }//end of files

  //*********Fill some other event histos:
  heffec = calculateEffEff (hright, hwrong, direction);

  ///////////////////////////////////////////////////////////////////////
  double tp=0; 
  std::vector<double> dummy(10); 
  Classify("TMVA", "finalize", dummy, tp);
  fhisto.Write(); fhisto.Close();

  //*********Dump things:
  if(NNetTrain=="none") {
    tp = PrintPerformance();
  } else {
    if(mlptype=="TMVA") TMVATuning( NNetTrain );
    if(mlptype=="MultiLayerPerceptron")MultiLayerPerceptronTuning(NNetTrain);
  }

  return 0;
}
////////// THE END


    

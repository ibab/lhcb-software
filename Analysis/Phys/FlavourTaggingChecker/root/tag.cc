#include "tag.h"
#include "tag.tuner.h"
#include "TROOT.h"
#include "TArc.h"
#include "TLine.h"

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> HOW TO RUN:
  SetupProject Gaudi ROOT
  make 
  make run 
  make plots
  
  or comment out main() and call from root
  root> .x tag.cc+

  tag.cuts.txt contains the input directory of data and tagging cuts
  tag.exe fills histograms in tag.root, then tag.C reads them.
  >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

int main(){ return (int)tag(); }

double tag(){

  //load initial values of cuts and init variables
  #include "tag.hdef.h"
  //  TApplication app("appKey",0,0);
  TCanvas visual("visual","event",0,0,600,600);
  hxz->Draw();

  //output for NN of comb of taggers
  ofstream mysigfile, mybkgfile;
  mysigfile.open("combsigNN_.txt");
  mybkgfile.open("combbkgNN_.txt");
  mysigfile<<"tt/F:pmu/F:pe/F:pk/F:pss/F:pvtx/F"<<endl;
  mybkgfile<<"tt/F:pmu/F:pe/F:pk/F:pss/F:pvtx/F"<<endl;

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
      int idummy, l0tis, l0tos;
      decode(L0TisTos, idummy, l0tis, l0tos ); 
      globall0tis=l0tis;
      globall0tos=l0tos;

      //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      // GENERAL CUTS TO SELECT EVENTS FOR STUDY PURPOSES
      if(askL0)   if( trigL0 == 0 ) continue;
      if(askHlt1) if( trigHlt1 == 0 ) continue;
      if(askHlt2) if( trigHlt2 == 0 ) continue;
      if(askTis)  if( !l0tis ) continue;
      if(askTos)  if( !l0tos ) continue;
      if( bkgCat!=0 && bkgCat!=50 && bkgCat!=10 ) continue;
      //if(BSpart.P()>100) continue;
      //if(BOP<144) continue;
      //if(BSID>0) continue; //flavour dependence
      //if(abs(BOID)==521) continue; //vertex charge finds Bu
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

      //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>            
      if (askPtB) {
	if(BSpart.Pt()>11) continue;
	if(BSpart.Pt()<8) continue;
      }

      isBs = false; if(abs(BSID)==531)  isBs = true;
      bool isBd = false; if(abs(BSID)==511)  isBd = true;
      bool isBu = false; if(abs(BSID)==521)  isBu = true;

      if(abs(BOID)==511)      ++nB511;
      else if(abs(BOID)==521) ++nB521;
      else if(abs(BOID)==531) ++nB531;
      else  ++nBother;
      if(BSID<0)  ++nBanti;
      if(trig!=0) ++ntrig;
      if( l0tis ) ++ntis;
      if( l0tos ) ++ntos;
      if( l0tos && l0tis ) ++ntistos;

      if(DBG) cout<< froot <<" ----------- "
                  << Run <<" "<< Event <<" "<< trig  <<endl;

      if(DBGNN) cout<< "Evt: "<<ievt<<endl;

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

      nsele++;

      //event inits ----------------------------
      int imuon,iele,ikaon,ikaonS,ipionS, vflagged=0;
      imuon=iele=ikaon=ikaonS=ipionS=0;
      bool bmuon,bele,bkaon,bkaonS,bpionS;
      bmuon=bele=bkaon=bkaonS=bpionS=false;
      float k_weightedpt,k_weightedch,kS_weightedpt,kS_weightedch,pS_weightedpt,pS_weightedch;
      k_weightedpt=k_weightedch=kS_weightedpt=kS_weightedch=pS_weightedpt=pS_weightedch=0;
      float ptmaxm,ptmaxe,ptmaxk,ptmaxkS,ptmaxpS;
      ptmaxm=ptmaxe=ptmaxk=ptmaxkS=ptmaxpS=-999;
      double WeigVch=0, norm=0;
      float ncand_mu=0, ncand_ele=0, ncand_k=0, ncand_kS=0, ncand_pS=0; 
      int tagdecision=0, catt=0;
      double pnsum=0.5, pnsum_a=0.5, pnsum_b=0.5, rnet=0;
      std::vector<double> NNinputs(10),pn;
      std::vector<int> itag;
      for( int j=0; j<6; ++j ) {pn.push_back(0.50); itag.push_back(0);}
      if(useDV) N=0; //skip useless calculations

      //--- retuning of SVvtx seeds
      if(DBG) cout<<"==SVertexOneSeed=="<<endl;
      double maxprobf=0;
      TLorentzVector p1, p2;
      int ivseed=-1, iv_seed1=-1, iv_seed2=-1;
      for ( int iv=0; iv<V; iv++ ) {
	int i_seed1=-1, i_seed2=-1; 
	for ( int i=0; i<N; i++ ) {

	  bool dup=0;
	  for ( int j=i+1; j<N; j++ ) {
	    if(P[i]==P[j]) {
	      dup=true;
	      // cout<<i<<" duplicato con "<<j<<"   "<<ID[i]<<" ~ "<<ID[j]
	      //<<"  P="<<P[i]<<endl;
	      break;
	    }
	  }
	  if(dup) continue;

	  if(SecVtx_pt1[iv]==Pt[i]) i_seed1=i; //N-block index 
	  if(SecVtx_pt2[iv]==Pt[i]) i_seed2=i;

	}
	if( i_seed1==-1 || i_seed2==-1 )  {
	  //cout<<"not found 2 seed "<<endl;
	  continue;//info missing in N-block
	}


	TVector3 SVpoint(SecVtx_x[iv]-RVx, SecVtx_y[iv]-RVy, SecVtx_z[iv]-RVz);
	double rdist = sqrt( SVpoint.x()* SVpoint.x()
			    +SVpoint.y()* SVpoint.y()
			    +SVpoint.z()* SVpoint.z()*.074*.074 );
	if (DBG) cout<<"rdist:"<<rdist<<endl;
	p1= build4V(P[i_seed1], Pt[i_seed1], phi[i_seed1], ID[i_seed1]);
	p2= build4V(P[i_seed2], Pt[i_seed2], phi[i_seed2], ID[i_seed2]);
	double dphi= fabs(phi[i_seed1]-phi[i_seed2]); 
	if(dphi>PI) dphi= 2*PI-dphi;
	double ipsmin= 
	  min(ip[i_seed1]/iperr[i_seed1], ip[i_seed2]/iperr[i_seed2]);
 
	if(SVpoint.Theta()>.350 || SVpoint.Theta()<.010) continue;
	if(rdist < 0.30) continue;
	if(rdist > 10.0) continue;
	if(SecVtx_chi2[iv]>10) continue;
	if(min(p1.Pt(), p2.Pt())<0.15) continue;
	if(max(p1.Pt(), p2.Pt())<0.3) continue;
	if(max(ip[i_seed1],ip[i_seed2])>3.0) continue;
	if(dphi<0.001) continue;
	if(ipsmin<2.5) continue;

	if (DBG) cout<<"seed part2:"<<p2.Pt()<<endl;
	
	bool twofromB= bFlag[i_seed1]==1 && bFlag[i_seed2]==1;
	bool fromD=isNonResonantD(mothID[i_seed1]) || isNonResonantD(mothID[i_seed2]);
	fromD = twofromB && fromD;
	//twofromB = (TrueTag==ch[i_seed1] && TrueTag==ch[i_seed2] );
	
	//evaluate likelihood
	double prob_chi2  = pol(SecVtx_chi2[iv], 
				0.615074, -0.081797, 0.00421188);
	double prob_ptmin = pol(min(p1.Pt(), p2.Pt()), 
				0.0662687, 1.10754, -0.350278);
	double prob_ipmax = pol(max(ip[i_seed1],ip[i_seed2]), 
				0.763837, -0.0822829, -0.0154407);
	double prob_ipsmin = 0;
	if(ipsmin>2.5 && ipsmin<6) 
	  prob_ipsmin    = pol(ipsmin, -0.642335, 0.356381, -0.0239819);
	else prob_ipsmin = pol(ipsmin, 0.536929, 0.0254873, -0.000439594);

	double prob_deltaphi = pol(dphi, 0.699251, -0.19263, 0.00319839);
	
	double prob_rdist = 0;
	if(rdist<1) prob_rdist= pol(rdist, 9.61771e-05, 0.936598, -0.433183);
	else prob_rdist= pol(rdist, 0.44296, 0.0956002, -0.0130237);

	double probs = prob_chi2*prob_ptmin*prob_rdist
	  *prob_ipmax*prob_ipsmin*prob_deltaphi;
	double probb = (1-prob_chi2)*(1-prob_ptmin)*(1-prob_rdist)
	  *(1-prob_ipmax)*(1-prob_ipsmin)*(1-prob_deltaphi);
	double probf = probs/(probs+probb);

	if(DBG) cout<<"prop seed (chi2, ptmin, ipmax, ipsmin, deltaphi, rdist):"<<endl;
	if(DBG) cout<<" "<<prob_chi2<<", "<<prob_ptmin<<", "<<prob_ipmax<<", "<<prob_ipsmin<<", "<<prob_deltaphi<<", "<<prob_rdist<<endl;

	if(DBG) cout<<"seed formed, probf:"<<probf<<endl;
	if(DBG)	cout<<"           , pt:"<<p1.Pt()<<", "<<p2.Pt()<<endl;

	if(probf>maxprobf) {
	  ivseed=iv;
	  iv_seed1=i_seed1; iv_seed2=i_seed2; 
	  maxprobf=probf;
	}

	if(twofromB) hv2D2->Fill(SVpoint.z(), SVpoint.Perp());
	else              hv2D0->Fill(SVpoint.z(), SVpoint.Perp());
	if(fromD)         hv2Dd->Fill(SVpoint.z(), SVpoint.Perp());
	
	if(twofromB) hz2->Fill(rdist);
	else              hz0->Fill(rdist);
	if(fromD)         hzd->Fill(rdist);
	
	if(twofromB) hvChi2->Fill(SecVtx_chi2[iv]);
	else              hvChi0->Fill(SecVtx_chi2[iv]);
	if(fromD)         hvChid->Fill(SecVtx_chi2[iv]);

	if(twofromB) hvpmin_2->Fill(min(p1.Pt(), p2.Pt()));
	else              hvpmin_0->Fill(min(p1.Pt(), p2.Pt()));
	if(fromD)         hvpmin_d->Fill(min(p1.Pt(), p2.Pt()));
	
	if(twofromB) hvpmax_2->Fill(max(p1.Pt(), p2.Pt()));
	else              hvpmax_0->Fill(max(p1.Pt(), p2.Pt()));
	if(fromD)         hvpmax_d->Fill(max(p1.Pt(), p2.Pt()));
	
	if(twofromB) hvipmin2->Fill(min(ip[i_seed1],ip[i_seed2]));
	else              hvipmin0->Fill(min(ip[i_seed1],ip[i_seed2]));
	if(fromD)         hvipmind->Fill(min(ip[i_seed1],ip[i_seed2]));

	if(twofromB) hvipmax2->Fill(max(ip[i_seed1],ip[i_seed2]));
	else              hvipmax0->Fill(max(ip[i_seed1],ip[i_seed2]));
	if(fromD)         hvipmaxd->Fill(max(ip[i_seed1],ip[i_seed2]));
	
	if(twofromB) hvdphi2->Fill(dphi);
	else              hvdphi0->Fill(dphi);
	if(fromD)         hvdphid->Fill(dphi);

	if(twofromB) hvipsmin2->Fill(ipsmin);
	else              hvipsmin0->Fill(ipsmin);
	if(fromD)         hvipsmind->Fill(ipsmin);
	
	if(twofromB) hvmass2->Fill((p1+p2).M());
	else              hvmass0->Fill((p1+p2).M());
	if(fromD)         hvmassd->Fill((p1+p2).M());

	if(twofromB) hvprobf2->Fill(probf);
	else         hvprobf0->Fill(probf);
	if(fromD)    hvprobfd->Fill(probf);

      }
      if(maxprobf) {
	if(maxprobf>1) maxprobf=1;
	if(maxprobf<0.4) {maxprobf=0; ivseed=-1;}
	if(DBG) cout<<"maxprobf:"<<maxprobf<<endl;
      }
      //----------------
      if(ivseed != -1) {
	if(bFlag[iv_seed1]==1 && bFlag[iv_seed2]==1) hmaxprobf2->Fill(maxprobf);
	else hmaxprobf0->Fill(maxprobf);
      }

      /////////////////////////////////////////////////////
      //retune svtx for adding tracks
      std::vector<int> vecSV(0);
      if(maxprobf) {

	vecSV.push_back(iv_seed1);
	vecSV.push_back(iv_seed2);
	int nfromb=0, nfromd=0, nbadtr=0;

	for ( int i=0; i<N; i++ ) {

	  if(i==iv_seed1 || i==iv_seed2) continue; 

	  if(ip[i]/iperr[i]<3.5) continue;
	  if(ip[i]<0.1) continue;
	  if(IPSV[i]>0.7) continue;
	  if(DOCA[i]>0.2) continue;
	  if(DBG) cout<<"added track, pt:"<<Pt[i]<<endl;

	  vecSV.push_back(i);////add part

	  TLorentzVector p= build4V(P[i], Pt[i], phi[i], ID[i]);
	  bool fromD = isNonResonantD(mothID[i]);

	  //bool crit = (TrueTag==ch[i]);
	  bool crit = (bFlag[i]==1);
	  	
	  if(crit)  nfromb++; else nbadtr++;  
	  if(fromD) nfromd++;

	  if(crit) htr_pt1->Fill(Pt[i]);
	  else     htr_pt0->Fill(Pt[i]);
	  if(fromD)htr_ptd->Fill(Pt[i]);

	  if(crit) htr_ip1->Fill(ip[i]);
	  else     htr_ip0->Fill(ip[i]);
	  if(fromD)htr_ipd->Fill(ip[i]);
	
	  if(crit) htr_ips1->Fill(ip[i]/iperr[i]);
	  else     htr_ips0->Fill(ip[i]/iperr[i]);
	  if(fromD)htr_ipsd->Fill(ip[i]/iperr[i]);
	
	  if(crit) htr_ipsv1->Fill(IPSV[i]);
	  else     htr_ipsv0->Fill(IPSV[i]);
	  if(fromD)htr_ipsvd->Fill(IPSV[i]);
	
	  if(crit) htr_ipsvs1->Fill(IPSV[i]/IPSVerr[i]);
	  else     htr_ipsvs0->Fill(IPSV[i]/IPSVerr[i]);
	  if(fromD)htr_ipsvsd->Fill(IPSV[i]/IPSVerr[i]);
	
	  if(crit) htr_DOCA1->Fill(DOCA[i]);
	  else     htr_DOCA0->Fill(DOCA[i]);
	  if(fromD)htr_DOCAd->Fill(DOCA[i]);

	  if(crit) htr_Chi2Seed1->Fill(Chi2Seed[i]);
	  else     htr_Chi2Seed0->Fill(Chi2Seed[i]);
	  if(fromD)htr_Chi2Seedd->Fill(Chi2Seed[i]);

	  if(crit) htr_mass1->Fill((p1+p2+p).M());
	  else     htr_mass0->Fill((p1+p2+p).M());
	  if(fromD)htr_massd->Fill((p1+p2+p).M());

	  double dfi=fabs((p1+p2).Phi()-p.Phi()); if(dfi>PI) dfi=2*PI-dfi;
	  if(crit) htr_deltaphi1->Fill(dfi);
	  else     htr_deltaphi0->Fill(dfi);
	  if(fromD)htr_deltaphid->Fill(dfi);
 
	}
	htr_count1->Fill(nfromb);
	htr_count0->Fill(nbadtr);
	htr_countd->Fill(nfromd);
      }

      //now we have the Sec. vertex!
      //vecSV.clear();//kill it


      //- loop over N : select candidate m, e, K ---------------------------->
      for ( int i=0; i<N; i++ ) {

	bool dup=0;
	for ( int j=i+1; j<N; j++ ) {
	  if(P[i]==P[j]) {
	    dup=true;
	    //cout<<i<<"  duplicato con  "<<j<<" "<<ID[i]<<" ~ "<<ID[j]<<endl;
	  }
	}
	if(dup) continue;

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
         
        //-------------------------------------------------------
        //select among tag candidates take the highest Pt of each
        double IPsig = fabs(ip[i]/iperr[i]);
        double deta= fabs(log(tan(BSpart.Theta()/2.))
                          -log(tan(asin(Pt[i]/P[i])/2.)));
        double dphi= fabs(phi[i]-BSpart.Phi()); if(dphi>PI) dphi=2*PI-dphi;
        double dQ_asKaon = calc_dQ(BSpart, build4V(P[i],Pt[i],phi[i], 321) );
        double dQ_asPion = calc_dQ(BSpart, build4V(P[i],Pt[i],phi[i], 211) );

        if(DBG) {
          cout<< "--- trtyp="<<trtyp[i] << " ID="<<ID[i]<<setprecision(4)
              << " P="<<P[i] << " Pt="<<Pt[i] << " lcs="<<lcs[i]<<endl;
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
		      //weight pt
		      double kpow = pow(Pt[i],powkOS);
		      k_weightedch += kpow * ch[i];
		      k_weightedpt += kpow;
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
			  //weight pt
			  double kSpow = pow(Pt[i],powkSS);
			  kS_weightedch += kSpow * ch[i];
			  kS_weightedpt += kSpow;
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
			  //weight pt
			  double pSpow = pow(Pt[i],powpSS);
			  pS_weightedch += pSpow * ch[i];
			  pS_weightedpt += pSpow;
                          if(DBG) cout<<"accepting pScand "<<endl;
                          if( Pt[i] > ptmaxpS ) { //Pt ordering
                            bpionS = true;
                            ipionS = i;
                            ptmaxpS = Pt[i];
                          }
                        }
        }

        //Vertex charge as it is in FT package ----
        //reweight Vertex Charge with product pt
	//if(vFlag[i]==1 || vFlag[i]==11 ) { //SVertexTool.cpp
	  //if(vFlag[i]==10 || vFlag[i]==11) { //SVertexOneSeedTool.cpp
	  //vecSV.push_back(i);
	//}

      } //end of loop on parts <---

      //extracuts for weightedch with pt
      if (askWeightedch){
	if (bkaon) if(fabs(k_weightedch/k_weightedpt) < kwcut) {bkaon=false; ikaon=0;}                  
	if (bkaonS) if(fabs(kS_weightedch/kS_weightedpt) < kSwcut) {bkaonS=false; ikaonS=0;}                  
	if (bpionS) if(fabs(pS_weightedch/pS_weightedpt) < pSwcut) {bpionS=false; ipionS=0;}                  
      }

      //extra cut for pions SS candidates
      if(bpionS) {
        double dQp=calc_dQ(BSpart,build4V(P[ipionS],Pt[ipionS],phi[ipionS],211));
        if( dQp > PionSame_dQ2_cut ) {bpionS=false; ipionS=0;}
      }

      //myprop
      bool myprop;
      myprop=false;
      //      if (l0tis==1) myprop=true; //tis evts
      //      if (N>30) myprop=true; //high multiplicity evts
      if(BSpart.Pt()>8) myprop=true; //high PtB evts

      //vertex charge
      float bflagged=0, vflagged_long=0; //vtx_ptmin= 10000, vtx_ipsmin= 10000, vtx_docamax= 0;
      vtx_ptmin= 0, vtx_ipsmin= 0, vflagged_long=0, vtx_docamax= 0;
      for(vector<int>::iterator it=vecSV.begin();it!=vecSV.end(); ++it){
	if (DBG) cout<<"vecSV, size: "<<vecSV.size()<<endl;
	//double a = pow(IPSV[*it], VchPowerK);
	double a = pow(Pt[*it], VchPowerK);
	WeigVch += ch[*it] * a;
	norm+= a;
	vflagged++;
	if(bFlag[*it]==1) bflagged++;
	//info for NN
	// 	  vtx_ptmin = min(vtx_ptmin, (float)Pt[*it]);
	// 	  vtx_ipsmin= min(vtx_ipsmin, (float)ip[*it]/iperr[*it]);
	// 	  if(trtyp[*it]==3 ||trtyp[*it]==7) vflagged_long++;
	//  	  vtx_docamax= std::max(vtx_docamax, DOCA[*it]);
	
	vtx_ptmin  += Pt[*it];
	vtx_ipsmin += ip[*it]/iperr[*it];
	if(trtyp[*it]==3 ||trtyp[*it]==7) vflagged_long++;
	vtx_docamax += DOCA[*it];
	if (DBG) cout<<"debug(): docasv:"<<DOCA[*it]<<endl;
      }
      if(norm) {
	WeigVch /= norm;
	if(DBG) cout<<"--- weightedCh:"<<WeigVch<<endl;
	if(DBG) cout<<"accepting Vtxcand if abs(weightedCh)>0.12 "<<endl;
	if(fabs(WeigVch) < MinimumVCharge ) WeigVch = 0;
// 	cout<<vtx_ptmin<<endl;
// 	cout<<vflagged<<endl;
	vtx_ptmin  /= vflagged;
	vtx_ipsmin /= vflagged;
	vtx_docamax/= vflagged;
      }
      

      //if( bkaon || bmuon || bele) WeigVch=0;

      //--- itag is the individual B-flavour guess of each separate tagger:
      if( bmuon ) itag[1]= (int) -ch[imuon];
      if( bele  ) itag[2]= (int) -ch[iele];
      if( bkaon ) itag[3]= (int) -ch[ikaon];
      if( bkaonS) itag[4]= (int)  ch[ikaonS]; // SS kaon type
      if( bpionS) itag[4]= (int)  ch[ipionS]; //    pion type
      if( bpionS) if( isBu )  itag[4]= -itag[4];//     swap same-side if Bu
      if(WeigVch) itag[5]=  WeigVch>0? -1 : 1;
      //--- ch for weighted charge                                              
      if (askWeightedch){
	if( bkaon ) itag[3] = 0;                                                  
	if( bkaon ) itag[3]=  (k_weightedch/k_weightedpt)<0? 1: -1; //inverse of ch                        
	if( bkaon or bpionS) itag[4] = 0;                                         
	if( bkaonS) itag[4]=  (kS_weightedch/kS_weightedpt)<0? -1: 1;                        
	if( bpionS) itag[4]=  (pS_weightedch/pS_weightedpt)<0? -1: 1;                        
	if( bpionS) if( isBu )  itag[4]= -itag[4];//     swap same-side if Bu     
      }

      if(itag[5]) {
	bool ok= (itag[5]==TrueTag);
	if(ok) hvtxch1->Fill(WeigVch); else hvtxch0->Fill(WeigVch);
	if(bflagged/vflagged>0.75) hvtxchb->Fill(WeigVch);
	if(ok) hvtx_ptmin1  ->Fill(vtx_ptmin);  else hvtx_ptmin0  ->Fill(vtx_ptmin);
	if(ok) hvtx_ipsmin1 ->Fill(vtx_ipsmin); else hvtx_ipsmin0 ->Fill(vtx_ipsmin);
	if(ok) hvtx_docamax1->Fill(vtx_docamax);else hvtx_docamax0->Fill(vtx_docamax);
	if(ok) hvtx_maxprobf1->Fill(maxprobf);  else hvtx_maxprobf0->Fill(maxprobf);
	if(ok) hvtx_vflagged1->Fill(vflagged);  else hvtx_vflagged0->Fill(vflagged);
// 	bool seedok1 = abs(mothID[iv_seed1])>500 && abs(mothID[iv_seed1])<599;
// 	bool seedok2 = abs(mothID[iv_seed2])>500 && abs(mothID[iv_seed2])<599;
// 	bool seedok  = seedok1 || seedok2;
 	bool seedok = bFlag[iv_seed1] && bFlag[iv_seed2]; 
	hvtx_pullz->Fill(SecVtx_z[ivseed]-BOz); 
	if(seedok) hvtx_pullz2->Fill(SecVtx_z[ivseed]-BOz); 
	bool fromD=isNonResonantD(mothID[iv_seed1])&&isNonResonantD(mothID[iv_seed2]);
	fromD = seedok && fromD;
	if(fromD) hvtx_pullzd->Fill(SecVtx_z[ivseed]-BOz); 
	
	TVector3 SVpoint(SecVtx_x[ivseed]-RVx, 
			 SecVtx_y[ivseed]-RVy, 
			 SecVtx_z[ivseed]-RVz);
	TVector3 BOpoint(BOx-RVx, BOy-RVy, BOz-RVz);
	double df = dPhi(SVpoint.Phi(),BOpoint.Phi()); 
	df= df*SVpoint.Phi()/fabs(SVpoint.Phi());
	hvtx_pullf->Fill(df);
	hvtx_pulltheta->Fill(SVpoint.Theta()-BOpoint.Theta());
      }

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
     
        if(NNetTrain == "vtx" && itag[5]){  
          iscorrect = (itag[5] == TrueTag) ? 1 : 0 ; 
          mult        = double (N/90.);
          ptB         = BSpart.Vect().Perp()/25;
	  ncand       = double (vflagged/4.);
	  vtx_WeigVch = fabs(WeigVch);
	  vtx_ptmin   = vtx_ptmin/2;
	  vtx_ipsmin  = vtx_ipsmin/30;
 	  vtx_docamax = vtx_docamax*10;
	  vtx_likel   = maxprobf;
	  vtx_longfrac= vflagged_long/(vflagged? vflagged:1);
	  //other vars are filled above.
          nntrain->Fill();// <-- fills branch
	}

      }
  
      //--- Combine tags: -----------------------------------------
      if(bmuon) {      
        NNinputs.at(0) = N;
        NNinputs.at(1) = BSpart.Vect().Perp();
        NNinputs.at(2) = P[imuon];
        NNinputs.at(3) = Pt[imuon];
        NNinputs.at(4) = ip[imuon]/iperr[imuon];
        NNinputs.at(8) = krec;
        NNinputs.at(9) = ncand_mu;
        pn[1] = Classify( mlptype, "mu", NNinputs, rnet );
	//        if(pn[1]<ProbMin_muon) { imuon=0; itag[1]=0; pn[1]=0.50; }
        if(itag[1] == TrueTag) h613->Fill(rnet); else h513->Fill(rnet);
        if(itag[1] == TrueTag) h613p->Fill(1-pn[1]); else h513p->Fill(1-pn[1]);
        if(itag[1] == TrueTag) homphi_mu_r->Fill(distPhi[imuon]);
        else homphi_mu_w->Fill(distPhi[imuon]);
	//
	if (myprop==true){
	  if(itag[1] == TrueTag) h91mr->Fill(rnet); else h91mw->Fill(rnet);
	  if(itag[1] == TrueTag) h91mpr->Fill(1-pn[1]); else h91mpw->Fill(1-pn[1]);
	} else {
	  if(itag[1] == TrueTag) h90mr->Fill(rnet); else h90mw->Fill(rnet);
	  if(itag[1] == TrueTag) h90mpr->Fill(1-pn[1]); else h90mpw->Fill(1-pn[1]);
	}
      }

      if(bele) {
        NNinputs.at(0) = N;
        NNinputs.at(1) = BSpart.Vect().Perp();
        NNinputs.at(2) = P[iele];
        NNinputs.at(3) = Pt[iele];
        NNinputs.at(4) = ip[iele]/iperr[iele];
        NNinputs.at(8) = krec;
        NNinputs.at(9) = ncand_ele;
        pn[2] = Classify( mlptype, "e", NNinputs, rnet );
	//        if(pn[2]<ProbMin_ele) { iele=0; itag[2]=0; pn[2]=0.50; }
        if(itag[2] == TrueTag) h611->Fill(rnet); else h511->Fill(rnet);
        if(itag[2] == TrueTag) h611p->Fill(1-pn[2]); else h511p->Fill(1-pn[2]);
	//
	if (myprop==true){
	  if(itag[2] == TrueTag) h91er->Fill(rnet); else h91ew->Fill(rnet);
	  if(itag[2] == TrueTag) h91epr->Fill(1-pn[2]); else h91epw->Fill(1-pn[2]);
	} else {
	  if(itag[2] == TrueTag) h90er->Fill(rnet); else h90ew->Fill(rnet);
	  if(itag[2] == TrueTag) h90epr->Fill(1-pn[2]); else h90epw->Fill(1-pn[2]);
	}
      }

      if(bkaon) {
        NNinputs.at(0) = N;
        NNinputs.at(1) = BSpart.Vect().Perp();
        NNinputs.at(2) = P[ikaon];
        NNinputs.at(3) = Pt[ikaon];
        NNinputs.at(4) = ip[ikaon]/iperr[ikaon];
        NNinputs.at(8) = krec;
        NNinputs.at(9) = ncand_k;
        pn[3] = Classify( mlptype, "k", NNinputs, rnet );
	//        if(pn[3]<ProbMin_kaon) { ikaon=0; itag[3]=0; pn[3]=0.50; }
        if(itag[3] == TrueTag) h6321->Fill(rnet); else h5321->Fill(rnet);
        if(itag[3] == TrueTag) h6321p->Fill(1-pn[3]); else h5321p->Fill(1-pn[3]);
        if(itag[3] == TrueTag) homphi_kO_r->Fill(distPhi[ikaon]);
        else homphi_kO_w->Fill(distPhi[ikaon]);
	//
	if (myprop==true){
	  if(itag[3] == TrueTag) h91kr->Fill(rnet); else h91kw->Fill(rnet);
	  if(itag[3] == TrueTag) h91kpr->Fill(1-pn[3]); else h91kpw->Fill(1-pn[3]);
	} else {
	  if(itag[3] == TrueTag) h90kr->Fill(rnet); else h90kw->Fill(rnet);
	  if(itag[3] == TrueTag) h90kpr->Fill(1-pn[3]); else h90kpw->Fill(1-pn[3]);
	}
      }
      
      if(bkaonS) {
        double ang = asin(Pt[ikaonS]/P[ikaonS]);
        double deta= log(tan(BSpart.Theta()/2.))-log(tan(ang/2.));
        double dphi= fabs(phi[ikaonS]-BSpart.Phi()); if(dphi>PI)dphi=2*PI-dphi;
        NNinputs.at(0) = N;
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
	//        if(pn[4]<ProbMin_kaonS) { ikaonS=0; itag[4]=0; pn[4]=0.50; }
        if(itag[4] == TrueTag) h53210->Fill(rnet); else h63210->Fill(rnet);
        if(itag[4] == TrueTag) h53210p->Fill(1-pn[4]); else h63210p->Fill(1-pn[4]);
        if(itag[4] == TrueTag) homphi_kS_r->Fill(distPhi[ikaonS]);
        else homphi_kS_w->Fill(distPhi[ikaonS]);
	//
	if (myprop==true){
	  if(itag[4] == TrueTag) h91ssr->Fill(rnet); else h91ssw->Fill(rnet);
	  if(itag[4] == TrueTag) h91sspr->Fill(1-pn[4]); else h91sspw->Fill(1-pn[4]);
	} else {
	  if(itag[4] == TrueTag) h90ssr->Fill(rnet); else h90ssw->Fill(rnet);
	  if(itag[4] == TrueTag) h90sspr->Fill(1-pn[4]); else h90sspw->Fill(1-pn[4]);
	}
      }

      if(bpionS) {
        double ang = asin((Pt[ipionS])/(P[ipionS]));
        double deta= log(tan(BSpart.Theta()/2.))-log(tan(ang/2.));
        double dphi= fabs(phi[ipionS]-BSpart.Phi()); if(dphi>PI)dphi=2*PI-dphi;
        NNinputs.at(0) = N;
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
        if(itag[4] == TrueTag) { h53210->Fill(rnet); h53210p->Fill(1-pn[4]); }
        if(itag[4] ==-TrueTag) { h63210->Fill(rnet); h63210p->Fill(1-pn[4]); }
	//
	if (myprop==true){
	  if(itag[4] == TrueTag) h91ssr->Fill(rnet); else h91ssw->Fill(rnet);
	  if(itag[4] == TrueTag) h91sspr->Fill(1-pn[4]); else h91sspw->Fill(1-pn[4]);
	} else {
	  if(itag[4] == TrueTag) h90ssr->Fill(rnet); else h90ssw->Fill(rnet);
	  if(itag[4] == TrueTag) h90sspr->Fill(1-pn[4]); else h90sspw->Fill(1-pn[4]);
	}
      }

      if( itag[5] ) {   //---------
	NNinputs.at(0) = double (N/90.);
	NNinputs.at(1) = krec;
	NNinputs.at(2) = BSpart.Vect().Perp()/25;
	NNinputs.at(3) = double (vflagged/4.); //ncand
	NNinputs.at(4) = vtx_ptmin/2;
	NNinputs.at(5) = vtx_ipsmin/30;
	NNinputs.at(6) = vtx_docamax*10;
	NNinputs.at(7) = maxprobf;
	NNinputs.at(8) = vflagged_long/(vflagged? vflagged:1);
	NNinputs.at(9) = fabs(WeigVch);
	pn[5] = Classify( mlptype, "vtx", NNinputs, rnet );
        double omega=1-pn[5];
	if( 1-omega < ProbMinVtx || omega > ProbMinVtx ) {
          itag[5]=0;
          pn[5] = 0.50;
        }
        if(itag[5]==TrueTag) h540 ->Fill(rnet);   else h640 ->Fill(rnet);
        if(itag[5]==TrueTag) h540p->Fill(1-pn[5]);else h640p->Fill(1-pn[5]);
	//
	if (myprop==true){
	  if(itag[5] == TrueTag) h91vr->Fill(rnet); else h91vw->Fill(rnet);
	  if(itag[5] == TrueTag) h91vpr->Fill(1-pn[5]); else h91vpw->Fill(1-pn[5]);
	} else {
	  if(itag[5] == TrueTag) h90vr->Fill(rnet); else h90vw->Fill(rnet);
	  if(itag[5] == TrueTag) h90vpr->Fill(1-pn[5]); else h90vpw->Fill(1-pn[5]);
	}
      }

      //save omega and decision of individual taggers for NN
      if (askNNcomb){
	if ((abs(itag[1])+abs(itag[2])+abs(itag[3])+abs(itag[4])+abs(itag[5]))>1){
	  ommu=1-pn[1];
	  decmu=itag[1];
	  ome=1-pn[2];
	  dece=itag[2];
	  omk=1-pn[3];
	  deck=itag[3];
	  omSS=1-pn[4];
	  decSS=itag[4];
	  omvtx=1-pn[5];
	  decvtx=itag[5];
	  if (decmu==1) ommu=1-ommu;
	  if (dece==1) ome=1-ome;
	  if (deck==1) omk=1-omk;
	  if (decSS==1) omSS=1-omSS;
	  if (decvtx==1) omvtx=1-omvtx;
	  nntraincomb->Fill();
	  if (TrueTag==1) mysigfile<<"1."<<" "<<ommu<<" "<<ome<<" "<<omk<<" "<<omSS<<" "<<omvtx<<endl;
	  if (TrueTag==-1) mybkgfile<<"-1."<<" "<<ommu<<" "<<ome<<" "<<omk<<" "<<omSS<<" "<<omvtx<<endl;   
	}
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

      //make decision with NN
      if (askNNcomb){
	if(abs(itag[1])+abs(itag[2])+abs(itag[3])+abs(itag[4])+abs(itag[5])>1){
	  if (DBGNN) cout<<"#####\nitag[i]*pn[i]: "<<itag[1]*pn[1]<<" "<<itag[2]*pn[2]<<" "<<itag[3]*pn[3]<<" "<<itag[4]*pn[4]<<" "<<itag[5]*pn[5]<<endl;
	  double probPlus = CombineTaggers( itag, pn, pnsum );
	  if (DBGNN) cout<<"probPlus: "<<probPlus<<endl;
	  //marc - per mirar w=a+b*nncomboutput - look tuner.h
	  if(TrueTag==-1) h200->Fill(probPlus);
	  if(TrueTag==1) h202->Fill(probPlus);
	  tagdecision = (probPlus>0.5026)? 1 : -1; //number tuned in tuner.h
	  //transform probPlus->pnsum
	  if(tagdecision==1) pnsum = probPlus;
	  if(tagdecision==-1) pnsum = 1-probPlus;
	  if (DBGNN) cout<<"tagdecision: "<<tagdecision<<", pnsum: "<<pnsum<<endl;
	  //throw away poorly significant tags
	  if(pnsum < ProbMin_NNcomb) {
	    pnsum = 0.50;
	    tagdecision = 0;
	  }
	}
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
      //      #include "tag.event.h"

    }//end of loop on events
 
    f->Close();
  }//end of files

  //*********Fill some other event histos:
  //heffec = calculateEffEff (hright, hwrong, direction);
 
  calculateEffEff(nsele, hright, hwrong, heffec, direction);

  calculateEffEff(nsele, hr_mu_p, hw_mu_p, he_mu_p);
  calculateEffEff(nsele, hr_mu_pt, hw_mu_pt, he_mu_pt);
  calculateEffEff(nsele, hr_mu_ips, hw_mu_ips, he_mu_ips);
  calculateEffEff(nsele, hr_mu_ippu, hw_mu_ippu, he_mu_ippu);
  calculateEffEff(nsele, hr_mu_lcs, hw_mu_lcs, he_mu_lcs, "right2left");
  calculateEffEff(nsele, hr_mu_pid, hw_mu_pid, he_mu_pid);
  calculateEffEff(nsele, hr_mu_tsal, hw_mu_tsal, he_mu_tsal);
  calculateEffEff(nsele, hr_mu_mult, hw_mu_mult, he_mu_mult, "right2left");
  calculateEffEff(nsele, hr_mu_ncand, hw_mu_ncand, he_mu_ncand, "right2left");

  calculateEffEff(nsele, hr_ele_p, hw_ele_p, he_ele_p);
  calculateEffEff(nsele, hr_ele_pt, hw_ele_pt, he_ele_pt);
  calculateEffEff(nsele, hr_ele_ips, hw_ele_ips, he_ele_ips);
  calculateEffEff(nsele, hr_ele_ippu, hw_ele_ippu, he_ele_ippu);
  calculateEffEff(nsele, hr_ele_lcs, hw_ele_lcs, he_ele_lcs, "right2left");
  calculateEffEff(nsele, hr_ele_pid, hw_ele_pid, he_ele_pid);
  calculateEffEff(nsele, hr_ele_tsal, hw_ele_tsal, he_ele_tsal);
  calculateEffEff(nsele, hr_ele_mult, hw_ele_mult, he_ele_mult, "right2left");
  calculateEffEff(nsele, hr_ele_ncand, hw_ele_ncand, he_ele_ncand, "right2left");

  calculateEffEff(nsele, hr_k_p, hw_k_p, he_k_p);
  calculateEffEff(nsele, hr_k_pt, hw_k_pt, he_k_pt);
  calculateEffEff(nsele, hr_k_ips, hw_k_ips, he_k_ips);
  calculateEffEff(nsele, hr_k_ippu, hw_k_ippu, he_k_ippu);
  calculateEffEff(nsele, hr_k_lcs, hw_k_lcs, he_k_lcs, "right2left");
  calculateEffEff(nsele, hr_k_pid, hw_k_pid, he_k_pid);
  calculateEffEff(nsele, hr_k_tsal, hw_k_tsal, he_k_tsal);
  calculateEffEff(nsele, hr_k_mult, hw_k_mult, he_k_mult, "right2left");
  calculateEffEff(nsele, hr_k_ncand, hw_k_ncand, he_k_ncand, "right2left");

  calculateEffEff(nsele, hr_kS_p, hw_kS_p, he_kS_p);
  calculateEffEff(nsele, hr_kS_pt, hw_kS_pt, he_kS_pt);
  calculateEffEff(nsele, hr_kS_ips, hw_kS_ips, he_kS_ips, "right2left");
  calculateEffEff(nsele, hr_kS_ippu, hw_kS_ippu, he_kS_ippu);
  calculateEffEff(nsele, hr_kS_lcs, hw_kS_lcs, he_kS_lcs, "right2left");
  calculateEffEff(nsele, hr_kS_pid, hw_kS_pid, he_kS_pid, "right2left");
  calculateEffEff(nsele, hr_kS_tsal, hw_kS_tsal, he_kS_tsal);
  calculateEffEff(nsele, hr_kS_mult, hw_kS_mult, he_kS_mult, "right2left");
  calculateEffEff(nsele, hr_kS_ncand, hw_kS_ncand, he_kS_ncand, "right2left");


  ///////////////////////////////////////////////////////////////////////
  double tp=0; 
  std::vector<double> dummy(10); 
  Classify("TMVA", "finalize", dummy, tp);
 
  //*********Dump things:
  cout<<"\n\nvertex eff    = "
      <<(hmaxprobf2->GetEntries()+hmaxprobf0->GetEntries())/nsele;
  cout<<"\nvertex purity = "
      <<hmaxprobf2->GetEntries()/(hmaxprobf2->GetEntries()+hmaxprobf0->GetEntries());

  double totinvtx=(htr_count0->GetMean()+htr_count1->GetMean());

  //cout<<"\n\nntotB in the event:   "<<htr_countB->GetMean();
  cout<<"\nMean nr of added tracks   = "<<totinvtx;
  //cout<<"\n(EFF) B in SV/nntotB      = "<<htr_count1->GetMean()/htr_countB->GetMean();
  cout<<"\n(PUR) good added tracks   = "<<htr_count1->GetMean()/totinvtx;
  cout<<"\nFrac of tracks from D     = "<<htr_countd->GetMean()/totinvtx<<endl;
  
  fhisto.Write(); fhisto.Close();

  if(NNetTrain=="none") {
    tp = PrintPerformance();
  } else {
    if(mlptype=="TMVA") TMVATuning( NNetTrain );
    if(mlptype=="MultiLayerPerceptron")MultiLayerPerceptronTuning(NNetTrain);
  }

  cout<<"TIS fraction="<<ntis/nsele<<"  TOS="<<ntos/nsele
      <<"  TISTOS="<<ntistos/nsele<<endl;

  mysigfile.close();
  mybkgfile.close();

  return tp;
}
////////// THE END

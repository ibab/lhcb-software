//Fill standard control plots

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
	bool okk = (itag[ikaon] == TrueTag);
	bool k3 = (krec==3);
	bool k1 = (krec==1);
        h70->Fill(Pt[ikaon]); if(okk&&k1) h80->Fill(Pt[ikaon]); if(okk&&k3) h80_3->Fill(Pt[ikaon]); 
        h71->Fill(P[ikaon]);  if(okk&&k1) h81->Fill(P[ikaon]);  if(okk&&k3) h81_3->Fill(P[ikaon]); 
        h72->Fill(ip[ikaon]/iperr[ikaon]); 
	if(okk&&k1) h82->Fill(ip[ikaon]/iperr[ikaon]);
	if(okk&&k3) h82_3->Fill(ip[ikaon]/iperr[ikaon]);
        h73->Fill(lcs[ikaon]);  if(okk&&k1) h83->Fill(lcs[ikaon]); if(okk&&k3) h83_3->Fill(lcs[ikaon]); 
        h74->Fill(tsal[ikaon]);  if(okk&&k1) h84->Fill(tsal[ikaon]); if(okk&&k3) h84_3->Fill(tsal[ikaon]); 
        h75->Fill(PIDk[ikaon]);  if(okk&&k1) h85->Fill(PIDk[ikaon]); if(okk&&k3) h85_3->Fill(PIDk[ikaon]); 
        h76->Fill(PIDk[ikaon]-PIDp[ikaon]);  
        if(okk&&k1) h86->Fill(PIDk[ikaon]-PIDp[ikaon]); 
        if(okk&&k3) h86_3->Fill(PIDk[ikaon]-PIDp[ikaon]); 
        h77->Fill(distPhi[ikaon]);  
	if(okk&&k1) h87->Fill(distPhi[ikaon]); 
	if(okk&&k3) h87_3->Fill(distPhi[ikaon]); 
 	if(okk&&k1) h88->Fill(ncand_k); if(okk&&k3) h88_3->Fill(ncand_k); 
 	if(okk&&k1) h89->Fill(N); if(okk&&k3) h89_3->Fill(N); 
 
      }
      if(bkaonS) {
	h1013->Fill(PIDk[ikaonS]); h1014->Fill(PIDk[ikaonS]-PIDp[ikaonS]); 
	if( abs(MCID[ikaonS])== 321 ) h1003->Fill(PIDk[ikaonS]);
	if( abs(MCID[ikaonS])== 321 ) h1004->Fill(PIDk[ikaonS]-PIDp[ikaonS]);
	if( abs(MCID[ikaonS])== 0 )   h1023->Fill(PIDk[ikaonS]);
	if( abs(MCID[ikaonS])== 0 )   h1024->Fill(PIDk[ikaonS]-PIDp[ikaonS]);
      }

      /////////////////////////marco
      if(bmuon) { 
	//direction="right2left";
	if(itag[1]==TrueTag) { 
	  hr_mu_p->Fill(P[imuon]); 
	  hr_mu_pt->Fill(Pt[imuon]); 
	  hr_mu_ips->Fill(fabs(ip[imuon]/iperr[imuon])); 
	  hr_mu_pt->Fill(Pt[imuon]); 
	  if(krec>1) hr_mu_ippu->Fill(ipPU[imuon]);
	  hr_mu_lcs->Fill(lcs[imuon]); 
	  hr_mu_pid->Fill(PIDm[imuon]); 
	  hr_mu_tsal->Fill(tsal[imuon]); 
	  hr_mu_mult->Fill(N); 
	  hr_mu_ncand->Fill(ncand_mu); 
	} else {
	  hw_mu_p->Fill(P[imuon]); 
	  hw_mu_pt->Fill(Pt[imuon]);
	  hw_mu_ips->Fill(fabs(ip[imuon]/iperr[imuon])); 
	  hw_mu_pt->Fill(Pt[imuon]); 
	  if(krec>1) hw_mu_ippu->Fill(ipPU[imuon]);
	  hw_mu_lcs->Fill(lcs[imuon]); 
	  hw_mu_pid->Fill(PIDm[imuon]); 
	  hw_mu_tsal->Fill(tsal[imuon]); 
	  hw_mu_mult->Fill(N); 
	  hw_mu_ncand->Fill(ncand_mu); 
	}
      }
      if(bele) { 
	if(itag[2]==TrueTag) { 
	  hr_ele_p->Fill(P[iele]); 
	  hr_ele_pt->Fill(Pt[iele]); 
	  hr_ele_ips->Fill(fabs(ip[iele]/iperr[iele])); 
	  hr_ele_pt->Fill(Pt[iele]); 
	  if(krec>1) hr_ele_ippu->Fill(ipPU[iele]);
	  hr_ele_lcs->Fill(lcs[iele]); 
	  hr_ele_pid->Fill(PIDe[iele]); 
	  hr_ele_tsal->Fill(tsal[iele]); 
	  hr_ele_mult->Fill(N); 
	  hr_ele_ncand->Fill(ncand_ele); 
	} else {
	  hw_ele_p->Fill(P[iele]); 
	  hw_ele_pt->Fill(Pt[iele]);
	  hw_ele_ips->Fill(fabs(ip[iele]/iperr[iele])); 
	  hw_ele_pt->Fill(Pt[iele]); 
	  if(krec>1) hw_ele_ippu->Fill(ipPU[iele]);
	  hw_ele_lcs->Fill(lcs[iele]); 
	  hw_ele_pid->Fill(PIDe[iele]); 
	  hw_ele_tsal->Fill(tsal[iele]); 
	  hw_ele_mult->Fill(N); 
	  hw_ele_ncand->Fill(ncand_ele); 
	}
      }
      if(bkaon) { 
	if(itag[3]==TrueTag) { 
	  hr_k_p->Fill(P[ikaon]); 
	  hr_k_pt->Fill(Pt[ikaon]); 
	  hr_k_ips->Fill(fabs(ip[ikaon]/iperr[ikaon])); 
	  hr_k_pt->Fill(Pt[ikaon]); 
	  if(krec>1) hr_k_ippu->Fill(ipPU[ikaon]);
	  hr_k_lcs->Fill(lcs[ikaon]); 
	  hr_k_pid->Fill(PIDk[ikaon]); 
	  hr_k_tsal->Fill(tsal[ikaon]); 
	  hr_k_mult->Fill(N); 
	  hr_k_ncand->Fill(ncand_k); 
	} else {
	  hw_k_p->Fill(P[ikaon]); 
	  hw_k_pt->Fill(Pt[ikaon]);
	  hw_k_ips->Fill(fabs(ip[ikaon]/iperr[ikaon])); 
	  hw_k_pt->Fill(Pt[ikaon]); 
	  if(krec>1) hw_k_ippu->Fill(ipPU[ikaon]);
	  hw_k_lcs->Fill(lcs[ikaon]); 
	  hw_k_pid->Fill(PIDk[ikaon]); 
	  hw_k_tsal->Fill(tsal[ikaon]); 
	  hw_k_mult->Fill(N); 
	  hw_k_ncand->Fill(ncand_k); 
	}
      }
      if(bkaonS) { 
	if(itag[4]==TrueTag) { 
	  hr_kS_p->Fill(P[ikaonS]); 
	  hr_kS_pt->Fill(Pt[ikaonS]); 
	  hr_kS_ips->Fill(fabs(ip[ikaonS]/iperr[ikaonS])); 
	  hr_kS_pt->Fill(Pt[ikaonS]); 
	  if(krec>1) hr_kS_ippu->Fill(ipPU[ikaonS]);
	  hr_kS_lcs->Fill(lcs[ikaonS]); 
	  hr_kS_pid->Fill(PIDk[ikaonS]); 
	  hr_kS_tsal->Fill(tsal[ikaonS]); 
	  hr_kS_mult->Fill(N); 
	  hr_kS_ncand->Fill(ncand_kS); 
	} else {
	  hw_kS_p->Fill(P[ikaonS]); 
	  hw_kS_pt->Fill(Pt[ikaonS]);
	  hw_kS_ips->Fill(fabs(ip[ikaonS]/iperr[ikaonS])); 
	  hw_kS_pt->Fill(Pt[ikaonS]); 
	  if(krec>1) hw_kS_ippu->Fill(ipPU[ikaonS]);
	  hw_kS_lcs->Fill(lcs[ikaonS]); 
	  hw_kS_pid->Fill(PIDk[ikaonS]); 
	  hw_kS_tsal->Fill(tsal[ikaonS]); 
	  hw_kS_mult->Fill(N); 
	  hw_kS_ncand->Fill(ncand_kS); 
	}
      }
      if(bpionS) { 
	if(itag[4]==TrueTag) { 
	  hr_pS_p->Fill(P[ipionS]); 
	  hr_pS_pt->Fill(Pt[ipionS]); 
	  hr_pS_ips->Fill(fabs(ip[ipionS]/iperr[ipionS])); 
	  hr_pS_pt->Fill(Pt[ipionS]); 
	  if(krec>1) hr_pS_ippu->Fill(ipPU[ipionS]);
	  hr_pS_lcs->Fill(lcs[ipionS]); 
	  hr_pS_pid->Fill(PIDk[ipionS]); 
	  hr_pS_tsal->Fill(tsal[ipionS]); 
	  hr_pS_mult->Fill(N); 
	  hr_pS_ncand->Fill(ncand_pS); 
	} else {
	  hw_pS_p->Fill(P[ipionS]); 
	  hw_pS_pt->Fill(Pt[ipionS]);
	  hw_pS_ips->Fill(fabs(ip[ipionS]/iperr[ipionS])); 
	  hw_pS_pt->Fill(Pt[ipionS]); 
	  if(krec>1) hw_pS_ippu->Fill(ipPU[ipionS]);
	  hw_pS_lcs->Fill(lcs[ipionS]); 
	  hw_pS_pid->Fill(PIDk[ipionS]); 
	  hw_pS_tsal->Fill(tsal[ipionS]); 
	  hw_pS_mult->Fill(N); 
	  hw_pS_ncand->Fill(ncand_pS); 
	}
      }

//       if(bpionS) {
//  	direction="right2left";
//   	//double dphi= fabs(phi[bpionS]-BSpart.Phi()); 
// 	//if(dphi>3.1416) dphi=6.283-dphi;
// 	//double deta= fabs(log(tan(BSpart.Theta()/2.))
// 	// 		  -log(tan(asin(Pt[ipionS]/P[ipionS])/2.)));//[0,3]
// 	//double dQ_asPion = calc_dQ(BSpart, 
// 	//	   build4V(P[ipionS],Pt[ipionS],phi[ipionS], 211) );
// 	//double IPsig = fabs(ip[ipionS]/iperr[ipionS]);
//  	double var = ncand_pS;
//  	 if( itag[ 4 ]==TrueTag ) hright->Fill(var); else hwrong->Fill(var);
//       }
//       if(bkaonS) {
//  	//direction="right2left";
//   	//double dphi= fabs(phi[ikaonS]-BSpart.Phi()); 
// 	//if(dphi>3.1416) dphi=6.283-dphi;
// 	//double deta= fabs(log(tan(BSpart.Theta()/2.))
// 	// 		  -log(tan(asin(Pt[ikaonS]/P[ikaonS])/2.)));//[0,3]
// 	//double dQ_asKaon = calc_dQ(BSpart, 
// 	//	   build4V(P[ikaonS],Pt[ikaonS],phi[ikaonS], 321) );
// 	//double IPsig = fabs(ip[ikaonS]/iperr[ikaonS]);
//  	double var = ip_r[ikaonS]/iperr_r[ikaonS];
//  	 if( itag[ 4 ]==TrueTag ) hright->Fill(var); else hwrong->Fill(var);
//       }
//       if(bele) { 
// 	//direction="right2left";
// 	double var = PIDe[iele];
// 	if( itag[ 2 ]==TrueTag ) hright->Fill(var); else hwrong->Fill(var);
//       }
      if(bkaon) { 
	//direction="right2left";
	double var = ip[ikaon]/iperr[ikaon];
 	 if( itag[ 3 ]==TrueTag ) hright->Fill(var); else hwrong->Fill(var);
      }



     //more histos:
      if(TrueTag== tagdecision) h100->Fill(pnsum); 
      if(TrueTag==-tagdecision) h102->Fill(pnsum); 
      if(TrueTag== tagdecision) h103->Fill(1-pnsum); 
      if(TrueTag==-tagdecision) h104->Fill(1-pnsum); 
      if(DBG) if(tagdecision) cout<<"estimated omega="<<1-pnsum<<endl;
      //oscillation plots:
      if(itag[1]){if(Btag*itag[1]>0) h2011->Fill(Btime); else h2012->Fill(Btime);}
      if(itag[2]){if(Btag*itag[2]>0) h2021->Fill(Btime); else h2022->Fill(Btime);}
      if(itag[3]){if(Btag*itag[3]>0) h2031->Fill(Btime); else h2032->Fill(Btime);}
      if(itag[4]){if(Btag*itag[4]>0) h2041->Fill(Btime); else h2042->Fill(Btime);}
      if(itag[5]){if(Btag*itag[5]>0) h2051->Fill(Btime); else h2052->Fill(Btime);}
      int iosc = Btag*tagdecision;
      if(catt==1){if(iosc>0) h3011->Fill(Btime);else h3012->Fill(Btime);}
      if(catt==2){if(iosc>0) h3021->Fill(Btime);else h3022->Fill(Btime);}
      if(catt==3){if(iosc>0) h3031->Fill(Btime);else h3032->Fill(Btime);}
      if(catt==4){if(iosc>0) h3041->Fill(Btime);else h3042->Fill(Btime);}
      if(catt==5){if(iosc>0) h3051->Fill(Btime);else h3052->Fill(Btime);}
      if(catt!=0){if(iosc>0) h3061->Fill(Btime);else h3062->Fill(Btime);}

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
	 && bFlag[imuon]==1 && isD((int) mothID[imuon])) nbc_mu++;
      if(bele>0 && abs(MCID[iele])  == 11
	 && bFlag[iele]==1 && isD((int) mothID[iele])) nbc_el++;
      if(bkaon>0 && abs(MCID[ikaon]) == 321 && bFlag[ikaon] ==1 ) nbc_kO++;
      if(bkaonS>0&& abs(MCID[ikaonS])== 321 && xFlag[ikaonS]!=0 ) nbc_kS++;


      double var=BSpart.Pt();
      if(bkaonS||bpionS){
	if(itag[4]==TrueTag) h6rSS->Fill(var); else h6wSS->Fill(var);
      }
      if(TagOS){
	if(TagOS==TrueTag)  h6rOS->Fill(var); else h6wOS->Fill(var);
      }


      if(useDV) {
	tagdecision=Tag; catt=TagCat;
	for(int i=0; i<T; ++i) {
	  if(TaggerDecision[i]==0) continue;
	  if(TaggerType[i]==2) itag[1] = (int) TaggerDecision[i];
	  else if(TaggerType[i]==3) itag[2] = (int) TaggerDecision[i];
	  else if(TaggerType[i]==4) itag[3] = (int) TaggerDecision[i];
	  else if(TaggerType[i]==5) itag[4] = (int) TaggerDecision[i];
	  else if(TaggerType[i]==6) itag[4] = (int) TaggerDecision[i];
	  else if(TaggerType[i]==10)itag[5] = (int) TaggerDecision[i];
	}
      } 

      if(tagdecision) { 
        if(tagdecision==TrueTag) nrt[catt]++; else nwt[catt]++;
      }

      for(int k=1; k<6; ++k) {
	if(itag[k]) { 
	  if( itag[k] == TrueTag ) nrtag[k]++; else nwtag[k]++; 
	}
      }

      if(DBG)if(Tag!=tagdecision || TagCat!=catt) {
	int offtag_mu=0, offtag_e=0, offtag_k=0, offtag_SS=0, offtag_vtx=0;
	for(int i=0; i<T; ++i) {
	  if(TaggerDecision[i]==0) continue;
	  if(TaggerType[i]==2) offtag_mu = (int) TaggerDecision[i];
	  else if(TaggerType[i]==3) offtag_e  = (int) TaggerDecision[i];
	  else if(TaggerType[i]==4) offtag_k  = (int) TaggerDecision[i];
	  else if(TaggerType[i]==5) offtag_SS = (int) TaggerDecision[i];
	  else if(TaggerType[i]==6) offtag_SS = (int) TaggerDecision[i];
	  else if(TaggerType[i]==10)offtag_vtx= (int) TaggerDecision[i];
	}

	cout <<"--------------- Taggers Summary"<<endl;
	if(bmuon) cout <<"*mu  P="<<P[imuon]<<endl;
	if(bele)  cout <<"*ele P="<<P[iele]<<endl;
	if(bkaon) cout <<"*K   P="<<P[ikaon]<<endl;
	if(bkaonS)cout <<"*KS  P="<<P[ikaonS]<<endl;
	if(bpionS)cout <<"*pS  P="<<P[ipionS]<<endl;
	cout << "\nTAG  "<< Run  << " " << Event
	     << std::setw(5) << tagdecision
	     << std::setw(3) << catt  
	     << std::setw(5) << itag[1]
	     << std::setw(3) << itag[2]
	     << std::setw(3) << itag[3]
	     << std::setw(3) << itag[4]
	     << std::setw(3) << itag[5] 
	     << " <-- mine"
	     << endl;

	 
	  cout <<  "TAG  "<< Run  << " " << Event
	       << std::setw(5) << Tag
	       << std::setw(3) << TagCat  
	       << std::setw(5) << offtag_mu
	       << std::setw(3) << offtag_e
	       << std::setw(3) << offtag_k
	       << std::setw(3) << offtag_SS
	       << std::setw(3) << offtag_vtx
	       << " <-- TAG-Warning: official was this"
	       <<endl;


      }//debug

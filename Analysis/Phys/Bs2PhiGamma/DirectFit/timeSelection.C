TCut pionID(std::string part,std::string NNpi="0.2",std::string NNk="0.2"){
  std::string c = part+"_ProbNNpi > "+NNpi;//+" && "+part+"_ProbNNk  < "+NNk;
  return TCut(c.c_str());
}  

TCut kaonID(std::string part,std::string NNk="0.2"){ 
  std::string c = part+"_ProbNNk > "+NNk ;
  //std::string c = part+"_PIDK >5 ";
  return TCut(c.c_str());
} 
  
TCut protonID(std::string part,std::string NNp="0.3",std::string NNk="0.2"){ 
  std::string c = part+"_ProbNNp > "+NNp+" && "+part+"_ProbNNk  < "+NNk;  
  return TCut(c.c_str());
}

TCut hID(std::string hypo, std::string part,std::string NN1="0.3",std::string NN2="1.",std::string NN3="1."){ 
  std::string c;
  if(hypo=="pi"     )c = part+"_ProbNNpi > "+NN1+" && "+part+"_ProbNNk   < "+NN2+" && "+part+"_ProbNNp  < "+NN3;  
  else if(hypo=="K" )c = part+"_ProbNNk  > "+NN1+" && "+part+"_ProbNNpi  < "+NN2+" && "+part+"_ProbNNp  < "+NN3;  
  else if(hypo=="p" )c = part+"_ProbNNp  > "+NN1+" && "+part+"_ProbNNpi  < "+NN2+" && "+part+"_ProbNNk  < "+NN3;  
  else c="0";

  return TCut(c.c_str());
}


//------- Vector & Hadron cuts -------------------------------------------------------------------
TCut vectorSel(std::string h1, std::string h2,std::string v,std::string mass, std::string window,bool lifetime=true, std::string IP1="55.0",std::string IP2="55.0",std::string isol="0"){
  TCut cut = TCut("");
  //------- daughter cuts
  cut += TCut(std::string(h1+"_IPCHI2_OWNPV > "+IP1+" && "+h2+"_IPCHI2_OWNPV > "+IP2).c_str());
  //cut += TCut(std::string(h1+"_IPCHI2_OWNPV > 16 && "+h2+"_IPCHI2_OWNPV > 16").c_str());
     
  //cut += TCut(std::string(h1+"_MINIPCHI2 > "+IP1+" && "+h2+"_MINIPCHI2 > "+IP2).c_str());               // Chi2(IPmin)(h) > 25
  cut += TCut(std::string("max("+h1+"_PT,"+h2+"_PT) > 1200 && min("+h1+"_PT,"+h2+"_PT) > 500").c_str());  // min(pT) > 500 && max(pT) > 1200

  //cut += TCut(std::string(h1+"_PT > "+PT1+" && "+h2+"_PT > "+PT2).c_str());                        // scaled PT cuts

  //-- PID-fiducial cuts
  cut += TCut(std::string(h1+"_P > 500 && "+h2+"_P > 500").c_str());                                      //  P > 0.5 GeV
  cut += TCut(std::string(h1+"_P < 100000 && "+h2+"_P < 100000").c_str());                                //  P < 100 GeV
  std::string eta1 = "-log( tan( atan( "+h1+"_PT/"+h1+"_PZ)/2 ))";
  std::string eta2 = "-log( tan( atan( "+h2+"_PT/"+h2+"_PZ)/2 ))";
  cut += TCut(std::string(eta1+" > 1.5 && "+eta2+" > 1.5").c_str());                                      // Eta > 1.5   
  cut += TCut(std::string(eta1+" < 5.0 && "+eta2+" < 5.0").c_str());                                      // Eta < 5.0   

  //=========== Lifetime fiducial cuts ==============
  if( lifetime ){
    cut += TCut(std::string(eta1+" > 2.0 && "+eta2+" > 2.0").c_str());                                      // Eta > 1.5   
    cut += TCut(std::string(eta1+" < 4.5 && "+eta2+" < 4.5").c_str());                                      // Eta < 5.0   
  }
  //=================================================

  //------- vector cut
  //cut +=  TCut(std::string(v+"_SmallestDeltaChi2OneTrack > " + isol).c_str());                                     // Vertex Isolation DeltaChi2 
  //cut +=  TCut(std::string("B_SmallestDeltaChi2OneTrack > " + isol).c_str());                                     // Vertex Isolation DeltaChi2 
  cut +=  TCut(std::string(v+"_ENDVERTEX_CHI2 < 9").c_str());                                                // Chi2(Vtx)
  if(mass != "")cut +=  TCut(std::string("abs("+mass+"-"+v+"_MM) < "+ window).c_str());                            // Vector mass window  
  if(mass == "" && window != "")cut +=  TCut(std::string(v+"_MM  < "+ window).c_str());
  //------ Track quality
  cut += TCut(std::string(h1+"_TRACK_CHI2NDOF  < 3 &&"+h2+"_TRACK_CHI2NDOF < 3").c_str());           // Chi2/Ndof(Track) < 3   
  cut += TCut(std::string(h1+"_TRACK_GhostProb < 0.3 && "+h2+"_TRACK_GhostProb < 0.3").c_str());     // Ghost probability < 0.3
  return TCut( cut );
}


//------- Main selection ------------------------------------------------
TCut timeSelection(int selFlag,bool trigger=true,bool scaling=true,double maxDIRA=-1){
  //------------------------------------------------//
  // usage :
  // selFlag      : selection flag (1=K*g / 2=K*psi / 4=Phig / 8=phipsi
  //------------------------------------------------//
  

  // === RADIATIVE TRIGGER & STRIPPING
  TCut radL0    = TCut("1");
  TCut radHLT1  = TCut("1");
  TCut radHLT2  = TCut("1");
  TCut radStrip = TCut("1");
  radL0   = TCut("(B_L0PhotonDecision_TOS || B_L0ElectronDecision_TOS)");
  //radHLT1 = TCut("(B_Hlt1TrackAllL0Decision_TOS)");
  //testeing pay attention
  //std::cout << "===== WARNING : HLT1 photon is applied, check this, if not disable this message ======" <<std::endl;
  radHLT1 = TCut("(B_Hlt1TrackAllL0Decision_TOS)");// || B_Hlt1TrackPhotonDecision_TOS)");    
  if(selFlag == 1 )radHLT2 =  TCut("(B_Hlt2Bd2KstGammaDecision_TOS == 1)");
  if(selFlag == 4 )radHLT2 =  TCut("(B_Hlt2Bs2PhiGammaDecision_TOS == 1)");
  if(selFlag == 1 )radStrip=TCut("StrippingB2KstarGamma_B2VG_LineDecision==1");
  if(selFlag == 4 )radStrip=TCut("StrippingB2PhiGamma_B2VG_LineDecision==1");
  TCut radTrigger =  (trigger) ? radHLT2 && radHLT1 && radL0 && radStrip : TCut("1");
  
  // === CC TRIGGER & STRIPPING
  TCut psiL0    = TCut("1");
  TCut psiHLT1  = TCut("1");
  TCut psiHLT2  = TCut("1");
  TCut psiStrip = TCut("1");
  
  psiL0 = TCut("B_L0MuonDecision_TOS==1 || B_L0DiMuonDecision_TOS==1");
  psiHLT1 = TCut("B_Hlt1DiMuonHighMassDecision_TOS==1");
  if( selFlag == 2)psiHLT1 += TCut("(Kst_892_0_Hlt1TrackAllL0Decision_TOS==1)");
  if( selFlag == 8)psiHLT1 += TCut("(phi_1020_Hlt1TrackAllL0Decision_TOS==1)");
  psiHLT2 = TCut("B_Hlt2DiMuonJPsiHighPTDecision_TOS==1");
  psiStrip=TCut("StrippingFullDSTDiMuonJpsi2MuMuTOSLineDecision==1");
  TCut psiTrigger = (trigger) ? psiHLT2 && psiHLT1 && psiL0 && psiStrip : TCut("1");

  if( !trigger )std::cout << "===== WARNING : TRIGGER & STRIPPING DECISION IS NOT APPLIED ======" <<std::endl;


  //============================ Common part (cuts on B)
  //--- B selection

  if( maxDIRA < 0){
    if(selFlag==1)maxDIRA=0.04;
    if(selFlag==4)maxDIRA=0.06;
    if(selFlag==2)maxDIRA=(scaling ) ? 0.0231 : 0.04;
    if(selFlag==8)maxDIRA=(scaling ) ? 0.0337 : 0.06;
  }
  else 
    std::cout << "DIRA cut is set to " << maxDIRA << endl;



  std::ostringstream sdira("");
  sdira<<maxDIRA;
  TCut bCut = TCut(std::string("B_DiraAngle <"+sdira.str()).c_str());        // DIRA

  //bCut += TCut("(B_DiraAngle*B_DiraAngle)/B_DiraAngleError<0.055");

  bCut += TCut("B_PT>3000");                                            // pT(B) > 3 GeV/c  [ Sum_Pt(i) > 5 GeV/c in stripping ]
  bCut += TCut("B_IPCHI2_OWNPV<9.0");                                    // Chi2(IP(B))  < 9
  bCut += TCut("B_ENDVERTEX_CHI2 < 9");                                  // Chi2(Vtx(B)) < 9  !!!  NDOF = 1 here even for J/psiV(hh) !!!

  //==== lifetime fiducial
  bCut += TCut("abs(B_OWNPV_Z)<100");
  bCut += TCut("(nPVs== 1 || B_MINIPCHI2NEXTBEST>50)");


  if(selFlag==2){bCut +=TCut("abs(B_MM-5280)<40");}
  


  //--- Photon selection
  TCut radCut = TCut("gamma_CL > 0.25");                     // CL(gamma) > 0.25
  radCut += TCut("gamma_PP_IsPhoton>0.6" );              // isPhoton  > 0.6
  radCut += TCut("gamma_PT > 3000");                     // pT(gamma) > 3 GeV [2012 L0 threshold max = 2.96 GeV ]

  //--- Psi selection
  TCut psiCut = TCut("J_psi_1S_PT > 3000");                      // pT(gamma) > 3 GeV [2012 L0 threshold max = 2.96 GeV ]
  psiCut += TCut("muplus_PT>650 && muminus_PT>650");
  psiCut += TCut("muplus_P>10000 && muminus_P>10000");
  psiCut += TCut("muplus_PIDmu>0 && muminus_PIDmu>0");
  psiCut += TCut("muplus_TRACK_CHI2NDOF<5 && muminus_TRACK_CHI2NDOF<5");
  psiCut += TCut("abs(J_psi_1S_MM-3095)<80"); // asymmetrical cut 
  psiCut += TCut("abs(J_psi_1S_ENDVERTEX_CHI2)<20"); 

  psiCut += TCut("muplus_isMuon==1 && muminus_isMuon==1");
  psiCut += TCut("muplus_TRACK_GhostProb < 0.3  && muminus_TRACK_GhostProb < 0.3 ");

  //  psiCut += TCut("muplus_ProbNNmu>0.2 && muminus_ProbNNmu>0.2");

  // ==== SCALED IP/ISOL CUTS

  // loose default cut
  std::string IP1="55.0";
  std::string IP2="55.0";
  std::string ISOL="2.0";

  // selection cuts:
  // scaling should be rechecked after the change in the cut
  if(scaling){
    std::cout << " ===== SCALING APPLIED ===== " << std::endl;

    if(selFlag==1){IP1="55.0" ;IP2="55.0" ;ISOL="2.0";}    
    if(selFlag==2){IP1="47.8" ;IP2="52.38" ;ISOL="2.9";} 
    if(selFlag==4){IP1="55.0" ;IP2="55.0" ;ISOL="2.0";}   
    if(selFlag==8){IP1="38.73" ;IP2="38.73" ;ISOL="2.4";}
  }else{
    std::cout << " ===== WARNING : SCALING IS DISABLED ===== " << std::endl;
  }

  // no isolation cuts :
  //ISOL="-99";  


  // -- Vector selection
  TCut kstCut = vectorSel("Kplus","piminus","Kst_892_0","892","100",true,IP1,IP2,ISOL);
  kstCut += kaonID("Kplus") + pionID("piminus");
  kstCut += TCut("abs(Kst_892_0_cosThetaH) < 0.8");

  TCut phiCut = vectorSel("Kplus","Kminus","phi_1020","1020","15",true,IP1,IP2,ISOL);
  phiCut += kaonID("Kplus") + kaonID("Kminus");
  phiCut += TCut("abs(phi_1020_cosThetaH) < 0.8");

  

  std::cout<<"Loading selection ("<<selFlag<<")"<<std::endl;
  if(selFlag == 1)return bCut + radCut + kstCut + radTrigger ;//+ TCut("B_M01_Subst1_pi2K < 1075");
  if(selFlag == 2)return bCut + psiCut + kstCut + psiTrigger ;//+ TCut("B_M23_Subst3_pi2K < 1075");
  if(selFlag == 4)return bCut + radCut + phiCut + radTrigger ;//+ TCut("B_DiraAngle < 0.02");
  if(selFlag == 8)return bCut + psiCut + phiCut + psiTrigger;
  std::cout <<"unknown selection selFlag "<< selFlag <<std::endl; 
  return TCut("0"); 
}



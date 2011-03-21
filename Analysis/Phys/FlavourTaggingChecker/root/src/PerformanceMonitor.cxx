#include "PerformanceMonitor.h"
#include "globals.h"
#include "Tagger.h"
#include "taggingutils.h"

using namespace std;

PerformanceMonitor::PerformanceMonitor() {
  
  nsele=0;
  for(int i=0; i<20; ++i) {nrt[i]=0; nwt[i]=0; nrtag[i]=0; nwtag[i]=0;}

  nlm=0; nllm=0; nle=0; nlle=0; nlk=0; nlkS=0; nllk=0; 
  nllkS=0; nidm=0; nide=0; nidk=0; nidkS=0;  
  nghost_m=0; nghost_e=0; nghost_k=0;  nghost_kS=0;  ghrate_k=0; gherror_k=0;  
  ghrate_kS=0; gherror_kS=0;  
  ghrate_m=0; gherror_m=0; ghrate_e=0; gherror_e=0; 
  nrTisTis=0; nrTosTos=0; nrTisTos=0; nrTosTis=0; nrTob=0; 
  ntruemu=0; ntrueel=0; ntruekO=0; ntruekS=0;
  ntruemutag=0; ntrueeltag=0; ntruekOtag=0; ntruekStag=0;
  ndirect_mu=0;ndirect_el=0;nbc_mu=0;nbc_el=0;nbc_kO=0;nbc_kS=0;
  den_m= den_e= den_k= den_kS=0;

  nB511= nB521= nB531= nBother= nBanti= 0;
  ntrigL0= ntrigHlt1= ntrigHlt2= ntisL0= ntosL0= ntistosL0= 0;

  npartsinSV = nrSV = ninSVtrue = nkrec=0;
}

int PerformanceMonitor::N(){ return nsele; }

//=====================================================================
void PerformanceMonitor::increment( Event& event, FlavourTag* theTag ){

  nsele++;
  
  if(event.oppositeB()){
    int BOid = event.oppositeB()->absID();
    if(BOid==511)      ++nB511;
    else if(BOid==521) ++nB521;
    else if(BOid==531) ++nB531;
    else               ++nBother;
  }
  
  if( event.signalB()->ID() < 0)  ++nBanti;
  nkrec += event.pileup();

  //trigger stuff
  if( event.L0()  !=0) ++ntrigL0;
  if( event.Hlt1()!=0) ++ntrigHlt1;
  if( event.Hlt2()!=0) ++ntrigHlt2;
  if( event.L0TisTos() == 11 || event.L0TisTos() == 10 ) ++ntisL0;
  if( event.L0TisTos() == 11 || event.L0TisTos() ==  1 ) ++ntosL0;
  if( event.L0TisTos() == 11 ) ++ntistosL0;


  Taggers mytaggers = theTag->taggers();

  Tagger* tmuon = mytaggers.at(0);
  Tagger* tele  = mytaggers.at(1);
  Tagger* tkaon = mytaggers.at(2);
  Tagger* tsame = mytaggers.at(3);
  Tagger* tvtx  = mytaggers.at(4);

  int tmuon_dec = tmuon->decision();
  int tele_dec  = tele ->decision();
  int tkaon_dec = tkaon->decision();
  int tsame_dec = tsame->decision();
  int tvtx_dec  = tvtx ->decision();

  int truetag = theTag->TrueTag();
  if(tmuon_dec) {tmuon_dec == truetag ? nrtag[0]++ : nwtag[0]++;}
  if(tele_dec)  {tele_dec  == truetag ? nrtag[1]++ : nwtag[1]++;}
  if(tkaon_dec) {tkaon_dec == truetag ? nrtag[2]++ : nwtag[2]++;}
  if(tsame_dec) {tsame_dec == truetag ? nrtag[3]++ : nwtag[3]++;}
  if(tvtx_dec)  {tvtx_dec  == truetag ? nrtag[4]++ : nwtag[4]++;}

  int tagdec  = theTag->decision(); 
  int tagcat  = theTag->category();

  if(tagdec) {if(tagdec==truetag) nrt[tagcat]++; else nwt[tagcat]++;}
  if(tagdec) {if(tagdec==truetag) nrt[6]++; else nwt[6]++;} //add 6cat, all cat

  Particle* imuon = tmuon_dec? tmuon->taggerparts().at(0) : NULL;
  Particle* iele  = tele_dec ? tele ->taggerparts().at(0) : NULL;
  Particle* ikaon = tkaon_dec? tkaon->taggerparts().at(0) : NULL;
  Particle* isame = tsame_dec? tsame->taggerparts().at(0) : NULL;
    
  //counting
  isBs = event.isBs();

  Particles parts = event.particles();//candidate particles
  for(Particles::iterator i=parts.begin(); i!=parts.end(); ++i){
    int mcid=(*i)->absMCID();
    if( (*i)->fromB()==1 ) {
      if     (mcid==13)  ntruemu++;
      else if(mcid==11)  ntrueel++;
      else if(mcid==321) ntruekO++;
    } 
    else if(mcid==321 && (*i)->fromB()==0 && (*i)->xFlag()!=0) ntruekS++;

    if((*i)->p() > 5.0) { //purity and efficiency of MCID
      if     (mcid==13) {nlm++; if( checkPIDhypo(Particle::muon,*i) )    nllm++;} 
      else if(mcid==11) {nle++; if( checkPIDhypo(Particle::electron,*i)) nlle++;} 
      else if(mcid==321) {
        nlk++;  if( checkPIDhypo(Particle::kaon_opposite,*i) ) nllk++;
        nlkS++; if( checkPIDhypo(Particle::kaon_same,*i) )     nllkS++;
      }
    }
  }

  //pid purity
  if(imuon)  ++den_m;
  if(iele)   ++den_e;
  if(ikaon)  ++den_k;
  if(isBs) if(isame) ++den_kS;

  if(imuon)  {if(imuon->absMCID()==13)  nidm++; else if(imuon->MCID()==0) nghost_m++;}
  if(iele)   {if(iele ->absMCID()==11)  nide++; else if(iele ->MCID()==0) nghost_e++;}
  if(ikaon)  {if(ikaon->absMCID()==321) nidk++; else if(ikaon->MCID()==0) nghost_k++;}
  if(isBs) if(isame) 
  {if(isame->absMCID()==321) nidkS++;else if(isame->MCID()==0) nghost_kS++;}

  
  if(imuon) if(imuon->absMCID()==13 && imuon->fromB()==1) ntruemutag++;
  if(iele ) if(iele ->absMCID()==11 && iele ->fromB()==1) ntrueeltag++;
  if(ikaon) if(ikaon->absMCID()==321&& ikaon->fromB()==1) ntruekOtag++;
  if(isBs)  if(isame) 
    if(isame->absMCID()==321 &&
       isame->fromB()==0 && isame->xFlag()==0)  ntruekStag++;

  if(imuon) if(imuon->absMCID()==13 && imuon->fromB()==1
               && imuon->motherID() == imuon->ancestorID()) ndirect_mu++;
  if(iele ) if(iele ->absMCID()==11 && iele ->fromB()==1
               && iele ->motherID() == iele ->ancestorID()) ndirect_el++;

  //decay b -> c -> mu,e,K
  if(imuon) if(imuon->absMCID()==13 && imuon->fromB()==1
               && isD( (int) imuon->motherID() ) )  nbc_mu++;
  if(iele ) if(iele ->absMCID()==11 && iele ->fromB()==1
               && isD( (int) iele ->motherID() ) )  nbc_el++;
  if(ikaon) if(ikaon->absMCID()==321 && ikaon->fromB()==1)  nbc_kO++;
  if(isBs)  if(isame) if(isame->absMCID()==321 && isame->xFlag()) nbc_kS++;

  //SVertex
  if( ! event.getSecondaryVertices().empty()){
    Particles pinSV = event.getSecondaryVertices().at(0)->outgoingParticles();
    npartsinSV += pinSV.size();
    if(!pinSV.empty()) nrSV++; //SV exists
    if( event.getSecondaryVertices().at(0)->type() == Vertex::Secondary ){
      for(Particles::iterator i=pinSV.begin(); i!=pinSV.end(); ++i){
        if( (*i)->fromB()==1 ) ninSVtrue++; 
      }
    }
  }

  //check if decision is the same for DV
  if(property["checkDV"]==true) {
    int dvflag= event.taggersDecisionFromDV();
    int myflag=0;
    if(tmuon_dec) myflag += 10000*(tmuon_dec+2);
    if(tele_dec ) myflag +=  1000*(tele_dec+2 ); 
    if(tkaon_dec) myflag +=   100*(tkaon_dec+2);
    if(tsame_dec) myflag +=    10*(tsame_dec+2); 
    if(tvtx_dec ) myflag +=     1*(tvtx_dec+2 );
    
    if(myflag!=dvflag || fabs(theTag->omega() - event.StdOmega()) >0.001)
      warning()<<"diff "<<setw(7)<<event.eventNr()
               <<setw(3)<<theTag->decision()<<setw(3)<<theTag->category()
               <<setw(4)<<tmuon_dec<<setw(3)<<tele_dec<<setw(3)
               <<setw(3)<<tkaon_dec<<setw(3)<<tsame_dec<<setw(3)<<tvtx_dec
               <<setw(7)<<myflag <<setw(7)<<dvflag
               <<"   w= "<<theTag->omega()<<"  "<<event.StdOmega()<<endmsg;
  } 


}


//=============================================================================
void PerformanceMonitor::printStats() {
 
  int maxnrofcat = 11;

  if(nsele==0) { warning()<<"No events selected."<<endmsg; return; }

  cout <<BOLD<<"\n=========================================================\n";  
  cout <<ENDC<<setprecision(3);
  cout <<"Trigger eff: L0="<<setw(5)<<ntrigL0/nsele*100
       <<"\tHlt1="<<setw(5)<<ntrigHlt1/nsele*100
       <<"\tHlt2="<<setw(5)<<ntrigHlt2/nsele*100<<" %"<<endl;
  cout <<"Fraction of TIS="<<setw(5)<< ntisL0/nsele*100
       <<"\tTOS="   <<setw(5)<< ntosL0/nsele*100
       <<"\tTISTOS="<<setw(5)<< ntistosL0/nsele*100<<" %"<<endl;
  cout <<"Fraction of  Bd="<<setw(5)<< nB511/nsele*100
       <<"\tBu="<<setw(5)<< nB521/nsele*100
       <<"\tBs="<<setw(5)<< nB531/nsele*100<<" %"<<endl;
  cout <<"Fraction of  anti-B="<<setw(5)<< nBanti/nsele*100<<"%"
       <<"\tnr.PV="<<setw(5)<<nkrec/nsele<<endl;

  if(ntruekO) {
    cout <<BOLD<<"=========================================================\n";  
    cout<<"Fraction in event sample of true particles from B:"<<endl;
    cout<<ENDC<<setprecision(3)
        <<"mu= "<<float(ntruemu)/nsele*100
        << "%\t selected as tagger: " <<float(ntruemutag)/nsele*100<<"%"<<endl;
    cout<<"el= "<<float(ntrueel)/nsele*100
        << "%\t selected as tagger: " <<float(ntrueeltag)/nsele*100<<"%"<<endl;
    cout<<"kO= "<<float(ntruekO)/nsele*100
        << "%\t selected as tagger: "<<float(ntruekOtag) /nsele*100<<"%"<<endl;
    if(isBs)
      cout<<"kS= "<<float(ntruekS)/nsele*100
          << "%\t selected as tagger: "<<float(ntruekStag) /nsele*100<<"%"<<endl;
    cout<<BOLD<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    cout<<BOLD<<"Fraction in tagger sample of:\n"<<ENDC;
    cout<<"b->mu= "   <<ndirect_mu /den_m*100<<"%\t";
    cout<<"b->c->mu= "<<nbc_mu     /den_m*100<<"%\n";
    cout<<"b->e = "   <<ndirect_el /den_e*100<<"%\t";
    cout<<"b->c->e = "<<nbc_el     /den_e*100<<"%\n";
    cout<<"b->K = "   <<nbc_kO     /den_k*100<<"%\t";
    if(isBs)cout<<"b->Ksame= "<<nbc_kS/den_kS*100<<"%";
    cout<<endl;
  }

  if(npartsinSV && nrSV) {
    cout<<BOLD<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
    cout<<"Inclusive SV:"<<ENDC<<"  eff.=" <<setw(5)<< nrSV/nsele*100 << "%"
        << "\tpurity=" <<setw(5)<< ninSVtrue/float(npartsinSV)*100<< "%" <<endl;
    cout<<"mean nr. of tracks = "<<setw(4)<< npartsinSV/float(nrSV) <<endl;
  }

  // calculate effective efficiency in various categories
  double rtt=0, wtt=0;
  double rtag,wtag,utag;
  double ef_tot=0, effe_tot=0;
  double epsilerr, epsilerrtot=0;
  
  if(ntruekO) {
    if(den_m) ghrate_m = nghost_m      /den_m*100;
    if(den_m) gherror_m= sqrt(nghost_m)/den_m*100;
    if(den_e) ghrate_e = nghost_e      /den_e*100;
    if(den_e) gherror_e= sqrt(nghost_e)/den_e*100;
    if(den_k) ghrate_k = nghost_k      /den_k*100;
    if(den_k) gherror_k= sqrt(nghost_k)/den_k*100;
    if(den_kS)ghrate_kS =nghost_kS     /den_kS*100;
    if(den_kS)gherror_kS=sqrt(nghost_kS)/den_kS*100;

    cout <<BOLD<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<ENDC;
    cout<<setprecision(3);
    if(den_m) cout<<"Muon Ghosts = "<<ghrate_m<<" +- "<<gherror_m<<" %"<<endl;
    if(den_e) cout<<"Elec        = "<<ghrate_e<<" +- "<<gherror_e<<" %"<<endl;
    if(den_k) cout<<"Kaon_opp    = "<<ghrate_k<<" +- "<<gherror_k<<" %"<<endl;
    if(nghost_kS && den_kS)
      cout<<  "Kaon_same   = "<<ghrate_kS<<" +- "<<gherror_kS<<" %"<<endl;
    cout <<BOLD<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    cout <<setprecision(3)<<"PID purity (in r+w), and efficiency (P>5GeV)\n"<<ENDC;
    if(den_m) cout <<"muon:\t"<<nidm/den_m*100
                   <<"\t\t"   <<nllm/nlm*100<<" %"<<endl;
    if(den_e) cout <<"elec:\t"<<nide/den_e*100
                   <<"\t\t"   <<nlle/nle*100<<" %"<<endl;
    if(den_k) cout <<"kaon:\t"<<nidk/den_k*100
                   <<"\t\t"   <<nllk/nlk*100<<" %"<<endl;
    if(isBs && den_kS)
      cout <<"kaonS:\t"       <<nidkS/den_kS*100
           <<"\t\t"           <<nllkS/nlkS*100<<" %"<<endl;
    
  }
  cout<<BOLD<<"=========================================================\n";
  cout<< " Category            EFF.          Etag         Wrong TF"
      << FAINT <<"      r       w\n"<<ENDC;

  for( int it=1; it < 19; it++ ) {
    rtag = wtag = 0; 
    string cats;
    if(it<maxnrofcat)  cats =  "  Cat    "; 
    if(it<maxnrofcat) { rtag = nrt[it]; wtag = nwt[it]; }
    if(it==maxnrofcat)
      cout<<"---------------------------------------------------------\n";
    if(it==maxnrofcat+1){cats =  "  OS muons"; rtag = nrtag[0]; wtag = nwtag[0]; }
    if(it==maxnrofcat+2){cats =  "  OS elect"; rtag = nrtag[1]; wtag = nwtag[1]; }
    if(it==maxnrofcat+3){cats =  "  OS kaons"; rtag = nrtag[2]; wtag = nwtag[2]; }
    if(it==maxnrofcat+4){cats =  "  SS pions"; rtag = nrtag[3]; wtag = nwtag[3]; }
    if(isBs)if(it==maxnrofcat+4)
    {cats ="  SS kaons"; rtag= nrtag[3]; wtag= nwtag[3]; }
    if(it==maxnrofcat+5){cats =  "  VertexCh"; rtag = nrtag[4]; wtag = nwtag[4]; }
    //if(it==maxnrofcat+6){cats =  "  Fragment"; rtag = nrtag[5]; wtag = nwtag[5]; }
 
    if(rtag+wtag == 0) continue; //empty category

    utag = nsele-rtag-wtag;       // untagged
    double omtag = wtag/(rtag+wtag);
    double eftag = (rtag+wtag)/nsele;           // tagging efficiency
    double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
    if(rtag<wtag) epsil= -epsil;

    if((it<maxnrofcat) and (it!=6)){ //not count the average cat
      rtt      += rtag;
      wtt      += wtag;
      ef_tot   += eftag;
      effe_tot += epsil;
    }

    //errors on efficiency and omega
    double eftag_err= sqrt((rtag*utag + utag*wtag)/nsele)/nsele;
    double omtag_err= sqrt( rtag*wtag /(rtag+wtag) ) / (rtag+wtag);
    epsilerr = sqrt((pow(rtag - wtag,2)*
                     (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
                      *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
                    /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
    if(it<maxnrofcat) epsilerrtot = sqrt(pow(epsilerrtot,2)+pow(epsilerr,2));

    //PRINT: ----------------------------------
    cout.setf(ios::fixed);
    //    if((it<maxnrofcat) and (it!=6)) cout<<VIOLETA<<setw(2)<< it; else cout<<AZUL<<"**";
    if (it!=6){
      if(it<maxnrofcat) cout<<VIOLETA<<setw(2)<< it; else cout<<AZUL<<"**";
      cout<<BOLD<< cats<<setprecision(2)
	  <<" "<<setw(8)<< epsil*100 << "+-" << epsilerr*100 
	  <<" "<<setw(8)<< eftag*100 << "+-" <<eftag_err*100
	  <<" "<<setw(8)<< omtag*100 << "+-" <<omtag_err*100
	  <<" "<<ENDC<<FAINT<<setw(7)<< (int) rtag
	  <<" "<<setw(7)<< (int) wtag<<ENDC
	  << endl;
    }
    if(it==6) {
      cout<<ROJO<<setw(2)<<"Average    "<<setprecision(2)
	  <<" "<<setw(8)<< epsil*100 << "+-" << epsilerr*100 
	  <<" "<<setw(8)<< eftag*100 << "+-" <<eftag_err*100
	  <<" "<<setw(8)<< omtag*100 << "+-" <<omtag_err*100
	  <<" "<<ENDC<<FAINT<<setw(7)<< (int) rtag
	  <<" "<<setw(7)<< (int) wtag<<ENDC
	  << endl;
    }
  }

  //calculate global tagging performances -------------------------------

  //equivalent value of the wrong tag fraction 
  double avw_invert=(1-sqrt(fabs(effe_tot)/ef_tot))/2;
  if(effe_tot<0) avw_invert= 1-avw_invert;
  double utt = nsele-rtt-wtt;
  double eftot_err= sqrt((rtt*utt + utt*wtt)/nsele)/nsele;
  double avw_invert_err= sqrt( rtt*wtt /(rtt+wtt) ) / (rtt+wtt);

  cout << "---------------------------------------------------------\n";
  cout << "Tagging efficiency =  "<<setw(5)
       << ef_tot*100 << " +- "<<eftot_err*100<< " %"<< endl;       
  cout << "Wrong Tag fraction =  "<<setw(5)
       << avw_invert*100 << " +- " <<avw_invert_err*100 << " %"<< endl;
  cout << ROJO <<BOLD<< "EFFECTIVE COMB. TE =  "<<setw(5)
       << effe_tot*100 << " +- "<<epsilerrtot*100<< " %" <<ENDC<<FAINT
       << "    (Events = " << int(nsele) <<")"<<ENDC<< endl;
  cout <<BOLD<< 
    "=========================================================\n\n"<<ENDC;

}


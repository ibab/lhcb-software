/*---------------------------------------------------------------------------
 This program reads the output logfiles of 
 YourSelection + BTaggingTool + BTaggingMonitor
 and gives tagging performances with correct statistical errors

 To run it:
> c++ summaryof.cpp -o summaryof
> ./summaryof mylogfile.txt 3 N

-----------------------------------------------------------------------------
    Caveat (by O. Awunor):                                               
    Be aware that since this program is designed to read in the output       
    from the Monitor algorithms, it may depend on those. 
    Some event candidates are output twice by the Monitoring 
    algorithm for cases where 2 genuine B canditates are stored in TES.   
    When use with DoubleTagging,                                             
    it cant distinguish between OSChecker output and AllChecker output.     
    Therefore it counts all events twice, hence doubling the total number of 
    events.                                                                  
                                                                             
-----------------------------------------------------------------------------
*/

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <iomanip>

using namespace std;

int main(int argc, char **argv) {

  int nrt[20], nwt[20];
  int nrt_m=0, nwt_m=0;
  int nrt_e=0, nwt_e=0;
  int nrt_k=0, nwt_k=0;
  int nrt_s=0, nwt_s=0;
  int nrt_v=0, nwt_v=0;
  int run, event, tag,truetag,categ,Trig=-1,L0,HLT,nsele=0;
  int bid,fe,fm,fk,fS,fV;
  string flagstring;
  bool MCBpass=false, MONpass=false, TAGpass=false;
  for(int it=1; it!=20; ++it) nrt[it]=nwt[it]=0;
 
  ifstream in(argv[1]);
  if(!in) { cout<<"File does not exist! \n\n"; return 0; }
  if(argc<4){
    cout << "Usage: summaryof  mylogfile.txt  NR  TYPE" << endl;
    cout << "       NR=1   -> Before triggers  " << endl;
    cout << "       NR=2   -> After L0 trigger " << endl;
    cout << "       NR=3   -> After L0*HLT triggers"    << endl;
    cout << "       TYPE=T -> TDR categories " << endl;
    cout << "       TYPE=N -> NNet categories" << endl;
    return 0;
  }

  //LOOP ---
  while(!in.eof()) {

    in >> flagstring;    //read in from file
    if(flagstring != "BTAGGING") continue; else in >> flagstring;  

    if(flagstring == "MCB") { //reads in from BTaggingAnalysis
      in >> bid;
      truetag = bid>0? 1:-1;
      MCBpass=true;
    }
    else if(flagstring == "MON") { //reads in from BTaggingMonitor
      in >> truetag;
      MONpass=true;
    }
    else if(flagstring == "TAG") { //reads in tagging info from BTagging
      in >> run >> event >> tag >> categ;
      in >> fm >> fe >> fk >> fS >> fV; //taggers
      TAGpass=true;
    } 
    if( !( (MCBpass||MONpass) && TAGpass ) ) continue;
    MCBpass=MONpass=TAGpass=false;
//  cout<<flagstring<<" "<< tag <<" "<< truetag 
// 	<<"     "<< fm  <<" "<< fe<<" " << fk<<" " << fS<<" " << fV <<endl;

    //discard untriggered 
    if(Trig>-1) {
      HLT = (int) Trig/10;
      L0  = (int) Trig-10*HLT;
      if(*argv[2]=='2') if( !L0 ) continue;
      if(*argv[2]=='3') if( !L0 || !HLT ) continue;
    }

    //------------- event passes: count right and wrongs
    nsele++;

    if(tag== truetag)  nrt[categ]++; 
    if(fm == truetag)  nrt_m++;
    if(fe == truetag)  nrt_e++;
    if(fk == truetag)  nrt_k++;
    if(fS == truetag)  nrt_s++;
    if(fV == truetag)  nrt_v++;

    if(tag==-truetag)  nwt[categ]++; 
    if(fm ==-truetag)  nwt_m++;
    if(fe ==-truetag)  nwt_e++;
    if(fk ==-truetag)  nwt_k++;
    if(fS ==-truetag)  nwt_s++;
    if(fV ==-truetag)  nwt_v++;

  } //end of loop

  // calculate effective efficiency in various categories
  double rtt=0, wtt=0;
  double rtag,wtag,utag;
  double ef_tot=0, effe_tot=0;
  double epsilerr, epsilerrtot=0;

  cout << "\n========================================================================"<<endl;
  cout << "Summary tagging performance for: " << argv[1] <<endl;
  if(Trig == -1) cout << "Trigger info not available in file." << endl;
  else if(*argv[2] == '1') cout << "Before triggers, " << endl;
  else if(*argv[2] == '2') cout << "After L0 trigger only, "<< endl;
  else if(*argv[2] == '3') cout << "After L0*HLT triggers, " <<endl;
  else { cout << "Wrong argument NR: " <<*argv[2]<<endl<<endl; return 0; }
  cout << "------------------------------------------------------------------------"<<endl;
  cout<< " Category            EFF.          Etag         Wrong TF"
      << "      r       w\n";

  for( int it=1; it != 20; it++ ) {
    rtag = wtag = 0; 
    string cats;
    if(it== 1) cats =  "   mu only";
    if(it== 2) cats =  "    e only";
    if(it== 3) cats =  "    k only";
    if(it== 4) cats =  "    mu + k";
    if(it== 5) cats =  "     e + k";
    if(it== 6) cats =  "  vtx only";
    if(it== 7) cats =  "     ps/ks";
    if(it== 8) cats =  "   mu + ks";
    if(it== 9) cats =  "    e + ks";
    if(it==10) cats =  "    k + ks";
    if(it==11) cats =  "   mu+k+ks";
    if(it==12) cats =  "    e+k+ks";
    if(it==13) { cats =  "     muons"; rtag = nrt_m; wtag = nwt_m; }
    if(it==14) { cats =  " electrons"; rtag = nrt_e; wtag = nwt_e; }
    if(it==15) { cats =  "  OS kaons"; rtag = nrt_k; wtag = nwt_k; }
    if(it==16) { cats =  "  SS pi/k "; rtag = nrt_s; wtag = nwt_s; }
    if(it==17) { cats =  "  VertexCh"; rtag = nrt_v; wtag = nwt_v; }
    if(*argv[3] != 'T' && it<13) cats = "  NNet    "; 
    else if(it==13) 
      cout<<"------------------------------------------------------------------------"<<endl;

    int itmax=6;
    if(*argv[3] == 'T') itmax=13;
    if(it<itmax) { rtag = nrt[it]; wtag = nwt[it]; }

    if(rtag+wtag == 0) continue; //empty category

    utag = nsele-rtag-wtag;       // untagged
    double omtag = wtag/(rtag+wtag);
    double eftag = (rtag+wtag)/nsele;           // tagging efficiency
    double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
    if(rtag<wtag) epsil= -epsil;

    if(it<itmax){
      rtt      += rtag;
      wtt      += wtag;
      ef_tot   += eftag;
      effe_tot += epsil;
    }

    epsilerr = sqrt((pow(rtag - wtag,2)*
		     (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
		      *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
		    /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
    if(it<itmax) epsilerrtot = sqrt(pow(epsilerrtot,2)+pow(epsilerr,2));

    //errors on efficiency and omega
    double eftag_err= sqrt((rtag*utag + utag*wtag)/nsele)/nsele;
    double omtag_err= sqrt( rtag*wtag /(rtag+wtag) ) / (rtag+wtag);


    //PRINT: ----------------------------------
    cout.setf(ios::fixed);
    if(it<itmax) cout<<setw(2)<< it; else cout<<"**";
    cout<< cats
        <<" "<<setprecision(2)<<setw(8)<< epsil*100 << "+-" << epsilerr*100 
        <<" "<<setw(8)<< eftag*100 << "+-" <<eftag_err*100
        <<" "<<setprecision(1)<<setw(8)<< omtag*100 << "+-" <<omtag_err*100
        <<" "<<setw(7)<< (int) rtag
        <<" "<<setw(7)<< (int) wtag
	<< endl;
  }

  //calculate global tagging performances -------------------------------

  //equivalent value of the wrong tag fraction 
  double avw_invert=(1-sqrt(fabs(effe_tot)/ef_tot))/2;
  if(effe_tot<0) avw_invert= 1-avw_invert;
  double utt = nsele-rtt-wtt;
  double eftot_err= sqrt((rtt*utt + utt*wtt)/nsele)/nsele;
  double avw_invert_err= sqrt( rtt*wtt /(rtt+wtt) ) / (rtt+wtt);

  cout << "------------------------------------------------------------------------"<<endl;;
  cout << "Tagging efficiency =  "<<setprecision(2)<<setw(5)
       << ef_tot*100 << " +/- "<<eftot_err*100<< " %"<< endl;
  cout << "Wrong Tag fraction =  "<<setprecision(2)<<setw(5)
       << avw_invert*100 << " +/- " <<avw_invert_err*100 << " %"<< endl;
  cout << "EFFECTIVE COMB. TE =  "<<setprecision(2)<<setw(5)
       << effe_tot*100 << " +/- "<<epsilerrtot*100<< " %"
       << "      (Total events= "<<nsele<<")"<< endl;
  cout << "========================================================================\n"<<endl;
}

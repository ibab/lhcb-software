// This program reads the output text of BTagging
// and gives tagging performances with statistical errors
// To run it:
// c++ summaryof.cpp -o summaryof; summaryof taginfo 3
// (this command could be useful:
//  grep -ih tagginginfo *txt | sed -e "s/BTagging //" | sed -e "s/INFO TAGGINGINFO  //" >! taginfo

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <iomanip>

#define NCATEG 12

using namespace std;
 
int nrt[NCATEG+1], nwt[NCATEG+1];
int nrt_m=0, nwt_m=0;
int nrt_e=0, nwt_e=0;
int nrt_k=0, nwt_k=0;
int nrt_ks=0, nwt_ks=0;
int Run,Event,tag,truetag,categ,L0,L1,knrec,nsele=-1;
string dummystring;
bool mtag,etag,ktag,kstag;

int main(int argc, char **argv) {

  ifstream in(argv[1]);
  if(!in) { cout<<"File does not exist! \n\n"; return 0; }
  if(argc<3){
    cout << "Usage: summaryof tagfile.txt NR " << endl;
    cout << "       NR=1 -> Before triggers  " << endl;
    cout << "       NR=2 -> After L0 trigger " << endl;
    cout << "       NR=3 -> After L0*L1 " << endl;
    return 0;
  }

  bool RequireOnly1ReconstrPV = false;

  while(!in.eof()) {

    //in >> dummystring; in >> dummystring; in >> dummystring;
    in >> Run;
    in >> Event;
    in >> tag;
    in >> truetag;
    in >> categ;
    in >> L0; in >> L1;
    in >> knrec;

    //---------------------------------- cuts
    if((*argv[2]=='2' || *argv[2]=='3') && L0 == 0 ) continue;
    if( *argv[2]=='3' && L1 == 0 ) continue;
    if( RequireOnly1ReconstrPV && knrec != 1 ) continue; 
    //----------------------------------

    nsele++;

    mtag=etag=ktag=kstag=false;
    if(categ==1 || categ==4 || categ==8 || categ==11) mtag=true;
    if(categ==2 || categ==5 || categ==9 || categ==12) etag=true;
    if(categ==3 || categ==4 || categ==5 || categ >9 ) ktag=true;
    if(categ >6 && categ <13) kstag=true;

    if(tag == truetag) {
      nrt[categ]++; 
      if(mtag)  nrt_m++;
      if(etag)  nrt_e++;
      if(ktag)  nrt_k++;
      if(kstag) nrt_ks++;
    }
    else if(tag ==-truetag) {
      nwt[categ]++; 
      if(mtag)  nwt_m++;
      if(etag)  nwt_e++;
      if(ktag)  nwt_k++;
      if(kstag) nwt_ks++;
    }
  }

  // calculate effective efficiency in various categories
  double rtt=0,wtt=0;
  double rtag,wtag,utag;
  double ef_tot=0,effe_tot=0;
  double epsilerr, epsilerrtot=0;

  cout << "\n======================================================="<<endl;
  cout << "Summary for: " << argv[1] <<endl;
  if(*argv[2] == '1'){
    cout << "Before triggers, " << endl;
  }
  else if(*argv[2] == '2'){
    cout << "After L0 trigger only, "<< endl;
  } 
  else if(*argv[2] == '3') {
    cout << "After L0 and L1 triggers, " <<endl;
  }
  else { cout << "Wrong argument: " <<*argv[2]<<endl; return 0; }
  if(!RequireOnly1ReconstrPV)  cout<<"any nr of primary vertices"<<endl;
  else cout<< "Only one reconstructed Primary Vertex"<<endl;
  cout << "Total nr of events =  "<<setw(5) << nsele << endl;
  cout << "---------------------------------------------------------"<<endl;

  cout<< " Category            EFF.          Etag         Wrong TF"
      << "      r       w       \n";

  for( int it=1; it < (NCATEG+1)+4; it++ ) {

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
    if(it==13) 
      cout << "---------------------------------------------------------"<<endl;
    if(it==13) { cats =  "     muons"; rtag = nrt_m; wtag = nwt_m; }
    if(it==14) { cats =  " electrons"; rtag = nrt_e; wtag = nwt_e; }
    if(it==15) { cats =  "  OS kaons"; rtag = nrt_k; wtag = nwt_k; }
    if(it==16) { cats =  "  SS pi/k "; rtag = nrt_ks; wtag = nwt_ks; }

    if(it<NCATEG+1) { rtag = nrt[it]; wtag = nwt[it]; }

    if((rtag+wtag) == 0) continue; //empty category

    utag = nsele-rtag-wtag;       // untagged
    double omtag = wtag/(rtag+wtag);
    double eftag = (rtag+wtag)/nsele;           // tagging efficiency
    double epsil = eftag*pow(1-2*omtag,2);      // effective efficiency
    if(rtag<wtag) epsil= -epsil;

    if(it<NCATEG+1){
      rtt      += rtag;
      wtt      += wtag;
      ef_tot   += eftag;
      effe_tot += epsil;
    }

    //errors on efficiency and omega  
    double eftag_err=
      sqrt((rtag*utag + 2*rtag*wtag + utag*wtag)/nsele)/nsele;
    double omtag_err=  
      sqrt((rtag*wtag*(rtag*rtag + rtag*utag + wtag*(utag + wtag)))/
	   ( pow(rtag + wtag,4) *(rtag + utag + wtag)));

    epsilerr = sqrt((pow(rtag - wtag,2)*
		     (-(pow(rtag - wtag,2)*(rtag +wtag))+nsele
		      *(pow(rtag,2) +14*rtag*wtag+ pow(wtag,2))))
		    /(pow(rtag+wtag+utag,3)*pow(rtag + wtag,3)));
    epsilerrtot=sqrt(pow(epsilerrtot,2)+pow(epsilerr,2));

    cout.setf(ios::fixed);
    if(it<NCATEG+1) cout<<setw(2)<< it; else cout<<"**";
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
  double eftot_err=
    sqrt((rtt*utt + 2*rtt*wtt + utt*wtt)/nsele)/nsele;
  double avw_invert_err=  
    sqrt((rtt*wtt*(rtt*rtt + rtt*utt + wtt*(utt + wtt)))/
	 ( pow(rtt + wtt,4) *(rtt + utt + wtt)));

  cout << "---------------------------------------------------------"<<endl;
  cout << "Tagging efficiency =  "<<setprecision(2)<<setw(5)
       << ef_tot*100 << " +/- "<<eftot_err*100<< " %"<< endl;
  cout << "Wrong Tag fraction =  "<<setprecision(2)<<setw(5)
       << avw_invert*100 << " +/- " <<avw_invert_err*100 << " %"<< endl;
  cout << "EFFECTIVE COMB. TE =  "<<setprecision(2)<<setw(5)
       << effe_tot*100 << " +/- "<<epsilerrtot*100<< " %"<< endl;
  cout << "========================================================="<<endl;
}

// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:57 GMT
#include "Utils.h"
#include <iostream>
#include <string>

#include "GaussFct.h"
#include "TH1F.h"
#include "TFile.h"

using namespace std;
using namespace MINT;

int testStringStuff(){
  string abc = "abcdefg";

  const int N=7;
  string x[N] = {"ab", "fg","g", "abcdefg", "h", "dg", "de"};

  for(int i=0; i<N; i++){
    cout << " is " << x[i] << " in " << abc << " ? "
	 << (A_is_in_B(x[i], abc) ?  "yes"  : "no" )
	 << endl;
  }
  return 0;
}

int testGauss(){
  
  GaussFct g(0, 1, 0, 1);

  int bins=100;
  double mi=-5, ma=5;
  double bw = (ma - mi)/((double)bins);
  TH1F histo("GaussFct", "GaussFct", bins, mi, ma);

  for(int i=0; i< bins; i++){
    double x = mi + (0.5 + (double) i)*bw;
    g.set_x(x);
    histo.Fill(x, g.getVal());
  }
  TFile f("GaussFct.root", "RECREATE");
  f.cd();
  histo.Write();
  f.Close();


  return 0;

}

int main(){
  //  return testStringStuff();
  return testGauss();
}

//

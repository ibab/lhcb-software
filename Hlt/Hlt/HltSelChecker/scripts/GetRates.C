#include <iostream>
#include <cmath>
#include <vector>
#include "TChain.h"
#include "stdlib.h" // to get atoi
using namespace std ;

void SelsPlots(TChain*);
TChain* MakeChain(TString head, int max, TString loc = "TriggerTuple/EventTuple");

int main(int argc, char** argv){
  cout << "Hello world " << argc << endl ;
  if ( argc!=3 ) {
    cout << "./GetRates.exe Tuples/Hlt12-StatsTuple-305- 30" << endl ;
    return -9 ;
  }
  
  TString head = argv[1];
  int maxI = atoi(argv[2]);
  cout << head << " " << maxI << endl ;
  TChain* MB = MakeChain(head,maxI);
  cout << "2" << endl ;
  SelsPlots(MB);
  return 0;
}


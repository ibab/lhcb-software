#ifndef  MUONALGS_ComaparePC_H
#define  MUONALGS_ComaprePC_H 1

#include "MuonAlgs/MuonPhysicalChannel.h"
#include <iostream>

inline bool ComparePC(MuonPhysicalChannel* p1, MuonPhysicalChannel* p2){
  bool test=false;
//  cout<<"primo "<<(p1->phChID()->getFETile())<<" secondo "<<(p2->phChID()->getFETile())<<endl;
	if(((p1->phChID()->getFETile())==(p2->phChID()->getFETile())))test=true;
//   cout<<"test "<<test<<endl;
  return test ;
}

#endif

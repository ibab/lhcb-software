#ifndef  MUONALGS_ComparePC_H
#define  MUONALGS_ComparePC_H 1

#include "MuonPhysicalChannel.h"
#include <iostream>

inline bool ComparePC(MuonPhysicalChannel* p1, MuonPhysicalChannel* p2){
  bool test=false;
	if(((p1->phChID()->getFETile())==(p2->phChID()->getFETile())))test=true;
  return test ;
}

#endif

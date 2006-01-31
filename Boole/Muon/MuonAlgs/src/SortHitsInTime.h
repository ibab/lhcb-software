#ifndef  MUONALGS_SortHitsInTime_H
#define  MUONALGS_SortHitsInTime_H 1


 
#include <iostream>
#include <functional>
#include "MuonHitTraceBack.h"
class SortHitsInTime :
public std::binary_function<MuonHitTraceBack,MuonHitTraceBack,bool>{
public : 
bool operator()(  const MuonHitTraceBack& first, const MuonHitTraceBack& second )
	const; 
};


inline bool SortHitsInTime::operator()(const MuonHitTraceBack& first, 
const MuonHitTraceBack& second )const
	{ 
//	   bool ale=(first.hitArrivalTime()>second.hitArrivalTime());
//	   cout<<"alessia "<<first.hitArrivalTime()<<" "<<second.hitArrivalTime()<<
//		 " "<<ale <<endl;
 	    return (first.hitArrivalTime()<second.hitArrivalTime());
  	} 
	
#endif 

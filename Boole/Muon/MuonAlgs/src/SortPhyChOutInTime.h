#ifndef  MUONALGS_SortPhyChOutInTime_H
#define  MUONALGS_SortPhyChOutInTime_H 1


 
#include <iostream>
#include <functional>
#include "MuonHitTraceBack.h"
class SortPhyChOutInTime :
public std::binary_function<std::pair<float,MuonCardiacTraceBack>,std::pair<float,MuonCardiacTraceBack>,bool>{
public : 
bool operator()(  const std::pair<float,MuonCardiacTraceBack>& first, const std::pair<float,MuonCardiacTraceBack>& second )
	const; 
};


inline bool SortPhyChOutInTime::operator()(const std::pair<float,MuonCardiacTraceBack>& one, 
const std::pair<float,MuonCardiacTraceBack>& two )const
	{ 
//	   bool ale=(one.first>two.first);
//	   std::cout<<"alessia "<<one.first<<" "<<two.first<<
//		 " "<<ale <<std::endl;
 	    return (one.first<two.first);
  	} 
	
#endif 

#ifndef  MUONALGS_SortPhPreInput_H
#define  MUONALGS_SortPhPreInput_H 1

#include "MuonPhPreInput.h"
#include <iostream>
#include <functional>

class SortPhChID :
public std::binary_function<MuonPhPreInput,MuonPhPreInput,bool>{
public : 
bool operator()(  const MuonPhPreInput& first, const MuonPhPreInput& second )
	const; 
};


inline bool SortPhChID::operator()(const MuonPhPreInput& first, 
const MuonPhPreInput& second )const
	{ 
//      static int count=0;
//	if(first!=0&&second!=0){
//  	  if(first->phChID()!=0&&second->phChID()!=0){
//		   count++;
//	           cout<<"count"<<count<<endl;
//                   cout<<"first"<<first<<endl;
//                  cout<<"second"<<second<<endl;
//                 MuonPhChID* pippo=first.phChID();
//								 MuonPhChID* pluto=second.phChID();
//                   cout<<"first to chid"<<
//                     (first.phChID())<<" "<<
//											pippo<<
//                     first.phChID()->getID()<<endmsg;
//                   cout<<"second to chid "<<
//                     second.phChID()<<"" <<
//                     second.phChID()->getID()<<endmsg; 

	    return (first.phChID()->getID())>=(second.phChID()->getID());
//			}
//	  else{cout<<"scema uno"<<endl;} 
//	}
//	else{
//	cout<<"scema due"<<endl;
//	}
 	} 
	
#endif 

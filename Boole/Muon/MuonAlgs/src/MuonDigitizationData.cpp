#ifndef MUONALGS_MUONDIGITIZATIONDATA_CPP
#define MUONALGS_MUONDIGITIZATIONDATA_CPP 1


#include "Event/MCMuonHit.h"
#include "MuonAlgs/MuonDigitizationData.h"    
#include "MuonAlgs/MuonPhyChannelInput.h"    
#include "MuonAlgs/MuonPhysicalChannel.h" 
#include "MuonAlgs/MuonPhysicalChannelOutput.h" 

template<class T> 
MuonDigitizationData<T>::MuonDigitizationData(const std::string &path, 
                                              MsgStream* mlog, 
                                              IDataProviderSvc*
																							eveSvc,const std::string 
                                              &specpath)
{
  //  MsgStream log(msgSvc(), name());
  s_basePathInTES = path ;
  m_registered = 0 ;
  log = (mlog);
  eventSvc=eveSvc ;
	s_specificPath=specpath ;
}


template<class T> MuonDigitizationData<T>::~MuonDigitizationData(){
  if(m_registered=0){
    *log << MSG::INFO 
         <<" the contatiners have not been registed in the TES"
         << " => deleting objects "<<endreq;
    KeyedContainer<T>::iterator it;
    for(int i=0;i<MuonDigitizationData_m_partitionNumber;i++){
      for(it=muonDataPartition[i].begin();it<muonDataPartition[i].end();it++)
        delete  (*it);
    }
  }
  else if(m_registered>0){
    *log << MSG::INFO 
         <<" the contatiners have been registed in the TES " <<endreq;
  }
  else if(m_registered<0){
    *log << MSG::INFO
         << " the contatiners have not been registed in the TES"
         << " for some error."
         << endreq;
    
  } ;  
}

template<class T> StatusCode MuonDigitizationData<T>::registerPartitions(){
  std::string path;
  for(int i=0;i<MuonDigitizationData_m_partitionNumber;i++){
    path=s_basePathInTES+endPathInTES[i]+s_specificPath;
    eventSvc->registerObject(path,& muonDataPartition[i]);
  }
  return StatusCode::SUCCESS;
}

template<class T> StatusCode
MuonDigitizationData<T>::addMuonObject(int i, T* p0){
  muonDataPartition[i].add(p0);
  return StatusCode::SUCCESS;
}

template<class T> StatusCode
MuonDigitizationData<T>::eraseMuonObject(int i, T* p0){
  muonDataPartition[i].remove(p0);
  delete p0;
  return StatusCode::SUCCESS;
}

template<class T> bool MuonDigitizationData<T>::isObjectIn(int i,T* p0, T*&
                                                           pFound, 
                                                           bool (*comp)
                                                           (T* p1,T* p2))
{  
  KeyedContainer<T>::iterator iter ;
  iter=muonDataPartition[i].begin();
  bool found=false;
  if(iter<muonDataPartition[i].end()){
    
    do{
      found=(*comp)(p0,(*iter));
      if(found) pFound=*iter ;
      ++iter ;
    }while(iter<muonDataPartition[i].end()&&!found);
  }
	return found;
}	


template<class T> KeyedContainer<T>*
MuonDigitizationData<T>::getPartition(int i)
{
	return &muonDataPartition[i];
}

template<class T> bool MuonDigitizationData<T>::isEmpty(int i){
//  return  ((getPartition(i)->size()=0)?true:false);
  long size=muonDataPartition[i].size() ;
	if(size==0) return true;
	else return false; 
}


template   MuonDigitizationData<MCMuonHit>;
template   MuonDigitizationData<MuonPhyChannelInput>;
template   MuonDigitizationData<MuonPhysicalChannel>;
template   MuonDigitizationData<MuonPhysicalChannelOutput>;
#endif



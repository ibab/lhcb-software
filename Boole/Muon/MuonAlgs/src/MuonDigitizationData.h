#ifndef MUONALGS_MUONDIGITIZATIONDATA_H
#define MUONALGS_MUONDIGITIZATIONDATA_H 1
//#include <GaudiKernel/ObjectVector.h>   
#include "GaudiKernel/IDataProviderSvc.h"   
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/KeyedContainer.h"
#include "Event/MCHit.h"
#include "MuonDet/MuonBasicGeometry.h"

static const int MuonDigitizationData_m_partitionNumber=20;

template<class T> class MuonDigitizationData
{
public:
  
  //add or remove a pointer of a T object to the ith data partition
  StatusCode addMuonObject(int i, T* pO);
  StatusCode eraseMuonObject(int i, T* pO);
  
  //check if the ith partition is empty
  bool isEmpty(int i);
  bool isObjectIn(int i,T* p0,T*& pFound, bool (*comp)(T* p1, T* p2));
  StatusCode registerPartitions(MuonBasicGeometry* gbase);
  
  KeyedContainer<T>* getPartition(int i);
  
  
  //standard constructor
  MuonDigitizationData(const std::string & path, 
                       MsgStream* mlog, IDataProviderSvc* eveSvc,
                       const std::string & specpath);    
  
  
  //standard destructor
  ~MuonDigitizationData();
  
private:
  
  //static const int m_partitionNumber=20;  !!! windows
  
  std::string endPathInTES[20];
  //  std::map<std::string, std::string> endPathInTES;
  
  KeyedContainer<T>* muonDataPartition[MuonDigitizationData_m_partitionNumber];
  
  static std::string s_endPathInTES[MuonDigitizationData_m_partitionNumber];
  
  std::string s_basePathInTES;
  std::string s_specificPath;
  
  
  int m_registered;
  
  MsgStream* log;
  
  IDataProviderSvc*  eventSvc;
}; 

/*template<class T> std::string MuonDigitizationData<T>::s_endPathInTES[]=
{std::string("M1/R1"),std::string("M1/R2"),std::string("M1/R3"),
   std::string("M1/R4"),
   std::string("M2/R1"),std::string("M2/R2"),std::string("M2/R3"),
   std::string("M2/R4"),
   std::string("M3/R1"),std::string("M3/R2"),std::string("M3/R3"),
   std::string("M3/R4"),
   std::string("M4/R1"),std::string("M4/R2"),std::string("M4/R3"),
   std::string("M4/R4"),
   std::string("M5/R1"),std::string("M5/R2"),std::string("M5/R3"),
   std::string("M5/R4")}; */
typedef KeyedContainer<LHCb::MCHit, Containers::HashMap> MCMuonHits;

#endif //  MUONALGS_MUONDIGITIZATIONDATA_H

            

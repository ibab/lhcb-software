#ifndef MUONALGS_MUONDETECTORRESPONSE_H
#define MUONALGS_MUONDETECTORRESPONSE_H 1

#include <vector>

#include "GaudiKernel/RndmGenerators.h"

class IToolSvc;
class IRndmGenSvc;
class IDataProviderSvc;
class IMessageSvc;
class MuonPhysicalChannelResponse;
class MuonPhChID;
class MuonChamberResponse;
class DeMuonDetector;

 
class MuonDetectorResponse  {
public:
//constructor
  MuonDetectorResponse() : m_toolSvc(NULL), m_muonDetector(NULL) {};
  void initialize(IToolSvc* toolSvc,IRndmGenSvc * randSvc,
                      IDataProviderSvc* detSvc, IMessageSvc * msgSvc);
  void finalize();
  
  ~MuonDetectorResponse() {};
  MuonPhysicalChannelResponse* getResponse(MuonPhChID& phChID);
  MuonPhysicalChannelResponse* getResponse(int part,int areadout);
  MuonChamberResponse* getChamberResponse(int part);
private:	
  Rndm::Numbers m_gaussDist;
  Rndm::Numbers m_flatDist;
  std::vector<Rndm::Numbers*> m_poissonDist;
  std::vector<Rndm::Numbers*> m_electronicNoise;
  std::vector<Rndm::Numbers*> m_timeJitter;
  
//  IMuonGetInfoTool* m_pGetInfo ;
  IToolSvc* m_toolSvc;
  
  //(toolSvc, detSvc,msgSvc);
  MuonPhysicalChannelResponse* responseVector[2][20];
  MuonChamberResponse* responseChamber[20];
  int m_stationNumber;
  int m_regionNumber;
  int m_partition;  
  DeMuonDetector* m_muonDetector;

 };


inline MuonChamberResponse* MuonDetectorResponse::getChamberResponse(int
                            part){
       return    responseChamber[part];					
}

inline MuonPhysicalChannelResponse* MuonDetectorResponse::getResponse(int part,
                                                               int readout)
{
 	return responseVector[readout][part]; 
}

#endif

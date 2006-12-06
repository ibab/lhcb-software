// $Id: OTDataSvc.h,v 1.7 2006-12-06 14:35:00 mneedham Exp $
#ifndef _OTDataSvc_H
#define _OTDataSvc_H

/** @class OTDataSvc OTDataSvc Tsa/OTDataSvc.h
 *
 *  Interface to data svc
 *
 *  @author M.Needham
 *  @date   30/04/2004
 */

#include "GaudiAlg/GaudiTool.h"
#include "TsaKernel/IOTDataSvc.h"

#include <map>
#include <utility>
#include <string>

namespace LHCb{
  class OTChannelID;
}
class DeOTDetector;



class OTDataSvc: public GaudiTool,
                 virtual public IOTDataSvc{

public:

  /// constructer
  OTDataSvc(const std::string& type,
            const std::string& name,
            const IInterface* parent);

  // destructer
  virtual ~OTDataSvc();

  /// init
  virtual StatusCode initialize();

  // init event with container
  virtual StatusCode initializeEvent(Tsa::OTClusters* clusters);
  
  // init event
  virtual StatusCode initializeEvent();
  
  virtual  Tsa::OTRange layer(const int& iStation, const int& iLayer) const;
  virtual  Tsa::OTRange partition(const int& iStation, const int& iLayer, const int& iSector) const;

  virtual unsigned int dataSize() const;

private:

  StatusCode initPartitions();

  unsigned int nModulesInPartition(const int& iStation, const int& iLayer) const;
  unsigned int lastQuad(const int& iStation, const int& iLayer) const;
  int offSet(const int& iStation, const int& iLayer) const;

  std::vector<Tsa::OTRange> m_iterVector; 
  std::map<LHCb::OTChannelID,unsigned int> m_Mapping;
  int m_firstStation; 

  std::vector<int> m_lastQuad;
  std::vector<int> m_lastModule;


  //  int m_nPartitions;
  int m_partitionsPerStation;
  int m_partitionsPerLayer;

  DeOTDetector* m_tracker;
  unsigned int m_dataSize;

  std::string m_inputLocation;
  bool m_sortData;

};

inline int OTDataSvc::offSet(const int& iStation, const int& iLayer) const{
  return (((iStation-m_firstStation)*m_partitionsPerStation)+(iLayer*m_partitionsPerLayer));
}

#endif


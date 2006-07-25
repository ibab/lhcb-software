// $Id: ITDataSvc.h,v 1.2 2006-07-25 07:00:27 cattanem Exp $
#ifndef _ITDataSvc_H
#define _ITDataSvc_H

/** @class ITDataSvc ITDataSvc Tsa/ITDataSvc.h
 *
 *  Interface to data svc
 *
 *  @author M.Needham
 *  @date   30/04/2004
 */

#include "GaudiAlg/GaudiTool.h"
#include "TsaKernel/IITDataSvc.h"

#include <map>
#include <utility>

class DeSTDetector;

class ITDataSvc: public GaudiTool,
                 virtual public IITDataSvc{

public:

  /// constructer
  ITDataSvc(const std::string& type,
            const std::string& name,
            const IInterface* parent);

  // destructer
  virtual ~ITDataSvc();

  /// init
  virtual StatusCode initialize();

  // init event
  virtual StatusCode initializeEvent();

  virtual  Tsa::STRange layer(const int& iStation, const int& iLayer) const;
  
  virtual  Tsa::STRange partition(const int& iStation, const int& iLayer,
                                  const int& iSector) const;
private:

  StatusCode initPartitions();

  int offSet(const int& iStation, const int& iLayer) const;

  std::vector<Tsa::STRange> m_iterVector; 
  std::map<LHCb::STChannelID,unsigned int> m_Mapping;

  int m_firstStation; 
  int m_nPartitions;
  int m_partitionsPerStation;
  int m_partitionsPerLayer;
  int m_sectorsPerPartition;

  DeSTDetector* m_tracker;

};

inline int ITDataSvc::offSet(const int& iStation, const int& iLayer) const{
  return (((iStation-m_firstStation)*m_partitionsPerStation)+((iLayer-1)*m_partitionsPerLayer));
}

#endif


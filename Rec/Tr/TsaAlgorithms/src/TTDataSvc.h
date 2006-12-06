// $Id: TTDataSvc.h,v 1.5 2006-12-06 14:35:00 mneedham Exp $
#ifndef _TTDataSvc_H
#define _TTDataSvc_H

/** @class TTDataSvc TTDataSvc Tsa/TTDataSvc.h
 *
 *  Interface to data svc
 *
 *  @author M.Needham
 *  @date   30/04/2004
 */

#include "GaudiAlg/GaudiTool.h"
#include "TsaKernel/ITTDataSvc.h"

#include <map>
#include <vector>
#include <utility>
#include <string>


class DeSTDetector;

class TTDataSvc: public GaudiTool,
                 virtual public ITTDataSvc{

public:

  /// constructer
  TTDataSvc(const std::string& type,
            const std::string& name,
            const IInterface* parent);

  // destructer
  virtual ~TTDataSvc();

  /// init
  virtual StatusCode initialize();

  // init event
  virtual StatusCode initializeEvent();

  // init event
  virtual StatusCode initializeEvent(Tsa::STClusters* clusters);

  virtual  Tsa::STRange layer(const int& iStation, const int& iLayer) const;
   
  virtual unsigned int dataSize() const;

private:

 
  StatusCode initPartitions();
  int offSet(const int& iStation, const int& iLayer) const;

  std::vector<Tsa::STRange> m_iterVector; 
  std::map<LHCb::STChannelID,unsigned int> m_Mapping;
  unsigned int m_dataSize;

  unsigned int m_layersPerStation;
  unsigned int m_nPartitions;

  std::vector<int> m_nSectorsPerPartition;
  std::vector<int> m_nRegions;

  DeSTDetector* m_tracker;

  std::string m_inputLocation;
  bool m_sortData;
 
};

inline int TTDataSvc::offSet(const int& iStation, const int& iLayer) const{
  return (((iStation-1)*m_layersPerStation)+iLayer-1);
}

#endif


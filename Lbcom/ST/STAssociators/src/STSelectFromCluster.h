// $Id: STSelectFromCluster.h,v 1.1.1.1 2005-12-19 15:42:42 mneedham Exp $
#ifndef _STSelectFromCluster_H
#define _STSelectFromCluster_H

#include "GaudiAlg/GaudiTool.h"
#include "ISTDigitSelector.h"
#include "Event/STCluster.h"
#include "Event/STDigit.h"

/** @class STSelectFromCluster STSelectFromCluster.h  ITAssociators STSelectFromCluster
 *
 *  Select only those that go into the Raw buffer
 *
 *  @author M.Needham
 *  @date   15/1/2004
*/


class STSelectFromCluster: public GaudiTool, virtual public ISTDigitSelector {

public:

  /// constructer
  STSelectFromCluster(const std::string& type,
            const std::string& name,
            const IInterface* parent);

  // destructer
  virtual ~STSelectFromCluster();

  /// init event
  virtual StatusCode initializeEvent();

  /// initalize
  StatusCode initialize();

  /// get data
  virtual bool execute(const LHCb::STDigit* aDigit) ;

private:

  std::string m_detType;
  void updateCache(const int iTest);
  LHCb::STClusters::iterator m_cachedIter;
  int m_cachedSector;

  LHCb::STClusters* m_dataCont;
  std::string m_dataLocation;

};


#endif

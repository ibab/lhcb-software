// $Id: STClustersToLite.h,v 1.1 2008-10-11 10:42:23 mneedham Exp $
#ifndef STCLUSTERSTOLITE_H
#define STCLUSTERSTOLITE_H 1

#include "GaudiAlg/GaudiAlgorithm.h"

/** @class STClustersToLite
 *
 *  simple class for stripping liteCluster from clusters
 *
 *  @author M.Needham
 *  @date   02/10/2008
 */

namespace LHCb{
  class STChannelID;
}

class STClustersToLite :public GaudiAlgorithm {

public:
  
  // Constructors and destructor
  STClustersToLite( const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~STClustersToLite();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();

private:

  std::string m_detType;
  std::string m_inputLocation;
  std::string m_outputLocation;


};

#endif // STEventMerge

// $Id: STClustersToLite.h,v 1.2 2008-10-14 08:49:05 mneedham Exp $
#ifndef STCLUSTERSTOLITE_H
#define STCLUSTERSTOLITE_H 1

#include "Kernel/STAlgBase.h"

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

class STClustersToLite :public ST::AlgBase {

public:
  
  // Constructors and destructor
  STClustersToLite( const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~STClustersToLite();  

  // IAlgorithm members
  virtual StatusCode execute();

private:

  std::string m_inputLocation;
  std::string m_outputLocation;


};

#endif // STEventMerge

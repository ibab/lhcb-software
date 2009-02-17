// $Id: SiClustersToLite.h,v 1.1 2009-02-17 09:47:35 mneedham Exp $
#ifndef SICLUSTERSTOLITE_H
#define SICLUSTERSTOLITE_H 1


/** @class SiClustersToLite
 *
 *  simple class for stripping liteCluster from clusters
 *
 *  @author M.Needham
 *  @date   11/02/2009
 */

#include "SiClusterTraits.h"

template <class TYPE>
class SiClustersToLite :public SiClusterTraits<TYPE>::BASEALG {

public:
  
  /// Constructor
  SiClustersToLite( const std::string& name, ISvcLocator* pSvcLocator); 

  /// destructor
  virtual ~SiClustersToLite();  

  /// IAlgorithm members
  virtual StatusCode execute();

private:

  std::string m_inputLocation;
  std::string m_outputLocation;


};

#endif // SiClustersToLite

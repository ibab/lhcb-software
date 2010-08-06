// $Id: $
#ifndef STCLUSTERCONTAINERCOPY_H
#define STCLUSTERCONTAINERCOPY_H 1

#include "Kernel/STAlgBase.h"
#include "Event/STCluster.h"

#include <string>

/** @class STClusterContainerCopy STClusterContainerCopy.h
 *
 *  Class for 
 *
 *  @author M.Needham
 *  @date   06/10/2007
 */

class ISTClusterSelector;

namespace LHCb{
  class STChannelID;
}

class STClusterContainerCopy :public ST::AlgBase {

public:
  
  // Constructors and destructor
  STClusterContainerCopy( const std::string& name, ISvcLocator* pSvcLocator); 
  virtual ~STClusterContainerCopy();  

  // IAlgorithm members
  virtual StatusCode initialize();
  virtual StatusCode execute();
  virtual StatusCode finalize();

private:
  

  std::string m_selectorType;
  std::string m_selectorName;
  ISTClusterSelector* m_clusterSelector;

  std::string m_inputLocation;
  std::string m_outputLocation;

};

#endif // STCLUSTERCONTAINERCOPY_H

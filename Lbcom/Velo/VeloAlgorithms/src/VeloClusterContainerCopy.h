// $Id: $
#ifndef VELOCLUSTERCONTAINERCOPY_H 
#define VELOCLUSTERCONTAINERCOPY_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
class IVeloClusterSelector;

namespace LHCb {
  class VeloChannelID;
}

/** @class VeloClusterContainerCopy VeloClusterContainerCopy.h
 *  
 *  Class to copy clusters from an input container onto a new one
 *  applying a selection on the input clusters
 *  (copied largely from STClusterContainerCopy).
 *
 *  @author Eduardo Rodrigues
 *  @date   2011-06-28
 */
class VeloClusterContainerCopy : public GaudiAlgorithm {
public: 
  /// Standard constructor
  VeloClusterContainerCopy( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~VeloClusterContainerCopy( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  IVeloClusterSelector* m_clusterSelector;
  
  std::string m_selectorType;
  std::string m_selectorName;
  
  std::string m_inputLocation;
  std::string m_outputLocation;
  
};
#endif // VELOCLUSTERCONTAINERCOPY_H

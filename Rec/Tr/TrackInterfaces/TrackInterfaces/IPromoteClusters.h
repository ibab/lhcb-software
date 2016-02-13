#ifndef TRACKINTERFACES_IPROMOTECLUSTERS_H 
#define TRACKINTERFACES_IPROMOTECLUSTERS_H 1

// Include files
// from STL
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
 class LHCbID;
}


/** @class IPromoteClusters IPromoteClusters.h TrackInterfaces/IPromoteClusters.h
 *  Interface to the tools to convert lite to full clusters
 *
 *  @author David Hutchcroft
 *  @date   2008-10-16
 */
class IPromoteClusters : public extend_interfaces<IAlgTool> {
public: 
  DeclareInterfaceID( IPromoteClusters, 2, 0 );
  
  /// Take a list of LHCbIDs and make clusters from them
  virtual StatusCode 
  promoteClusters( std::vector<LHCb::LHCbID> const & lhcbIDs ) const = 0;

};
#endif // TRACKINTERFACES_IPROMOTECLUSTERS_H 

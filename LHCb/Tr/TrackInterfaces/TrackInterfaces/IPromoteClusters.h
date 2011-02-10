// $Id: IPromoteClusters.h,v 1.1 2008-10-16 17:03:02 dhcroft Exp $
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

static const InterfaceID IID_IPromoteClusters ( "IPromoteClusters", 1, 0 );

/** @class IPromoteClusters IPromoteClusters.h TrackInterfaces/IPromoteClusters.h
 *  Interface to the tools to convert lite to full clusters
 *
 *  @author David Hutchcroft
 *  @date   2008-10-16
 */
class IPromoteClusters : virtual public IAlgTool {
public: 

  /// Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IPromoteClusters; }
  
  /// Take a list of LHCbIDs and make clusters from them
  virtual StatusCode 
  promoteClusters( std::vector<LHCb::LHCbID> const & lhcbIDs ) = 0;

protected:

private:

};
#endif // TRACKINTERFACES_IPROMOTECLUSTERS_H 

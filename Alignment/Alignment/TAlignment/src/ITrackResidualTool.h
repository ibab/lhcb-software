#ifndef TALIGNMENT_ITRACKRESIDUALTOOL_H
#define TALIGNMENT_ITRACKRESIDUALTOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AlResiduals.h"
namespace LHCb
{
  class Track ;
}

namespace Al
{
  /// Static ID object
  static const InterfaceID IID_ITrackResidualTool( "Al::ITrackResidualTool", 0, 0 );
  
  class ITrackResidualTool : virtual public IAlgTool
  {
  public:
    // Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_ITrackResidualTool ; }
    
     // the only method
    virtual const Al::TrackResiduals* get(const LHCb::Track& track) const = 0 ;
  } ;
}

#endif

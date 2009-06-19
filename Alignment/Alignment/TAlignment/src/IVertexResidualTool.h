#ifndef TALIGNMENT_IVERTEXRESIDUALTOOL_H
#define TALIGNMENT_IVERTEXRESIDUALTOOL_H
 
#include "GaudiKernel/IAlgTool.h"
#include "AlResiduals.h"
namespace LHCb
{
  class Track ;
  class RecVertex ;
}

namespace Al
{
  /// Static ID object
  static const InterfaceID IID_IVertexResidualTool( "Al::IVertexResidualTool", 0, 0 );
  
  class IVertexResidualTool : virtual public IAlgTool
  {
  public:
    // Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IVertexResidualTool ; }
    
     // the only method 
    typedef std::vector<const LHCb::Track*> TrackContainer ;
    virtual const Al::MultiTrackResiduals* get(const LHCb::RecVertex& vertex) const = 0 ;
  } ;
}

#endif

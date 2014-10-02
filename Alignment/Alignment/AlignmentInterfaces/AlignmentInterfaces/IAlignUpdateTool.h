#ifndef TALIGNMENT_IALIGNUPDATETOOL_H
#define TALIGNMENT_IALIGNUPDATETOOL_H

#include "GaudiKernel/IAlgTool.h"
#include "AlignKernel/AlEquations.h"

namespace Al
{
  /// Static ID object
  static const InterfaceID IID_IAlignUpdateTool( "Al::IAlignUpdateTool", 0, 0 );
  
  class IAlignUpdateTool : virtual public IAlgTool
  {
  public:
    // Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IAlignUpdateTool ; }
    
    virtual StatusCode process( const Al::Equations& equations, size_t iter, size_t maxiter ) const = 0;
  } ;
}

#endif

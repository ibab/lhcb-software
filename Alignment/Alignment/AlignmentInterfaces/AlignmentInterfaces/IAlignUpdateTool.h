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
    enum ConvergenceStatus { NotConverged=0, Converged=1, Unknown } ;

    // Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IAlignUpdateTool ; }
    
    // Old interface
    virtual StatusCode process( const Al::Equations& equations, size_t iter, size_t maxiter ) const = 0;

    // New interface
    virtual StatusCode process( const Al::Equations& equations, ConvergenceStatus& status ) const = 0 ;

    // Interface to function getting the alignment constants 
    virtual std::map<std::string, double> getAlignmentConstants(bool final=false) = 0 ;
  } ;
}

#endif

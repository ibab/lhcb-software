// $Id: IFunctionTool.h,v 1.2 2009-03-31 15:40:53 ibelyaev Exp $
// ============================================================================
#ifndef TRACKINTERFACES_IFUNCTIONTOOL_H 
#define TRACKINTERFACES_IFUNCTIONTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
/// forward declarations
// ============================================================================
namespace LHCb {
  class Track;
  class RecVertex;
}
// ============================================================================
/** @class IFunctionTool IFunctionTool.h TrackInterfaces/IFunctionTool.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-08
 */
template <class T>
class IFunctionTool : virtual public IAlgTool 
{
public: 
  // ==========================================================================
  // the main method:
  virtual double function(const T& t ) = 0;
  // ==========================================================================
public:
  // ==========================================================================
  /// Return the interface ID
  static const InterfaceID& interfaceID() 
  { 
    static const InterfaceID s_IID ( "IFunctionTool", 2 , 0 );
    return s_IID ; 
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual & protected destructor
  virtual ~IFunctionTool(){}                  // virtual & protected destructor
  // ==========================================================================
};
// ============================================================================
class ITrackFunctionTool 
  : virtual public IFunctionTool<LHCb::Track> 
{
public:
  // ==========================================================================
  /// Return the interface ID
  static const InterfaceID& interfaceID() 
  { 
    static const InterfaceID s_IID ( "ITrackFunctionTool", 2 , 0 );
    return s_IID ; 
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual & protected destructor, @todo move it into *.cpp
  virtual ~ITrackFunctionTool(){}             // virtual & protected destructor
  // ==========================================================================
};
// ============================================================================
class IRecVertexFunctionTool 
  : virtual public IFunctionTool<LHCb::RecVertex> 
{
public:
  // ==========================================================================
  /// Return the interface ID
  static const InterfaceID& interfaceID() 
  { 
    static const InterfaceID s_IID ( "IRecVertexFunctionTool", 2 , 0 );
    return s_IID ; 
  }
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual & protected destructor, @todo move it into *.cpp
  virtual ~IRecVertexFunctionTool(){}         // virtual & protected destructor
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // TRACKINTERFACES_IFUNCTIONTOOL_H
// ============================================================================

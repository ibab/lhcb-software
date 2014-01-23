// $Id: IBiFunctionTool.h,v 1.2 2009-03-31 15:42:33 ibelyaev Exp $
// ============================================================================
#ifndef HLTBASE_IBIFUNCTIONTOOL_H
#define HLTBASE_IBIFUNCTIONTOOL_H 1
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace LHCb {
class Track;
class RecVertex;
}
// ============================================================================
/** @class IBiFunctionTool IBiFunctionTool.h HltBase/IBiFunctionTool.h
 *
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-08
 */
template <class T1, class T2> class IBiFunctionTool : virtual public IAlgTool {
  public:
    // ==========================================================================
    // Return the interface ID
    static const InterfaceID &interfaceID() {
        static const InterfaceID s_IBiFunctionTool( "IBiFunctionTool", 1, 1 );
        return s_IBiFunctionTool;
    }
    // ==========================================================================
    virtual double function( const T1 &t1, const T2 &t2 ) = 0;
    // ==========================================================================
  protected:
    // ==========================================================================
    /// virtual and protected destructor
    virtual ~IBiFunctionTool() {}
    // ==========================================================================
};
// ============================================================================
class ITrackBiFunctionTool
    : virtual public IBiFunctionTool<LHCb::Track, LHCb::Track> {
  public:
    // ==========================================================================
    /// Return the interface ID
    static const InterfaceID &interfaceID();
    // ==========================================================================
  protected:
    // ==========================================================================
    /// virtual and protected destructor
    virtual ~ITrackBiFunctionTool();
    // ==========================================================================
};
// ============================================================================
class ITrackVertexBiFunctionTool
    : virtual public IBiFunctionTool<LHCb::Track, LHCb::RecVertex> {
  public:
    // ==========================================================================
    /// Return the interface ID
    static const InterfaceID &interfaceID();
    // ==========================================================================
  protected:
    // ==========================================================================
    /// virtual and protected destructor
    virtual ~ITrackVertexBiFunctionTool();
    // ==========================================================================
};
// ============================================================================
class IVertexBiFunctionTool
    : virtual public IBiFunctionTool<LHCb::RecVertex, LHCb::RecVertex> {
  public:
    // ==========================================================================
    /// Return the interface ID
    static const InterfaceID &interfaceID();
    // ==========================================================================
  protected:
    // ==========================================================================
    /// virtual and protected destructor
    virtual ~IVertexBiFunctionTool();
    // ==========================================================================
};
// ============================================================================
// The END
// ============================================================================
#endif // HLTBASE_IBIFUNCTIONTOOL_H
// ============================================================================

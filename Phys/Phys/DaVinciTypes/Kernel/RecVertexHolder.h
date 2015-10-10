// $Id$
// ============================================================================
#ifndef DAVINCIKERNEL_RECVERTEXHOLDER_H 
#define DAVINCIKERNEL_RECVERTEXHOLDER_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <memory>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/RecVertex.h"
// ============================================================================
namespace LHCb
{
  // ==========================================================================
  /** @class RecVertexHolder Kernel/RecVertexHolder.h
   *
   *  "Vertex"-holder:
   *  it is a reasonable type as return value for avriosu vertex 
   *  fitters/refitters/creators 
   *
   *  @attention "desctructive copy semantics!"
   *  @attention object, not registered in TES will be  deleted!
   *
   *  @author Vanya Belyaev Ivan.Belyaev@cern.ch
   *  @date   2010-12-03
   * 
   *                    $Revision$
   *  Last modification $Date$
   *                 by $Author$
   */
  class GAUDI_API RecVertexHolder
  {
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from the vertex 
    RecVertexHolder ( const LHCb::RecVertex* vertex =  0 ) ;
    /** copy constructor
     *  @attention "desctructive copy semantics!"
     *  @param right object to be copied (invalid after copy!!!)
     */
    RecVertexHolder ( const RecVertexHolder& right ) ;
    /** desctructor 
     *  @attention object, not registered in TES will be  deleted!
     */
    virtual ~RecVertexHolder() ;
    // ========================================================================
  public:  // the trival accessors 
    // ========================================================================
    /// return the const vertex 
    inline const LHCb::RecVertex* vertex () const { return m_vertex.get () ; }
    // ========================================================================
  public: // automatic dereference 
    // ========================================================================
    /// automatic dereference to a const vertex 
    inline const LHCb::RecVertex* operator->() const { return vertex () ; }
    // ========================================================================
  public: // automatic type conversion 
    // ========================================================================
    /// automatic type conversion to a const vertex 
    operator const LHCb::RecVertex*         () const { return vertex () ; }
    // ========================================================================
  public:
    // ========================================================================
    /// valid vertex ?
    bool valid    () const { return 0 != vertex  () ; }
    /// check the validity of the vertex 
    bool operator!() const { return   ! valid    () ; }
    // ========================================================================
  public:
    // ========================================================================
    /// take ownership of the vertex 
    const LHCb::RecVertex* release () { return m_vertex.release() ; }
    // ========================================================================
  private:
    // ========================================================================
    /// the vertex itself 
    mutable std::auto_ptr<const LHCb::RecVertex> m_vertex ;    // the vertex
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LHCb 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // DAVINCIKERNEL_RECVERTEXHOLDER_H
// ============================================================================

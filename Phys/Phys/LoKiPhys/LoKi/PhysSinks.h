// $Id$
// ============================================================================
#ifndef LOKI_PHYSSINKS_H 
#define LOKI_PHYSSINKS_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/BasicFunctors.h"
#include "LoKi/Interface.h"
#include "LoKi/PhysTypes.h"
// ============================================================================
class DVAlgorithm ;
// ============================================================================
/** @file 
 *  Collection of ``sinks''
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Vertices 
  {
    // ========================================================================
    /** @class SinkTES 
     * 
     *  @see LoKi::Cuts::RV_SINK
     *  simple "sink for vertices in TES", 
     *  save them as LHCb::RecVertex
     *  @see LHCb:RecVertex
     *  @see LHCb:RecVertex::Container 
     *  @author Vanya BELYAEV ibelyav@physics.syr.edu
     *  @date 2006-12-07
     */
    class SinkTES 
      : public LoKi::BasicFunctors<const LHCb::VertexBase*>::Pipe
    { 
      // ======================================================================
      typedef LoKi::BasicFunctors<const LHCb::VertexBase*>::Pipe _Sink;
      // ======================================================================
    public:
      // ======================================================================
      /// constructor from the service and TES location
      SinkTES 
      ( const std::string& path ) ; 
      /// copy constructor
      SinkTES ( const SinkTES& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~SinkTES() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  SinkTES* clone() const { return new SinkTES(*this) ; }
      /// MANDATORY: the only essential method:
      virtual result_type operator() 
        ( LoKi::Vertices::SinkTES::argument a ) const ;
      /// OPTIONAL: the nice printout
      virtual std::ostream& fillStream ( std::ostream& o ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      SinkTES() ;                      // the default constructor is disabled 
      // ======================================================================
    public:
      // ======================================================================
      /// get the path 
      const std::string& path() const { return m_path ; }
      // ======================================================================
    public:
      // ======================================================================
      /// set the  paths
      void setPath   ( const std::string& value ) { m_path = value ; }
      // ======================================================================
    private:
      // ======================================================================
      const GaudiAlgorithm* alg() const { return m_alg ; } // get the algorithm
      // ======================================================================
      /// TES locations of vertices  
      std::string                   m_path  ; // TES locations of vertices  
      /// the algorithm
      mutable const GaudiAlgorithm* m_alg   ; // the algorithm
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace Vertices
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @typedef RV_SINKTES
     *  simple "sink"-functor to save the vertices to TES
     * 
     *  @code
     *
     *  const RV_SINKTES sink ( "SomeLocationInTES" ) ;
     *
     *  // get the primary vertices:
     *  const LHCb::VertexBase::ConstVector result = sink() ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */
    typedef LoKi::Vertices::SinkTES                              RV_SINKTES   ;
    // ========================================================================
    /** @typedef RV_SINK
     *  simple "sink"-functor to save the vertices to TES
     * 
     *  @code
     *
     *  const RV_SINK sink ( "SomeLocationInTES" ) ;
     *
     *  // get the primary vertices:
     *  const LHCb::VertexBase::ConstVector result = sink() ;
     *
     *  @endcode 
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2008-03-29
     */
    typedef LoKi::Vertices::SinkTES                              RV_SINK      ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_PHYSSINKS_H
// ============================================================================

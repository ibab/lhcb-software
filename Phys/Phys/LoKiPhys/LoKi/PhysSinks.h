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
namespace LoKi
{
   // ==========================================================================
   namespace Vertices 
   {
      // ========================================================================
      /** @class SinkTES 
       * 
       *  @see LoKi::Cuts::VSINK
       *  simple "sink for the particles in TES"
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
         ( const std::string&            path                                  ) ; 
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
         std::string m_path;
         /// the algorithm
         mutable const GaudiAlgorithm* m_alg;
         // ======================================================================
      } ;
   } // end of namespace Vertices
   namespace Cuts 
   {
      // ========================================================================
      /** @typedef VSINKTES 
       *  simple "sink"-functor to get the vertices from TES
       * 
       *  @code
       *
       *  const VSINKTES sink ( "SomeLocationInTES" ) ;
       *
       *  // get the primary vertices:
       *  const LHCb::VertexBase::ConstVector result = sink() ;
       *
       *  @endcode 
       *
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2008-03-29
       */
      typedef LoKi::Vertices::SinkTES                              RV_SINKTES;
      // ========================================================================
   } // end of namespace LoKi::Cuts 
   // ==========================================================================
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSSINKS_H
// ============================================================================

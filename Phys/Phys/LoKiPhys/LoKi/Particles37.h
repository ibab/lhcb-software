// $Id: $
#ifndef FUNCTORS_H 
#define FUNCTORS_H 1

// Event model
#include <Event/RecVertex.h>
#include <Event/Particle.h>

// LoKiPhys 
#include <LoKi/PhysTypes.h>
#include <LoKi/Vertices1.h>
#include <LoKi/AuxDesktopBase.h>
#include <LoKi/PhysTypes.h>

// ============================================================================
/** @file LoKi/Particles37.h
 *  Collection of functors to calculate the decay length
 *  significance of a particle with respect to a vertex.
 *  The decay length significance is defined as the
 *  projection onto the momentum of a particle of the
 *  distance between the particle and a primary vertex,
 *  divided by the error on the distance.
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Roel Aaij roel.aaij@cern.ch
 *  @date 2010-06-18
 */

namespace LoKi {
   namespace Particles { 

// ============================================================================
/** @class DecayLengthSignificance LoKi/Particles37.h
 *  Functor which calculates the decay length significance of a
 *  particle with respect to a primary vertex.
 *  @code
 *    
 *    LHCb::VertexBase* pv = ...;
 *
 *    // get the DLS functor
 *    const DLS_FUN p1 = DLS( pv );
 *
 *    const LHCb::Particle* B = ...;
 *  
 *    const double val = p1 ( B );
 *
 *  @endcode 
 *
 *  @author Roel Aaij
 *  @date   2010-06-18
 */
class DecayLengthSignificance : 
   public LoKi::BasicFunctors< const LHCb::Particle* >::Function,
   public LoKi::Vertices::VertexHolder 
{
public:

   /// constructor from a vertex
   DecayLengthSignificance( const LHCb::VertexBase* vertex );

   /// constructor from a point
   DecayLengthSignificance( const LoKi::Point3D& vertex );

   /// copy constructor
   DecayLengthSignificance( const LoKi::Vertices::VertexHolder& base );

   /// copy constructor
   DecayLengthSignificance( const LoKi::Particles::DecayLengthSignificance& base );

   /// MANDATORY: virtual destructor
   virtual ~DecayLengthSignificance();

   /// MANDATORY: clone method ("virtual constructor")
   virtual DecayLengthSignificance* clone() const;

   /// The method where the actual calculation happens.
   virtual result_type dls( argument p ) const;

   /// MANDATORY: the only essential method
   virtual result_type operator()( argument p ) const;

   /// OPTIONAL: nice printout
   virtual std::ostream& fillStream( std::ostream& stream ) const;

private:

   DecayLengthSignificance();

};

// ============================================================================
/** @class DecayLengthSignificanceDV LoKi/Particles37.h
 *  Functor which uses the best primary vertex from the
 *  PhysDesktop to calculate the decay length significance.
 *  @see DecayLengthSignificance
 *
 *  @code
 *    
 *    // get the DLS functor
 *    const DLS_FUN fun = DLS();
 *
 *    const LHCb::Particle* B = ...;
 *  
 *    const double dls = fun ( B );
 *
 *  @endcode 
 *
 *  @author Roel Aaij
 *  @date   2010-05-07
 */
class DecayLengthSignificanceDV
   : public         DecayLengthSignificance,
     public virtual LoKi::AuxDesktopBase
{
public:

   /// basic constructor
   DecayLengthSignificanceDV();

   /// copy constructor
   DecayLengthSignificanceDV
   ( const DecayLengthSignificanceDV& other );

   /// MANDATORY: virtual destructor
   virtual ~DecayLengthSignificanceDV()
   {

   }

   /// MANDATORY: clone method ("virtual constructor")
   virtual  DecayLengthSignificanceDV* clone() const
   {
      return new DecayLengthSignificanceDV( *this );
   }

   /// MANDATORY: the only essential method
   virtual result_type operator()( argument p ) const;

   /// OPTIONAL: nice printout
   virtual std::ostream& fillStream( std::ostream& s ) const
   {
      return s << "BPVDLS";
   }
};


} // namespace Particles

namespace Cuts {

   // ==========================================================================
   /** @typedef DLS
    *  Functor which calculates the decay length significance of a
    *  particle with respect to a primary vertex.
    *  @see DecayLengthSignificance
    *
    *  @code
    *    
    *    LHCb::VertexBase* pv = ...;
    *
    *    // get the DLS functor
    *    const DLS_FUN p1 = DLS( pv );
    *
    *    const LHCb::Particle* B = ...;
    *  
    *    const double val = p1 ( B );
    *
    *  @endcode 
    *
    *  @author Roel Aaij
    *  @date   2010-06-18
    */
   typedef LoKi::Particles::DecayLengthSignificance DLS;

   // ==========================================================================
   /** @typedef BPVDLS
    *  Functor which uses the best primary vertex from the
    *  PhysDesktop to calculate the decay length significance.
    *  @see DecayLengthSignificance
    *  @see DecayLengthSignificanceDV
    *
    *  @code
    *    
    *    // get the DLS functor
    *    const DLS_FUN fun = DLS();
    *
    *    const LHCb::Particle* B = ...;
    *  
    *    const double dls = fun ( B );
    *
    *  @endcode 
    *
    *  @author Roel Aaij
    *  @date   2010-05-07
    */
   typedef LoKi::Particles::DecayLengthSignificanceDV BPVDLS;

}

} // namespace LoKi

#endif // FUNCTORS_H

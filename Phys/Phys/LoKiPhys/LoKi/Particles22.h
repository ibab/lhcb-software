// $Id: Particles22.h,v 1.1 2008-09-29 16:50:40 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES22_H 
#define LOKI_PARTICLES22_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Vertices1.h"
#include "LoKi/Particles1.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace Particles 
  {
    // ========================================================================
    /** @class DaughterVertexDistance 
     *  Simple functor wich evalautes "the distance" between 
     *  the 'daughetrs' vertices for the given cascade decays
     *
     *  @see LoKi::Cuts::D2DVVD 
     *
     *  @attention indices start form 1, index 0 correspond sto mother particle
     *
     *  The distance is evaluated using LoKi::Vertices::VertexDistance functor
     *
     *  @see LoKi::Vertices::VertexDistance 
     *  @see LoKi::Cuts::VVDIST
     *  @see LoKi::Cuts::VVD
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl 
     *  @date 2008-09-29
     */
    class DaughterVertexDistance 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// constructor form the daughter indices 
      DaughterVertexDistance  
      ( const unsigned int d1     , 
        const unsigned int d2 = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterVertexDistance () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterVertexDistance* clone() const 
      { return new DaughterVertexDistance ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DaughterVertexDistance () ;        // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the index of the first daughter 
      unsigned int m_d1 ;                    // the index of the first daughter 
      /// the index of the second daughter
      unsigned int m_d2 ;                   // the index of the second daughter
      /// the actual 'distance'-functor
      LoKi::Vertices::VertexDistance m_vfun ;  // the actual 'distance'-functor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DaughterVertexsignedDistance 
     *  Simple functor wich evalautes "the signed distance" between 
     *  the 'daughetrs' vertices for the given cascade decays
     *
     *  @see LoKi::Cuts::D2DVVDSIGN 
     *
     *  @attention indices start form 1, index 0 correspond sto mother particle
     *
     *  The distance is evaluated using LoKi::Vertices::VertexSignedDistance functor
     *
     *  @see LoKi::Vertices::VertexSignedDistance 
     *  @see LoKi::Cuts::VVDSIGN
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl 
     *  @date 2008-09-29
     */
    class DaughterVertexSignedDistance 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// constructor form the daughter indices 
      DaughterVertexSignedDistance  
      ( const unsigned int d1     , 
        const unsigned int d2 = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterVertexSignedDistance () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterVertexSignedDistance* clone() const 
      { return new DaughterVertexSignedDistance ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DaughterVertexSignedDistance () ; // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the index of the first daughter 
      unsigned int m_d1 ;                    // the index of the first daughter 
      /// the index of the second daughter
      unsigned int m_d2 ;                   // the index of the second daughter
      /// the actual 'distance'-functor
      LoKi::Vertices::VertexSignedDistance m_vfun ;  // the actual functor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DaughterVertexDistanceChi2 
     *  Simple functor wich evalautes "the \f$\chi^2\f$-distance" between 
     *  the 'daughetrs' vertices for the given cascade decays
     *
     *  @see LoKi::Cuts::D2DVVDCHI2  
     *
     *  @attention indices start form 1, index 0 correspond sto mother particle
     *
     *  The distance is evaluated using LoKi::Vertices::VertexDistanceChi2 functor
     *
     *  @see LoKi::Vertices::VertexDistanceChi2 
     *  @see LoKi::Cuts::VVDCHI2 
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl 
     *  @date 2008-09-29
     */
    class DaughterVertexDistanceChi2 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// constructor form the daughter indices 
      DaughterVertexDistanceChi2
      ( const unsigned int d1     , 
        const unsigned int d2 = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterVertexDistanceChi2 () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterVertexDistanceChi2* clone() const 
      { return new DaughterVertexDistanceChi2 ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DaughterVertexDistanceChi2 () ;    // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the index of the first daughter 
      unsigned int m_d1 ;                    // the index of the first daughter 
      /// the index of the second daughter
      unsigned int m_d2 ;                   // the index of the second daughter
      /// the actual 'distance'-functor
      LoKi::Vertices::VertexChi2Distance m_vfun ; // the actual functor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DaughterVertexDistanceDot
     *  Simple functor wich evalautes "the DOT-distance" between 
     *  the 'daughetrs' vertices for the given cascade decays, 
     *  It is a projected distance (on the momentum 
     *  direction of the 'first' index) 
     *
     *  @see LoKi::Cuts::D2DVVDDOT
     *
     *  @attention indices start form 1, index 0 correspond sto mother particle
     *
     *  The distance is evaluated using LoKi::Particles::VertexDotDistance functor
     *
     *  @see LoKi::Particles::VertexDotDistance 
     *  @see LoKi::Cuts::VDDOT 
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl 
     *  @date 2008-09-29
     */
    class DaughterVertexDistanceDot
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// constructor form the daughter indices 
      DaughterVertexDistanceDot
      ( const unsigned int d1     , 
        const unsigned int d2 = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterVertexDistanceDot () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterVertexDistanceDot* clone() const 
      { return new DaughterVertexDistanceDot ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DaughterVertexDistanceDot () ;    // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the index of the first daughter 
      unsigned int m_d1 ;                    // the index of the first daughter 
      /// the index of the second daughter
      unsigned int m_d2 ;                   // the index of the second daughter
      /// the actual 'distance'-functor
      LoKi::Particles::VertexDotDistance m_vfun ; // the actual functor
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DaughterVertexDistanceSignedChi2 
     *  Simple functor wich evalautes "the signed \f$\chi^2\f$-distance" between 
     *  the 'daughetrs' vertices for the given cascade decays.
     *  The sign is evaluated from the sign of the expresion:
     *   \f$ \left( \vect{v}_1-\vetc{v}_2\right)\cdot \vect{p}_1 \f$ 
     *
     *  @see LoKi::Cuts::D2DVVDCHI2SIGN
     *
     *  @attention indices start form 1, index 0 correspond sto mother particle
     *
     *  The distance is evaluated using LoKi::Vertices::VertexDistanceChi2 functor
     *
     *  @see LoKi::Vertices::VertexDistanceChi2 
     *  @see LoKi::Cuts::VVDCHI2 
     *  @author Vanya BELYAEV  Ivan.BElyaev@nikhef.nl 
     *  @date 2008-09-29
     */
    class DaughterVertexDistanceSignedChi2 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Function 
    {
    public:
      // ======================================================================
      /// constructor form the daughter indices 
      DaughterVertexDistanceSignedChi2
      ( const unsigned int d1     , 
        const unsigned int d2 = 0 ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DaughterVertexDistanceSignedChi2 () {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DaughterVertexDistanceSignedChi2* clone() const 
      { return new DaughterVertexDistanceSignedChi2 ( *this ) ; }
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DaughterVertexDistanceSignedChi2 () ; // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the index of the first daughter 
      unsigned int m_d1 ;                    // the index of the first daughter 
      /// the index of the second daughter
      unsigned int m_d2 ;                   // the index of the second daughter
      /// the actual 'distance'-functor
      LoKi::Vertices::VertexChi2Distance m_vfun ; // the actual functor
      // ======================================================================
    } ;
    // ========================================================================
  } // end of namespace LoKi::Particles 
  // ==========================================================================
} // end of namespace LoKi  
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES22_H
// ============================================================================

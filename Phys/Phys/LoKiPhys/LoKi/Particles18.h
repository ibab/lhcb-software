// $Id: Particles18.h,v 1.1 2007-07-23 17:35:46 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_PARTICLES18_H 
#define LOKI_PARTICLES18_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/ExtraInfo.h"
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================
namespace LoKi 
{
  namespace Particles
  {
    // ========================================================================    
    /** @class HasInfo
     *  Trivial predicate which evaluates LHCb::Particle::hasInfo
     *  function
     *
     *  It relies on the method LHCb::Particle::hasInfo
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::HASINFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class HasInfo : public LoKi::ExtraInfo::CheckInfo<const LHCb::Particle*>
    {
    public:
      /** constructor from "info"
       *  @param key info index/mark/key
       */
      HasInfo ( const int key ) ;
      /// copy constructor 
      HasInfo ( const HasInfo& right ) ;
      /// destructor 
      virtual ~HasInfo(){} ;
      /// clone method (mandatory!)
      virtual HasInfo* clone() const { return new HasInfo(*this) ; }
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      HasInfo();
    };
    // ========================================================================    
    /** @class Info
     *  Trivial function which evaluates LHCb::Particle::info
     *  
     *  It relies on the method LHCb::Particle::info
     *
     *  @see LHCb::Particle
     *  @see LoKi::Cuts::INFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class Info : public LoKi::ExtraInfo::GetInfo<const LHCb::Particle*>
    {
    public:
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default value for missing key/invalid object 
       */
      Info ( const int    key , const double def ) ;
      /// copy constructor 
      Info ( const Info& right ) ;
      /// destructor 
      virtual ~Info(){};
      /// clone method (mandatory!)
      virtual Info* clone() const { return new Info(*this); }
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      Info();
    };
  } // end of namepsace LoKi::Particles
} // end of namespace LoKi 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES18_H
// ============================================================================

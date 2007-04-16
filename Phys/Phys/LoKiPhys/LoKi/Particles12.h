// $Id: Particles12.h,v 1.5 2007-04-16 16:16:26 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.5 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2007/02/26 09:19:22  ibelyaev
//  add LoKi::Particles::IsMuon (alias: ISMUON)
//
// Revision 1.3  2006/11/27 12:01:31  ibelyaev
//  prepare for LoKi v4r3
//
// Revision 1.2  2006/03/08 14:14:51  ibelyaev
//  add Particles14.h/.cpp
//
// Revision 1.1  2006/02/23 21:14:09  ibelyaev
//   add new fuctors/predicates
//
// ============================================================================
#ifndef LOKI_PARTICLES12_H 
#define LOKI_PARTICLES12_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKiPhys 
// ============================================================================
#include "LoKi/PhysTypes.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-23 
 */
// ============================================================================

namespace LoKi
{
  namespace Particles 
  { 
    
    /** @class ProtoHasInfo
     *  The trivial predicate whcii evaluated for true 
     *  if the protoparticle "hasInfo".
     *  
     *  @see LHCb::Particle
     *  @see LHCb::Particle::proto
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::hasInfo
     *
     *  @see LoKi::Cuts::PPHASINFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-23 
     */
    class ProtoHasInfo 
      : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// constructor form the index in ProtoParticle::ExtraInfo
      ProtoHasInfo ( int index ) ;
      /// copy constructor 
      ProtoHasInfo ( const ProtoHasInfo& right ) ;
      /// MANDAROTY: virtual destructor 
      virtual ~ProtoHasInfo() {}
      /// clone method (mandatory!)
      virtual  ProtoHasInfo* clone() const 
      { return new ProtoHasInfo(*this) ; }    
      /// the only one essential method 
      result_type operator() ( argument p ) const ;
      /// the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // default contructor is disabled 
      ProtoHasInfo() ;
    private :
      // index in ProtoParticle::ExtraInfo 
      int m_info ; ///< index in ProtoParticle::ExtraInfo
    } ;
    
    /** @class ProtoInfo
     *  Trivial function which evaluates LHCb::Particle::info
     *  
     *  It relies on the method LHCb::Particle::info
     *
     *  @see LHCb::Particle
     *  @see LHCb::Particle::proto
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::info
     *  @see LoKi::Cuts::PPINFO 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class ProtoInfo
      : public LoKi::Function<const LHCb::Particle*>
    {
    public:
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default valeu for info 
       *  @param bad bad value to be retured for invalid particle 
       */
      ProtoInfo 
      ( const int    key , 
        const double def , 
        const double bad ) ;
      /** constructor from "info"
       *  @param key info index/mark/key
       *  @param def default value for info 
       */
      ProtoInfo 
      ( const int    key , 
        const double def ) ;
      /// copy constructor 
      ProtoInfo ( const ProtoInfo& right ) ;
      /// MANDATORY: virtual destructor 
      virtual ~ProtoInfo() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProtoInfo* clone() const { return new ProtoInfo(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    private:
      // the default constructor is disabled 
      ProtoInfo();
    private:
      // index in ProtoParticle::ExtraInfo
      int    m_key ; ///< index in ProtoParticle::ExtraInfo
      // value to be returned for invalid particle and protoparticle 
      double m_def ;
      // default value for missing infomration 
      double m_bad ;
    } ;

    
    /** @class ProtoHasRichPID 
     *  the trivial predicate wich tests the validity of 
     *  ProtoParticle::richPID 
     * 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::richPID 
     *  @see LHCb::RichPID 
     *  @see LoKi::Cuts::PPHASRICH 
     *  @see LoKi::Cuts::HASRICH 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class ProtoHasRichPID 
      : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~ProtoHasRichPID() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProtoHasRichPID* clone() const 
      { return new ProtoHasRichPID(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    } ;
    
    /** @class ProtoHasMuonPID 
     *  the trivial predicate wich tests the validity of 
     *  ProtoParticle::muonPID 
     * 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::muonPID 
     *  @see LHCb::MuonPID 
     *  @see LoKi::Cuts::PPHASMUON
     *  @see LoKi::Cuts::HASMUON
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class ProtoHasMuonPID 
      : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~ProtoHasMuonPID() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProtoHasMuonPID* clone() const 
      { return new ProtoHasMuonPID(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    } ;
    
    /** @class ProtoHasCaloHypos
     *  the trivial predicate wich tests the validity of 
     *  ProtoParticle::calo() 
     * 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::calo
     *  @see LHCb::CaloHypo
     *  @see LoKi::Cuts::PPHASCALO
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    class ProtoHasCaloHypos
      : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~ProtoHasCaloHypos() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  ProtoHasCaloHypos* clone() const 
      { return new ProtoHasCaloHypos(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    } ;

    
    /** @class IsMuon
     *  The trivial predicate whith returns LHCb::MuonPID::isMuon
     *
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::MuonPID 
     *  @see LoKi::CutsISMUON
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-02-25
     */
    class IsMuon 
      : public LoKi::Predicate<const LHCb::Particle*>
    {
    public:
      /// MANDATORY: virtual destructor 
      virtual ~IsMuon() {} ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual  IsMuon* clone() const { return new IsMuon(*this); }
      /// MANDATORY: the only one essential method 
      result_type operator() ( argument p ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream( std::ostream& s ) const ;
    } ;
    
  }  // end of namespace Particles 
  
}  // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES12_H
// ============================================================================

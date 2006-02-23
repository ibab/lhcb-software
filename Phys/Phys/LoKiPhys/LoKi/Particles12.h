// $Id: Particles12.h,v 1.1 2006-02-23 21:14:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
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
     *  @see LoKi::Cuts::INFO 
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

    
    /** @struct ProtoHasRichPID 
     *  the trivial predicate wich tests the validity of 
     *  ProtoParticle::richPID 
     * 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::richPID 
     *  @see LHCb::RichPID 
     *  @see LoKi::Cuts::PPHASRICH 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    struct ProtoHasRichPID 
      : public LoKi::Predicate<const LHCb::Particle*>
    {
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
    
    /** @struct ProtoHasMuonPID 
     *  the trivial predicate wich tests the validity of 
     *  ProtoParticle::muonPID 
     * 
     *  @see LHCb::Particle
     *  @see LHCb::ProtoParticle
     *  @see LHCb::ProtoParticle::muonPID 
     *  @see LHCb::MuonPID 
     *  @see LoKi::Cuts::PPHASMUON
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-02-15
     */
    struct ProtoHasMuonPID 
      : public LoKi::Predicate<const LHCb::Particle*>
    {
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
    
    /** @struct ProtoHasCaloHypos
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
    struct ProtoHasCaloHypos
      : public LoKi::Predicate<const LHCb::Particle*>
    {
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
    

  } ; // end of namespace Particles 
  
} ; // end of namespace LoKi 


// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARTICLES12_H
// ============================================================================

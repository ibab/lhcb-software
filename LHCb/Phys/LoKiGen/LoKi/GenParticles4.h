// $Id$
// ============================================================================
#ifndef LOKI_GENPARTICLES4_H 
#define LOKI_GENPARTICLES4_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/HepMCEvent.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Kinematics.h"
#include "LoKi/GenTypes.h"
#include "LoKi/GenChildSelector.h"
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
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2012-01-28 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  namespace GenParticles
  {
    // ========================================================================
    /** @class FourMomentum
     *  base class for implementation of many function dealing
     *  with the various subcombinations of particles 
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date 2012-06-17
     */
    class GAUDI_API FourMomentum : public LoKi::GenTypes::GFunc 
    {
    public:
      // ======================================================================
      /// constructor from one 
      FourMomentum ( const LoKi::GenChild::Selector& c1 ) ;
      /// constructor from two selectors
      FourMomentum ( const LoKi::GenChild::Selector& c1 ,
                     const LoKi::GenChild::Selector& c2 ) ;
      /// constructor from three selectors 
      FourMomentum ( const LoKi::GenChild::Selector& c1 ,
                     const LoKi::GenChild::Selector& c2 ,
                     const LoKi::GenChild::Selector& c3 ) ;
      /// constructor from four selectors
      FourMomentum ( const LoKi::GenChild::Selector& c1 ,
                     const LoKi::GenChild::Selector& c2 ,
                     const LoKi::GenChild::Selector& c3 ,
                     const LoKi::GenChild::Selector& c4 ) ;
      /// contructor from list of selectors 
      FourMomentum ( const LoKi::GenChild::Selector::Vector& cs ) ;      
      // ======================================================================
      /// constructor from one selector
      FourMomentum ( const LoKi::GenTypes::GCuts&    c1 ) ;
      /// constructor from two selectors
      FourMomentum ( const LoKi::GenTypes::GCuts&    c1 ,
                     const LoKi::GenTypes::GCuts&    c2 ) ;
      /// constructor from three selectors 
      FourMomentum ( const LoKi::GenTypes::GCuts&    c1 ,
                     const LoKi::GenTypes::GCuts&    c2 ,
                     const LoKi::GenTypes::GCuts&    c3 ) ;
      /// constructor from four selectors
      FourMomentum ( const LoKi::GenTypes::GCuts&    c1 ,
                     const LoKi::GenTypes::GCuts&    c2 ,
                     const LoKi::GenTypes::GCuts&    c3 ,
                     const LoKi::GenTypes::GCuts&    c4 ) ;
      /// default constructor 
      FourMomentum () ;
      // =======================================================================
      /// vitual destructor 
      virtual ~FourMomentum() {}
      // ======================================================================
    public:
      // ======================================================================
      /// evaluate the full four momentum of combination
      LoKi::LorentzVector momentum ( const HepMC::GenParticle* p ) const ;
      /**  get the rapidity y  
       *  \f$ y = \frac{1}{2}\log \frac{ E + p_z }{ E - p_z } \f$ 
       */
      double  y   ( const LoKi::LorentzVector& lv ) const ;
      /**  get the rapidity y0  
       *  \f$ y_0 = \frac{1}{2}\log \frac{ E + p }{ E - p } \f$ 
       */
      double  y0     ( const LoKi::LorentzVector& lv ) const ;
      // get the angle phi 
      double  phi    ( const LoKi::LorentzVector& lv ) const ;
      // get the pseudorapidity 
      double  eta    ( const LoKi::LorentzVector& lv ) const ;
      // get beta-factor 
      double  beta   ( const LoKi::LorentzVector& lv ) const ;
      // get gamma-factor 
      double  gamma  ( const LoKi::LorentzVector& lv ) const ;      
      // get beta*gamma-factor 
      double  bgamma ( const LoKi::LorentzVector& lv ) const ;      
      /// adjust delta phi into the range of [-180:180]degrees 
      double adjust  ( double angle ) const ;
      // ======================================================================
    protected:
      // ======================================================================
      /// base class printput 
      std::ostream& print_ 
      ( std::ostream&      s  , 
        const std::string& n1 , 
        const std::string& n2 ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the actual list of selectors 
      LoKi::GenChild::Selector::Vector m_children ;
      // ======================================================================
    } ;  
    // ========================================================================
    /** @class Mass
     *  evaluator of the 'Mass for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GM
     *  @see LoKi::Cuts::GMASS
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API Mass : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      Mass ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Mass ( const LoKi::GenChild::Selector& c1 ,
             const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Mass ( const LoKi::GenChild::Selector& c1 ,
             const LoKi::GenChild::Selector& c2 ,
             const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Mass ( const LoKi::GenChild::Selector& c1 ,
             const LoKi::GenChild::Selector& c2 ,
             const LoKi::GenChild::Selector& c3 ,
             const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      Mass ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      Mass ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Mass ( const LoKi::GenTypes::GCuts&    c1 ,
             const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Mass ( const LoKi::GenTypes::GCuts&    c1 ,
             const LoKi::GenTypes::GCuts&    c2 ,
             const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Mass ( const LoKi::GenTypes::GCuts&    c1 ,
             const LoKi::GenTypes::GCuts&    c2 ,
             const LoKi::GenTypes::GCuts&    c3 ,
             const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      Mass () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~Mass () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Mass* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Energy
     *  evaluator of the 'Energy for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GE
     *  @see LoKi::Cuts::GENERGY
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API Energy : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      Energy ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Energy ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Energy ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Energy ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ,
               const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      Energy ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      Energy ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Energy ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Energy ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Energy ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ,
               const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      Energy () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~Energy () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Energy* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Momentum
     *  evaluator of the 3-momentum for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GP
     *  @see LoKi::Cuts::GMOMENTUM
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API Momentum : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      Momentum ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Momentum ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Momentum ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Momentum ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ,
               const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      Momentum ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      Momentum ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Momentum ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Momentum ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Momentum ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ,
               const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      Momentum () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~Momentum () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Momentum* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MomentumX
     *  evaluator of the x-momentum for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPX
     *  @see LoKi::Cuts::GMOMX
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API MomentumX : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      MomentumX ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      MomentumX ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      MomentumX ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      MomentumX ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ,
               const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      MomentumX ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      MomentumX ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      MomentumX ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      MomentumX ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      MomentumX ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ,
               const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      MomentumX () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~MomentumX () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  MomentumX* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MomentumY
     *  evaluator of the x-momentum for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPY
     *  @see LoKi::Cuts::GMOMY
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API MomentumY : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      MomentumY ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      MomentumY ( const LoKi::GenChild::Selector& c1 ,
                  const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      MomentumY ( const LoKi::GenChild::Selector& c1 ,
                  const LoKi::GenChild::Selector& c2 ,
                  const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      MomentumY ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ,
               const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      MomentumY ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      MomentumY ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      MomentumY ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      MomentumY ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      MomentumY ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ,
               const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      MomentumY () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~MomentumY () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  MomentumY* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class MomentumZ
     *  evaluator of the x-momentum for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPZ
     *  @see LoKi::Cuts::GMOMZ
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API MomentumZ : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      MomentumZ ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      MomentumZ ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      MomentumZ ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      MomentumZ ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ,
               const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      MomentumZ ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      MomentumZ ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      MomentumZ ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      MomentumZ ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      MomentumZ ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ,
               const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      MomentumZ () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~MomentumZ () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  MomentumZ* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class TransverseMomentum
     *  evaluator of the x-momentum for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPT
     *  @see LoKi::Cuts::GMOMT
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API TransverseMomentum : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      TransverseMomentum ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      TransverseMomentum ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      TransverseMomentum ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      TransverseMomentum ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ,
               const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      TransverseMomentum ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      TransverseMomentum ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      TransverseMomentum ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      TransverseMomentum ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      TransverseMomentum ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ,
               const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      TransverseMomentum () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~TransverseMomentum () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  TransverseMomentum* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class PseudoRapidity
     *  evaluator of the x-momentum for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GETA
     *  @see LoKi::Cuts::GPSEUDORAPIDITY
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API PseudoRapidity : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      PseudoRapidity ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      PseudoRapidity ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      PseudoRapidity ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      PseudoRapidity ( const LoKi::GenChild::Selector& c1 ,
               const LoKi::GenChild::Selector& c2 ,
               const LoKi::GenChild::Selector& c3 ,
               const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      PseudoRapidity ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      PseudoRapidity ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      PseudoRapidity ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      PseudoRapidity ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      PseudoRapidity ( const LoKi::GenTypes::GCuts&    c1 ,
               const LoKi::GenTypes::GCuts&    c2 ,
               const LoKi::GenTypes::GCuts&    c3 ,
               const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      PseudoRapidity () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~PseudoRapidity () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  PseudoRapidity* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Rapidity
     *  evaluator of the rapidity for particle and/or combination of daughters 
     *  
     *  \f$ y = \frac{1}{2}\log \frac{ E + p_z }{ E - p_z } \f$ 
     *
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GY
     *  @see LoKi::Cuts::GRAPIDITY
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API Rapidity : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      Rapidity ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Rapidity ( const LoKi::GenChild::Selector& c1 ,
                 const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Rapidity ( const LoKi::GenChild::Selector& c1 ,
                 const LoKi::GenChild::Selector& c2 ,
                 const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Rapidity ( const LoKi::GenChild::Selector& c1 ,
                 const LoKi::GenChild::Selector& c2 ,
                 const LoKi::GenChild::Selector& c3 ,
                 const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      Rapidity ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      Rapidity ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Rapidity ( const LoKi::GenTypes::GCuts&    c1 ,
                 const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Rapidity ( const LoKi::GenTypes::GCuts&    c1 ,
                 const LoKi::GenTypes::GCuts&    c2 ,
                 const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Rapidity ( const LoKi::GenTypes::GCuts&    c1 ,
                 const LoKi::GenTypes::GCuts&    c2 ,
                 const LoKi::GenTypes::GCuts&    c3 ,
                 const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      Rapidity () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~Rapidity () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Rapidity* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Rapidity0
     *  evaluator of the rapidity0 for particle and/or combination of daughters 
     *  
     *  \f$ y_0 = \frac{1}{2}\log \frac{ E + p }{ E - p } \f$ 
     *
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GY0
     *  @see LoKi::Cuts::GRAPIDITY0
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API Rapidity0 : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      Rapidity0 ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Rapidity0 ( const LoKi::GenChild::Selector& c1 ,
                 const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Rapidity0 ( const LoKi::GenChild::Selector& c1 ,
                 const LoKi::GenChild::Selector& c2 ,
                 const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Rapidity0 ( const LoKi::GenChild::Selector& c1 ,
                 const LoKi::GenChild::Selector& c2 ,
                 const LoKi::GenChild::Selector& c3 ,
                 const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      Rapidity0 ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      Rapidity0 ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Rapidity0 ( const LoKi::GenTypes::GCuts&    c1 ,
                 const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Rapidity0 ( const LoKi::GenTypes::GCuts&    c1 ,
                 const LoKi::GenTypes::GCuts&    c2 ,
                 const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Rapidity0 ( const LoKi::GenTypes::GCuts&    c1 ,
                 const LoKi::GenTypes::GCuts&    c2 ,
                 const LoKi::GenTypes::GCuts&    c3 ,
                 const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      Rapidity0 () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~Rapidity0 () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Rapidity0* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Phi
     *  evaluator of the \f$\phi\f$ for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPHI
     *  @see LoKi::Cuts::GASYMUTH
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API Phi : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      Phi ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Phi ( const LoKi::GenChild::Selector& c1 ,
            const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Phi ( const LoKi::GenChild::Selector& c1 ,
            const LoKi::GenChild::Selector& c2 ,
            const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Phi ( const LoKi::GenChild::Selector& c1 ,
            const LoKi::GenChild::Selector& c2 ,
            const LoKi::GenChild::Selector& c3 ,
            const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      Phi ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      Phi ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Phi ( const LoKi::GenTypes::GCuts&    c1 ,
            const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Phi ( const LoKi::GenTypes::GCuts&    c1 ,
            const LoKi::GenTypes::GCuts&    c2 ,
            const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Phi ( const LoKi::GenTypes::GCuts&    c1 ,
            const LoKi::GenTypes::GCuts&    c2 ,
            const LoKi::GenTypes::GCuts&    c3 ,
            const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      Phi () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~Phi () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Phi* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Theta
     *  evaluator of the \f$\phi\f$ for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GPHI
     *  @see LoKi::Cuts::GASYMUTH
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2006-01-17
     */
    class GAUDI_API Theta : public LoKi::GenParticles::FourMomentum
    {
    public:
      // ======================================================================
      /// constructor from one selectors
      Theta ( const LoKi::GenChild::Selector& c1 )
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Theta ( const LoKi::GenChild::Selector& c1 ,
              const LoKi::GenChild::Selector& c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Theta ( const LoKi::GenChild::Selector& c1 ,
              const LoKi::GenChild::Selector& c2 ,
              const LoKi::GenChild::Selector& c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Theta ( const LoKi::GenChild::Selector& c1 ,
              const LoKi::GenChild::Selector& c2 ,
              const LoKi::GenChild::Selector& c3 ,
              const LoKi::GenChild::Selector& c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// contructor from list of selectors 
      Theta ( const LoKi::GenChild::Selector::Vector& cs ) 
        : LoKi::GenParticles::FourMomentum ( cs ) {} ;
      /// constructor from two selectors
      Theta ( const LoKi::GenTypes::GCuts&    c1 ) 
        : LoKi::GenParticles::FourMomentum ( c1 ) {} ;
      /// constructor from two selectors
      Theta ( const LoKi::GenTypes::GCuts&    c1 ,
              const LoKi::GenTypes::GCuts&    c2 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 ) {} ;
      /// constructor from three selectors
      Theta ( const LoKi::GenTypes::GCuts&    c1 ,
              const LoKi::GenTypes::GCuts&    c2 ,
              const LoKi::GenTypes::GCuts&    c3 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 ) {} ;
      /// constructor from four selectors
      Theta ( const LoKi::GenTypes::GCuts&    c1 ,
              const LoKi::GenTypes::GCuts&    c2 ,
              const LoKi::GenTypes::GCuts&    c3 ,
              const LoKi::GenTypes::GCuts&    c4 ) 
        : LoKi::GenParticles::FourMomentum ( c1 , c2 , c3 , c4 ) {} ;
      /// default constructor
      Theta () : LoKi::GenParticles::FourMomentum () {} ;
      /// MANDATORY: virtual destructor 
      virtual ~Theta () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Theta* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Beta
     *  evaluator of the \f$\beta\f$ for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GBETA
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-04-14
     */
    class GAUDI_API Beta : public LoKi::GenTypes::GFunc 
    {
    public:
      // ======================================================================
      /// default constructor
      Beta () ;
      /// MANDATORY: virtual destructor 
      virtual ~Beta () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Beta* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    public:
      // ======================================================================
      // get beta-factor 
      double  beta      ( const HepMC::GenParticle* p ) const ;
      // get gamma-factor 
      double  gamma     ( const HepMC::GenParticle* p ) const ;      
      // get beta*gamma-factor 
      double  betagamma ( const HepMC::GenParticle* p ) const ;      
      // ======================================================================
    } ;
    // ========================================================================
    /** @class Gamma
     *  evaluator of the \f$\gamma\f$ for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GGAMMA
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-04-14
     */
    class GAUDI_API Gamma : public LoKi::GenParticles::Beta
    {
    public:
      // ======================================================================
      /// default constructor
      Gamma () ;
      /// MANDATORY: virtual destructor 
      virtual ~Gamma () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  Gamma* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class BetaGamma
     *  evaluator of the \f$\beta\gamma\f$ for particle and/or combination of daughters 
     *  
     *  @see HepMC::GenParticle 
     *  @see LoKi::Cuts::GBETAGAMMA
     *  @see LoKi::Cuts::GBGAMMA
     *  @see LoKi::Cuts::GBG
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2013-04-14
     */
    class GAUDI_API BetaGamma : public LoKi::GenParticles::Beta
    {
    public:
      // ======================================================================
      /// default constructor
      BetaGamma () ;
      /// MANDATORY: virtual destructor 
      virtual ~BetaGamma () ;
      /// MANDATORY: clone method ("virtual destructor")
      virtual  BetaGamma* clone() const ;
      /// MANDATORY: the only one essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: nice printout
      virtual std::ostream& fillStream ( std::ostream& s )const ;
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DeltaPhi
     *  Simple evaluator of "DeltaPhi" for the particle
     *
     *  @see LoKi::Cuts::GDPHI 
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class GAUDI_API DeltaPhi : public LoKi::GenParticles::Phi
    {
    public:
      // ======================================================================
      /// constructor from the angle
      DeltaPhi ( const double phi ) ;
      /// constructor from the vector 
      DeltaPhi ( const LoKi::ThreeVector&   v ) ;
      /// constructor from the vector 
      DeltaPhi ( const LoKi::LorentzVector& v ) ;
      /// constructor from the particle
      DeltaPhi ( const HepMC::GenParticle*  p ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaPhi() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaPhi* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: "SHORT" representation
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the phi
      double phi0 () const { return m_phi ; }                  // get the phi
      double dphi ( argument p ) const ;
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DeltaPhi ();                       // the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the angle itself 
      double                  m_phi  ;                      // the angle itself 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DeltaEta
     *  Simple evaluator of "DeltaEta" for the particle
     *
     *  @see LoKi::Cuts::GDETA
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class GAUDI_API DeltaEta : public LoKi::GenParticles::PseudoRapidity 
    {
    public:
      // ======================================================================
      /// constructor from the eta
      DeltaEta ( const double eta ) ;
      /// constructor from the vector 
      DeltaEta ( const LoKi::ThreeVector&   v ) ;
      /// constructor from the vector 
      DeltaEta ( const LoKi::LorentzVector& v ) ;
      /// constructor from the particle
      DeltaEta ( const HepMC::GenParticle*  p ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaEta() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaEta* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: "SHORT" representation
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the eta
      double eta0 () const { return m_eta ; }
      double deta ( argument p ) const ; 
      // ======================================================================
    private:
      // ======================================================================
      /// the default constructor is disabled 
      DeltaEta ();                       // the default constructor is disabled
      // ======================================================================
    private:
      // ======================================================================
      /// the angle itself 
      double                             m_eta  ; // the angle itself 
      // ======================================================================
    } ;
    // ========================================================================
    /** @class DeltaR2
     *  Simple evaluator of "DeltaPhi**2+ DeltaEta**2"
     *
     *  @see LoKi::Cuts::GDR2 
     *  @see LoKi::Cuts::GDELTAR2 
     *  @see LoKi::Cuts::GDPHI
     *  @see LoKi::Cuts::GDETA
     *  @see LoKi::GenParticles::DeltaPhi
     *  @see LoKi::GenParticles::DeltaEta
     *  @see LoKi::GenParticles::Phi
     *  @see LoKi::GenParticles::PseudoRapidity
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2007-03-03
     */
    class GAUDI_API DeltaR2 : public LoKi::GenParticles::DeltaPhi 
    {
    public:
      // ======================================================================
      /// constructor from eta & phi
      DeltaR2 ( const double eta , const double phi ) ;
      /// constructor from the vector 
      DeltaR2 ( const LoKi::ThreeVector&   v ) ;
      /// constructor from the vector 
      DeltaR2 ( const LoKi::LorentzVector& v ) ;
      /// constructor from the particle
      DeltaR2 ( const HepMC::GenParticle* p ) ;
      /// MANDATORY: virtual destructor 
      virtual ~DeltaR2() {}
      /// MANDATORY: clone method ("virtual constructor")
      virtual  DeltaR2* clone() const ; 
      /// MANDATORY: the only essential method 
      virtual result_type operator() ( argument p ) const ;
      /// OPTIONAL: "SHORT" representation
      virtual  std::ostream& fillStream( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// get the eta
      double eta0 () const { return m_deta.eta0()  ; }
      double deta ( argument p ) const { return m_deta.deta ( p )  ; }
      // ======================================================================
    private:
      // ======================================================================
      // the default constructor is disabled 
      DeltaR2 ();                       /// the default constructor is disabled 
      // ======================================================================
    private:
      // ======================================================================
      /// the actual evaluator of delta eta
      LoKi::GenParticles::DeltaEta m_deta ; // the actual evaluator of delta eta
      // ======================================================================
    } ;
    // ========================================================================
  } //                                      end of namespace LoKi::GenParticles 
  // ==========================================================================
  namespace Cuts 
  {
    // ========================================================================
    /** @var GM
     *  evaluator of particle mass 
     *
     *  @see LoKi::Cuts::GMASS
     *  @see LoKi::GenParticles::Mass
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::Mass                                         GM ;
    // ========================================================================
    /** @typedef GMASS
     *  evaluator of particle mass
     *
     *  @see LoKi::Cuts::GM
     *  @see LoKi::GenParticles::Mass
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::Mass                                   GMASS ;
    // ========================================================================
    /** @var GE
     *  evaluator of particle energy 
     *
     *  @see LoKi::Cuts::GENERGY
     *  @see LoKi::GenParticles::Energy
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::Energy                                       GE ;
    // ========================================================================
    /** @typedef GENERGY
     *  evaluator of particle energy 
     *
     *  @see LoKi::Cuts::GE
     *  @see LoKi::GenParticles::Energy
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::Energy                                GENERGY ;
    // ========================================================================
    /** @var GP
     *  evaluator of particle momentum
     *
     *  @see LoKi::Cuts::GENERGY
     *  @see LoKi::GenParticles::Momentum
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::Momentum                                     GP ;
    // ========================================================================
    /** @typedef GMOMENTUM
     *  evaluator of particle momentum
     *
     *  @see LoKi::Cuts::GP
     *  @see LoKi::Cuts::GMOMENTUM
     *  @see LoKi::GenParticles::Momentum
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::Momentum                            GMOMENTUM ;
    // ========================================================================
    /** @var GPX
     *  evaluator of particle momentum
     *
     *  @see LoKi::Cuts::GMOMX 
     *  @see LoKi::GenParticles::MomentumX
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::MomentumX                                   GPX ;
    // ========================================================================
    /** @typedef GMOMX
     *  evaluator of particle momentum
     *
     *  @see LoKi::Cuts::GPX
     *  @see LoKi::GenParticles::MomentumX
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::MomentumX                               GMOMX ;
    // ========================================================================
    /** @var GPY
     *  evaluator of particle momentum
     *
     *  @see LoKi::Cuts::GMOMY
     *  @see LoKi::GenParticles::MomentumY
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::MomentumY                                   GPY ;
    // ========================================================================
    /** @typedef GMOMY
     *  evaluator of particle momentum
     *
     *  @see LoKi::Cuts::GPY
     *  @see LoKi::GenParticles::MomentumY
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::MomentumY                               GMOMY ;
    // ========================================================================
    /** @var GPZ
     *  evaluator of particle momentum
     *
     *  @see LoKi::Cuts::GMOMZ
     *  @see LoKi::GenParticles::MomentumZ
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::MomentumZ                                   GPZ ;
    // ========================================================================
    /** @typedef GMOMZ
     *  evaluator of particle momentum
     *
     *  @see LoKi::Cuts::GPZ
     *  @see LoKi::GenParticles::MomentumZ
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::MomentumZ                               GMOMZ ;
    // ========================================================================
    /** @var GPT
     *  evaluator of particle transverse momentum
     *
     *  @see LoKi::Cuts::GMOMT
     *  @see LoKi::GenParticles::TransverseMomentum
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::TransverseMomentum                          GPT ;
    // ========================================================================
    /** @typedef GMOMT
     *  evaluator of particle transverse momentum
     *
     *  @see LoKi::Cuts::GPZ
     *  @see LoKi::GenParticles::TransverseMomentum
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::TransverseMomentum                      GMOMT ;
    // ========================================================================
    /** @var GETA
     *  evaluator of particle pseudorapidity 
     *
     *  @see LoKi::Cuts::GPSEUDORAPIDITY 
     *  @see LoKi::GenParticles::PseudoRapidity
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::PseudoRapidity                             GETA ;
    // ========================================================================
    /** @typedef GPSEUDORAPIDITY
     *  evaluator of particle transverse momentum
     *
     *  @see LoKi::Cuts::GETA
     *  @see LoKi::GenParticles::PseudoRapidity
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::PseudoRapidity                GPSEUDORAPIDITY ;
    // ========================================================================
    /** @var GY
     *  evaluator of particle rapidity 
     *
     *  \f$ y = \frac{1}{2}\log \frac{ E + p_z }{ E - p_z } \f$ 
     *
     *  @see LoKi::Cuts::GRAPIDITY 
     *  @see LoKi::GenParticles::Rapidity
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::Rapidity                                     GY ;
    // ========================================================================
    /** @typedef GRAPIDITY
     *  evaluator of particle rapidity
     *
     *  \f$ y = \frac{1}{2}\log \frac{ E + p_z }{ E - p_z } \f$ 
     *
     *  @see LoKi::Cuts::GY
     *  @see LoKi::GenParticles::Rapidity
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::Rapidity                            GRAPIDITY ;
    // ========================================================================
    /** @var GY0
     *  evaluator of particle rapidity0
     *
     *  \f$ y_0 = \frac{1}{2}\log \frac{ E + p }{ E - p } \f$ 
     *
     *  @see LoKi::Cuts::GRAPIDITY0 
     *  @see LoKi::GenParticles::Rapidity0
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::Rapidity0                                   GY0 ;
    // ========================================================================
    /** @typedef GRAPIDITY0
     *  evaluator of particle rapidity0
     *
     *  \f$ y_0 = \frac{1}{2}\log \frac{ E + p }{ E - p } \f$ 
     *
     *  @see LoKi::Cuts::GY0
     *  @see LoKi::GenParticles::Rapidity0
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::Rapidity0                          GRAPIDITY0 ;
    // ========================================================================
    /** @var GPHI
     *  evaluator of particle \f$\phi\f$
     *
     *  @see LoKi::Cuts::GASYMUTH
     *  @see LoKi::GenParticles::Phi
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::Phi                                        GPHI ;
    // ========================================================================
    /** @typedef GASYMUTH
     *  evaluator of particle \f$\phi\f$
     *
     *  @see LoKi::Cuts::GPHI
     *  @see LoKi::GenParticles::Phi
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::Phi                                  GASYMUTH ;
    // ========================================================================
    /** @var GTHETA
     *  evaluator of particle \f$\theta\f$
     *
     *  @see LoKi::Cuts::GPOLAR
     *  @see LoKi::GenParticles::Theta
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    const LoKi::GenParticles::Theta                                    GTHETA ;
    // ========================================================================
    /** @typedef GPOLAR
     *  evaluator of particle \f$\theta\f$
     *
     *  @see LoKi::Cuts::GTHETA
     *  @see LoKi::GenParticles::Theta
     *
     *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
     *  @date 2006-01-18
     */
    typedef LoKi::GenParticles::Theta                                  GPOLAR ;
    // ========================================================================
    /** @typedef GDPHI 
     *  evaluator of \f$\Delta\phi\f$
     *  @see LoKi::GenParticles::DeltaPhi
     */
    typedef LoKi::GenParticles::DeltaPhi                                GDPHI ;    
    // ========================================================================
    /** @typedef GDETA
     *  evaluator of \f$\Delta\eta\f$
     *  @see LoKi::GenParticles::DeltaEta
     */
    typedef LoKi::GenParticles::DeltaEta                                GDETA ;    
    // ========================================================================
    /** @typedef GDR2
     *  evaluator of \f$\Delta\eta^2+\Delta\phi^2\f$
     *  @see LoKi::GenParticles::DeltaR2
     */
    typedef LoKi::GenParticles::DeltaR2                                  GDR2 ;    
    // ========================================================================
    /** @var GBETA 
     *  get \f$\beta\f$ for the particle 
     *  @see LoKi::GenParticles::Beta
     */
    const LoKi::GenParticles::Beta                                      GBETA ;
    // ========================================================================
    /** @var GGAMMA 
     *  get \f$\gamma\f$ for the particle 
     *  @see LoKi::GenParticles::Gamma
     */
    const LoKi::GenParticles::Gamma                                    GGAMMA ;
    // ========================================================================
    /** @var GBETAGAMMA 
     *  get \f$\beta\gamma\f$ for the particle 
     *  @see LoKi::GenParticles::BetaGamma
     */
    const LoKi::GenParticles::BetaGamma                            GBETAGAMMA ;
    // ========================================================================
    /** @var GBGAMMA 
     *  get \f$\beta\gamma\f$ for the particle 
     *  @see LoKi::GenParticles::BetaGamma
     */
    const LoKi::GenParticles::BetaGamma                               GBGAMMA ;
    // ========================================================================
    /** @var GBG
     *  get \f$\beta\gamma\f$ for the particle 
     *  @see LoKi::GenParticles::BetaGamma
     */
    const LoKi::GenParticles::BetaGamma                                   GBG ;
    // ========================================================================
  } //                                              end of namespace LoKi::Cuts 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_GENPARTICLES4_H
// ============================================================================

// $Id$
// ============================================================================
#ifndef LOKI_MISSINGPARTICLE_H 
#define LOKI_MISSINGPARTICLE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/KinTypes.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Kinematics
  {
    // ========================================================================
    /** @class MissingParticle LoKi/MissingParticle.h
     *  Simple two fold solution for missing particle. 
     *  Assuming the decay \f$ B \righarrrow R \nu \f$, where 
     *  - \f$B\f$ is "mother" particle
     *  - \f$R\f$ is "reconstructed" system
     *  - \f$\nu\f$ is some "missing" particle.
     *  When mass of "mother" and "missing"  particles are known, 
     *  and "mother" particle has known momentum direction 
     *  (e.g. "mother" particle is flying particle, and momenutm 
     *  direciton is known as vector from privry vertex to 
     *  origin vertex for "reconstructed" system).
     *  The solution (if exists) has two fold ambiguity. 
     *  @author Vanya Belyaev
     *  @date   2015-02-04
     */
    class GAUDI_API MissingParticle 
    {
    public :
      // ======================================================================
      /** constructor from all arguments 
       *  @param p4  four momentum of recotructed system 
       *  @param dir the flight direction of mother particle
       *  @param mother the mass of mother particle 
       *  @param missing the mass of missing particle 
       */
      MissingParticle
        ( const LoKi::LorentzVector& p4        , 
          const LoKi::ThreeVector&   direction ,
          const double               mother    , 
          const double               missing   ) ;
      /** constructor from all arguments 
       *  @param mother the mass of mother particle 
       *  @param missing the mass of missing particle 
       *  @param p4  four momentum of recotructed system 
       *  @param dir the flight direction of mother particle
       */
      MissingParticle
        ( const double               mother    , 
          const double               missing   , 
          const LoKi::LorentzVector& p4        , 
          const LoKi::ThreeVector&   direction ) ;
      // ======================================================================
    public:
      // ======================================================================
      // get number of solutions (0,1 or 2) 
      unsigned short nSolutions () const ;
      /// the first solution for 4-momentum of missing particle 
      const LoKi::LorentzVector& v1 () const { return m_v1 ; }
      /// the first solution for 4-momentum of missing particle 
      const LoKi::LorentzVector& v2 () const { return m_v2 ; }
      // ======================================================================
    private:
      // ======================================================================
      /// solve the system 
      double solve () ; // solve the system 
      // ======================================================================
    private:
      // ======================================================================
      /// default constructor is disabled ;
      MissingParticle() ; // default constructor is disabled ;
      // ======================================================================
    private:
      // ======================================================================
      /// 4-momentum of reconstructed system 
      LoKi::LorentzVector m_p4       ; // 4-momentum of reconstructed system 
      /// direction flight of the mother particle  
      LoKi::ThreeVector   m_dir      ; // direction flight of the mother particle  
      /// mass of the mother particle 
      double              m_mother2  ; // squared mass of the mother particle 
      /// mass of missing particle 
      double              m_missing2 ; // squared mass of missing particle 
      // =======================================================================
      /// the first  solution  for 4-momentum of missing particle 
      LoKi::LorentzVector m_v1      ; // the first solution  
      /// the second solution  for 4-momentum of missing particle 
      LoKi::LorentzVector m_v2      ; // the first solution
      /// determinant for quadratic equation 
      double              m_det     ; // determinant for quadratic equation 
      // =======================================================================
    };
    // =========================================================================
  } //                                         end of namespace LoKi::Kinematics 
  // ===========================================================================
} //                                                       end of namespace LoKi
// =============================================================================
//                                                                       The END 
// =============================================================================
#endif // LOKI_MISSINGPARTICLE_H
// =============================================================================

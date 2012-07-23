// $Id$
// ============================================================================
#ifndef LOKI_AUXDTFBASE_H 
#define LOKI_AUXDTFBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// Include files
// ============================================================================
// DaVinciInterfaces
// ============================================================================
#include "Kernel/IDecayTreeFit.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Interface.h"
#include "LoKi/AuxDesktopBase.h"
// ============================================================================
/** @file LoKi/AuxDTFBase.h
 *
 *  This file is a part of LoKi project -
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas,
 *  contributions and advices from G.Raven, J.van Tilburg,
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-06-03
 */
namespace LoKi
{
  // =========================================================================
  /** @class AuxDTFBase LoKi/AuxDTFBase.h
   *  
   *  Helper base class to deal with DecayTreeFitter 
   *  @see IDecayTreeFit
   *  @see  DecayTreeFit::Fitter 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
   *  @date   2010-06-03
   */
  class GAUDI_API AuxDTFBase : public virtual LoKi::AuxDesktopBase 
  {
  public:
    // ========================================================================
    /// the actual type to define the non-standard masses 
    typedef std::map<std::string,double>                              MASSES  ;
    /// the actual type to define the non-standard masses 
    typedef std::map<LHCb::ParticleID,double>                         MASSES2 ;
    // ========================================================================
  public:
    // ========================================================================
    /// constructor from the fitter 
    AuxDTFBase ( const IDecayTreeFit* fitter ) ;
    /// constructor from the fitter  
    AuxDTFBase ( const std::string&   fitter ) ;
    /// constructor from the fitter 
    AuxDTFBase ( const std::vector<std::string>&      constraints         , 
                 const MASSES&                        masses = MASSES ()  ,
                 const IDecayTreeFit*                 fitter = 0          ) ;
    /// constructor from the fitter 
    AuxDTFBase ( const std::vector<std::string>&      constraints         , 
                 const MASSES2&                       masses              ,
                 const IDecayTreeFit*                 fitter = 0          ) ;
    /// constructor from the fitter 
    AuxDTFBase ( const std::vector<LHCb::ParticleID>& constraints         , 
                 const MASSES&                        masses = MASSES ()  ,
                 const IDecayTreeFit*                 fitter = 0          ) ;
    /// constructor from the fitter 
    AuxDTFBase ( const std::vector<LHCb::ParticleID>& constraints         , 
                 const MASSES2&                       masses              ,
                 const IDecayTreeFit*                 fitter = 0          ) ;
    /// constructor from the fitter 
    AuxDTFBase ( const std::vector<std::string>&      constraints         ,
                 const std::string&                   fitter              ,
                 const MASSES&                        masses = MASSES ()  ) ;
    /// constructor from the fitter 
    AuxDTFBase ( const std::vector<std::string>&      constraints         , 
                 const std::string&                   fitter              ,
                 const MASSES2&                       masses              ) ;
    /// constructor from the fitter 
    AuxDTFBase ( const std::vector<LHCb::ParticleID>& constraints         , 
                 const std::string&                   fitter              ,
                 const MASSES&                        masses = MASSES ()  ) ;
    /// constructor from the fitter 
    AuxDTFBase ( const std::vector<LHCb::ParticleID>& constraints         , 
                 const std::string&                   fitter              ,
                 const MASSES2&                       masses              ) ;
    /// copy constructor 
    AuxDTFBase ( const AuxDTFBase& right ) ;
    /// virtual destructor
    virtual ~AuxDTFBase() ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    AuxDTFBase() ;                       // the default constructor is disabled 
    // ========================================================================
  public:
    // ========================================================================
    /// get the fitter  
    IDecayTreeFit* fitter() const ;                          // get the fitter  
    /// load the fitter 
    void loadFitter ( const std::string& name ) const ;
    /// get the fitter name 
    const std::string& fitterName() const { return m_fitterName ; }
    /// get constraints 
    std::vector<std::string> constraints ()  const ;
    /// get non-standard masses 
    MASSES                   masses      ()  const ;
    // ========================================================================    
  protected :
    // ========================================================================
    /// set vector of constraints
    unsigned int setConstraint ( const std::vector<std::string>&      pids ) ;
    /// set constraint
    unsigned int setConstraint ( const             std::string &      pids ) ;
    /// set vector of constraints
    unsigned int setConstraint ( const std::vector<LHCb::ParticleID>& pids ) ;
    /// set constraints
    unsigned int setConstraint ( const             LHCb::ParticleID&  pids ) ;
    /// set vector of constraints
    unsigned int setConstraint ( const MASSES&  pids ) ;    
    /// set vector of constraints
    unsigned int setConstraint ( const MASSES2& pids ) ;
    // apply mass-constraints  
    void applyConstraints () const ;
    // ========================================================================    
  protected :
    // ========================================================================
    /// print constraints 
    std::ostream& printConstraints ( std::ostream& s ) const ;
    // ========================================================================
  private:
    // ========================================================================
    /// the fitter name 
    std::string                            m_fitterName  ; // the fitter name 
    /// the fitter itself 
    mutable LoKi::Interface<IDecayTreeFit> m_fitter      ; // the fiter itself 
    /// the list of mass constraints 
    std::vector<LHCb::ParticleID>          m_constraints ; // mass-constarints
    /// the list of mass constraints 
    MASSES2                                m_masses      ; // mass-constraints
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace LoKi 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_AUXDTFBASE_H
// ============================================================================

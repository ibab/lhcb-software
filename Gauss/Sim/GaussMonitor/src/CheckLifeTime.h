// $Id: CheckLifeTime.h,v 1.2 2008-12-08 10:11:22 ibelyaev Exp $
// ============================================================================
#ifndef GAUSS_CHECKLIFETIME_H 
#define GAUSS_CHECKLIFETIME_H 1
// ============================================================================
// STD & STL 
// ============================================================================
#include <math.h>
// ============================================================================
// AIDA 
// ============================================================================
#include "AIDA/IHistogram1D.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/PhysicalConstants.h"
// ============================================================================
// Kernel/PartProp
// ============================================================================
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiHistoAlg.h"
// ============================================================================
// local
// ============================================================================
#include "LifeTime.h"
// ============================================================================
namespace GaussMonitor
{
  // ==========================================================================
  /** @class CheckLifeTime
   *  simple algorithm (base class) 
   *  to check the lifetime of particles 
   *
   *  Essentially it has two major properties
   *
   *   - <c>Frequency</c>:
   *         the property which indicates the printout requency
   *         the defautl valeu corresponds to "eventy 100 events"
   *   - <c>Particles</c>: 
   *         the list of particles which lifetiems to be checked, 
   *         default value corresponds to *almost* all stable charm and beauty 
   *         particles: [ "B0" , "B+" , "B_s0" , "B_c+" , "Lambda_b0" , 
   *         "D0" , "D+" , "D_s+" , "Lambda_c+" ]. The antiparticles are taken  
   *         into accout autonatically.   
   *
   *  The algorithm fill few counters and histograms.
   *  Af the finalization step (and each "Frequency" event) 
   *  it performs the analysis of the current status of the 
   *  counters and makes printout:
   * @code 
   *    INFO  >0.5 sigma deviation in lifetime for 'B0'        Actual is 0.489439+-0.049298(0.45982) Nominal is 0.46048 [mm] 
   *    INFO  >0.5 sigma deviation in lifetime for 'B+'        Actual is 0.528686+-0.054560(0.47250) Nominal is 0.50095 [mm] 
   *    INFO  <0.5 sigma deviation in lifetime for 'B_s0'      Actual is 0.471664+-0.100333(0.45978) Nominal is 0.43799 [mm] 
   * WARNING  >1   sigma deviation in lifetime for 'Lambda_b0' Actual is 0.282185+-0.059657(0.23105) Nominal is 0.36844 [mm] 
   * WARNING  >1   sigma deviation in lifetime for 'D0'        Actual is 0.143134+-0.011404(0.13151) Nominal is 0.12340 [mm] 
   *    INFO  >0.5 sigma deviation in lifetime for 'D+'        Actual is 0.295561+-0.035003(0.26194) Nominal is 0.31521 [mm] 
   *    INFO  >0.5 sigma deviation in lifetime for 'D_s+'      Actual is 0.133379+-0.026760(0.16712) Nominal is 0.14704 [mm] 
   * WARNING  >1   sigma deviation in lifetime for 'Lambda_c+' Actual is 0.077262+-0.013329(0.05655) Nominal is 0.05979 [mm] 
   * @endcode 
   *
   *  - ">3.0 sigma" discrepances are printed as <b>FATAL</b>
   *  - ">2.0 sigma" discrepances are printed as <b>ERROR</b>
   *  - ">1.0 sigma" discrepances are printed as <b>WARNING</b>
   *  - ">0.5 sigma" discrepances are printed as <b>INFO</b>
   *  - "<0.5 sigma" discrepances are printed as <b>INFO</b>
   *
   *  At the finalization, in addition to the fatal/error/warning 
   *  messages the calls for Error/Warnings functions are performed
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2008-06-14
   */
  class CheckLifeTime: public GaudiHistoAlg
  {
  public:
    // ========================================================================
    /// standard initialization of the algorithm 
    virtual StatusCode initialize ()  ;
    /// standard finalization   of  the algorithm 
    virtual StatusCode finalize   ()  ;
    // ========================================================================
  protected:
    // ========================================================================
    /** The standard constructor 
     *  @param name the algorithm instance name 
     *  @param pSvc service locator
     */
    CheckLifeTime 
    ( const std::string& name ,   // algorithm instance name 
      ISvcLocator*       pSvc ) ; // service locator 
    /// protected and virtual destructor 
    virtual ~CheckLifeTime() {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    CheckLifeTime () ;                                // no default constructor 
    /// Copy constructor is disabled 
    CheckLifeTime ( const CheckLifeTime& ) ;             // no copy constructor 
    /// Assignement operator is disabled 
    CheckLifeTime& operator=( const CheckLifeTime& ) ;        // no assignement
    // ========================================================================
  protected:
    // ========================================================================
    /// check the lifetimes 
    void check ( const bool iErr ) const ;
    // ========================================================================
    typedef std::vector<std::string>  PIDs ;
    // ========================================================================
    const PIDs& particles() const { return m_particles ; }
    // ========================================================================
    /// check the event? 
    bool checkEvent () 
    {
      if ( 0 == m_frequency || 0 != ++m_event % m_frequency ) { return false ; }
      m_event = 0 ;
      return false ;
    }
    // ========================================================================
  private:
    // ========================================================================
    /// the list of the particles 
    PIDs m_particles ;                             // the list of the particles
    /// how often to check the lifetimes :
    unsigned int m_frequency ;              // how often to check the lifetimes 
    /// event counter 
    unsigned int m_event     ;                             // the event counter
    // ========================================================================
  } ;  
  // ==========================================================================
} // end of namespace HidValley 
// ============================================================================
// The END 
// ============================================================================
#endif // COMPONENTS_CHECKLIFETIME_H
// ============================================================================

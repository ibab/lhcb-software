// $Id$
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IDecay.h"
#include "LoKi/Decays.h"
// ============================================================================
// local
// ============================================================================
#include "FilterDesktop.h"
// ============================================================================
/** @class FilterDecays  
 *  Simple algorithm to filter certain decays or decay components 
 *
 *  The algorithm selects certain decays or decay components 
 * 
 *  @code 
 * 
 *  from Configurables import FilterDecays 
 *
 *  myAlg1 = FilterDecays ( 
 *     ...                                                 , 
 *     Code          = " [B_s0 -> J/psi(1S) phi(1020)]CC"  , 
 *     ...                                                 ,
 *     ) 
 *    
 *  myAlg2 = FilterDecays ( 
 *     ...                                                 , 
 *     Code          = " [B_s0 -> ^J/psi(1S) phi(1020)]CC" , 
 *     ...                                                 ,
 *     ) 
 *    
 *  @endcode  
 *
 *  @author Vanya BELYAEV   Ivan.Belyaev@cern.ch
 *  @date 2011-05-13
 *                    $Revision$
 *  Last modification $Date$
 *                by  $Author$
 */
class FilterDecays : public FilterDesktop 
{
  // ==========================================================================
  /// friend factory for instantiation 
  friend class AlgFactory<FilterDecays> ;
  // ==========================================================================
 public:
  // ==========================================================================
  /// finalize  the algorithm 
  virtual StatusCode finalize   () ; 
  // ==========================================================================
protected:
  // ==========================================================================
  /** standard constructor 
   *  @see DVAlgorithm
   *  @see GaudiTupleAlg
   *  @see GaudiHistoAlg
   *  @see GaudiAlgorithm
   *  @see Algorithm
   *  @see AlgFactory
   *  @see IAlgFactory
   *  @param name the algorithm instance name 
   *  @param pSvc pointer to Service Locator 
   */
  FilterDecays                                 //        standard constructor 
  ( const std::string& name ,                  // the algorithm instance name 
    ISvcLocator*       pSvc ) ;                //  pointer to Service Locator
  // ==========================================================================
  /// virtual & protected destructor 
  virtual ~FilterDecays () ;                  // virtual & protected destructor  
  // ==========================================================================
public:
  // ==========================================================================
  /** the major method for filter input particles 
   *  @param input    (INPUT) the input  container of particles
   *  @param filtered (OUPUT) the output container of particles
   *  @return Status code 
   */
  virtual StatusCode filter 
  ( const LHCb::Particle::ConstVector& input    , 
    LHCb::Particle::ConstVector&       filtered ) ;
  // ==========================================================================
  /// decode the code
  virtual StatusCode decodeCode () ;
  // ==========================================================================
private:
  // ==========================================================================
  /// the default constructor is disabled 
  FilterDecays () ;                       // the default consructor is disabled
  /// copy constructor is disabled 
  FilterDecays ( const FilterDecays& ) ;        // copy constructor is disabled 
  /// assignement operator is disabled 
  FilterDecays& operator=( const FilterDecays& ) ;            // no assignement
  // ==========================================================================
protected:
  // ==========================================================================
  const Decays::IDecay::Finder& finder() const { return m_finder ; }
  // ==========================================================================
private:
  // ==========================================================================
  /// the decay finder 
  Decays::IDecay::Finder m_finder  ;     //                    the decay finder 
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================

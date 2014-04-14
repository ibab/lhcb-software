// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// Local/Kali
// ============================================================================
#include "Destroyer.h"
// ============================================================================
/** @file Cloner.cpp
 *  ''Piecefull'' variant of Kali::Destroyer 
 *
 *  @see Kali::Destroyer 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 *  @date 2011-07-31
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace Kali 
{
  // ==========================================================================
  /** @class cloner 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2011-07-31
   *  
   *                    $Revision$
   *  Last modification $Date$
   *                 by $Author$
   */
  class Cloner : public Kali::Destroyer 
  {
    // ========================================================================
    /// the friend factory for instantiation
    friend class AlgFactory<Kali::Cloner> ;               // the friend factory
    // ========================================================================
  public:
    // ========================================================================
    /// the only one essential method 
    virtual StatusCode execute    () ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard destructor 
     *  @param name (INPUT) the algorithm instance name 
     *  @param pSvc (INPUT) the pointer to Service Locator 
     */
    Cloner ( const std::string& name ,     //    the algorithm instance name 
                ISvcLocator*       pSvc )     // the pointer to Service Locator 
      : Destroyer ( name , pSvc ) 
    {}
    /// virtual & protected destructor 
    virtual ~Cloner () {}                     // virtual & protected destructor 
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Cloner ();                           // the default constructor is disabled 
    /// the copy constructor is disabled 
    Cloner ( const Cloner& );            //   the cpoy  constructor is disabled     
    /// the assignement operator is disabled
    Cloner & operator=( const Cloner& ); //         the assignement is disabled
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace Kali 
// ============================================================================
// the only one essential method 
// ============================================================================
StatusCode Kali::Cloner::execute () 
{ 
  // collect all interetsing digits 
  LHCb::CaloDigit::Set     digits ;
  // collect all interesting tracks 
  TRACKS                   tracks ;
  
  // 
  // collect info 
  StatusCode sc = collect ( digits , tracks ) ;
  if ( sc.isFailure() ) 
  {
    Warning ("Error from collect", sc ) ;
    setFilterPassed ( false ) ;
    return StatusCode::SUCCESS ;
  }
  //
  const bool OK = !digits.empty() || !tracks.empty();
  //
  // copy/clone TES
  sc = copy ( digits , tracks ) ;
  if ( sc.isFailure() ) 
  {
    Warning ("Error from destroy", sc ) ;
    setFilterPassed ( false ) ;
    return StatusCode::SUCCESS ;
  }
  //
  setFilterPassed  ( OK ) ;
  //
  return StatusCode::SUCCESS ;        
}    
// ===========================================================================
// The factory:
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Kali,Cloner)
// ============================================================================
// The END 
// ============================================================================

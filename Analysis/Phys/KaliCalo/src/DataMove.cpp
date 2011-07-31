// $Id: $
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// Event
// ============================================================================
#include "Event/Track.h"
#include "Event/CaloDigit.h"
// ============================================================================
// Local/Kali
// ============================================================================
#include "Kali.h"
// ============================================================================
namespace Kali 
{
  // ==========================================================================
  /** @class DataMove
   *
   *  simple data-mover, to recover TES structure 
   *
   *  Unfortunately links in TES does not work :-(
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date 2011-07-31
   *  
   *                    $Revision$
   *  Last modification $Date$
   *                 by $Author$
   */  
  class DataMove : public GaudiAlgorithm
  {
    // ========================================================================
    /// the friend factory for instantiation 
    friend class AlgFactory<Kali::DataMove> ;
    // ========================================================================
  public:
    // ========================================================================
    /// the only one essential method 
    virtual StatusCode execute()  ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard constructor 
     *  @param name algorithm instance name 
     *  @param psvc pointer to Service Locator 
     */
    DataMove  ( const std::string& name ,  // algorithm instance name 
               ISvcLocator*       pSvc )  // poiunetr to Service locator
      : GaudiAlgorithm ( name , pSvc ) 
      , m_locations (        ) 
      , m_kaliHat   ( "Kali" ) 
    {
      //
      m_locations.push_back ( LHCb::TrackLocation     ::Default ) ;
      m_locations.push_back ( LHCb::CaloDigitLocation ::Spd     ) ;
      m_locations.push_back ( LHCb::CaloDigitLocation ::Prs     ) ;
      m_locations.push_back ( LHCb::CaloDigitLocation ::Ecal    ) ;
      m_locations.push_back ( LHCb::CaloDigitLocation ::Hcal    ) ;
      //
      declareProperty 
        ( "Locations" , 
          m_locations , 
          "The list of locations to be moved " ) ;
    }
    /// virtual & protected destructor 
    virtual ~DataMove() {}
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    DataMove () ;                        // the default constructor is disabled 
    /// the copy constructor is disabled 
    DataMove ( const DataMove& ) ;          // the copy constructor is disabled 
    /// the assignement operato is disabled 
    DataMove& operator=( const DataMove& ) ;     // the assignement is disabled 
    // ========================================================================
  private:
    // ========================================================================
    /// The list of locations to be created
    std::vector<std::string> m_locations ; //     List of locations to be moved 
    /// Kali-hat in TES
    std::string              m_kaliHat   ; //                   Kali-hat in TES
    // ========================================================================
  } ;
  // ==========================================================================
} //                                                      end of namespace Kali 
// ============================================================================
// the only one essential method 
// ============================================================================
StatusCode Kali::DataMove::execute()  
{
  //
  for (  std::vector<std::string>::const_iterator ites = m_locations.begin() ; 
         m_locations.end() != ites ; ++ites ) 
  {
    std::string loc = Kali::kalify ( *ites , m_kaliHat ) ;
    if ( !exist<DataObject>( loc ) ) 
    { 
      Warning ( "Location does not exist: '" + loc + "'") ;
      continue ;                                                 // CONTINUE 
    }
    //
    DataObject* obj = get<DataObject> ( loc ) ;
    // unregister it at "kali" location 
    StatusCode sc = evtSvc()->unregisterObject ( obj ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "Unable unregister '" + loc + "'" , sc ) ;
      continue ;                                              // CONTINUE 
    }
    // register it at "standard" location 
    put ( obj , *ites ) ;
  }
  //
  return StatusCode::SUCCESS ;
  //
}
// ============================================================================
// The Factory
// ============================================================================
DECLARE_NAMESPACE_ALGORITHM_FACTORY(Kali,DataMove)
// ============================================================================
//                                                                      The END 
// ============================================================================

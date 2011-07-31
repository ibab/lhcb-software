// $Id: Destroyer.cpp 122804 2011-05-04 17:53:14Z ibelyaev $
// ============================================================================
#ifndef KALI_DESTROYER_H 
#define KALI_DESTROYER_H 1 
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <set>
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
#include "Event/CaloHypo.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDigit.h"
// ============================================================================
#include "Event/CaloDataFunctor.h"
// ============================================================================
#include "Event/ProtoParticle.h"
// ============================================================================
// CaloInterfaces
// ============================================================================
#include "CaloInterfaces/ICaloDigits4Track.h"
// ============================================================================
// Local/Kali
// ============================================================================
#include "Kali.h"
// ============================================================================
namespace Kali 
{
  // ==========================================================================
  /** @class Destroyer
   *
   *  the base class for TES manipulation in Kali 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  
   *                    $Revision$
   *  Last modification $Date$
   *                 by $Author$
   */
  class Destroyer : public GaudiAlgorithm
  {
    // ========================================================================
    /// the friend factory for instantiation
    friend class AlgFactory<Kali::Destroyer> ;            // the friend factory
    // ========================================================================
  public:
    // ========================================================================
    /// initialize 
    virtual StatusCode initialize () ;
    /// the only one essential method 
    virtual StatusCode execute    () ;
    /// finalize 
    virtual StatusCode finalize   () ;
    // ========================================================================
  protected:
    // ========================================================================
    /// perform "kalification" of input TES location
    std::string kalify ( const std::string& input ) const 
    { return Kali::kalify ( input , m_kaliHat ) ; }
    // ========================================================================
  protected:
    // ========================================================================
    typedef std::set<const LHCb::Track*> TRACKS ;
    // ========================================================================
    /// collect all ``interesting'' digits & tracks 
    StatusCode collect 
    ( LHCb::CaloDigit::Set& digits , 
      TRACKS&               tracks ) const ;
    // ========================================================================
    /// destroy all ``non-interesting'' information 
    StatusCode destroy 
    ( LHCb::CaloDigit::Set& digits , 
      TRACKS&               tracks ) const ;
    // ========================================================================
    /// copy all   ``interesting'' information 
    StatusCode copy 
    ( LHCb::CaloDigit::Set& digits , 
      TRACKS&               tracks ) const ;
    // ========================================================================
  protected:
    // ========================================================================
    /** standard destructor 
     *  @param name (INPUT) the algorithm instance name 
     *  @param pSvc (INPUT) the pointer to Service Locator 
     */
    Destroyer ( const std::string& name ,     //    the algorithm instance name 
                ISvcLocator*       pSvc )     // the pointer to Service Locator 
      : GaudiAlgorithm( name , pSvc ) 
      , m_particles ()
      , m_destroy   ( true    )
      , m_kaliHat   ( "Kali"  ) 
      , m_toolNames ()
      , m_tools ()
      , m_bremNames ()
      , m_brems ()  
    {
      //
      declareProperty 
        ( "Particles" , 
          m_particles , 
          "The list of input TES locations for particles" );
      declareProperty
        ( "Destroy" ,
          m_destroy , 
          "Destroy the content of TES containers" ) ->
        declareUpdateHandler(&Destroyer::updateDestroy, this ) ; 
      declareProperty 
        ( "KaliHat" , 
          m_kaliHat , 
          "Hat for Kali stuff in TES" );
     //
      m_toolNames.push_back ( "SpdEnergyForTrack/SpdDigits"   ) ;
      m_toolNames.push_back ( "PrsEnergyForTrack/PrsDigits"   ) ;
      m_toolNames.push_back ( "EcalEnergyForTrack/EcalDigits" ) ;
      m_toolNames.push_back ( "HcalEnergyForTrack/HcalDigits" ) ;
      declareProperty 
        ( "Digits4Track" , 
          m_toolNames ,
          "Tools to collect the Calo-digits for the track" );
      m_bremNames.push_back ( "SpdEnergyForTrack/BremSpdDigits"   ) ;
      m_bremNames.push_back ( "PrsEnergyForTrack/BremPrsDigits"   ) ;
      m_bremNames.push_back ( "EcalEnergyForTrack/BremEcalDigits" ) ;
      declareProperty 
        ( "BremDigits4Track" , 
          m_bremNames ,
          "Tools to collect the Brem-digits for the track" );
      // ======================================================================
    }
    /// virtual & protected destructor 
    virtual ~Destroyer() {}                   // virtual & protected destructor 
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    Destroyer ();                        // the default constructor is disabled 
    /// the copy constructor is disabled 
    Destroyer ( const Destroyer& );        // the cpoy  constructor is disabled     
    /// the assignement operator is disabled 
    Destroyer& operator=( const Destroyer& );    // the assignement is disabled
    // ========================================================================
  public:
    // ========================================================================
    /// Update handler for the property
    void updateDestroy ( Property& /* p */ ) ;
    // ========================================================================
  private:
    // ========================================================================
    typedef std::vector<std::string> Locations ;
    /// the list of input locations for particles 
    Locations m_particles ;        // the list of input locations for particles 
    /// flag to actually destroy TES 
    bool      m_destroy   ;                    //  flag to actually destroy TES 
    /// Kali-hat in TES
    std::string m_kaliHat ;                    // Kali-hat in TES
    // ========================================================================
    typedef std::vector<std::string>              Names ;
    typedef std::vector<const ICaloDigits4Track*> Tools ;
    /// the list of tools to accumulate the digits  
    Names m_toolNames ;           // the list of tools to accumulate the digits  
    /// the list of tools to accumulate the digits  
    Tools m_tools     ;           // the list of tools to accumulate the digits  
    /// tools for brem-digits collection 
    Names m_bremNames ;                     // tools for brem-digits collection 
    /// tools for brem-digits collection 
    Tools m_brems     ;                     // tools for brem-digits collection 
    // ========================================================================
  };
  // ==========================================================================
} //                                                      end of namespace Kali
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // KALI_DESTROYER_H 1 
// ============================================================================
// The END 
// ============================================================================

  


// $Id: GetGiGaHitsAlg.cpp,v 1.4 2003-07-16 12:43:29 seaso Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/MsgStream.h"

// GiGa 
#include "GiGa/IGiGaSvc.h"

/// from Event 
#include "Event/MCHit.h"
#include "Event/MCVeloHit.h"
#include "Event/MCMuonHit.h"
#include "Event/MCCaloHit.h"
#include "Event/MCMuonHitPath.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"

// local
#include "GetGiGaHitsAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GetGiGaHitsAlg
//
// 2002-08-13 : Witold Pokorski
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<GetGiGaHitsAlg>          s_factory ;
const        IAlgFactory& GetGiGaHitsAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetGiGaHitsAlg::GetGiGaHitsAlg( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) 
  , m_othits      ( MCHitLocation::OTHits   )
  , m_ithits      ( MCHitLocation::ITHits   )
  , m_velohits    ( MCVeloHitLocation::Default )
  , m_puvelohits  ( MCVeloHitLocation::PuVeto )
  , m_muonhits    ( MCMuonHitLocation::MCMuonHits )
  , m_richhits    ( MCRichHitLocation::Default )
  , m_richop      ( MCRichOpticalPhotonLocation::Default )
  , m_caloHits    ()
{ 
  m_caloHits.push_back ( MCCaloHitLocation::Spd  ) ;
  m_caloHits.push_back ( MCCaloHitLocation::Prs  ) ;
  m_caloHits.push_back ( MCCaloHitLocation::Ecal ) ;
  m_caloHits.push_back ( MCCaloHitLocation::Hcal ) ;
  
  declareProperty( "OTHits"    , m_othits     ); 
  declareProperty( "ITHits"    , m_ithits     );
  declareProperty( "VeloHits"  , m_velohits   );
  declareProperty( "PuVeloHits", m_puvelohits );
  declareProperty( "MuonHits"  , m_muonhits   );
  declareProperty( "RichHits"  , m_richhits   );
  declareProperty( "RichOpticalPhot"  , m_richop   );
  declareProperty( "CaloHits"  , m_caloHits   );
}

//=============================================================================
// Destructor
//=============================================================================
GetGiGaHitsAlg::~GetGiGaHitsAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode GetGiGaHitsAlg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode GetGiGaHitsAlg::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

      if( !m_othits.empty() )
      {
        SmartDataPtr<MCHits> obj( eventSvc() , m_othits ) ;
        if( obj ) 
          { 
            log << MSG::INFO
                << "Number of extracted OThits  '"
                << m_othits << "'  \t" 
                << obj->size() 
                << endreq ;
          Stat stat( chronoSvc() , "#hits" , obj->size() ) ; 
 
          MCHits::const_iterator hiter;
          for(hiter=obj->begin(); hiter!=obj->end(); hiter++)
            {
              log << MSG::VERBOSE << "Energy of OTHit: "
                  << (*hiter)->energy() 
                  << "     MCParticle: " << (*hiter)->mcParticle()
                ->particleID().pid()
                  << "     Energy: " << (*hiter)->mcParticle() 
                -> momentum().e()
                  << endreq;
            }
        } 
      else 
        { 
          log << MSG::INFO 
              << " No 'MCHits' to be extracted from '"
              << m_othits << "' \t" 
              << endreq ;
          ///
        } 
    }
  ///
    if( !m_ithits.empty() )
      {
        SmartDataPtr<MCHits> obj( eventSvc() , m_ithits ) ;
        if( obj ) 
          { 
            log << MSG::INFO
                << "Number of extracted IThits  '"
                << m_ithits << "'  \t" 
                << obj->size() 
                << endreq ;
          Stat stat( chronoSvc() , "#hits" , obj->size() ) ; 

          MCHits::const_iterator hiter;
          for(hiter=obj->begin(); hiter!=obj->end(); hiter++)
            {
              log << MSG::VERBOSE << "Energy of ITHit: "
                  << (*hiter)->energy() 
                  << "     MCParticle: " << (*hiter)->mcParticle()
                ->particleID().pid()
                  << "     Energy: " << (*hiter)->mcParticle() 
                -> momentum().e()
                  << endreq;
            }
        } 
      else 
        { 
          log << MSG::INFO 
              << " No 'MCHits' to be extracted from '"
              << m_ithits << "' \t" 
              << endreq ;
          ///
        } 
    }
  ///
    if( !m_velohits.empty() )
      {
        SmartDataPtr<MCVeloHits> obj( eventSvc() , m_velohits ) ;
        if( obj ) 
          { 
            log << MSG::INFO
                << "Number of extracted Velohits  '"
                << m_velohits << "'  \t" 
                << obj->size() 
                << endreq ;
          Stat stat( chronoSvc() , "#hits" , obj->size() ) ;          

          MCVeloHits::const_iterator vhiter;          
          for(vhiter=obj->begin(); vhiter!=obj->end(); vhiter++)
            {
              log << MSG::VERBOSE << "E_MCVeloHit: "
                  << (*vhiter)->energy() 
                  << "   Sensor: " << (*vhiter)-> sensor()
                  << "   MCParticle: " << (*vhiter)->mcParticle()
                ->particleID().pid()
                  << "     Energy: " << (*vhiter)->mcParticle() 
                -> momentum().e()
                  <<  endreq;
            }
        } 
      else 
        { 
          log << MSG::INFO 
              << " No 'MCVeloHits' to be extracted from '"
              << m_velohits << "' \t" 
              << endreq ;
          ///
        } 
    }
  ///
    if( !m_puvelohits.empty() )
      {
        SmartDataPtr<MCVeloHits> obj( eventSvc() , m_puvelohits ) ;
        if( obj ) 
          { 
            log << MSG::INFO
                << "Number of extracted PuVelohits  '"
                << m_puvelohits << "'  \t" 
                << obj->size() 
                << endreq ;
          Stat stat( chronoSvc() , "#hits" , obj->size() ) ; 
        } 
      else 
        { 
          log << MSG::INFO 
              << " No 'MCVeloHits' to be extracted from '"
              << m_puvelohits << "' \t" 
              << endreq ;
          ///
        } 
    }
  ///
    if( !m_muonhits.empty() )
      {

        for(int i=0;i<15;i++)
          {
            SmartDataPtr<MCMuonHits> obj( eventSvc() , 
                                          MCMuonHitPath::MCMuonHitPath[i]) ;
            if( obj ) 
              { 
                log << MSG::INFO
                    << "Number of extracted Muonhits  '"
                    << MCMuonHitPath::MCMuonHitPath[i] << "'  \t"
                    << obj->size()
                    << endreq ;
                Stat stat( chronoSvc() , "#hits" , obj->size() ) ; 
              } 
            else 
              { 
                log << MSG::INFO 
                    << " No 'MCMuonHits' to be extracted from '"
                    << MCMuonHitPath::MCMuonHitPath[i] << "' \t" 
                    << endreq ;
                ///
              }
          }
      }
  ///
    if( !m_richhits.empty() )
      {
        SmartDataPtr<MCRichHits> 
          obj( eventSvc(), m_richhits) ;

            if( obj ) 
              { 
                log << MSG::INFO
                    << "Number of extracted Richhits  \t"
                    << obj->size()
                    << endreq ;
                Stat stat( chronoSvc() , "#hits" , obj->size() ) ; 
              } 
            else 
              { 
                log << MSG::INFO 
                    << " No 'MCRichHits' to be extracted from '"
                    << m_richhits << "' \t" 
                    << endreq ;
                ///
              }
      }
  ///
    if( !m_richop.empty() )
      {
        SmartDataPtr<MCRichOpticalPhotons> 
          obj( eventSvc(), m_richop) ;

            if( obj ) 
              { 
                log << MSG::INFO
                    << "Number of extracted RichOpticalPhot  \t"
                    << obj->size()
                    << endreq ;
                Stat stat( chronoSvc() , "#hits" , obj->size() ) ; 
              } 
            else 
              { 
                log << MSG::INFO 
                    << " No 'MCRichOpticalPhotons' to be extracted from '"
                    << MCRichOpticalPhotonLocation::Default << "' \t" 
                    << endreq ;
                ///
              }
      }
    
    /// calorimeter hits 
    for(Addresses::const_iterator address = m_caloHits.begin() ; 
        m_caloHits.end() != address ; ++address )
      {
        SmartDataPtr<MCCaloHits> hits( eventSvc() , *address );
        if( hits ) 
          {
            log << MSG::INFO
                << "Number of extracted MCCaloHits  '" << *address << "' is "
                << hits->size() << endreq ;
          }
        else 
          {
            log << MSG::INFO 
                << " No 'MCCaloHits' extracted from '"
                << *address << "'" << endreq ;
          }
      }
    
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetGiGaHitsAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================

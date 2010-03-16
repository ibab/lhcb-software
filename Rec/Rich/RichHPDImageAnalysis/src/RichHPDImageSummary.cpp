// $Id: RichHPDImageMovement.cpp,v 1.2 2009/11/25 18:35:55 tblake Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "RichHPDImageSummary.h"
#include "Event/ODIN.h"
#include "GaudiUtils/Aida2ROOT.h"

// local files
#include "HPDBoundaryFcn.h"

// Include root + Minuit2
#include "TH2D.h"
#include "Minuit2/FunctionMinimum.h"
#include "Minuit2/MnUserParameterState.h"
#include "Minuit2/MnMigrad.h"

#include "boost/lexical_cast.hpp"

//-----------------------------------------------------------------------------
// Implementation file for class : RichHPDImageSummary
//
// 2010-03-16 : Thomas BLAKE
//-----------------------------------------------------------------------------


namespace Rich 
{
  namespace Mon 
  {
    
    // Declaration of the Algorithm Factory    
    DECLARE_ALGORITHM_FACTORY( RichHPDImageSummary );
    
    
    //=============================================================================
    // Standard constructor, initializes variables
    //=============================================================================
    
    RichHPDImageSummary::RichHPDImageSummary( const std::string& name,
                                                ISvcLocator* pSvcLocator)
      : RichRecHistoAlgBase( name , pSvcLocator ) ,  
        m_nEvt( 0 )
    {
      declareProperty( "MakeHistos" , m_bookHistos = true );
      declareProperty( "DisplaySmartIDWarnings" , m_displayWarnings = false );
      declareProperty( "CutThreshold" , m_cutFraction = 0.1 );
    }
    
    //=============================================================================
    // Destructor
    //=============================================================================
    
    RichHPDImageSummary::~RichHPDImageSummary() {} ;
    
    //=============================================================================
    // Initialization
    //=============================================================================
    
    StatusCode RichHPDImageSummary::initialize() {
      
      StatusCode sc = RichRecHistoAlgBase::initialize(); // must be executed first
      if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
      
      if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
      m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );
      
      acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );      
      
      const LHCb::RichSmartID::Vector &activeHPDs  = m_RichSys->activeHPDRichSmartIDs();
      
      LHCb::RichSmartID::Vector::const_iterator iHPD;
      
      
      for ( iHPD = activeHPDs.begin(); iHPD != activeHPDs.end(); ++iHPD ) {
        const Rich::DAQ::HPDCopyNumber hpdID = m_RichSys->copyNumber( *iHPD );
        
        std::string name = "Rich_HPD_" + boost::lexical_cast<std::string>( hpdID.data() );
        name += "_Image";
        
        if ( msgLevel(MSG::DEBUG) ) debug() << " Booking histogram " << name << endmsg ;
        
        m_histo.insert( std::make_pair( hpdID.data() ,this->create2D(name) ) );
      }      
      return StatusCode::SUCCESS;
    }
    
    TH2D* RichHPDImageSummary::create2D( const std::string& name )
    {
      if ( m_bookHistos ){
        IHistogram2D* hist = book2D(name,name,-0.5,31.5,32, -0.5,31.5,32) ;
        return Gaudi::Utils::Aida2ROOT::aida2root( hist );
      }
      
      return new TH2D(name.c_str(),name.c_str(),32,-0.5,31.5,32,-0.5,31.5 ) ;
    }
    
    
    //=============================================================================
    // Main execution
    //=============================================================================
    
    StatusCode RichHPDImageSummary::timeInformation()
    {
      LHCb::ODIN* odin = get<LHCb::ODIN*>( LHCb::ODINLocation::Default );
      if ( NULL == odin ){
        return StatusCode::FAILURE;
      }
      else{
        counter("RICH_EventTime") += odin->gpsTime() ;
      }
      return StatusCode::SUCCESS ;
    }
    

    StatusCode RichHPDImageSummary::execute() {
      
      if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
      m_nEvt++;
      
      StatusCode sc = timeInformation();
      if ( sc.isFailure() && m_displayWarnings ){
        warning() << " Unable to retrieve ODIN " << endmsg;
      }
      
      // Standard loop over Rich Smart IDs 
      const Rich::DAQ::L1Map& mapUKL1 = m_SmartIDDecoder->allRichSmartIDs();
      Rich::DAQ::L1Map::const_iterator iUKL1;
      
      for ( iUKL1 = mapUKL1.begin(); iUKL1 != mapUKL1.end(); ++iUKL1 ){
        const Rich::DAQ::IngressMap& mapIngress = iUKL1->second;
        Rich::DAQ::IngressMap::const_iterator iIngress;
        
        for ( iIngress = mapIngress.begin(); iIngress != mapIngress.end(); ++iIngress ){
          
          const Rich::DAQ::HPDMap& mapHPD = (iIngress->second).hpdData() ;
          Rich::DAQ::HPDMap::const_iterator iHPD;
          
          for ( iHPD = mapHPD.begin(); iHPD != mapHPD.end(); ++iHPD ){
            
            const LHCb::RichSmartID &smartID = (iHPD->second).hpdID();
            
            if ( !smartID.isValid() ) {
              if ( m_displayWarnings ) {
                warning() << " Invalid Rich Smart ID " << endmsg;
              } 
              continue;
            }
        
            const LHCb::RichSmartID::Vector& hitIDs = (iHPD->second).smartIDs() ;
        
            LHCb::RichSmartID::Vector::const_iterator iHit;
            
            const Rich::DAQ::HPDCopyNumber hpdID = m_RichSys->copyNumber( smartID );
            TH2D* hist = m_histo[ hpdID.data() ];
            
            if ( NULL == hist ) {
              if ( m_displayWarnings ) {
                warning() << " Can not retrieve boundary FCN, invalid hardware ID " << endmsg;
              } 
              continue;
            }

            for ( iHit = hitIDs.begin(); iHit != hitIDs.end(); ++iHit ){
              hist->Fill(  iHit->pixelCol(), iHit->pixelRow() ) ;
            }
          }
        }
      }  
      
      return StatusCode::SUCCESS;
    }
    
    //=============================================================================
    //  Finalize
    //=============================================================================
    

    void RichHPDImageSummary::summaryINFO( const unsigned int ID, const TH2D* hist ) const
    {
      HPDBoundaryFcn FCN( hist , m_cutFraction );
      
      FCN.findBoundary() ;
      int nPix = int(FCN.nPixels() );
      
      ROOT::Minuit2::MnUserParameters par;
      
      par.Add("Col0", 16. , 0.5 );
      par.Add("Row0", 16. , 0.5 );
      par.Add("Radius", 16. , 0.5 );
      
      ROOT::Minuit2::MnMigrad migrad( FCN, par );
      ROOT::Minuit2::FunctionMinimum min = migrad() ;
      
      double Col = migrad.Value("Col0");
      double ColErrSq = pow(migrad.Error("Col0"),2);
      double Row = migrad.Value("Row0");
      double RowErrSq = pow(migrad.Error("Row0"),2);
      double Rad = migrad.Value("Radius");
      double RadErrSq = pow(migrad.Error("Radius"),2);
      
      std::string nameHPD = "RICH_HPD_" + boost::lexical_cast<std::string>( ID );
      
      // Stores the fit result in a StatEntity
      counter( nameHPD + "_AvCol" ) = StatEntity( nPix, nPix*Col, nPix*ColErrSq, 0. , 0. );
      counter( nameHPD + "_AvRow" ) = StatEntity( nPix, nPix*Row, nPix*RowErrSq, 0. , 0. );
      counter( nameHPD + "_AvRad" ) = StatEntity( nPix, nPix*Rad, nPix*RadErrSq, 0. , 0. );
      
      return;
    }
    
    
    StatusCode RichHPDImageSummary::finalize() {
      
      if ( msgLevel(MSG::DEBUG) ){ 
        debug() << "==> Finalize" << endmsg;
        debug() << "    Algorithm has seen " << m_nEvt << " events" << endmsg;
      }
      
      if ( msgLevel(MSG::DEBUG) ){ 
        debug() << " Creating summary information " << endmsg;
      }
      
      for ( m_iter = m_histo.begin() ; m_iter != m_histo.end() ; ++m_iter ){
        summaryINFO( m_iter->first, m_iter->second );
      }
      
      if ( !m_bookHistos ){  
        for ( m_iter = m_histo.begin() ; m_iter != m_histo.end() ; ++m_iter ){
          delete m_iter->second;
        }
      }
      
      m_histo.clear() ;
      
      if ( msgLevel(MSG::DEBUG) ) debug() << " Freed map of HPD objects " << endmsg;
      
      return RichRecHistoAlgBase::finalize();  // must be called after all other actions
    }
  } 
} 

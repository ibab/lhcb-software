// $Id: RichHPDImageMovement.cpp,v 1.2 2009/11/25 18:35:55 tblake Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "RichHPDImageSummary.h"
#include "Event/ODIN.h"
#include "GaudiUtils/Aida2ROOT.h"

#include "DetDesc/IGeometryInfo.h"

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
      : HistoAlgBase( name , pSvcLocator ) ,
        m_nEvt( 0 ),
        m_pixelsize( 0.5 ),
        m_siliconx( 16.0 ),
        m_silicony( 16.0 )
    {
      setProperty( "StatPrint", false );
      declareProperty( "DisplaySmartIDWarnings" , m_displayWarnings = false );
      declareProperty( "BoundaryCutThreshold" , m_cutFraction = 0.1 );
      declareProperty( "MinHPDOccupancy", m_minOccupancy = 1000 );
      declareProperty( "MinBoundaryPixels" , m_minBoundary = 3 );
      declareProperty( "CompareToCondDB" , m_compareCondDB = true );
      declareProperty( "MaxAllowedMovement" , m_maxMovement = 0.3 );
      declareProperty( "Keep2DHistograms", m_keep2Dhistos = false );
    }

    //=============================================================================
    // Destructor
    //=============================================================================

    RichHPDImageSummary::~RichHPDImageSummary() {} ;

    //=============================================================================
    // Initialization
    //=============================================================================

    StatusCode RichHPDImageSummary::initialize()
    {
      const StatusCode sc = HistoAlgBase::initialize();
      if ( sc.isFailure() ) return sc;

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

      return sc;
    }

    //=============================================================================
    // Main execution
    //=============================================================================

    StatusCode RichHPDImageSummary::execute()
    {
      ++m_nEvt;

      LHCb::ODIN* odin = get<LHCb::ODIN*>( LHCb::ODINLocation::Default );
      if ( odin ) {
        counter("RICH_EventTime") += odin->gpsTime() ;
      } else {
        if ( m_displayWarnings ) Warning(" Unable to retrieve ODIN ").ignore();
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
                Warning(" Invalid Rich Smart ID ").ignore();
              }
              continue;
            }

            const LHCb::RichSmartID::Vector& hitIDs = (iHPD->second).smartIDs() ;

            const Rich::DAQ::HPDCopyNumber hpdID = m_RichSys->copyNumber( smartID );
            TH2D* hist = m_histo[ hpdID.data() ];

            if ( NULL == hist ) {
              if ( m_displayWarnings ) {
                Warning(" Can not retrieve boundary FCN, invalid hardware ID ").ignore();
              }
              continue;
            }

            for ( LHCb::RichSmartID::Vector::const_iterator iHit = hitIDs.begin();
                  iHit != hitIDs.end(); ++iHit )
            {
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

    StatusCode RichHPDImageSummary::finalize() {

      if ( msgLevel(MSG::DEBUG) ){
        debug() << "==> Finalize" << endmsg;
        debug() << "    Algorithm has seen " << m_nEvt << " events" << endmsg;
      }

      if ( msgLevel(MSG::DEBUG) ) debug() << " Creating summary information " << endmsg;

      for ( m_iter = m_histo.begin() ; m_iter != m_histo.end() ; ++m_iter ){
        summaryINFO( m_iter->first, m_iter->second );
      }

      if ( !m_keep2Dhistos )
      {
        for ( m_iter = m_histo.begin() ; m_iter != m_histo.end() ; ++m_iter )
        {
          delete m_iter->second;
        }
      }

      m_histo.clear() ;

      if ( msgLevel(MSG::DEBUG) ) debug() << " Freed map of HPD objects " << endmsg;

      return HistoAlgBase::finalize();  // must be called after all other actions
    }

    //=============================================================================

    TH2D* RichHPDImageSummary::create2D( const std::string& name )
    {
      using namespace Gaudi::Utils;
      TH2D * hist(NULL);
      if ( m_keep2Dhistos ) 
      {
        hist = Aida2ROOT::aida2root(book2D(name,name,-0.5,31.5,32, -0.5,31.5,32));
      }
      else
      {
        hist = new TH2D(name.c_str(),name.c_str(),32,-0.5,31.5,32,-0.5,31.5 ) ;
      }
      return hist;
    }

    //=============================================================================

    double RichHPDImageSummary::localXFromPixels( const double col ) const
    {
      return col*m_pixelsize + 0.5*m_pixelsize - 0.5*m_siliconx ;
    }

    double RichHPDImageSummary::localYFromPixels( const double row ) const
    {
      return  0.5*m_silicony - row*m_pixelsize - 0.5*m_pixelsize ;
    }

    //=============================================================================

    double RichHPDImageSummary::distanceToCondDBValue( const unsigned int ID,
                                                       const double x0,
                                                       const double y0 ) const
    {
      const LHCb::RichSmartID smartID = m_RichSys->richSmartID( Rich::DAQ::HPDCopyNumber( ID ) );
      std::string sensorpath = m_RichSys->getDeHPDLocation(smartID);
      sensorpath += ( "/SiSensor:" + boost::lexical_cast<std::string>( ID ) );

      DetectorElement* dd = getDet<DetectorElement>( sensorpath );

      Gaudi::XYZPoint zero;
      Gaudi::XYZPoint offsetCondDB = (dd->geometry()->ownMatrix())*zero;

      double condDBx = -offsetCondDB.x();
      double condDBy = -offsetCondDB.y();

      return std::sqrt( (x0-condDBx)*(x0-condDBx) +
                        (y0-condDBy)*(y0-condDBy) ) ;
    }

    //=============================================================================

    void RichHPDImageSummary::summaryINFO( const unsigned int ID, const TH2D* hist ) const
    {
      int nPix = (int) hist->Integral();
      if ( nPix < m_minOccupancy ) return ;

      HPDBoundaryFcn FCN( hist , m_cutFraction );

      int boundarySize = FCN.findBoundary() ;
      if ( boundarySize < m_minBoundary ) return ;

      ROOT::Minuit2::MnUserParameters par;

      par.Add("Col0", 16. , 0.5 );
      par.Add("Row0", 16. , 0.5 );
      par.Add("Radius", 16. , 0.5 );

      ROOT::Minuit2::MnMigrad migrad( FCN, par );
      ROOT::Minuit2::FunctionMinimum min = migrad() ;

      double Col = migrad.Value("Col0");
      double Row = migrad.Value("Row0");

      double x0 = -1.0*localXFromPixels( Col );
      double y0 = -1.0*localYFromPixels( Row );

      double ds = distanceToCondDBValue( ID, x0, y0 );

      plot1D( ds, "dPosCondDB", "Distance between image centre and CondDB value",0.0,3.0,30);
      plot1D( ID, "dPosCondDBvsCopyNr", "Distance versus HPD Copy Nr",-0.5,483.5,484,ds);

      plot1D( 2*ID+1 , "dPosXvsCopyNr", "x-displacement versus HPD Copy Nr",-0.5,967.5,968,nPix*x0);
      plot1D( 2*ID+1 , "dPosYvsCopyNr", "y-displacement versus HPD Copy Nr",-0.5,967.5,968,nPix*y0);
      plot1D( 2*ID   , "dPosXvsCopyNr", "x entries for HPD Copy Nr",-0.5,967.5,968,nPix);
      plot1D( 2*ID   , "dPosYvsCopyNr", "y entries for HPD Copy Nr",-0.5,967.5,968,nPix);

      double Rad = m_pixelsize*migrad.Value("Radius");
      double RadErrSq = pow(m_pixelsize*migrad.Error("Radius"),2);

      plot1D( 2*ID+1 , "RadiusvsCopyNr", "Fitted image radius vs HPD Copy Nr",-0.5,967.5,968,nPix*Rad);
      plot1D( 2*ID   , "RadiusvsCopyNr", "Fitted image radius HPD Copy Nr",-0.5,967.5,968,nPix);

      if ( m_compareCondDB && ( ds < m_maxMovement ) ){
        if ( msgLevel(MSG::DEBUG) ) debug() << " Exisiting CondDB value ok for " << ID <<  endmsg;
      }
      else {
        std::string nameHPD = "RICH_HPD_" + boost::lexical_cast<std::string>( ID );

        if ( msgLevel(MSG::DEBUG) ) debug() << " Adding counter " << nameHPD << endmsg ;

        double x0ErrSq = pow(m_pixelsize*migrad.Error("Col0"),2);
        double y0ErrSq = pow(m_pixelsize*migrad.Error("Row0"),2);

        counter( nameHPD + "_XOffset" ) = StatEntity( nPix, nPix*x0, nPix*x0ErrSq, 0. , 0. );
        counter( nameHPD + "_YOffset" ) = StatEntity( nPix, nPix*y0, nPix*y0ErrSq, 0. , 0. );
        counter( nameHPD + "_Radius" )  = StatEntity( nPix, nPix*Rad, nPix*RadErrSq, 0. , 0. );
      }
      return;
    }

    //=============================================================================
  } // Namespace Mon
} // Namespace Rich

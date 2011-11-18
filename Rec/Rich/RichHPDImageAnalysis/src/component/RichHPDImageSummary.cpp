
// Local
#include "RichHPDImageSummary.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichHPDImageSummary
//
// 2010-03-16 : Thomas BLAKE
//-----------------------------------------------------------------------------

using namespace Rich::HPDImage;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Summary )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

  Summary::Summary( const std::string& name,
                    ISvcLocator* pSvcLocator )
    : HistoAlgBase ( name , pSvcLocator ) ,
      m_nEvt       ( 0    )
{
  setProperty( "StatPrint", false );
  declareProperty( "MinHPDOccupancy", m_minOccupancy = 1000 );
  declareProperty( "HPDFitType" ,     m_params.type );
  declareProperty( "CompareToCondDB" , m_compareCondDB = true );
  declareProperty( "MaxAllowedMovement", m_maxMovement = 0.3 );
  declareProperty( "Keep2DHistograms", m_keep2Dhistos = false );
  declareProperty( "FinalHPDFit",      m_finalFit = false );
}

//=============================================================================
// Destructor
//=============================================================================

Summary::~Summary() { }

//=============================================================================
// Initialization
//=============================================================================

StatusCode Summary::initialize()
{
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );

  const LHCb::RichSmartID::Vector & allHPDs = m_RichSys->allPDRichSmartIDs();

  for ( LHCb::RichSmartID::Vector::const_iterator iHPD = allHPDs.begin();
        iHPD != allHPDs.end(); ++iHPD )
  {
    const Rich::DAQ::HPDCopyNumber hpdID = m_RichSys->copyNumber( *iHPD );

    std::ostringstream name;
    name << "Rich_HPD_" << hpdID.data() << "_Image";

    if ( m_histo.find(*iHPD) == m_histo.end() )
    {
      m_histo[*iHPD] = create2D( name.str() );
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Created image histogram for " << *iHPD << endmsg;
    }
    else
    {
      Warning( "Histogram for " + name.str() + " already exists !!" ).ignore();
    }
  }

  debug() << m_params << endmsg;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode Summary::execute()
{
  ++m_nEvt;

  LHCb::ODIN* odin = get<LHCb::ODIN*>( LHCb::ODINLocation::Default );
  if ( odin )
  {
    counter("RICH_EventTime") += odin->gpsTime() ;
  }
  else
  {
    Warning( "Unable to retrieve ODIN" ).ignore();
  }

  // Standard loop over Rich Smart IDs
  const Rich::DAQ::L1Map& mapUKL1 = m_SmartIDDecoder->allRichSmartIDs();

  for ( Rich::DAQ::L1Map::const_iterator iUKL1 = mapUKL1.begin();
        iUKL1 != mapUKL1.end(); ++iUKL1 )
  {
    const Rich::DAQ::IngressMap& mapIngress = iUKL1->second;

    for ( Rich::DAQ::IngressMap::const_iterator iIngress = mapIngress.begin();
          iIngress != mapIngress.end(); ++iIngress )
    {

      const Rich::DAQ::HPDMap & mapHPD = (iIngress->second).hpdData();

      for ( Rich::DAQ::HPDMap::const_iterator iHPD = mapHPD.begin();
            iHPD != mapHPD.end(); ++iHPD )
      {
        const LHCb::RichSmartID smartID = (iHPD->second).hpdID();

        // Skip inhibited HPDs
        if ( (iHPD->second).header().inhibit() ) { continue; }
        // skip bad HPD IDs
        if ( !smartID.isValid() ) { continue; }

        // Find an fill histogram image plot
        PD2Histo::iterator iHist = m_histo.find(smartID);
        if ( iHist == m_histo.end() )
        {
          std::ostringstream mess;
          mess << "No HPD Image histogram for " << smartID;
          Warning( mess.str() ).ignore();
          continue;
        }
        else
        {
          const LHCb::RichSmartID::Vector & hitIDs = (iHPD->second).smartIDs() ;
          for ( LHCb::RichSmartID::Vector::const_iterator iHit = hitIDs.begin();
                iHit != hitIDs.end(); ++iHit )
          {
            iHist->second->Fill( iHit->pixelCol(), iHit->pixelRow() );
          }
        }

      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Finalize
//=============================================================================

StatusCode Summary::finalize()
{

  debug() << "==> Finalize" << endmsg;
  debug() << "    Algorithm has seen " << m_nEvt << " events" << endmsg;

  // Make summary info
  if ( m_finalFit )
  {
    verbose() << "Fitting histograms and making summaries" << endmsg;
    for ( PD2Histo::iterator it = m_histo.begin(); it != m_histo.end(); ++it )
    {
      summaryINFO( it->first, it->second );
    }
  }

  // Clean out histogram storage
  if ( !m_keep2Dhistos )
  {
    verbose() << "Cleaning out histograms" << endmsg;
    for ( PD2Histo::iterator it = m_histo.begin(); it != m_histo.end(); ++it )
    {
      delete it->second;
    }
  }
  m_histo.clear() ;

  return HistoAlgBase::finalize();  // must be called after all other actions
}

//=============================================================================

TH2D* Summary::create2D( const std::string& name )
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

double Summary::distanceToCondDBValue( const Rich::DAQ::HPDCopyNumber copyNumber,
                                       const double x0,
                                       const double y0 ) const
{
  const LHCb::RichSmartID smartID = m_RichSys->richSmartID( copyNumber );

  std::ostringstream sensorpath;
  sensorpath << m_RichSys->getDePDLocation(smartID)
             << "/SiSensor:" << copyNumber.data();

  DetectorElement * dd = getDet<DetectorElement>( sensorpath.str() );

  Gaudi::XYZPoint zero;
  Gaudi::XYZPoint offsetCondDB = (dd->geometry()->ownMatrix())*zero;

  const double condDBx = -offsetCondDB.x();
  const double condDBy = -offsetCondDB.y();

  return std::sqrt( std::pow(x0-condDBx,2) +
                    std::pow(y0-condDBy,2) ) ;
}

//=============================================================================

void Summary::summaryINFO( const LHCb::RichSmartID id,
                           const TH2D* hist ) const
{
  if ( !hist ) return;

  const Rich::DAQ::HPDCopyNumber copyNumber = m_RichSys->copyNumber( id );

  const unsigned int nPix = (unsigned int) (hist->Integral());
  if ( nPix < m_minOccupancy )
  {
    debug() << "Fit for HPD " << copyNumber
            << " ABORTED -> Too few hits (" << nPix << ")" << endmsg;
    return;
  }

  // Do the fit
  const HPDFit::Result result = m_fitter.fit( *hist, m_params );

  // if fit failed, don't fill.
  if ( !result.OK() )
  {
    std::ostringstream mess;
    mess << "Fit for HPD " << copyNumber << " FAILED | #hits = " << nPix;
    Warning( mess.str(), StatusCode::SUCCESS, 0 ).ignore();
    return;
  }

  const unsigned int nHPDs = 484;

  const double x0     = result.x();
  const double y0     = result.y();
  const double xErr0  = result.xErr();
  const double yErr0  = result.yErr();
  const double Rad    = result.radInMM();
  const double RadErr = result.radErrInMM();

  const double OneOverXErrSq = ( xErr0>0.0  ? 1.0/(xErr0*xErr0)   : 0.0 );
  const double OneOverYErrSq = ( yErr0>0.0  ? 1.0/(yErr0*yErr0)   : 0.0 );
  const double OneOverRErrSq = ( RadErr>0.0 ? 1.0/(RadErr*RadErr) : 0.0 );

  const double ds = distanceToCondDBValue( copyNumber, x0, y0 );

  plot1D( ds, "dPosCondDB", "Distance between image centre and CondDB value",0.0,3.0,30);
  plot1D( copyNumber.data(), "dPosCondDBvsCopyNr", "Distance versus HPD",-0.5,nHPDs-0.5,nHPDs,ds);

  // Update these to allow the weighted mean of the fit results to be correctly computed
  // when ROOT first are merged.  Need to compute
  //                weighted mean     = Sum( x_i / error_i^2 ) / Sum( 1 / error_i^2 )
  //                (error of mean)^2 = 1 /  Sum( 1 / error_i^2 )
  plot1D( copyNumber.data(), "dPosXvsCopyNr",    "x-displacement versus HPD",      -0.5,nHPDs-0.5,nHPDs,x0*OneOverXErrSq);
  plot1D( copyNumber.data(), "dPosXvsCopyNrErr", "x-displacement error versus HPD",-0.5,nHPDs-0.5,nHPDs,OneOverXErrSq);
  plot1D( copyNumber.data(), "dPosYvsCopyNr",    "y-displacement versus HPD",      -0.5,nHPDs-0.5,nHPDs,y0*OneOverYErrSq);
  plot1D( copyNumber.data(), "dPosYvsCopyNrErr", "y-displacement error versus HPD",-0.5,nHPDs-0.5,nHPDs,OneOverYErrSq);

  plot1D( copyNumber.data(), "RadiusvsCopyNr", "Fitted image radius vs HPD",-0.5,nHPDs-0.5,nHPDs,Rad*OneOverRErrSq);
  plot1D( copyNumber.data(), "RadiusErrvsCopyNr", "Fitted image radius error vs HPD",-0.5,nHPDs-0.5,nHPDs,OneOverRErrSq);

  plot1D( copyNumber.data(), "entriesvsCopyNr", "# entries for HPD Copy Nr",-0.5,nHPDs-0.5,nHPDs,nPix);

  if ( m_compareCondDB && ( ds < m_maxMovement ) )
  {
    if ( msgLevel(MSG::DEBUG) ) debug() << " Exisiting CondDB value ok for " << copyNumber <<  endmsg;
  }
  else
  {
    std::ostringstream nameHPD;
    nameHPD << "RICH_HPD_" << copyNumber;

    if ( msgLevel(MSG::DEBUG) ) debug() << "Adding counter " << nameHPD.str() << endmsg ;

    const double x0ErrSq  = std::pow(xErr0,2);
    const double y0ErrSq  = std::pow(yErr0,2);
    const double RadErrSq = std::pow(RadErr,2);

    counter( nameHPD.str() + "_XOffset" ) = StatEntity( nPix, nPix*x0,  nPix*x0ErrSq,  0. , 0. );
    counter( nameHPD.str() + "_YOffset" ) = StatEntity( nPix, nPix*y0,  nPix*y0ErrSq,  0. , 0. );
    counter( nameHPD.str() + "_Radius" )  = StatEntity( nPix, nPix*Rad, nPix*RadErrSq, 0. , 0. );
  }
}

//=============================================================================

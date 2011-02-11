
// Local
#include "RichHPDImageSummary.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichHPDImageSummary
//
// 2010-03-16 : Thomas BLAKE
//-----------------------------------------------------------------------------

using namespace Rich::Mon;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RichHPDImageSummary )

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

RichHPDImageSummary::~RichHPDImageSummary() {}

//=============================================================================
// Initialization
//=============================================================================

StatusCode RichHPDImageSummary::initialize()
{
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_RichSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  acquireTool( "RichSmartIDDecoder", m_SmartIDDecoder, 0, true );

  const LHCb::RichSmartID::Vector &activeHPDs = m_RichSys->activeHPDRichSmartIDs();

  for (  LHCb::RichSmartID::Vector::const_iterator iHPD = activeHPDs.begin();
         iHPD != activeHPDs.end(); ++iHPD )
  {
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

  for ( Rich::DAQ::L1Map::const_iterator iUKL1 = mapUKL1.begin();
        iUKL1 != mapUKL1.end(); ++iUKL1 )
  {
    const Rich::DAQ::IngressMap& mapIngress = iUKL1->second;

    for ( Rich::DAQ::IngressMap::const_iterator iIngress = mapIngress.begin();
          iIngress != mapIngress.end(); ++iIngress )
    {

      const Rich::DAQ::HPDMap& mapHPD = (iIngress->second).hpdData() ;

      for ( Rich::DAQ::HPDMap::const_iterator iHPD = mapHPD.begin();
            iHPD != mapHPD.end(); ++iHPD )
      {
        const LHCb::RichSmartID &smartID = (iHPD->second).hpdID();

        // Skip inhibited HPDs
        if ( (iHPD->second).header().inhibit() ) continue;

        if ( !smartID.isValid() ) 
        {
          if ( m_displayWarnings ) 
          {
            Warning(" Invalid Rich Smart ID ").ignore();
          }
          continue;
        }

        const LHCb::RichSmartID::Vector& hitIDs = (iHPD->second).smartIDs() ;

        const Rich::DAQ::HPDCopyNumber hpdID = m_RichSys->copyNumber( smartID );
        TH2D* hist = m_histo[ hpdID.data() ];

        if ( NULL == hist ) 
        {
          if ( m_displayWarnings ) 
          {
            Warning(" Can not retrieve boundary FCN, invalid hardware ID ").ignore();
          }
          continue;
        }

        for ( LHCb::RichSmartID::Vector::const_iterator iHit = hitIDs.begin();
              iHit != hitIDs.end(); ++iHit )
        {
          hist->Fill( iHit->pixelCol(), iHit->pixelRow() ) ;
        }

      }
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================

StatusCode RichHPDImageSummary::finalize() 
{

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "==> Finalize" << endmsg;
    debug() << "    Algorithm has seen " << m_nEvt << " events" << endmsg;
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << " Creating summary information " << endmsg;

  for ( m_iter = m_histo.begin() ; m_iter != m_histo.end() ; ++m_iter )
  {
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

double RichHPDImageSummary::distanceToCondDBValue( const unsigned int ID,
                                                   const double x0,
                                                   const double y0 ) const
{
  const LHCb::RichSmartID smartID = m_RichSys->richSmartID( Rich::DAQ::HPDCopyNumber( ID ) );
  std::string sensorpath = m_RichSys->getDeHPDLocation(smartID);
  sensorpath += ( "/SiSensor:" + boost::lexical_cast<std::string>( ID ) );

  DetectorElement * dd = getDet<DetectorElement>( sensorpath );

  Gaudi::XYZPoint zero;
  Gaudi::XYZPoint offsetCondDB = (dd->geometry()->ownMatrix())*zero;

  const double condDBx = -offsetCondDB.x();
  const double condDBy = -offsetCondDB.y();

  return std::sqrt( (x0-condDBx)*(x0-condDBx) +
                    (y0-condDBy)*(y0-condDBy) ) ;
}

//=============================================================================

void RichHPDImageSummary::summaryINFO( const unsigned int ID,
                                       const TH2D* hist ) const
{
  if ( !hist ) return;

  const unsigned int nPix = (unsigned int) (hist->Integral());
  if ( nPix < m_minOccupancy ) return ;
  
  HPDBoundaryFcn FCN( hist , m_cutFraction );

  const int boundarySize = FCN.findBoundary() ;
  if ( boundarySize < m_minBoundary ) return ;

  ROOT::Minuit2::MnUserParameters par;
  par.Add("Col0",   16. , 0.5 );
  par.Add("Row0",   16. , 0.5 );
  par.Add("Radius", 16. , 0.5 );

  // Make the minimiser
  ROOT::Minuit2::MnMigrad migrad( FCN, par );
  // minimise ...
  const ROOT::Minuit2::FunctionMinimum min = migrad();

  // if fit failed, don't fill.
  if ( !min.IsValid() ) 
  { 
    std::ostringstream mess;
    mess << "Fit for HPD " << ID << " FAILED";
    Warning( mess.str() ).ignore();
    return;
  }

  const unsigned int nHPDs = 484;

  const double Col    = min.UserParameters().Value("Col0");
  const double Row    = min.UserParameters().Value("Row0");
  const double ColErr = min.UserParameters().Error("Col0");
  const double RowErr = min.UserParameters().Error("Row0");

  const double x0    = localXFromPixels( Col );
  const double y0    = localYFromPixels( Row );
  const double xErr0 = localErrorFromPixels( ColErr );
  const double yErr0 = localErrorFromPixels( RowErr ); 

  const double Rad    = m_pixelsize * min.UserParameters().Value("Radius");
  const double RadErr = m_pixelsize * min.UserParameters().Error("Radius");

  const double OneOverXErrSq = ( xErr0>0.0  ? 1.0/(xErr0*xErr0) : 0.0 );
  const double OneOverYErrSq = ( yErr0>0.0  ? 1.0/(yErr0*yErr0) : 0.0 );
  const double OneOverRErrSq = ( RadErr>0.0 ? 1.0/(RadErr*RadErr) : 0.0 );

  const double ds = distanceToCondDBValue( ID, x0, y0 );

  plot1D( ds, "dPosCondDB", "Distance between image centre and CondDB value",0.0,3.0,30);
  plot1D( ID, "dPosCondDBvsCopyNr", "Distance versus HPD",-0.5,nHPDs-0.5,nHPDs,ds);

  // Update these to allow the weighted mean of the fit results to be correctly computed
  // when ROOT first are merged.  Need to compute 
  //                weighted mean     = Sum( x_i / error_i^2 ) / Sum( 1 / error_i^2 )
  //                (error of mean)^2 = 1 /  Sum( 1 / error_i^2 ) 
  plot1D( ID, "dPosXvsCopyNr",    "x-displacement versus HPD",      -0.5,nHPDs-0.5,nHPDs,x0*OneOverXErrSq);
  plot1D( ID, "dPosXvsCopyNrErr", "x-displacement error versus HPD",-0.5,nHPDs-0.5,nHPDs,OneOverXErrSq);
  plot1D( ID, "dPosYvsCopyNr",    "y-displacement versus HPD",      -0.5,nHPDs-0.5,nHPDs,y0*OneOverYErrSq);
  plot1D( ID, "dPosYvsCopyNrErr", "y-displacement error versus HPD",-0.5,nHPDs-0.5,nHPDs,OneOverYErrSq);

  plot1D( ID, "RadiusvsCopyNr", "Fitted image radius vs HPD",-0.5,nHPDs-0.5,nHPDs,Rad*OneOverRErrSq);
  plot1D( ID, "RadiusErrvsCopyNr", "Fitted image radius error vs HPD",-0.5,nHPDs-0.5,nHPDs,OneOverRErrSq);

  plot1D( ID, "entriesvsCopyNr", "# entries for HPD Copy Nr",-0.5,nHPDs-0.5,nHPDs,nPix);

  if ( m_compareCondDB && ( ds < m_maxMovement ) )
  {
    if ( msgLevel(MSG::DEBUG) ) debug() << " Exisiting CondDB value ok for " << ID <<  endmsg;
  }
  else
  {
    std::string nameHPD = "RICH_HPD_" + boost::lexical_cast<std::string>( ID );

    if ( msgLevel(MSG::DEBUG) ) debug() << "Adding counter " << nameHPD << endmsg ;

    const double x0ErrSq  = std::pow(xErr0,2);
    const double y0ErrSq  = std::pow(yErr0,2);
    const double RadErrSq = std::pow(RadErr,2);

    counter( nameHPD + "_XOffset" ) = StatEntity( nPix, nPix*x0,  nPix*x0ErrSq,  0. , 0. );
    counter( nameHPD + "_YOffset" ) = StatEntity( nPix, nPix*y0,  nPix*y0ErrSq,  0. , 0. );
    counter( nameHPD + "_Radius" )  = StatEntity( nPix, nPix*Rad, nPix*RadErrSq, 0. , 0. );
  }
  return;
}

//=============================================================================

RichHPDImageSummary::HPDBoundaryFcn::HPDBoundaryFcn( const TH2* hist , 
                                                     const double thr ) 
  : m_errDef(1.),
    m_threshold( thr ),
    m_hist( hist ),
    m_sf ( hist ? (1.0*hist->GetNbinsX())/(1.0*hist->GetNbinsY()) : 0.0 )
{ }

RichHPDImageSummary::HPDBoundaryFcn::~HPDBoundaryFcn() {}

unsigned int RichHPDImageSummary::HPDBoundaryFcn::findBoundary() const
{
  if ( NULL == m_hist ) return 0 ;
  m_boundary.clear() ;

  const int nbins  = m_hist->GetNbinsX()*m_hist->GetNbinsY();
  const double thr = m_threshold*m_hist->Integral()/(1.0*nbins);

  for ( int icol = 0 ; icol < m_hist->GetNbinsX() ; ++icol )
  {
    int ROW0 = -1;
    int ROW1 = -1;
 
    for ( int irow = 0; irow <m_hist->GetNbinsY() ; ++irow )
    {
      if ( hasNeighbour( icol, irow, thr ) &&
           m_hist->GetBinContent( icol+1, irow+1 ) > thr ) 
      {
        ROW0 = irow ;
        break;
      }
    }
    for ( int irow = 0; irow < m_hist->GetNbinsY() ; ++irow )
    {
      if ( hasNeighbour( icol, irow, thr ) &&
           m_hist->GetBinContent( icol+1, m_hist->GetNbinsX()-irow ) > thr )
      {
        ROW1 = m_hist->GetNbinsX() - irow - 1;
        break;
      }
    }
    if ( -1 != ROW0 )
    {
      m_boundary.push_back( std::make_pair( icol, ROW0 ) );
    }
    if ( -1 != ROW1 && ROW1 != ROW0 ) 
    {
      m_boundary.push_back( std::make_pair( icol, ROW1 ) );
    }
  }

  return m_boundary.size() ;
}

bool
RichHPDImageSummary::HPDBoundaryFcn::hasNeighbour( const int COL,
                                                   const int ROW,
                                                   const double thr ) const 
{
  for ( int icol = COL-1; icol <= COL+1 ; ++icol )
  {
    for ( int irow = ROW-1; irow <= ROW+1 ; ++irow )
    {
      if ( COL == icol && ROW == irow ) { continue ; }
      else if ( icol >= 0 && icol < m_hist->GetNbinsX() &&
                irow >= 0 && irow < m_hist->GetNbinsY() )
      {
        if ( m_hist->GetBinContent( icol+1, irow+1 ) > thr ) return true ;
      }
    }
  }
  return false;
}

double
RichHPDImageSummary::HPDBoundaryFcn::operator()( const std::vector<double>& par ) const
{
  double chi2 = 0.0;
  for ( std::vector< std::pair<int,int> >::const_iterator iter = m_boundary.begin(); 
        iter != m_boundary.end(); ++iter )
  {
    const double deltaCol = (1.0*iter->first) - par[0];
    const double deltaRow = (m_sf*iter->second) - par[1];
    const double dist = std::sqrt( deltaCol*deltaCol + deltaRow*deltaRow );
    chi2 += ( dist-par[2] )*( dist-par[2] )*12.0;
  }
  return chi2 ;
}

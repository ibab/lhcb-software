
//-----------------------------------------------------------------------------
/** @file RichMirrorSegFinderLookUpTable.cpp
 *
 *  Implementation file for class : Rich::MirrorSegFinderLookUpTable
 *
 *  @author Chris Jones
 *  @date   2015-02-01
 */
//-----------------------------------------------------------------------------

// local
#include "RichMirrorSegFinderLookUpTable.h"

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::MirrorSegFinderLookUpTable::MirrorSegFinderLookUpTable( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
  : Rich::ToolBase ( type, name, parent ),
    m_firstUpdate  ( true ),
    m_maxDistDiff  ( Rich::NRiches, 0   ),
    m_failRate     ( Rich::NRiches, std::make_pair(0,0) )

{
  using namespace Gaudi::Units;

  // define interface
  declareInterface<IMirrorSegFinder>(this);

  // job options
  //declareProperty( "TestFinding", m_testFinding = false );
  //                                              Rich1    Rich2
  declareProperty( "TableSize",   m_tableSize = { 200,      400    } );
  declareProperty( "ExtraXY",     m_extraSize = { 100*mm,  2500*mm } );

  //setProperty( "OutputLevel", 2 );
}

//=============================================================================
// Destructor
//=============================================================================
Rich::MirrorSegFinderLookUpTable::~MirrorSegFinderLookUpTable() { }

//=========================================================================
//  initialization
//=========================================================================
StatusCode Rich::MirrorSegFinderLookUpTable::initialize( )
{
  // intialise base class
  StatusCode sc = Rich::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  //if ( m_testFinding )
  //{ Warning( "Will run slow checks of mirror search", StatusCode::SUCCESS ).ignore(); }

  // Force an update of the cached mirror information
  // Also sets up the UMS service for future updates as needed
  sc = mirrorUpdate();

  // return
  return sc;
}

//=========================================================================
//  finalize
//=========================================================================
StatusCode Rich::MirrorSegFinderLookUpTable::finalize( )
{

  // if ( UNLIKELY(m_testFinding) )
  // {
  //   info() << "R1/R2 TableSize = " << m_tableSize << endmsg;
  //   info() << "R1/R2 ExtraSize = " << m_extraSize << endmsg;
  //   info() << "Max. R1/R2 mirror delta = " << m_maxDistDiff << endmsg;
  //   const double R1FailRate = ( m_failRate[0].second > 0 ?
  //                               (100.0*m_failRate[0].first)/m_failRate[0].second : 0.0 );
  //   const double R2FailRate = ( m_failRate[1].second > 0 ?
  //                               (100.0*m_failRate[1].first)/m_failRate[1].second : 0.0 );
  //   info() << "Fail rate  R1=" << R1FailRate << "% R2=" << R2FailRate << "%" << endmsg;
  // }

  // unregister from UMS
  updMgrSvc()->unregister(this);

  return Rich::ToolBase::finalize();
}

//=========================================================================
// Update Mirror information on Condition changes
//=========================================================================
StatusCode Rich::MirrorSegFinderLookUpTable::mirrorUpdate()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Mirror Update Triggered" << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  // get all the mirror segments
  sc = sc && loadMirrors( Rich::Rich1 );
  sc = sc && loadMirrors( Rich::Rich2 );

  // Flag the first update as having been done
  m_firstUpdate = false;

  // debug printout
  if ( msgLevel(MSG::DEBUG) ) { printMirrors(); }

  return sc;
}

//=========================================================================
// Load the mirrors from the DB
//=========================================================================
StatusCode Rich::MirrorSegFinderLookUpTable::loadMirrors( const Rich::DetectorType rich )
{
  // Load the RICH detector element
  const DeRich * deRich = getDet<DeRich>( DeRichLocations::location(rich) );

  // reset the mirror finder objects
  m_sphMirrors[rich][0].reset();
  m_sphMirrors[rich][1].reset();
  m_secMirrors[rich][0].reset();
  m_secMirrors[rich][1].reset();

  // load the primary mirrors
  for ( const auto& loc : deRich->paramVect<std::string>("SphericalMirrorDetElemLocations") )
  {
    DeRichSphMirror * m = getDet<DeRichSphMirror>(loc);
    m_sphMirrors[rich][side(m,rich)].mirrors.push_back( m );
    if ( m_firstUpdate )
      updMgrSvc()->registerCondition( this, m, &Rich::MirrorSegFinderLookUpTable::mirrorUpdate );
  }
  if ( m_sphMirrors[rich][0].mirrors.empty() &&
       m_sphMirrors[rich][1].mirrors.empty() )
  {
    return Error( "No primary mirrors in " + Rich::text(rich) );
  }

  // secondary mirrors
  for ( const auto& loc : deRich->paramVect<std::string>("SecondaryMirrorDetElemLocations") )
  {
    DeRichSphMirror * m = getDet<DeRichSphMirror>(loc);
    m_secMirrors[rich][side(m,rich)].mirrors.push_back( m );
    if ( m_firstUpdate )
      updMgrSvc()->registerCondition( this, m, &Rich::MirrorSegFinderLookUpTable::mirrorUpdate );
  }
  if ( m_secMirrors[rich][0].mirrors.empty() &&
       m_secMirrors[rich][1].mirrors.empty() )
  {
    return Error( "No secondary mirrors in " + Rich::text(rich) );
  }

  // Initialise the finder objects
  _ri_debug << "Initialising " << rich << " mirror lookup maps" << endmsg;
  m_sphMirrors[rich][0].init( m_tableSize[rich], m_extraSize[rich] );
  m_sphMirrors[rich][1].init( m_tableSize[rich], m_extraSize[rich] );
  m_secMirrors[rich][0].init( m_tableSize[rich], m_extraSize[rich] );
  m_secMirrors[rich][1].init( m_tableSize[rich], m_extraSize[rich] );

  return StatusCode::SUCCESS;
}

//=========================================================================
// reset the mirror finder
//=========================================================================
void Rich::MirrorSegFinderLookUpTable::MirrorFinder::reset()
{
  mirrors.clear();
  m_lookupTable.clear();
  m_minX   = m_minY   =  9e9;
  m_maxX   = m_maxY   = -9e9;
  m_incX   = m_incY   =  0.0;
  m_nXBins = m_nYBins =    0;
}

//=========================================================================
// Initialise the mirror finder
//=========================================================================
void
Rich::MirrorSegFinderLookUpTable::MirrorFinder::init( const unsigned int nBins,
                                                      const double extraSize )
{
  // number of bins in the lookup table
  m_nXBins = nBins;
  m_nYBins = nBins;

  // Loop over the mirrors to find the (x,y) range
  for ( const auto* M : mirrors )
  {
    if ( M->mirrorCentre().X() > m_maxX ) { m_maxX = M->mirrorCentre().X(); }
    if ( M->mirrorCentre().X() < m_minX ) { m_minX = M->mirrorCentre().X(); }
    if ( M->mirrorCentre().Y() > m_maxY ) { m_maxY = M->mirrorCentre().Y(); }
    if ( M->mirrorCentre().Y() < m_minY ) { m_minY = M->mirrorCentre().Y(); }
  }

  // Add a little to take the extent of the mirrors into account
  m_maxX += extraSize;
  m_maxY += extraSize;
  m_minX -= extraSize;
  m_minY -= extraSize;

  // 1 / X and Y increments ( inverse, for speed in lookup )
  m_incX = (double)m_nXBins / ( m_maxX - m_minX );
  m_incY = (double)m_nYBins / ( m_maxY - m_minY );

  m_lookupTable = LookupTable( m_nXBins, m_nYBins );
  for ( unsigned int iX = 0; iX < m_nXBins; ++iX )
  {
    for ( unsigned int iY = 0; iY < m_nYBins; ++iY )
    {
      // Find the nearest mirror segment to the centre of this bin
      // and save it in the lookup table
      m_lookupTable.set( iX, iY, closestXY( binX(iX), binY(iY) ) );
    }
  }
}

//=========================================================================
// Get the mirror closest to a given (x,y) point
//=========================================================================
const DeRichSphMirror*
Rich::MirrorSegFinderLookUpTable::MirrorFinder::closestXY( const double& x,
                                                           const double& y ) const
{
  const DeRichSphMirror* minM = NULL;

  // Loop over all the mirrors to find the closest
  double minDist(9e99);
  for ( const auto* M : mirrors )
  {
    const double dist = distance2( x, y, M );
    if ( dist < minDist ) { minM = M; minDist = dist; }
  }

  return minM;
}

// //=========================================================================
// // test mirror from lookup table
// //=========================================================================
// void
// Rich::MirrorSegFinderLookUpTable::testFinding( const MirrorFinder& finder,
//                                                const Rich::DetectorType rich,
//                                                const Rich::Side side,
//                                                const Gaudi::XYZPoint& reflPoint ) const
// {
//   // Find the mirrors using lookup table and slow search
//   const DeRichSphMirror * mirror = finder.find(reflPoint);
//   const DeRichSphMirror * testM  = finder.closestXY( reflPoint.x(), reflPoint.y() );

//   ++m_failRate[rich].second;
//   if ( mirror != testM )
//   {
//     ++m_failRate[rich].first;
//     const double dist1 = std::sqrt(MirrorFinder::distance2(reflPoint.x(),reflPoint.y(),mirror));
//     const double dist2 = std::sqrt(MirrorFinder::distance2(reflPoint.x(),reflPoint.y(),testM));
//     if ( msgLevel(MSG::DEBUG) )
//     {
//       debug() << Rich::text(rich,side) << " Lookup table gave different mirror to full search"
//               << endmsg;
//       debug() << " -> Lookup : #" << mirror->mirrorNumber()
//               << " Name=" << mirror->name()
//               << " Centre=" << mirror->mirrorCentre()
//               << " Dist=" << dist1
//               << endmsg;
//       debug() << " -> Full   : #" << testM->mirrorNumber()
//               << " Name=" << testM->name()
//               << " Centre=" << testM->mirrorCentre()
//               << " Dist=" << dist2
//               << endmsg;
//     }
//     const double diffDelta = fabs( dist1 - dist2 );
//     if ( m_maxDistDiff[rich] < diffDelta ) m_maxDistDiff[rich] = diffDelta;
//   }
// }

//=========================================================================
//  find spherical mirror segment and return pointer
//=========================================================================
const DeRichSphMirror*
Rich::MirrorSegFinderLookUpTable::findSphMirror( const Rich::DetectorType rich,
                                                 const Rich::Side side,
                                                 const Gaudi::XYZPoint& reflPoint ) const
{
  //if ( msgLevel(MSG::VERBOSE) )
  //  verbose() << "Searching for Sph. mirror for point " << reflPoint << endmsg;

  // Most likely mirror is the last one found... So test this one first
  const DeRichSphMirror* mirror = m_sphMirrors[rich][side].find(reflPoint);

  //if ( UNLIKELY(m_testFinding) )
  //{ testFinding( m_sphMirrors[rich][side], rich, side, reflPoint ); }

  //if ( msgLevel(MSG::VERBOSE) && mirror )
  //  verbose() << " -> Found mirror " << mirror->mirrorNumber() << endmsg;

  // return found mirror
  return mirror;
}

//=========================================================================
//  find secondary mirror segment and return pointer
//=========================================================================
const DeRichSphMirror*
Rich::MirrorSegFinderLookUpTable::findSecMirror( const Rich::DetectorType rich,
                                                 const Rich::Side side,
                                                 const Gaudi::XYZPoint& reflPoint ) const
{
  //if ( msgLevel(MSG::VERBOSE) )
  //  verbose() << "Searching for Sec. mirror for point " << reflPoint << endmsg;

  // Most likely mirror is the last one found... So test this one first
  const DeRichSphMirror* mirror = m_secMirrors[rich][side].find(reflPoint);

  //if ( UNLIKELY(m_testFinding) )
  //{ testFinding( m_secMirrors[rich][side], rich, side, reflPoint ); }

  //if ( msgLevel(MSG::VERBOSE) && mirror )
  //  verbose() << " -> Found mirror " << mirror->mirrorNumber() << endmsg;

  // return found mirror
  return mirror;
}

//=========================================================================
// Debug printout of the mirrors
//=========================================================================
void Rich::MirrorSegFinderLookUpTable::printMirrors() const
{

  typedef std::vector<std::pair<Rich::DetectorType,Rich::Side> > DetSide;
  const DetSide pairs = { std::make_pair(Rich::Rich1,Rich::top),
                          std::make_pair(Rich::Rich1,Rich::bottom),
                          std::make_pair(Rich::Rich2,Rich::left),
                          std::make_pair(Rich::Rich2,Rich::right) };
  for ( const auto& D : pairs )
  {
    
    debug() << "Found " << m_sphMirrors[D.first][D.second].mirrors.size()
            << " primary mirrors for " << D.first << " "
            << Rich::text(D.first,D.second) << " :";
    for ( const auto& M : m_sphMirrors[D.first][D.second].mirrors )
    { debug() << " " << M->mirrorNumber(); }
    debug() << endmsg;
    if ( msgLevel(MSG::VERBOSE) )
    {
      for ( const auto& M : m_sphMirrors[D.first][D.second].mirrors )
      {
        verbose() << " -> Spherical mirror " << M->mirrorNumber()
                  << " " << M->name()
                  << " " << M->mirrorCentre()
                  << endmsg;
      }
    }

    debug() << "Found " << m_secMirrors[D.first][D.second].mirrors.size()
            << " secondary mirrors for " << D.first << " "
            << Rich::text(D.first,D.second) << " :";
    for ( const auto& M : m_secMirrors[D.first][D.second].mirrors )
    { debug() << " " << M->mirrorNumber(); }
    debug() << endmsg;
    if ( msgLevel(MSG::VERBOSE) )
    {
      for ( const auto& M : m_secMirrors[D.first][D.second].mirrors )
      {
        verbose() << " -> Secondary mirror " << M->mirrorNumber()
                  << " " << M->name()
                  << " " << M->mirrorCentre()
                  << endmsg;
      }
    }

  }

}

//=========================================================================

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, MirrorSegFinderLookUpTable )

//=========================================================================

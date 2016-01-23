
//-----------------------------------------------------------------------------
/** @file RichMirrorSegFinder.cpp
 *
 * Implementation file for class : Rich::MirrorSegFinder
 *
 * @date   2003-11-05
 * @author Antonis Papanestis
 */
//-----------------------------------------------------------------------------

// local
#include "RichMirrorSegFinder.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, MirrorSegFinder )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  Rich::MirrorSegFinder::MirrorSegFinder( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
    : Rich::ToolBase ( type, name, parent ),
      m_firstUpdate  ( true )
{
  // define interface
  declareInterface<IMirrorSegFinder>(this);

  // job options
  declareProperty( "TestFinding", m_testFinding = false  );
  declareProperty( "ScaleFactor", m_tuneScale   = 0.9999 );

  // should come from XML or job options ?? (this is in mm^2)
  m_maxDist[Rich::Rich1][sph] = 41033;
  m_maxDist[Rich::Rich1][sec] = 30712;
  m_maxDist[Rich::Rich2][sph] = 49442;
  m_maxDist[Rich::Rich2][sec] = 36671;
  declareProperty( "Rich1SphericalMaxDist", m_maxDist[Rich::Rich1][sph] );
  declareProperty( "Rich2SphericalMaxDist", m_maxDist[Rich::Rich2][sph] );
  declareProperty( "Rich1SecondaryMaxDist", m_maxDist[Rich::Rich1][sec] );
  declareProperty( "Rich2SecondaryMaxDist", m_maxDist[Rich::Rich2][sec] );
}

//=============================================================================
// Destructor
//=============================================================================
Rich::MirrorSegFinder::~MirrorSegFinder() { }

//=========================================================================
//  initialization
//=========================================================================
StatusCode Rich::MirrorSegFinder::initialize( )
{
  // intialise base class
  StatusCode sc = Rich::ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  if ( m_testFinding )
    Warning( "Will run slow checks of mirror search", StatusCode::SUCCESS ).ignore();

  // Force an update the cached mirror information
  // Also sets up the UMS service for future updates as needed
  sc = mirrorUpdate();

  // return
  return sc;
}

StatusCode Rich::MirrorSegFinder::loadMirrors( const Rich::DetectorType rich )
{
  // Load the RICH detector element
  const DeRich * deRich = getDet<DeRich>( DeRichLocations::location(rich) );

  // clear mirror vectors
  m_sphMirrors[rich][0].clear();
  m_sphMirrors[rich][1].clear();
  m_secMirrors[rich][0].clear();
  m_secMirrors[rich][1].clear();

  typedef std::vector<std::string> StringV;

  // load the primary mirrors
  const auto & sphMirs = deRich->paramVect<std::string>("SphericalMirrorDetElemLocations");
  for ( const auto & L : sphMirs )
  {
    DeRichSphMirror * m = getDet<DeRichSphMirror>( L );
    m_sphMirrors[rich][side(m,rich)].push_back( m );
    if ( m_firstUpdate )
      updMgrSvc()->registerCondition( this, m, &Rich::MirrorSegFinder::mirrorUpdate );
  }
  if ( m_sphMirrors[rich][0].empty() &&
       m_sphMirrors[rich][1].empty() )
  {
    return Error( "No primary mirrors in " + Rich::text(rich) );
  }

  // secondary mirrors
  const auto & secMirs = deRich->paramVect<std::string>("SecondaryMirrorDetElemLocations");
  for ( const auto & L : secMirs )
  {
    DeRichSphMirror * m = getDet<DeRichSphMirror>( L );
    m_secMirrors[rich][side(m,rich)].push_back( m );
    if ( m_firstUpdate )
      updMgrSvc()->registerCondition( this, m, &Rich::MirrorSegFinder::mirrorUpdate );
  }
  if ( m_secMirrors[rich][0].empty() &&
       m_secMirrors[rich][1].empty() )
  {
    return Error( "No secondary mirrors in " + Rich::text(rich) );
  }

  // sort the mirrors
  std::stable_sort( m_sphMirrors[rich][0].begin(), 
                    m_sphMirrors[rich][0].end(), 
                    SortByDistFromBeam() );
  std::stable_sort( m_sphMirrors[rich][1].begin(), 
                    m_sphMirrors[rich][1].end(), 
                    SortByDistFromBeam() );
  std::stable_sort( m_secMirrors[rich][0].begin(), 
                    m_secMirrors[rich][0].end(), 
                    SortByDistFromBeam() );
  std::stable_sort( m_secMirrors[rich][1].begin(), 
                    m_secMirrors[rich][1].end(), 
                    SortByDistFromBeam() );

  // init the last found mirror vectors
  m_lastFoundMirror[rich][0][sph] = nullptr;
  m_lastFoundMirror[rich][1][sph] = nullptr;
  m_lastFoundMirror[rich][0][sec] = nullptr;
  m_lastFoundMirror[rich][1][sec] = nullptr;

  return StatusCode::SUCCESS;
}

StatusCode Rich::MirrorSegFinder::mirrorUpdate()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Mirror Update Triggered" << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  // get all the mirror segments
  sc = sc && loadMirrors( Rich::Rich1 );
  sc = sc && loadMirrors( Rich::Rich2 );

  // ----------------------------------------------------------------------------------------
  // Debug and verbose printout
  // ----------------------------------------------------------------------------------------
  if ( msgLevel(MSG::DEBUG) )
  {
    typedef std::vector<std::pair<Rich::DetectorType,Rich::Side> > DetSide;
    const DetSide pairs = boost::assign::list_of
      ( std::make_pair(Rich::Rich1,Rich::top) )
      ( std::make_pair(Rich::Rich1,Rich::bottom) )
      ( std::make_pair(Rich::Rich2,Rich::left) )
      ( std::make_pair(Rich::Rich2,Rich::right) );
    for ( const auto & D : pairs )
    {

      const auto & sphMirrors = m_sphMirrors[D.first][D.second];
      debug() << "Found " << sphMirrors.size()
              << " primary mirrors for " << D.first << " " 
              << Rich::text(D.first,D.second) << " :";
      for ( const auto * M : sphMirrors )
      {
        debug() << " " << M->mirrorNumber();
      }
      debug() << endmsg;
      if ( msgLevel(MSG::VERBOSE) )
      {
        for ( const auto * M : sphMirrors )
        {
          verbose() << " -> Spherical mirror " << M->mirrorNumber()
                    << " " << M->name()
                    << " " << M->mirrorCentre()
                    << endmsg;
        }
      }

      const auto & secMirrors = m_secMirrors[D.first][D.second];
      debug() << "Found " << secMirrors.size()
              << " secondary mirrors for " << D.first << " " 
              << Rich::text(D.first,D.second) << " :";
      for ( const auto * M : secMirrors )
      {
        debug() << " " << M->mirrorNumber();
      }
      debug() << endmsg;
      if ( msgLevel(MSG::VERBOSE) )
      {
        for ( const auto * M : secMirrors )
        {
          verbose() << " -> Secondary mirror " << M->mirrorNumber()
                    << " " << M->name()
                    << " " << M->mirrorCentre()
                    << endmsg;
        }
      }

    }
  }
  // ----------------------------------------------------------------------------------------

  // Flag the first update as having been done
  m_firstUpdate = false;

  return sc;
}

//=========================================================================
//  finalize
//=========================================================================
StatusCode Rich::MirrorSegFinder::finalize( )
{

  // if test was performed, printout final max distances
  if ( m_testFinding )
  {
    always() << "Rich1SphericalMaxDist = " << m_maxDist[Rich::Rich1][sph] << endmsg;
    always() << "Rich2SphericalMaxDist = " << m_maxDist[Rich::Rich2][sph] << endmsg;
    always() << "Rich1SecondaryMaxDist = " << m_maxDist[Rich::Rich1][sec] << endmsg;
    always() << "Rich2SecondaryMaxDist = " << m_maxDist[Rich::Rich2][sec] << endmsg;
  }

  // unregister from UMS
  updMgrSvc()->unregister(this);

  return Rich::ToolBase::finalize();
}

//=========================================================================
//  find spherical mirror segment and return pointer
//=========================================================================
const DeRichSphMirror*
Rich::MirrorSegFinder::findSphMirror( const Rich::DetectorType rich,
                                      const Rich::Side side,
                                      const Gaudi::XYZPoint& reflPoint ) const
{
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Searching for Sph. mirror for point " << reflPoint << endmsg;

  // Most likely mirror is the last one found... So test this one first
  const auto * mirror = m_lastFoundMirror[rich][side][sph];
  if ( mirror )
  {
    const double dist2 = (mirror->mirrorCentre()-reflPoint).Mag2();
    if ( dist2 > m_maxDist[rich][sph] )
    {
      mirror = fullSphSearch(rich,side,reflPoint);
    }
    else if ( m_testFinding )
    {
      const auto * test_mirror = fullSphSearch(rich,side,reflPoint);
      if ( test_mirror != mirror )
      {
        m_maxDist[rich][sph] *= m_tuneScale;
        info() << "Decreasing " << rich << " spherical parameter to "
               << m_maxDist[rich][sph] << endmsg;
        mirror = test_mirror;
      }
    }
  }
  else
  {
    mirror = fullSphSearch(rich,side,reflPoint);
  }

  if ( msgLevel(MSG::VERBOSE) && mirror )
    verbose() << " -> Found mirror " << mirror->mirrorNumber() << endmsg;

  // return found mirror
  return mirror;
}

const DeRichSphMirror*
Rich::MirrorSegFinder::fullSphSearch( const Rich::DetectorType rich,
                                      const Rich::Side side,
                                      const Gaudi::XYZPoint& reflPoint ) const
{
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << " -> Starting full Sph mirror search" << endmsg;

  const DeRichSphMirror* mirror = nullptr;
  double distance2 ( 1e99 ); // units are mm^2
  for ( const auto * M : m_sphMirrors[rich][side] )
  {
    _ri_verbo << "  -> Sph Mirror " << M->mirrorNumber()
              << " " << M->mirrorCentre() << endmsg;
    const double temp_d2 = (M->mirrorCentre()-reflPoint).Mag2();
    if ( temp_d2 < distance2 )
    {
      // Found new closest mirror, so update number
      mirror = M;
      // if within tolerance, abort tests on other mirrors
      if ( !m_testFinding && temp_d2 < m_maxDist[rich][sph] ) break;
      // ... otherwise, update minimum distance and continue
      distance2 = temp_d2;
    }
  }

  _ri_verbo << " -> selected mirror " << mirror->mirrorNumber() << endmsg;

  // update last found mirror and return
  return m_lastFoundMirror[rich][side][sph] = mirror;
}

//=========================================================================
//  find secondary mirror segment and return pointer
//=========================================================================
const DeRichSphMirror*
Rich::MirrorSegFinder::findSecMirror( const Rich::DetectorType rich,
                                      const Rich::Side side,
                                      const Gaudi::XYZPoint& reflPoint ) const
{
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Searching for Sec. mirror for point " << reflPoint << endmsg;

  // Most likely mirror is the last one found... So test this one first
  const auto * mirror = m_lastFoundMirror[rich][side][sec];
  if ( mirror )
  {
    const double dist2 = (mirror->mirrorCentre()-reflPoint).Mag2();
    if ( dist2 > m_maxDist[rich][sec] )
    {
      mirror = fullSecSearch(rich,side,reflPoint);
    }
    else if ( m_testFinding )
    {
      const auto * test_mirror = fullSecSearch(rich,side,reflPoint);
      if ( test_mirror != mirror )
      {
        m_maxDist[rich][sec] *= m_tuneScale;
        info() << "Decreasing " << rich << " secondary parameter to "
               << m_maxDist[rich][sec] << endmsg;
        mirror = test_mirror;
      }
    }
  }
  else
  {
    mirror = fullSecSearch(rich,side,reflPoint);
  }

  if ( msgLevel(MSG::VERBOSE) && mirror )
    verbose() << " -> Found mirror " << mirror->mirrorNumber() << endmsg;

  // return found mirror
  return mirror;
}

//=========================================================================
//  fullSecSearch
//=========================================================================
const DeRichSphMirror*
Rich::MirrorSegFinder::fullSecSearch( const Rich::DetectorType rich,
                                      const Rich::Side side,
                                      const Gaudi::XYZPoint& reflPoint ) const
{
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << " -> Starting full Sec mirror search" << endmsg;

  const DeRichSphMirror* mirror = nullptr;
  double distance2 ( 1e99 ); // units are mm^2
  for ( const auto * M : m_secMirrors[rich][side] )
  {
    _ri_verbo << "  -> Sec Mirror " << M->mirrorNumber()
              << " " << M->mirrorCentre() << endmsg;
    const double temp_d2 = (M->mirrorCentre()-reflPoint).Mag2();
    if ( temp_d2 < distance2 )
    {
      // Found new closest mirror, so update number
      mirror = M;
      // if within tolerance, abort tests on other mirrors
      if ( !m_testFinding && temp_d2 < m_maxDist[rich][sec] ) break;
      // ... otherwise, update minimum distance and continue
      distance2 = temp_d2;
    }
  }

  _ri_verbo << " -> selected mirror " << mirror->mirrorNumber() << endmsg;

  // update last found mirror and return
  return m_lastFoundMirror[rich][side][sec] = mirror;
}

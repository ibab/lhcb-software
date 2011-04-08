
//-----------------------------------------------------------------------------
/** @file RichMirrorSegFinder.cpp
 *
 * Implementation file for class : Rich::MirrorSegFinder
 *
 * @date   2003-11-05
 * @author Antonis Papanestis
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

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
  sc = sc && mirrorUpdate();

  // return
  return sc;
}

StatusCode Rich::MirrorSegFinder::mirrorUpdate()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Mirror Update Triggered" << endmsg;

  StatusCode sc = StatusCode::SUCCESS;

  // clear mirror vectors
  for ( int i=0; i<2; ++i )
  {
    for ( int j=0; j<2; ++j )
    {
      m_sphMirrors[i][j].clear();
      m_secMirrors[i][j].clear();
    }
  }

  // get the RICH detectors
  const DeRich* rich1 = getDet<DeRich>( DeRichLocations::Rich1 );
  const DeRich* rich2 = getDet<DeRich>( DeRichLocations::Rich2 );

  // get all the mirror segments
  if ( rich1->exists("SphericalMirrorDetElemLocations") ) // post DC06 description
  {
    // Rich1 spherical mirrors
    const std::vector<std::string> & r1SphMirLoc =
      rich1->paramVect<std::string>("SphericalMirrorDetElemLocations");
    for (unsigned int r1sphm=0; r1sphm<r1SphMirLoc.size(); ++r1sphm)
    {
      DeRichSphMirror* sm = getDet<DeRichSphMirror>( r1SphMirLoc[r1sphm] );
      if ( sm->mirrorCentre().y() > 0.0 )
      {
        m_sphMirrors[Rich::Rich1][Rich::top].push_back( sm );
      }
      else
      {
        m_sphMirrors[Rich::Rich1][Rich::bottom].push_back( sm );
      }
      if ( m_firstUpdate )
        updMgrSvc()->registerCondition( this, sm, &Rich::MirrorSegFinder::mirrorUpdate );
    }
    // Rich1 secondary mirrors
    const std::vector<std::string> & r1SecMirLoc =
      rich1->paramVect<std::string>("SecondaryMirrorDetElemLocations");
    for (unsigned int r1secm=0; r1secm<r1SecMirLoc.size(); ++r1secm)
    {
      DeRichSphMirror* secm = getDet<DeRichSphMirror>( r1SecMirLoc[r1secm] );
      if ( secm->mirrorCentre().y() > 0.0 )
      {
        m_secMirrors[Rich::Rich1][Rich::top].push_back( secm );
      }
      else
      {
        m_secMirrors[Rich::Rich1][Rich::bottom].push_back( secm );
      }
      if ( m_firstUpdate )
        updMgrSvc()->registerCondition( this, secm, &Rich::MirrorSegFinder::mirrorUpdate );
    }
    // Rich2 spherical mirrors
    const std::vector<std::string> & r2SphMirLoc =
      rich2->paramVect<std::string>("SphericalMirrorDetElemLocations");
    for (unsigned int r2sphm=0; r2sphm<r2SphMirLoc.size(); ++r2sphm)
    {
      DeRichSphMirror* sm = getDet<DeRichSphMirror>( r2SphMirLoc[r2sphm] );
      if ( sm->centreOfCurvature().x() > 0.0 )
      {
        m_sphMirrors[Rich::Rich2][Rich::left].push_back( sm );
      }
      else
      {
        m_sphMirrors[Rich::Rich2][Rich::right].push_back( sm );
      }
      if ( m_firstUpdate )
        updMgrSvc()->registerCondition( this, sm, &Rich::MirrorSegFinder::mirrorUpdate );
    }
    // Rich2 secondary mirrors
    const std::vector<std::string> & r2SecMirLoc =
      rich2->paramVect<std::string>("SecondaryMirrorDetElemLocations");
    for (unsigned int r2secm=0; r2secm<r2SecMirLoc.size(); ++r2secm)
    {
      DeRichSphMirror* secm = getDet<DeRichSphMirror>( r2SecMirLoc[r2secm] );
      if ( secm->mirrorCentre().x() > 0.0 )
      {
        m_secMirrors[Rich::Rich2][Rich::left].push_back( secm );
      }
      else
      {
        m_secMirrors[Rich::Rich2][Rich::right].push_back( secm );
      }
      if ( m_firstUpdate )
        updMgrSvc()->registerCondition( this, secm, &Rich::MirrorSegFinder::mirrorUpdate );
    }
  }
  else // DC06 way
  {
    sc = getMirrors_old();
    if ( sc.isFailure() ) return sc;
  }

  // checks and balances ...

  if ( m_sphMirrors[Rich::Rich1][Rich::top].empty()   &&
       m_sphMirrors[Rich::Rich1][Rich::bottom].empty() )
  {
    return Error( "No spherical mirrors in Rich1" );
  }

  // sort the RICH1 mirrors
  std::sort( m_sphMirrors[Rich::Rich1][Rich::top].begin(),
             m_sphMirrors[Rich::Rich1][Rich::top].end(),    SortByDistFromBeam() );
  std::sort( m_sphMirrors[Rich::Rich1][Rich::bottom].begin(),
             m_sphMirrors[Rich::Rich1][Rich::bottom].end(), SortByDistFromBeam() );
  std::sort( m_secMirrors[Rich::Rich1][Rich::top].begin(),
             m_secMirrors[Rich::Rich1][Rich::top].end(),    SortByDistFromBeam() );
  std::sort( m_secMirrors[Rich::Rich1][Rich::bottom].begin(),
             m_secMirrors[Rich::Rich1][Rich::bottom].end(), SortByDistFromBeam() );

  if ( m_sphMirrors[Rich::Rich2][Rich::left].empty() &&
       m_sphMirrors[Rich::Rich2][Rich::right].empty() )
  {
    return Error( "No spherical mirrors in Rich2" );
  }

  // sort the RICH2 mirrors
  std::sort( m_sphMirrors[Rich::Rich2][Rich::left].begin(),
             m_sphMirrors[Rich::Rich2][Rich::left].end(),  SortByDistFromBeam() );
  std::sort( m_sphMirrors[Rich::Rich2][Rich::right].begin(),
             m_sphMirrors[Rich::Rich2][Rich::right].end(), SortByDistFromBeam() );
  std::sort( m_secMirrors[Rich::Rich2][Rich::left].begin(),
             m_secMirrors[Rich::Rich2][Rich::left].end(),  SortByDistFromBeam() );
  std::sort( m_secMirrors[Rich::Rich2][Rich::right].begin(),
             m_secMirrors[Rich::Rich2][Rich::right].end(), SortByDistFromBeam() );

  if ( m_sphMirrors[Rich::Rich2][Rich::left].empty() )
  {
    // copy mirrors from the other side
    for ( unsigned int smr=0; smr<m_sphMirrors[Rich::Rich2][Rich::right].size(); ++smr )
    {
      m_sphMirrors[Rich::Rich2][Rich::left].push_back( m_sphMirrors[Rich::Rich2][Rich::right][smr] );
    }
    warning() << "No sph mirrors on left side of Rich2; copying from right side"
              << endmsg;
  }

  if ( m_sphMirrors[Rich::Rich2][Rich::right].empty() )
  {
    // copy mirrors from the other side
    for ( unsigned int smr=0; smr<m_sphMirrors[Rich::Rich2][Rich::left].size(); ++smr )
    {
      m_sphMirrors[Rich::Rich2][Rich::right].push_back( m_sphMirrors[Rich::Rich2][Rich::left][smr] );
    }
    warning() << "No sph mirrors on right side of Rich2; copying from left side"
              << endmsg;
  }

  // initialise the last found mirror pointers to the first in the vectors
  {
    for (int i=0; i<2; ++i)
    {
      for (int j=0; j<2; ++j)
      {
        if ( !m_sphMirrors[i][j].empty() )
        {
          m_lastFoundMirror[i][j][sph] = m_sphMirrors[i][j].front();
        }
        else
        {
          m_lastFoundMirror[i][j][sph] = NULL;
        }
        if ( !m_secMirrors[i][j].empty() )
        {
          m_lastFoundMirror[i][j][sec] = m_secMirrors[i][j].front();
        }
        else
        {
          m_lastFoundMirror[i][j][sec] = NULL;
        }
      }
    }
  }

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
    for ( DetSide::const_iterator iD = pairs.begin(); iD != pairs.end(); ++iD )
    {

      debug() << "Found " << m_sphMirrors[iD->first][iD->second].size()
              << " primary mirrors for " << iD->first << " " << Rich::text(iD->first,iD->second) << " :";
      for ( std::vector<const DeRichSphMirror*>::const_iterator iM = m_sphMirrors[iD->first][iD->second].begin();
            iM != m_sphMirrors[iD->first][iD->second].end(); ++iM )
      {
        debug() << " " << (*iM)->mirrorNumber();
      }
      debug() << endmsg;
      if ( msgLevel(MSG::VERBOSE) )
      {
        for ( std::vector<const DeRichSphMirror*>::const_iterator iM = m_sphMirrors[iD->first][iD->second].begin();
              iM != m_sphMirrors[iD->first][iD->second].end(); ++iM )
        {
          verbose() << " -> Spherical mirror " << (*iM)->mirrorNumber()
                    << " " << (*iM)->name()
                    << " " << (*iM)->mirrorCentre()
                    << endmsg;
        }
      }

      debug() << "Found " << m_secMirrors[iD->first][iD->second].size()
              << " secondary mirrors for " << iD->first << " " << Rich::text(iD->first,iD->second) << " :";
      for ( std::vector<const DeRichSphMirror*>::const_iterator iM = m_secMirrors[iD->first][iD->second].begin();
            iM != m_secMirrors[iD->first][iD->second].end(); ++iM )
      {
        debug() << " " << (*iM)->mirrorNumber();
      }
      debug() << endmsg;
      if ( msgLevel(MSG::VERBOSE) )
      {
        for ( std::vector<const DeRichSphMirror*>::const_iterator iM = m_secMirrors[iD->first][iD->second].begin();
              iM != m_secMirrors[iD->first][iD->second].end(); ++iM )
        {
          verbose() << " -> Secondary mirror " << (*iM)->mirrorNumber()
                    << " " << (*iM)->name()
                    << " " << (*iM)->mirrorCentre()
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
  const DeRichSphMirror* mirror = m_lastFoundMirror[rich][side][sph];
  if ( mirror )
  {
    const double dist2 = (mirror->mirrorCentre()-reflPoint).Mag2();
    if ( dist2 > m_maxDist[rich][sph] )
    {
      mirror = fullSphSearch(rich,side,reflPoint);
    }
    else if ( m_testFinding )
    {
      const DeRichSphMirror* test_mirror = fullSphSearch(rich,side,reflPoint);
      if ( test_mirror != mirror )
      {
        m_maxDist[rich][sph] *= m_tuneScale;
        info() << "Decreasing " << rich << " spherical parameter to "
               << m_maxDist[rich][sph] << endmsg;
        mirror = test_mirror;
      }
    }
  }

  if ( msgLevel(MSG::VERBOSE) )
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

  const DeRichSphMirror* mirror = NULL;
  double distance2 ( 1e99 ); // units are mm^2
  for ( std::vector<const DeRichSphMirror*>::const_iterator iM
          = m_sphMirrors[rich][side].begin();
        iM != m_sphMirrors[rich][side].end(); ++iM )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "  -> Sph Mirror " << (*iM)->mirrorNumber()
                << " " << (*iM)->mirrorCentre() << endmsg;
    const double temp_d2 = ((*iM)->mirrorCentre()-reflPoint).Mag2();
    if ( temp_d2 < distance2 )
    {
      // Found new closest mirror, so update number
      mirror = *iM;
      // if within tolerance, abort tests on other mirrors
      if ( !m_testFinding && temp_d2 < m_maxDist[rich][sph] ) break;
      // ... otherwise, update minimum distance and continue
      distance2 = temp_d2;
    }
  }

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << " -> selected mirror " << mirror->mirrorNumber() << endmsg;

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
  const DeRichSphMirror* mirror = m_lastFoundMirror[rich][side][sec];
  if ( mirror )
  {
    const double dist2 = (mirror->mirrorCentre()-reflPoint).Mag2();
    if ( dist2 > m_maxDist[rich][sec] )
    {
      mirror = fullSecSearch(rich,side,reflPoint);
    }
    else if ( m_testFinding )
    {
      const DeRichSphMirror* test_mirror = fullSecSearch(rich,side,reflPoint);
      if ( test_mirror != mirror )
      {
        m_maxDist[rich][sec] *= m_tuneScale;
        info() << "Decreasing " << rich << " secondary parameter to "
               << m_maxDist[rich][sec] << endmsg;
        mirror = test_mirror;
      }
    }
  }

  if ( msgLevel(MSG::VERBOSE) )
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

  const DeRichSphMirror* mirror = NULL;
  double distance2 ( 1e99 ); // units are mm^2
  for ( std::vector<const DeRichSphMirror*>::const_iterator iM
          = m_secMirrors[rich][side].begin();
        iM != m_secMirrors[rich][side].end(); ++iM )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "  -> Sec Mirror " << (*iM)->mirrorNumber()
                << " " << (*iM)->mirrorCentre() << endmsg;
    const double temp_d2 = ((*iM)->mirrorCentre()-reflPoint).Mag2();
    if ( temp_d2 < distance2 )
    {
      // Found new closest mirror, so update number
      mirror = *iM;
      // if within tolerance, abort tests on other mirrors
      if ( !m_testFinding && temp_d2 < m_maxDist[rich][sec] ) break;
      // ... otherwise, update minimum distance and continue
      distance2 = temp_d2;
    }
  }

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << " -> selected mirror " << mirror->mirrorNumber() << endmsg;

  // update last found mirror and return
  return m_lastFoundMirror[rich][side][sec] = mirror;
}

//=========================================================================
// DC06 compatibility
//=========================================================================
StatusCode Rich::MirrorSegFinder::getMirrors_old( )
{
  // get the RICH detectors
  const DeRich* rich1 = getDet<DeRich>( DeRichLocations::Rich1 );
  const DeRich* rich2 = getDet<DeRich>( DeRichLocations::Rich2 );

  // find all the mirrors in Rich1
  const IDetectorElement::IDEContainer& detelemsR1 = rich1->childIDetectorElements();
  {
    for ( IDetectorElement::IDEContainer::const_iterator det_it =  detelemsR1.begin();
          det_it != detelemsR1.end();
          ++det_it )
    {
      const std::string & detName = (*det_it)->name();

      if ( detName.find("Mirror1") != std::string::npos )
      {
        const DeRichSphMirror* sm = getDet<DeRichSphMirror>( detName );
        if ( sm->mirrorCentre().y() > 0.0 )
        {
          m_sphMirrors[Rich::Rich1][Rich::top].push_back( sm );
        }
        else
        {
          m_sphMirrors[Rich::Rich1][Rich::bottom].push_back( sm );
        }
      }

      if ( detName.find("Mirror2") != std::string::npos )
      {
        const DeRichSphMirror* secm = getDet<DeRichSphMirror>( detName );
        if ( secm->mirrorCentre().y() > 0.0 )
        {
          m_secMirrors[Rich::Rich1][Rich::top].push_back( secm );
        }
        else
        {
          m_secMirrors[Rich::Rich1][Rich::bottom].push_back( secm );
        }

      }

    }
  }

  // find all the mirrors in Rich2
  const IDetectorElement::IDEContainer & detelemsR2 = rich2->childIDetectorElements();
  {
    for ( IDetectorElement::IDEContainer::const_iterator det_it = detelemsR2.begin();
          det_it != detelemsR2.end();
          ++det_it )
    {
      const std::string & detName = (*det_it)->name();

      if ( detName.find("SphMirror") != std::string::npos )
      {
        const DeRichSphMirror* sm = getDet<DeRichSphMirror>( detName );
        if ( sm->centreOfCurvature().x() > 0.0 )
        {
          m_sphMirrors[Rich::Rich2][Rich::left].push_back( sm );
        }
        else
        {
          m_sphMirrors[Rich::Rich2][Rich::right].push_back( sm );
        }
      }

      if ( detName.find("SecMirror") != std::string::npos )
      {
        const DeRichSphMirror* secm = getDet<DeRichSphMirror>( detName );
        if ( secm->mirrorCentre().x() > 0.0 )
        {
          m_secMirrors[Rich::Rich2][Rich::left].push_back( secm );
        }
        else
        {
          m_secMirrors[Rich::Rich2][Rich::right].push_back( secm );
        }
      }

    }
  }
  return StatusCode ::SUCCESS;
}

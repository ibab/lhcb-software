
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
  : Rich::ToolBase ( type, name, parent )
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

  // clear mirror vectors
  for (int i=0; i<2; ++i)
  {
    for (int j=0; j<2; ++j)
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
      const DeRichSphMirror* sm = getDet<DeRichSphMirror>( r1SphMirLoc[r1sphm] );
      if ( sm->mirrorCentre().y() > 0.0 )
        m_sphMirrors[Rich::Rich1][Rich::top].push_back( sm );
      else
        m_sphMirrors[Rich::Rich1][Rich::bottom].push_back( sm );
    }
    // Rich1 secondary mirrors
    const std::vector<std::string> & r1SecMirLoc = 
      rich1->paramVect<std::string>("SecondaryMirrorDetElemLocations");
    for (unsigned int r1secm=0; r1secm<r1SecMirLoc.size(); ++r1secm)
    {
      const DeRichSphMirror* secm = getDet<DeRichSphMirror>( r1SecMirLoc[r1secm] );
      if ( secm->mirrorCentre().y() > 0.0 )
        m_secMirrors[Rich::Rich1][Rich::top].push_back( secm );
      else
        m_secMirrors[Rich::Rich1][Rich::bottom].push_back( secm );
    }
    // Rich2 spherical mirrors
    const std::vector<std::string> & r2SphMirLoc = 
      rich2->paramVect<std::string>("SphericalMirrorDetElemLocations");
    for (unsigned int r2sphm=0; r2sphm<r2SphMirLoc.size(); ++r2sphm)
    {
      const DeRichSphMirror* sm = getDet<DeRichSphMirror>( r2SphMirLoc[r2sphm] );
      if ( sm->centreOfCurvature().x() > 0.0 )
        m_sphMirrors[Rich::Rich2][Rich::left].push_back( sm );
      else
        m_sphMirrors[Rich::Rich2][Rich::right].push_back( sm );
    }
    // Rich2 secondary mirrors
    const std::vector<std::string> & r2SecMirLoc = 
      rich2->paramVect<std::string>("SecondaryMirrorDetElemLocations");
    for (unsigned int r2secm=0; r2secm<r2SecMirLoc.size(); ++r2secm)
    {
      const DeRichSphMirror* secm = getDet<DeRichSphMirror>( r2SecMirLoc[r2secm] );
      if ( secm->mirrorCentre().x() > 0.0 )
        m_secMirrors[Rich::Rich2][Rich::left].push_back( secm );
      else
        m_secMirrors[Rich::Rich2][Rich::right].push_back( secm );
    }
  }
  else // DC06 way
  {
    sc = getMirrors_old();
    if ( sc.isFailure() ) return sc;
  }

  // checks and balances...
  if ( m_sphMirrors[Rich::Rich1][Rich::top].empty() &&
       m_sphMirrors[Rich::Rich1][Rich::bottom].empty() )
    return Error( "No spherical mirrors in Rich1" );

  // sort the RICH1 mirrors
  std::sort( m_sphMirrors[Rich::Rich1][Rich::top].begin(),
             m_sphMirrors[Rich::Rich1][Rich::top].end(), SortByDistFromBeam() );
  std::sort( m_sphMirrors[Rich::Rich1][Rich::bottom].begin(),
             m_sphMirrors[Rich::Rich1][Rich::bottom].end(), SortByDistFromBeam() );
  std::sort( m_secMirrors[Rich::Rich1][Rich::top].begin(),
             m_secMirrors[Rich::Rich1][Rich::top].end(), SortByDistFromBeam() );
  std::sort( m_secMirrors[Rich::Rich1][Rich::bottom].begin(),
             m_secMirrors[Rich::Rich1][Rich::bottom].end(), SortByDistFromBeam() );

  debug() << "Found " << m_sphMirrors[Rich::Rich1][Rich::top].size() +
    m_sphMirrors[Rich::Rich1][Rich::bottom].size() << " spherical and "
          << m_secMirrors[Rich::Rich1][Rich::top].size() +
    m_secMirrors[Rich::Rich1][Rich::bottom].size() << " sec mirrors in Rich1"
          << endmsg;

  if ( m_sphMirrors[Rich::Rich2][Rich::left].empty() &&
       m_sphMirrors[Rich::Rich2][Rich::right].empty() )
    return Error( "No spherical mirrors in Rich2" );

  // sort the RICH2 mirrors
  std::sort( m_sphMirrors[Rich::Rich2][Rich::left].begin(),
             m_sphMirrors[Rich::Rich2][Rich::left].end(), SortByDistFromBeam() );
  std::sort( m_sphMirrors[Rich::Rich2][Rich::right].begin(),
             m_sphMirrors[Rich::Rich2][Rich::right].end(), SortByDistFromBeam() );
  std::sort( m_secMirrors[Rich::Rich2][Rich::left].begin(),
             m_secMirrors[Rich::Rich2][Rich::left].end(), SortByDistFromBeam() );
  std::sort( m_secMirrors[Rich::Rich2][Rich::right].begin(),
             m_secMirrors[Rich::Rich2][Rich::right].end(), SortByDistFromBeam() );

  if ( m_sphMirrors[Rich::Rich2][Rich::left].empty() )
  {
    // copy mirrors from the other side
    for (unsigned int smr=0; smr<m_sphMirrors[Rich::Rich2][Rich::right].size(); ++smr)
    {
      m_sphMirrors[Rich::Rich2][Rich::left].push_back( m_sphMirrors[Rich::Rich2][Rich::right][smr] );
    }
    warning() << "No sph mirrors on left side of Rich2; copying from right side"
              << endmsg;
  }

  if ( m_sphMirrors[Rich::Rich2][Rich::right].empty() )
  {
    // copy mirrors from the other side
    for (unsigned int smr=0; smr<m_sphMirrors[Rich::Rich2][Rich::left].size(); ++smr)
    {
      m_sphMirrors[Rich::Rich2][Rich::right].push_back( m_sphMirrors[Rich::Rich2][Rich::left][smr] );
    }
    warning() << "No sph mirrors on right side of Rich2; copying from left side"
              << endmsg;
  }

  debug() << "Found " << m_sphMirrors[Rich::Rich2][Rich::left].size() +
    m_sphMirrors[Rich::Rich2][Rich::right].size() << " spherical and "
          << m_secMirrors[Rich::Rich2][Rich::left].size() +
    m_secMirrors[Rich::Rich2][Rich::right].size() << " sec mirrors in Rich2"
          << endmsg;


  // initialise the last found mirror pointers to the first in the vectors
  {
    for (int i=0; i<2; ++i)
    {
      for (int j=0; j<2; ++j)
      {
        if ( !m_sphMirrors[i][j].empty() )
        { m_lastFoundMirror[i][j][sph] = m_sphMirrors[i][j].front(); }
        else
        { m_lastFoundMirror[i][j][sph] = NULL; }
        if ( !m_secMirrors[i][j].empty() )
        { m_lastFoundMirror[i][j][sec] = m_secMirrors[i][j].front(); }
        else
        { m_lastFoundMirror[i][j][sec] = NULL; }
      }
    }
  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Stored " << m_sphMirrors[Rich::Rich1][Rich::top].size()
              << " and " << m_sphMirrors[Rich::Rich1][Rich::bottom].size()
              << " spherical mirrors in the top and bottom of Rich1, and "
              << m_secMirrors[Rich::Rich1][Rich::top].size() << " and "
              << m_secMirrors[Rich::Rich1][Rich::bottom].size() << " secondary mirrors"
              << endmsg;
    verbose() << "Stored " << m_sphMirrors[Rich::Rich2][Rich::left].size()
              << " and " << m_sphMirrors[Rich::Rich2][Rich::right].size()
              << " spherical mirrors in the left and right of Rich2, and "
              << m_secMirrors[Rich::Rich2][Rich::left].size() << " and "
              << m_secMirrors[Rich::Rich2][Rich::right].size() << " secondary mirrors"
              << endmsg;

    {for ( unsigned int r=0; r<2; ++r )
        for ( unsigned int s=0; s<2; ++s )
          for ( unsigned int num=0; num<m_sphMirrors[r][s].size(); ++num )
          {
            verbose() << "Spherical mirror " << num << " rich=" << r << " side=" << s
                      << " " << m_sphMirrors[r][s][num]->name()
                      << " " << m_sphMirrors[r][s][num]->mirrorCentre()
                      << endmsg;
            //std::ostringstream title;
            //title << "Primary mirror Centres : Rich" << r+1 << " side " << s;
            //plot2D( m_sphMirrors[r][s][num]->mirrorCentre().x(),
            //        m_sphMirrors[r][s][num]->mirrorCentre().y(),
            //        title.str(), -maxX[r][sph], maxX[r][sph], -maxY[r][sph], maxY[r][sph],
            //        50, 50, (double)(num+1) );
          }
    }
    {for ( unsigned int r=0; r<2; ++r )
        for ( unsigned int s=0; s<2; ++s )
          for ( unsigned int num=0; num<m_secMirrors[r][s].size(); ++num )
          {
            verbose() << "Secondary mirror " << num << " rich=" << r << " side=" << s
                      << " " << m_secMirrors[r][s][num]->name()
                      << " " << m_secMirrors[r][s][num]->mirrorCentre()
                      << endmsg;
            //std::ostringstream title;
            //title << "Secondary mirror Centres : Rich" << r+1 << " side " << s;
            //plot2D( m_secMirrors[r][s][num]->mirrorCentre().x(),
            //        m_secMirrors[r][s][num]->mirrorCentre().y(),
            //        title.str(), -maxX[r][sec], maxX[r][sec], -maxY[r][sec], maxY[r][sec],
            //        50, 50, (double)(num+1) );
          }
    }

  } // verbose

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
        info() << "Decreasing " << rich << " spherical parameter to " << m_maxDist[rich][sph] << endmsg;
        mirror = test_mirror;
      }
    }
  }

  //if ( msgLevel(MSG::VERBOSE) )
  //  verbose() << "Found primary mirror " << mirror << endmsg;

  // return found mirror
  return mirror;
}

const DeRichSphMirror*
Rich::MirrorSegFinder::fullSphSearch( const Rich::DetectorType rich,
                                      const Rich::Side side,
                                      const Gaudi::XYZPoint& reflPoint ) const
{
  const DeRichSphMirror* mirror = NULL;
  double distance2 ( 1e99 ); // units are mm^2
  for ( std::vector<const DeRichSphMirror*>::const_iterator iM = m_sphMirrors[rich][side].begin();
        iM != m_sphMirrors[rich][side].end(); ++iM )
  {
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
  const DeRichSphMirror* mirror = NULL;
  double distance2 ( 1e99 ); // units are mm^2
  for ( std::vector<const DeRichSphMirror*>::const_iterator iM = m_secMirrors[rich][side].begin();
        iM != m_secMirrors[rich][side].end(); ++iM )
  {
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


//-----------------------------------------------------------------------------
/** @file RichMirrorSegFinder.cpp
 *
 * Implementation file for class : RichMirrorSegFinder
 *
 * CVS Log :-
 * $Id: RichMirrorSegFinder.cpp,v 1.19 2006-10-20 13:20:29 jonrob Exp $
 *
 * @date   2003-11-05
 * @author Antonis Papanestis
 */
//-----------------------------------------------------------------------------

// local
#include "RichMirrorSegFinder.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichMirrorSegFinder>          s_factory ;
const        IToolFactory& RichMirrorSegFinderFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichMirrorSegFinder::RichMirrorSegFinder( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : RichToolBase ( type, name, parent )
{
  // define interface
  declareInterface<IRichMirrorSegFinder>(this);

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
RichMirrorSegFinder::~RichMirrorSegFinder() { }

//=========================================================================
//  initialization
//=========================================================================
StatusCode RichMirrorSegFinder::initialize( )
{
  // intialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Initialise search parameters
  for (int i=0; i<2; ++i)
  {
    for (int j=0; j<2; ++j)
    {
      for (int k=0; k<2; ++k)
      {
        m_maxMirror[i][j][k] = 0;
        m_lastFoundMirror[i][j][k] = 0;
      }
    }
  }

  // get the RICH detectors
  const DeRich* rich1 = getDet<DeRich>( DeRichLocation::Rich1 );
  const DeRich* rich2 = getDet<DeRich>( DeRichLocation::Rich2 );

  // find all the mirrors in Rich1
  unsigned int mirrorNum(0);
  const IDetectorElement::IDEContainer& detelemsR1 = rich1->childIDetectorElements();
  {for ( IDetectorElement::IDEContainer::const_iterator det_it =  detelemsR1.begin();
         det_it != detelemsR1.end();
         ++det_it )
  {
    const std::string & detName = (*det_it)->name();

    if ( detName.find("Mirror1") != std::string::npos )
    {
      const DeRichSphMirror* sm = getDet<DeRichSphMirror>( detName );
      if ( sm->mirrorCentre().y() > 0.0 )
      {
        mirrorNum = m_maxMirror[Rich::Rich1][Rich::top][sph];
        m_sphMirrors[Rich::Rich1][Rich::top][mirrorNum] = sm;
        ++m_maxMirror[Rich::Rich1][Rich::top][sph];
      }
      else
      {
        mirrorNum = m_maxMirror[Rich::Rich1][Rich::bottom][sph];
        m_sphMirrors[Rich::Rich1][Rich::bottom][mirrorNum] = sm;
        ++m_maxMirror[Rich::Rich1][Rich::bottom][sph];
      }
    }

    if ( detName.find("Mirror2") != std::string::npos )
    {
      const DeRichSphMirror* secm = getDet<DeRichSphMirror>( detName );
      if ( secm->mirrorCentre().y() > 0.0 )
      {
        mirrorNum = m_maxMirror[Rich::Rich1][Rich::top][sec];
        m_secMirrors[Rich::Rich1][Rich::top][mirrorNum] = secm;
        ++m_maxMirror[Rich::Rich1][Rich::top][sec];
      }
      else
      {
        mirrorNum = m_maxMirror[Rich::Rich1][Rich::bottom][sec];
        m_secMirrors[Rich::Rich1][Rich::bottom][mirrorNum] = secm;
        ++m_maxMirror[Rich::Rich1][Rich::bottom][sec];
      }

    }

  }}

  debug() << "Found " << m_maxMirror[Rich::Rich1][Rich::top][sph] +
    m_maxMirror[Rich::Rich1][Rich::bottom][sph] << " spherical and "
          << m_maxMirror[Rich::Rich1][Rich::top][sec] +
    m_maxMirror[Rich::Rich1][Rich::bottom][sec] << " sec mirrors in Rich1"
          << endmsg;

  // find all the mirrors in Rich2
  const IDetectorElement::IDEContainer & detelemsR2 = rich2->childIDetectorElements();
  {for ( IDetectorElement::IDEContainer::const_iterator det_it = detelemsR2.begin();
         det_it != detelemsR2.end();
         ++det_it )
  {
    const std::string & detName = (*det_it)->name();

    if ( detName.find("SphMirror") != std::string::npos )
    {
      const DeRichSphMirror* sm = getDet<DeRichSphMirror>( detName );
      if ( sm->centreOfCurvature().x() > 0.0 )
      {
        mirrorNum = m_maxMirror[Rich::Rich2][Rich::left][sph];
        m_sphMirrors[Rich::Rich2][Rich::left][mirrorNum] = sm;
        ++m_maxMirror[Rich::Rich2][Rich::left][sph];
      }
      else
      {
        mirrorNum = m_maxMirror[Rich::Rich2][Rich::right][sph];
        m_sphMirrors[Rich::Rich2][Rich::right][mirrorNum] = sm;
        ++m_maxMirror[Rich::Rich2][Rich::right][sph];
      }
    }

    if ( detName.find("SecMirror") != std::string::npos )
    {
      const DeRichSphMirror* secm = getDet<DeRichSphMirror>( detName );
      if ( secm->mirrorCentre().x() > 0.0 )
      {
        mirrorNum = m_maxMirror[Rich::Rich2][Rich::left][sec];
        m_secMirrors[Rich::Rich2][Rich::left][mirrorNum] = secm;
        ++m_maxMirror[Rich::Rich2][Rich::left][sec];
      }
      else
      {
        mirrorNum = m_maxMirror[Rich::Rich2][Rich::right][sec];
        m_secMirrors[Rich::Rich2][Rich::right][mirrorNum] = secm;
        ++m_maxMirror[Rich::Rich2][Rich::right][sec];
      }
    }

  }}

  if ( 0 == m_maxMirror[Rich::Rich1][Rich::left][sph] &&
       0 == m_maxMirror[Rich::Rich1][Rich::right][sph] )
    return Error( "No spherical mirrors in Rich1" );

  if ( 0 == m_maxMirror[Rich::Rich2][Rich::left][sph] &&
       0 == m_maxMirror[Rich::Rich2][Rich::right][sph] )
    return Error( "No spherical mirrors in Rich2" );

  if ( 0 == m_maxMirror[Rich::Rich2][Rich::left][sph] ) {
    // copy mirrors from the other side
    for (unsigned int smr=0; smr<m_maxMirror[Rich::Rich2][Rich::right][sph]; ++smr)
      m_sphMirrors[Rich::Rich2][Rich::left][smr] =
        m_sphMirrors[Rich::Rich2][Rich::right][smr];
    m_maxMirror[Rich::Rich2][Rich::left][sph] =
      m_maxMirror[Rich::Rich2][Rich::right][sph];
    warning() << "No sph mirrors on left side of Rich2; copying from right side"
              << endmsg;
  }

  if ( 0 == m_maxMirror[Rich::Rich2][Rich::right][sph] ) {
    // copy mirrors from the other side
    for (unsigned int smr=0; smr<m_maxMirror[Rich::Rich2][Rich::left][sph]; ++smr)
      m_sphMirrors[Rich::Rich2][Rich::right][smr] =
        m_sphMirrors[Rich::Rich2][Rich::left][smr];
    m_maxMirror[Rich::Rich2][Rich::right][sph] =
      m_maxMirror[Rich::Rich2][Rich::left][sph];
    warning() << "No sph mirrors on right side of Rich2; copying from left side"
              << endmsg;
  }

  debug() << "Found " << m_maxMirror[Rich::Rich2][Rich::left][sph] +
    m_maxMirror[Rich::Rich2][Rich::right][sph] << " spherical and "
          << m_maxMirror[Rich::Rich2][Rich::left][sec] +
    m_maxMirror[Rich::Rich2][Rich::right][sec] << " sec mirrors in Rich2"
          << endmsg;

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Stored " << m_maxMirror[Rich::Rich1][Rich::top][sph]
              << " and " << m_maxMirror[Rich::Rich1][Rich::bottom][sph]
              << " spherical mirrors in the top and bottom of Rich1, and "
              << m_maxMirror[Rich::Rich1][Rich::top][sec] << " and "
              << m_maxMirror[Rich::Rich1][Rich::bottom][sec] << " secondary mirrors"
              << endreq;
    verbose() << "Stored " << m_maxMirror[Rich::Rich2][Rich::left][sph]
              << " and " << m_maxMirror[Rich::Rich2][Rich::right][sph]
              << " spherical mirrors in the left and right of Rich2, and "
              << m_maxMirror[Rich::Rich2][Rich::left][sec] << " and "
              << m_maxMirror[Rich::Rich2][Rich::right][sec] << " secondary mirrors"
              << endreq;

    {for ( unsigned int r=0; r<2; ++r )
      for ( unsigned int s=0; s<2; ++s )
        for ( unsigned int num=0; num<m_maxMirror[r][s][sph]; ++num )
          verbose() << "Stored spherical mirror "
                    << m_sphMirrors[r][s][num]->name() << endreq;
    }
    {for ( unsigned int r=0; r<2; ++r )
      for ( unsigned int s=0; s<2; ++s )
        for ( unsigned int num=0; num<m_maxMirror[r][s][sec]; ++num )
          verbose() << "Stored secondary mirror "
                    << m_secMirrors[r][s][num]->name() << endreq;
    }

  }

  return sc;
}

//=========================================================================
//  finalize
//=========================================================================
StatusCode RichMirrorSegFinder::finalize( )
{

  // if test was performed, printout final max distances
  if ( m_testFinding )
  {
    always() << "Rich1SphericalMaxDist = " << m_maxDist[Rich::Rich1][sph] << endreq;
    always() << "Rich2SphericalMaxDist = " << m_maxDist[Rich::Rich2][sph] << endreq;
    always() << "Rich1SecondaryMaxDist = " << m_maxDist[Rich::Rich1][sec] << endreq;
    always() << "Rich2SecondaryMaxDist = " << m_maxDist[Rich::Rich2][sec] << endreq;
  }

  return RichToolBase::finalize();
}

//=========================================================================
//  find spherical mirror segment and return pointer
//=========================================================================
const DeRichSphMirror*
RichMirrorSegFinder::findSphMirror( const Rich::DetectorType rich,
                                    const Rich::Side side,
                                    const Gaudi::XYZPoint& reflPoint ) const
{

  // Most likely mirror is the last one found... So test this one first
  unsigned int mirrorNum = m_lastFoundMirror[rich][side][sph];
  const double dist2 = (m_sphMirrors[rich][side][mirrorNum]->mirrorCentre()-reflPoint).Mag2();
  if ( dist2 > m_maxDist[rich][sph] )
  {
    mirrorNum = fullSphSearch(rich,side,reflPoint);
  }
  else if ( m_testFinding )
  {
    const unsigned int test_num = fullSphSearch(rich,side,reflPoint);
    if ( test_num != mirrorNum )
    {
      m_maxDist[rich][sph] *= m_tuneScale;
      info() << "Decreasing " << rich << " spherical parameter to " << m_maxDist[rich][sph] << endreq;
      mirrorNum = test_num;
    }
  }

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Found primary mirror " << mirrorNum << endreq;

  // return found mirror
  return m_sphMirrors[rich][side][mirrorNum];
}

unsigned int
RichMirrorSegFinder::fullSphSearch( const Rich::DetectorType rich,
                                    const Rich::Side side,
                                    const Gaudi::XYZPoint& reflPoint ) const
{
  unsigned int mirrorNum = 0;
  double distance2 ( 1e99 ); // units are mm^2
  for ( unsigned int i = 0; i < m_maxMirror[rich][side][sph]; ++i )
  {
    const double temp_d2 =
      (m_sphMirrors[rich][side][i]->mirrorCentre()-reflPoint).Mag2();
    if ( temp_d2 < distance2 )
    {
      // Found new closest mirror, so update number
      mirrorNum = i;
      // if within tolerance, abort tests on other mirrors
      if ( !m_testFinding && temp_d2 < m_maxDist[rich][sph] ) break;
      // ... otherwise, update minimum distance and continue
      distance2 = temp_d2;
    }
  }

  // update last found mirror and return
  return m_lastFoundMirror[rich][side][sph] = mirrorNum;
}

//=========================================================================
//  find secondary mirror segment and return pointer
//=========================================================================
const DeRichSphMirror*
RichMirrorSegFinder::findSecMirror( const Rich::DetectorType rich,
                                    const Rich::Side side,
                                    const Gaudi::XYZPoint& reflPoint ) const
{

  // Most likely mirror is the last one found... So test this one first
  unsigned int mirrorNum = m_lastFoundMirror[rich][side][sec];
  const double dist2 = (m_secMirrors[rich][side][mirrorNum]->mirrorCentre()-reflPoint).Mag2();
  if ( dist2 > m_maxDist[rich][sec] )
  {
    mirrorNum = fullSecSearch(rich,side,reflPoint);
  }
  else if ( m_testFinding )
  {
    const unsigned int test_num = fullSecSearch(rich,side,reflPoint);
    if ( test_num != mirrorNum )
    {
      m_maxDist[rich][sec] *= m_tuneScale;
      info() << "Decreasing " << rich << " secondary parameter to " << m_maxDist[rich][sec] << endreq;
      mirrorNum = test_num;
    }
  }

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Found secondary mirror " << mirrorNum << endreq;

  // return found mirror
  return m_secMirrors[rich][side][mirrorNum];

}

unsigned int
RichMirrorSegFinder::fullSecSearch( const Rich::DetectorType rich,
                                    const Rich::Side side,
                                    const Gaudi::XYZPoint& reflPoint ) const
{
  unsigned int mirrorNum = 0;
  double distance2 ( 1e99 ); // units are mm^2
  for ( unsigned int i = 0; i < m_maxMirror[rich][side][sec]; ++i )
  {
    const double temp_d2 =
      (m_secMirrors[rich][side][i]->mirrorCentre()-reflPoint).Mag2();
    if ( temp_d2 < distance2 )
    {
      // Found new closest mirror, so update number
      mirrorNum = i;
      // if within tolerance, abort tests on other mirrors
      if ( !m_testFinding && temp_d2 < m_maxDist[rich][sec] ) break;
      // ... otherwise, update minimum distance and continue
      distance2 = temp_d2;
    }
  }

  // update last found mirror and return
  return m_lastFoundMirror[rich][side][sec] = mirrorNum;
}


//-----------------------------------------------------------------------------
/** @file RichMirrorSegFinder.cpp
 *
 * Implementation file for class : RichMirrorSegFinder
 *
 * CVS Log :-
 * $Id: RichMirrorSegFinder.cpp,v 1.11 2005-10-13 16:11:07 jonrob Exp $
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

  debug() << "Initialise RichMirrorSegFinder" << endreq;
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

  // should come from XML or job options ?? (this is in mm^2)
  m_maxDist[Rich::Rich1][sph]  = 41271;
  m_maxDist[Rich::Rich1][flat] = 34297;
  m_maxDist[Rich::Rich2][sph]  = 49414;
  m_maxDist[Rich::Rich2][flat] = 36675;

  // get the RICH detectors
  const DeRich* rich1 = getDet<DeRich>( DeRichLocation::Rich1 );
  const DeRich* rich2 = getDet<DeRich>( DeRichLocation::Rich2 );

  bool foundFlatMirrors( false );
  bool foundSecMirrors( false );

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
      // try to get it as a spherical (secondary) mirror
      if( existDet<DeRichSphMirror>( detName ) ) 
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
        foundSecMirrors = true;
      }
      else 
      {
        // try to get it as a flat mirror
        const DeRichFlatMirror* fm = getDet<DeRichFlatMirror>( detName );
        if ( fm->mirrorCentre().y() > 0.0 )
        {
          mirrorNum = m_maxMirror[Rich::Rich1][Rich::top][flat];
          m_flatMirrors[Rich::Rich1][Rich::top][mirrorNum] = fm;
          ++m_maxMirror[Rich::Rich1][Rich::top][flat];
        }
        else
        {
          mirrorNum = m_maxMirror[Rich::Rich1][Rich::bottom][flat];
          m_flatMirrors[Rich::Rich1][Rich::bottom][mirrorNum] = fm;
          ++m_maxMirror[Rich::Rich1][Rich::bottom][flat];
        }
        foundFlatMirrors = true;
      }
    }

  }}

  if( foundSecMirrors && foundFlatMirrors )
    return Error( "Found mixture of flat and spherical secondary mirrors in Rich1" );
  if( !foundSecMirrors && !foundFlatMirrors )
    return Error( "Found no flat or spherical seconday mirrors in Rich1" );

  if( foundSecMirrors ) 
    debug() << "Rich1 has secondary spherical mirrors" << endmsg;
  else
    debug() << "Rich1 has flat mirrors" << endmsg;

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

    if ( detName.find("FlatMirror") != std::string::npos )
    {
      const DeRichFlatMirror* fm = getDet<DeRichFlatMirror>( detName );
      if ( fm->mirrorCentre().x() > 0.0 )
      {
        mirrorNum = m_maxMirror[Rich::Rich2][Rich::left][flat];
        m_flatMirrors[Rich::Rich2][Rich::left][mirrorNum] = fm;
        ++m_maxMirror[Rich::Rich2][Rich::left][flat];
      }
      else
      {
        mirrorNum = m_maxMirror[Rich::Rich2][Rich::right][flat];
        m_flatMirrors[Rich::Rich2][Rich::right][mirrorNum] = fm;
        ++m_maxMirror[Rich::Rich2][Rich::right][flat];
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

  debug() << "Stored " << m_maxMirror[Rich::Rich1][Rich::top][sph]
          << " and " << m_maxMirror[Rich::Rich1][Rich::bottom][sph]
          << " spherical mirrors in the top and bottom of Rich1, and "
          << m_maxMirror[Rich::Rich1][Rich::top][sec] << " and "
          << m_maxMirror[Rich::Rich1][Rich::bottom][sec] << " flat/secondary mirrors"
          << endreq;
  debug() << "Stored " << m_maxMirror[Rich::Rich2][Rich::left][sph]
          << " and " << m_maxMirror[Rich::Rich2][Rich::right][sph]
          << " spherical mirrors in the left and right of Rich2, and "
          << m_maxMirror[Rich::Rich2][Rich::left][sec] << " and "
          << m_maxMirror[Rich::Rich2][Rich::right][sec] << " flat/secondary mirrors"
          << endreq;

  if ( msgLevel(MSG::VERBOSE) )
  {
    {for ( unsigned int r=0; r<2; ++r )
      for ( unsigned int s=0; s<2; ++s )
        for ( unsigned int num=0; num<m_maxMirror[r][s][sph]; ++num )
          verbose() << "Stored spherical mirror "
                  << m_sphMirrors[r][s][num]->name() << endreq;
    }
    //     {for ( unsigned int r=0; r<2; ++r )
    //       for ( unsigned int s=0; s<2; ++s )
    //         for ( unsigned int num=0; num<m_maxMirror[r][s][flat]; ++num )
    //           verbose() << "Stored flat mirror "
    //                     << m_flatMirrors[r][s][num]->name() << endreq;
    //     }
    //     {for ( unsigned int r=0; r<2; ++r )
    //       for ( unsigned int s=0; s<2; ++s )
    //         for ( unsigned int num=0; num<m_maxMirror[r][s][sec]; ++num )
    //           verbose() << "Stored secondary mirror "
    //                     << m_secMirrors[r][s][num]->name() << endreq;
    //     }

  }

  return sc;
}

//=========================================================================
//  finalize
//=========================================================================
StatusCode RichMirrorSegFinder::finalize( )
{
  return RichToolBase::finalize();
}

//=========================================================================
//  find spherical mirror segment and return pointer
//=========================================================================
const DeRichSphMirror*
RichMirrorSegFinder::findSphMirror( const Rich::DetectorType rich,
                                    const Rich::Side side,
                                    const HepPoint3D& reflPoint ) const
{

  // Most likely mirror is the last one found... So test this one first
  unsigned int mirrorNum = m_lastFoundMirror[rich][side][sph];
  if ( m_sphMirrors[rich][side][mirrorNum]->mirrorCentre().distance2(reflPoint)
       > m_maxDist[rich][sph] )
  {

    // else... search through mirrors
    double distance2 ( 1e99 ); // units are mm^2
    for ( unsigned int i = 0; i < m_maxMirror[rich][side][sph]; ++i )
    {
      const double temp_d2 =
        m_sphMirrors[rich][side][i]->mirrorCentre().distance2(reflPoint);
      if ( temp_d2 < distance2 )
      {
        // Found new closest mirror, so update number
        mirrorNum = i;
        // if within tolerance, abort tests on other mirrors
        if ( temp_d2 < m_maxDist[rich][sph] ) break;
        // ... otherwise, update minimum distance and continue
        distance2 = temp_d2;
      }
    }

    // update last found mirror
    m_lastFoundMirror[rich][side][sph] = mirrorNum;

  }

  // return found mirror
  return m_sphMirrors[rich][side][mirrorNum];
}

//=========================================================================
//  find flat mirror segment and return pointer
//=========================================================================
const DeRichFlatMirror*
RichMirrorSegFinder::findFlatMirror( const Rich::DetectorType rich,
                                     const Rich::Side side,
                                     const HepPoint3D& reflPoint ) const
{

  // Most likely mirror is the last one found... So test this one first
  unsigned int mirrorNum = m_lastFoundMirror[rich][side][flat];
  if ( m_flatMirrors[rich][side][mirrorNum]->mirrorCentre().distance2(reflPoint)
       > m_maxDist[rich][flat] )
  {

    // else... search through mirrors
    double distance2 ( 1e99 ); // units are mm^2
    for ( unsigned int i = 0; i < m_maxMirror[rich][side][flat]; ++i )
    {
      const double temp_d2 =
        m_flatMirrors[rich][side][i]->mirrorCentre().distance2(reflPoint);
      if ( temp_d2 < distance2 )
      {
        // Found new closest mirror, so update number
        mirrorNum = i;
        // if within tolerance, abort tests on other mirrors
        if ( temp_d2 < m_maxDist[rich][flat] ) break;
        // ... otherwise, update minimum distance and continue
        distance2 = temp_d2;
      }
    }

    // update last found mirror
    m_lastFoundMirror[rich][side][flat] = mirrorNum;

  }

  // return found mirror
  return m_flatMirrors[rich][side][mirrorNum];
}

//=========================================================================
//  find secondary mirror segment and return pointer
//=========================================================================
const DeRichSphMirror*
RichMirrorSegFinder::findSecMirror( const Rich::DetectorType rich,
                                    const Rich::Side side,
                                    const HepPoint3D& reflPoint ) const
{

  // Most likely mirror is the last one found... So test this one first
  unsigned int mirrorNum = m_lastFoundMirror[rich][side][sec];
  if ( m_secMirrors[rich][side][mirrorNum]->mirrorCentre().distance2(reflPoint)
       > m_maxDist[rich][sec] )
  {

    // else... search through mirrors
    double distance2 ( 1e99 ); // units are mm^2
    for ( unsigned int i = 0; i < m_maxMirror[rich][side][sec]; ++i )
    {
      const double temp_d2 =
        m_secMirrors[rich][side][i]->mirrorCentre().distance2(reflPoint);
      if ( temp_d2 < distance2 )
      {
        // Found new closest mirror, so update number
        mirrorNum = i;
        // if within tolerance, abort tests on other mirrors
        if ( temp_d2 < m_maxDist[rich][sec] ) break;
        // ... otherwise, update minimum distance and continue
        distance2 = temp_d2;
      }
    }

    // update last found mirror
    m_lastFoundMirror[rich][side][sec] = mirrorNum;

  }

  // return found mirror
  return m_secMirrors[rich][side][mirrorNum];
}

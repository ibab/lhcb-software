// $Id: RichMirrorSegFinder.cpp,v 1.4 2004-06-21 13:39:15 jonesc Exp $

// local
#include "RichMirrorSegFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichMirrorSegFinder
//
// 2003-11-05 : Antonis Papanestis
//-----------------------------------------------------------------------------

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

  declareInterface<IRichMirrorSegFinder>(this);

  for (int i=0; i<2; ++i)
    for (int j=0; j<2; ++j)
      for (int k=0; k<2; ++k)
        m_lastMirror[i][j][k] = 0;

}

//=============================================================================
// Destructor
//=============================================================================
RichMirrorSegFinder::~RichMirrorSegFinder() { }

//=========================================================================
//  initialization
//=========================================================================
StatusCode RichMirrorSegFinder::initialize( ) {

  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get the RICH detectors
  DeRich1 * rich1 = getDet<DeRich1>( DeRich1Location::Default );
  DeRich2 * rich2 = getDet<DeRich2>( DeRich2Location::Default );

  // find all the mirrors in Rich1
  unsigned int mirrorNum(0);
  IDetectorElement::IDEContainer & detelemsR1 = rich1->childIDetectorElements();
  {for ( IDetectorElement::IDEContainer::iterator det_it =  detelemsR1.begin();
         det_it != detelemsR1.end();
         ++det_it ) {
    const std::string & detName = (*det_it)->name();

    if ( detName.find("Mirror1") != std::string::npos ) {
      DeRichSphMirror * sm = getDet<DeRichSphMirror>( detName );
      if ( sm->mirrorCentre().y() > 0.0 ) {
        mirrorNum = m_lastMirror[Rich::Rich1][Rich::top][sph];
        m_sphMirrors[Rich::Rich1][Rich::top][mirrorNum] = sm;
        ++m_lastMirror[Rich::Rich1][Rich::top][sph];
      }
      else {
        mirrorNum = m_lastMirror[Rich::Rich1][Rich::bottom][sph];
        m_sphMirrors[Rich::Rich1][Rich::bottom][mirrorNum] = sm;
        ++m_lastMirror[Rich::Rich1][Rich::bottom][sph];
      }
    }

    if ( detName.find("Mirror2") != std::string::npos ) {
      DeRichFlatMirror * fm = getDet<DeRichFlatMirror>( detName );
      if ( fm->mirrorCentre().y() > 0.0 ) {
        mirrorNum = m_lastMirror[Rich::Rich1][Rich::top][flat];
        m_flatMirrors[Rich::Rich1][Rich::top][mirrorNum] = fm;
        ++m_lastMirror[Rich::Rich1][Rich::top][flat];
      }
      else {
        mirrorNum = m_lastMirror[Rich::Rich1][Rich::bottom][flat];
        m_flatMirrors[Rich::Rich1][Rich::bottom][mirrorNum] = fm;
        ++m_lastMirror[Rich::Rich1][Rich::bottom][flat];
      }
    }
  }}

  // find all the mirrors in Rich2
  IDetectorElement::IDEContainer & detelemsR2 = rich2->childIDetectorElements();
  {for ( IDetectorElement::IDEContainer::iterator det_it = detelemsR2.begin();
         det_it != detelemsR2.end();
         ++det_it ) {
    const std::string & detName = (*det_it)->name();

    if ( detName.find("SphMirror") != std::string::npos ) {
      DeRichSphMirror * sm = getDet<DeRichSphMirror>( detName );
      if ( sm->centreOfCurvature().x() > 0.0 ) {
        mirrorNum = m_lastMirror[Rich::Rich2][Rich::left][sph];
        m_sphMirrors[Rich::Rich2][Rich::left][mirrorNum] = sm;
        ++m_lastMirror[Rich::Rich2][Rich::left][sph];
      }
      else {
        mirrorNum = m_lastMirror[Rich::Rich2][Rich::right][sph];
        m_sphMirrors[Rich::Rich2][Rich::right][mirrorNum] = sm;
        ++m_lastMirror[Rich::Rich2][Rich::right][sph];
      }
    }

    if ( detName.find("FlatMirror") != std::string::npos ) {
      DeRichFlatMirror * fm = getDet<DeRichFlatMirror>( detName );
      if ( fm->mirrorCentre().x() > 0.0 ) {
        mirrorNum = m_lastMirror[Rich::Rich2][Rich::left][flat];
        m_flatMirrors[Rich::Rich2][Rich::left][mirrorNum] = fm;
        ++m_lastMirror[Rich::Rich2][Rich::left][flat];
      }
      else {
        mirrorNum = m_lastMirror[Rich::Rich2][Rich::right][flat];
        m_flatMirrors[Rich::Rich2][Rich::right][mirrorNum] = fm;
        ++m_lastMirror[Rich::Rich2][Rich::right][flat];
      }
    }
  }}

  if ( msgLevel(MSG::DEBUG) ) {

    {for (unsigned int r=0; r<2; ++r)
      for (unsigned int s=0; s<2; ++s)
        for (unsigned int num=0; num<m_lastMirror[r][s][sph]; ++num)
          debug() << "Stored spherical mirror "
                  << m_sphMirrors[r][s][num]->name() << endreq;
    }
    {for (unsigned int r=0; r<2; ++r)
      for (unsigned int s=0; s<2; ++s)
        for (unsigned int num=0; num<m_lastMirror[r][s][flat]; ++num)
          debug() << "Stored flat mirror "
                  <<m_flatMirrors[r][s][num]->name() << endreq;
    }
    debug() << "Stored " << m_lastMirror[Rich::Rich1][Rich::top][sph]
            << " and " << m_lastMirror[Rich::Rich1][Rich::bottom][sph]
            << " spherical mirrors in the top and bottom of Rich1, and "
            << m_lastMirror[Rich::Rich1][Rich::top][flat] << " and "
            << m_lastMirror[Rich::Rich1][Rich::bottom][flat] << " flat mirrors"
            << endreq;
    debug() << "Stored " << m_lastMirror[Rich::Rich2][Rich::left][sph]
            << " and " << m_lastMirror[Rich::Rich2][Rich::right][sph]
            << " spherical mirrors in the left and right of Rich2, and "
            << m_lastMirror[Rich::Rich2][Rich::left][flat] << " and "
            << m_lastMirror[Rich::Rich2][Rich::right][flat] << " flat mirrors"
            << endreq;

  }

  return StatusCode::SUCCESS;
}


//=========================================================================
//  finalize
//=========================================================================
StatusCode RichMirrorSegFinder::finalize( )
{
  debug() << "Finalize" << endreq;

  return RichToolBase::finalize();
}

//=========================================================================
//  find sph mirror segment and return pointer
//=========================================================================
DeRichSphMirror*
RichMirrorSegFinder::findSphMirror( const Rich::DetectorType rich,
                                    const Rich::Side side,
                                    const HepPoint3D& reflPoint ) const
{
  double distance2(1e6);
  unsigned int mirrorNum(0);

  for ( unsigned int i=0; i<m_lastMirror[rich][side][sph]; ++i ) {
    const double temp_d2 =
      m_sphMirrors[rich][side][i]->mirrorCentre().distance2(reflPoint);
    if ( temp_d2 < distance2 ) {
      distance2 = temp_d2;
      mirrorNum = i;
    }
  }

  return m_sphMirrors[rich][side][mirrorNum];
}

//=========================================================================
//  find flat mirror segment and return pointer
//=========================================================================
DeRichFlatMirror*
RichMirrorSegFinder::findFlatMirror( const Rich::DetectorType rich,
                                     const Rich::Side side,
                                     const HepPoint3D& reflPoint ) const
{
  double distance2(1e6);
  unsigned int mirrorNum(0);

  for ( unsigned int i=0; i<m_lastMirror[rich][side][flat]; ++i ) {
    const double temp_d2 =
      m_flatMirrors[rich][side][i]->mirrorCentre().distance2(reflPoint);
    if ( temp_d2 < distance2 ) {
      distance2 = temp_d2;
      mirrorNum = i;
    }
  }

  return m_flatMirrors[rich][side][mirrorNum];
}

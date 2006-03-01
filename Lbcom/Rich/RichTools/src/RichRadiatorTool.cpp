// $Id: RichRadiatorTool.cpp,v 1.1 2006-03-01 17:12:25 papanest Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichRadiatorTool.h"

#include "RichDet/DeRich.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRadiatorTool
//
// 2006-03-01 : Antonis Papanestis
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichRadiatorTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichRadiatorTool::RichRadiatorTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IRichRadiatorTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
RichRadiatorTool::~RichRadiatorTool() {}

//=============================================================================


//=========================================================================
// initialize
//=========================================================================
StatusCode RichRadiatorTool::initialize ( ) {

  m_radiators[Rich::C4F10].push_back( getDet<DeRichRadiator>(DeRichRadiatorLocation::C4F10) );
  m_radiators[Rich::CF4].push_back( getDet<DeRichRadiator>(DeRichRadiatorLocation::CF4) );

  DetectorElement* rich1 = getDet<DetectorElement>(DeRichLocation::Rich1);

  const IDetectorElement::IDEContainer& detelemsR1 = rich1->childIDetectorElements();
  for ( IDetectorElement::IDEContainer::const_iterator det_it =  detelemsR1.begin();
        det_it != detelemsR1.end();
        ++det_it )
  {
    const std::string& detName = (*det_it)->name();

    if ( detName.find("AerogelT") != std::string::npos )
      m_radiators[Rich::Aerogel].push_back( getDet<DeRichRadiator>( detName ));
  }

  debug() << "Using the following DeRichRadiators:" << endmsg;

  for (unsigned int i=0; i<m_radiators.size(); ++i)
    for (unsigned int j=0; j<m_radiators[i].size(); ++j)
      debug() << m_radiators[i][j]->name() << endmsg;

  return StatusCode::SUCCESS;

}

//=========================================================================
//  intersections
//=========================================================================
unsigned int RichRadiatorTool::intersections( const Gaudi::XYZPoint& globalPoint,
                                              const Gaudi::XYZVector& globalVector,
                                              Rich::RadiatorType radiator,
                                              std::vector<RichRadIntersection>&
                                              intersections ) const
{

  unsigned int totalIntersections( 0 );
  Gaudi::XYZPoint entry;
  Gaudi::XYZPoint exit;

  for (unsigned int i=0; i<m_radiators[radiator].size(); ++i) {
    if ( m_radiators[radiator][i]->intersectionPoints( globalPoint,
                                                       globalVector,
                                                       entry,
                                                       exit ) ) {
      intersections.push_back( RichRadIntersection( entry, exit, m_radiators[radiator][i] ));
      ++totalIntersections;
    }
  }

  return totalIntersections;

}

//=========================================================================

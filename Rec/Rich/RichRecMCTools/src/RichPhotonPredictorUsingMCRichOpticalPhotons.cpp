
//-----------------------------------------------------------------------------
/** @file RichPhotonPredictorUsingMCRichOpticalPhotons.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichPhotonPredictorUsingMCRichOpticalPhotons
 *
 *  CVS Log :-
 *  $Id: RichPhotonPredictorUsingMCRichOpticalPhotons.cpp,v 1.3 2006-01-23 14:09:59 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonPredictorUsingMCRichOpticalPhotons.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonPredictorUsingMCRichOpticalPhotons>          s_factory ;
const        IToolFactory& RichPhotonPredictorUsingMCRichOpticalPhotonsFactory = s_factory ;

// Standard constructor
RichPhotonPredictorUsingMCRichOpticalPhotons::
RichPhotonPredictorUsingMCRichOpticalPhotons( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_mcRecTool     ( 0 )
{
  declareInterface<IRichPhotonPredictor>(this);
}

StatusCode RichPhotonPredictorUsingMCRichOpticalPhotons::initialize()
{
  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_mcRecTool );

  return StatusCode::SUCCESS;
}

StatusCode RichPhotonPredictorUsingMCRichOpticalPhotons::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// fast decision on whether a photon is possible
bool RichPhotonPredictorUsingMCRichOpticalPhotons::
photonPossible( RichRecSegment * segment,
                RichRecPixel * pixel ) const {

  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() != pixel->detector() ) return false;

  // Check if this combination due to a true cherenkov hit using MC
  const MCRichOpticalPhoton * mcPhoton = m_mcRecTool->trueOpticalPhoton(segment,pixel);
  return ( mcPhoton ? true : false );
}


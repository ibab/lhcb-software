
/** @file RichRecGeomTool.cpp
 *
 *  Implementation file for tool : RichRecGeomTool
 *
 *  CVS Log :-
 *  $Id: RichRecGeomTool.cpp,v 1.2 2004-07-27 20:15:32 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// local
#include "RichRecGeomTool.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecGeomTool>          s_factory ;
const        IToolFactory& RichRecGeomToolFactory = s_factory ;

// Standard constructor
RichRecGeomTool::RichRecGeomTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichRecToolBase( type, name, parent ) 
{
  declareInterface<IRichRecGeomTool>(this);
}

StatusCode RichRecGeomTool::initialize()
{
  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}

StatusCode RichRecGeomTool::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichRecGeomTool::trackPixelHitSep2Global( const RichRecSegment * segment,
                                                 const RichRecPixel * pixel ) const
{

  if ( Rich::Rich1 == segment->trackSegment().rich() ) {

    if ( pixel->globalPosition().y() * segment->pdPanelHitPoint().y() > 0 ) {
      return pixel->globalPosition().distance2( segment->pdPanelHitPoint() );
    } else if ( ( pixel->globalPosition().y() > 0 && segment->photonsInYPlus() ) ||
                ( pixel->globalPosition().y() < 0 && segment->photonsInYMinus() ) ) {
      return segment->pdPanelHitPoint().distance2( HepPoint3D( pixel->globalPosition().x(),
                                                               -pixel->globalPosition().y(),
                                                               pixel->globalPosition().z() ) );
    }

  } else if ( Rich::Rich2 == segment->trackSegment().rich() ) {

    if ( pixel->globalPosition().x() * segment->pdPanelHitPoint().x() > 0 ) {
      return pixel->globalPosition().distance2( segment->pdPanelHitPoint() );
    } else if ( ( pixel->globalPosition().x() > 0 && segment->photonsInXPlus()  ) ||
                ( pixel->globalPosition().x() < 0 && segment->photonsInXMinus() ) ) {
      return segment->pdPanelHitPoint().distance2( HepPoint3D( -pixel->globalPosition().x(),
                                                               pixel->globalPosition().y(),
                                                               pixel->globalPosition().z() ) );
    }

  }

  return 99999999.9;
}

double RichRecGeomTool::trackPixelHitSep2Local( const RichRecSegment * segment,
                                                const RichRecPixel * pixel ) const
{

  //return segment->pdPanelHitPointLocal().distance2( pixel->localPosition() );

  if ( Rich::Rich1 == segment->trackSegment().rich() ) {

    if ( pixel->localPosition().y() * segment->pdPanelHitPointLocal().y() > 0 ) {
      return pixel->localPosition().distance2( segment->pdPanelHitPointLocal() );
    } else if ( ( pixel->localPosition().y() > 0 && segment->photonsInYPlus() ) ||
                ( pixel->localPosition().y() < 0 && segment->photonsInYMinus() ) ) {
      return segment->pdPanelHitPointLocal().distance2( HepPoint3D( pixel->localPosition().x(),
                                                                    -pixel->localPosition().y(),
                                                                    pixel->localPosition().z() ) );
    }

  } else if ( Rich::Rich2 == segment->trackSegment().rich() ) {

    if ( pixel->localPosition().x() * segment->pdPanelHitPointLocal().x() > 0 ) {
      return pixel->localPosition().distance2( segment->pdPanelHitPointLocal() );
    } else if ( ( pixel->localPosition().x() > 0 && segment->photonsInXPlus()  ) ||
                ( pixel->localPosition().x() < 0 && segment->photonsInXMinus() ) ) {
      return segment->pdPanelHitPointLocal().distance2( HepPoint3D( -pixel->localPosition().x(),
                                                                    pixel->localPosition().y(),
                                                                    pixel->localPosition().z() ) );
    }

  }

  return 99999999.9;
}

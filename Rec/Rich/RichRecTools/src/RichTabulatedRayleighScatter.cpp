// $Id: RichTabulatedRayleighScatter.cpp,v 1.4 2004-04-19 23:06:16 jonesc Exp $

// local
#include "RichTabulatedRayleighScatter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichTabulatedRayleighScatter
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedRayleighScatter>          s_factory ;
const        IToolFactory& RichTabulatedRayleighScatterFactory = s_factory ;

// Standard constructor
RichTabulatedRayleighScatter::RichTabulatedRayleighScatter ( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_rayScatL( 0 ) {

  declareInterface<IRichRayleighScatter>(this);

  declareProperty( "RayScatLengthLocation",
                   m_rayScatLoc = "/dd/Materials/RichMaterialTabProperties/AerogelRayleighPT" );

}

StatusCode RichTabulatedRayleighScatter::initialize() {

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  SmartDataPtr<TabulatedProperty> tab( detSvc(), m_rayScatLoc );
  if ( !tab ) {
    return Error( "Failed to load Rayleigh scattering length from "+m_rayScatLoc );
  }
  m_rayScatL = new Rich1DTabProperty( tab );

  return StatusCode::SUCCESS;
}

StatusCode RichTabulatedRayleighScatter::finalize() 
{
  // clean up
  if ( m_rayScatL ) { delete m_rayScatL; m_rayScatL = 0; }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichTabulatedRayleighScatter::photonScatteredProb( const RichRecSegment * segment,
                                                   const double energy ) const 
{

  // check this is aerogel
  if ( Rich::Aerogel != segment->trackSegment().radiator() ) return 0;

  // check energy is valid
  if ( energy <= 0 ) return 0;

  // check path length is valid
  const double path = segment->trackSegment().pathLength();
  if ( path <= 0 ) return 0;

  // compute and return prob
  const double scatLeng = (*m_rayScatL)[energy*eV];
  return ( 1 - (scatLeng/path)*(1.0-exp(-path/scatLeng)) );
}

// $Id: RichDetParameters.cpp,v 1.2 2004-07-12 14:25:02 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

// local
#include "RichDetParameters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichDetParameters
//
// 2004-03-29 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichDetParameters>          s_factory ;
const        IToolFactory& RichDetParametersFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichDetParameters::RichDetParameters( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : RichToolBase ( type, name , parent ),
    m_maxPhotEn  ( Rich::NRadiatorTypes ),
    m_minPhotEn  ( Rich::NRadiatorTypes ),
    m_meanPhotEn ( Rich::NRadiatorTypes )
{

  declareInterface<IRichDetParameters>(this);

  m_maxPhotEn[Rich::Aerogel] = 3.8;
  m_maxPhotEn[Rich::C4F10]   = 7.0;
  m_maxPhotEn[Rich::CF4]     = 7.0;
  declareProperty( "MaxPhotonEnergy", m_maxPhotEn );

  m_minPhotEn[Rich::Aerogel] = 1.75;
  m_minPhotEn[Rich::C4F10]   = 1.75;
  m_minPhotEn[Rich::CF4]     = 1.75;
  declareProperty( "MinPhotonEnergy", m_minPhotEn );

  m_meanPhotEn[Rich::Aerogel] = 2.89;
  m_meanPhotEn[Rich::C4F10]   = 4.29;
  m_meanPhotEn[Rich::CF4]     = 4.34;
  declareProperty( "MeanPhotonEnergy", m_meanPhotEn );

}

StatusCode RichDetParameters::initialize()
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  return StatusCode::SUCCESS;
}

StatusCode RichDetParameters::finalize()
{
  // base class finalize
  return RichToolBase::finalize();
}

double RichDetParameters::maxPhotonEnergy( const Rich::RadiatorType rad ) const
{
  return m_maxPhotEn[rad];
}

double RichDetParameters::minPhotonEnergy( const Rich::RadiatorType rad ) const
{
  return m_minPhotEn[rad];
}

double RichDetParameters::meanPhotonEnergy( const Rich::RadiatorType rad ) const
{
  return m_meanPhotEn[rad];
}


//----------------------------------------------------------------------------
/** @file DeRichRadiator.cpp
 *
 *  Implementation file for detector description class : DeRichRadiator
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#define DERICHRADIATOR_CPP

// Include files
#include "RichDet/DeRichRadiator.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IAlgTool.h"

//----------------------------------------------------------------------------

//=========================================================================
//  default constructor
//=========================================================================
DeRichRadiator::DeRichRadiator(const std::string & name) : DeRichBase(name) { }

//=========================================================================
//  destructor
//=========================================================================
DeRichRadiator::~DeRichRadiator() { }

StatusCode DeRichRadiator::initialize()
{
  // store the name of the radiator
  const std::string::size_type pos = name().find("Rich");
  setMyName( std::string::npos != pos ? name().substr(pos) : "DeRichRadiator_NO_NAME" );

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Initializing Radiator : " << rich() << " " << radiatorID() << endmsg;

  return setRadiatorID();
}

//=========================================================================
// initTabPropInterpolators
//=========================================================================
StatusCode DeRichRadiator::initTabPropInterpolators()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Initialising interpolators" << endmsg;

  if ( m_refIndexTabProp )
  {
    if ( !m_refIndex )
    { m_refIndex.reset( new Rich::TabulatedProperty1D( m_refIndexTabProp ) ); }
    else
    { m_refIndex->initInterpolator( m_refIndexTabProp ); }
    if ( !m_refIndex->valid() )
    {
      error() << "Invalid RINDEX Rich::TabulatedProperty1D for "
              << m_refIndexTabProp->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_rayleighTabProp )
  {
    if ( !m_rayleigh )
    { m_rayleigh.reset( new Rich::TabulatedProperty1D( m_rayleighTabProp ) ); }
    else
    { m_rayleigh->initInterpolator( m_rayleighTabProp ); }
    if ( !m_rayleigh->valid() )
    {
      error() << "Invalid RAYLEIGH Rich::TabulatedProperty1D for "
              << m_rayleighTabProp->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if ( m_absorptionTabProp )
  {
    if ( !m_absorption )
    { m_absorption.reset( new Rich::TabulatedProperty1D( m_absorptionTabProp ) ); }
    else
    { m_absorption->initInterpolator( m_absorptionTabProp ); }
    if ( !m_absorption->valid() )
    {
      error() << "Invalid ABSORPTION Rich::TabulatedProperty1D for "
              << m_absorptionTabProp->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
// generateHltRefIndex
//=========================================================================
const Rich::TabulatedProperty1D* DeRichRadiator::generateHltRefIndex() const
{
  // return normal refractive index
  m_hltRefIndex = m_refIndex;
  return m_hltRefIndex.get();
}

//=========================================================================
// Set RichDetector and name
//=========================================================================
StatusCode DeRichRadiator::setRadiatorID()
{
  if ( exists("IDVector") )
  {
    const auto radID = paramVect<int>("IDVector");
    m_rich = Rich::DetectorType(radID[0]);
    m_radiatorID = Rich::RadiatorType(radID[1]);
    return StatusCode::SUCCESS;
  }
  else
  {

    if      ( std::string::npos != name().find("Rich2") )
    {
      m_radiatorID = Rich::Rich2Gas;
      m_rich = Rich::Rich2;
    }
    else if ( std::string::npos != name().find("Aerogel") )
    {
      m_radiatorID = Rich::Aerogel;
      m_rich = Rich::Rich1;
    }
    else if ( std::string::npos != name().find("Rich1Gas") )
    {
      m_radiatorID = Rich::Rich1Gas;
      m_rich = Rich::Rich1;
    }
    else
    {
      error() << "Cannot find radiator type for " << name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

//=============================================================================

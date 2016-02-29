/** @file DeRich.cpp
 *
 *  Implementation file for detector description class : DeRich
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#include <sstream>

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"

// DetDesc
#include "DetDesc/AlignmentCondition.h"

// local
#include "RichDet/DeRich.h"
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichPMTPanel.h"
#include "RichDet/DeRichPDPanel.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DeRich
//
// 2004-06-30 : Antonis Papanestis
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRich::DeRich( const std::string & name ) : DeRichBase ( name  ) { }

//=============================================================================
// Destructor
//=============================================================================
DeRich::~DeRich() { }

//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRich::initialize ( )
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Initialize " << name() << endmsg;

  if ( exists( "SphMirrorSegRows" ) )
  {
    m_sphMirrorSegRows  = param<int>( "SphMirrorSegRows" );
    m_sphMirrorSegCols  = param<int>( "SphMirrorSegColumns" );
    m_positionInfo      = true;
  }

  if ( exists( "SecMirrorSegRows" ) )
  {
    m_secMirrorSegRows = param<int>( "SecMirrorSegRows" );
    m_secMirrorSegCols = param<int>( "SecMirrorSegColumns" );
  }
  else if ( exists( "FlatMirrorSegRows" ) )
  {
    m_secMirrorSegRows = param<int>( "FlatMirrorSegRows" );
    m_secMirrorSegCols = param<int>( "FlatMirrorSegColumns" );
  }

  if ( exists ( "RichPhotoDetectorConfig" ) )
  {
    m_RichPhotoDetConfig
      = (Rich::RichPhDetConfigType) param<int>( "RichPhotoDetectorConfig" );
  }
  else
  {
    // assume HPDs
    m_RichPhotoDetConfig = Rich::HPDConfig;
  }

  if ( exists("RichGeometryRunConfiguration") )
  {
    m_RichGeometryConfig = param<int>("RichGeometryRunConfiguration");
  }
  else
  {
    // assume current RICH configuration
    m_RichGeometryConfig = 0;
  }

  m_Rich2PhotoDetectorArrayConfig = 0;
  m_Rich2UseGrandPmt              = false;
  m_Rich2UseMixedPmt              = false;

  if ( exists("Rich2PMTArrayConfig") )
  {
    m_Rich2PhotoDetectorArrayConfig = param<int>("Rich2PMTArrayConfig" );
    if ( m_Rich2PhotoDetectorArrayConfig >= 1 )
    {
      m_Rich2UseGrandPmt = true;
      if( m_Rich2PhotoDetectorArrayConfig == 2 ) {   
        m_Rich2UseMixedPmt = true;
      }
    }
  }

  return StatusCode::SUCCESS;
}

//=========================================================================

void DeRich::loadNominalQuantumEff() const
{
  if      ( m_RichPhotoDetConfig == Rich::HPDConfig )
  {
    m_nominalPDQuantumEff.reset( loadNominalHPDQuantumEff() );
  }
  else if ( m_RichPhotoDetConfig == Rich::PMTConfig )
  {
    m_nominalPDQuantumEff.reset( loadNominalPMTQuantumEff() );
  }
}

//=========================================================================

const Rich::TabulatedProperty1D * DeRich::loadNominalHPDQuantumEff() const
{
  const Rich::TabulatedProperty1D * nominalHPDQuantumEff = nullptr;

  // find the HPD quantum efficiency
  std::string HPD_QETabPropLoc;
  if ( exists( "RichHpdQETableName" ) )
  {
    HPD_QETabPropLoc = param<std::string>( "RichHpdQETableName" );
  }
  else
  {
    if ( exists( "RichNominalHpdQuantumEffTableName" ) )
    {
      HPD_QETabPropLoc = param<std::string>( "RichNominalHpdQuantumEffTableName" );
    }
    else
    {
      throw GaudiException( "Cannot find HPD QE location RichNominalHpdQuantumEffTableName",
                            "DeRich", StatusCode::FAILURE );
    }
  }

  SmartDataPtr<TabulatedProperty> tabQE ( dataSvc(), HPD_QETabPropLoc );
  if ( !tabQE )
  {
    throw GaudiException( "No information on HPD Quantum Efficiency",
                          "DeRich", StatusCode::FAILURE );
  }
  else
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Loaded HPD QE from: " << HPD_QETabPropLoc << endmsg;
    nominalHPDQuantumEff = new Rich::TabulatedProperty1D( tabQE );
    if ( !nominalHPDQuantumEff->valid() )
    {
      throw GaudiException( "Invalid HPD QE RichTabulatedProperty1D for " + tabQE->name(),
                            "DeRich", StatusCode::FAILURE );
    }
  }

  return nominalHPDQuantumEff;
}

//=========================================================================

const Rich::TabulatedProperty1D * DeRich::loadNominalPMTQuantumEff() const
{
  const Rich::TabulatedProperty1D * nominalPMTQuantumEff = nullptr;

  // find the PMT quantum efficiency
  std::string PMT_QETabPropLoc;
  if ( exists( "RichPmtQETableName" ) )
  {
    PMT_QETabPropLoc = param<std::string>( "RichPmtQETableName" );
  }
  else
  {
    if ( exists( "RichNominalPmtQuantumEffTableName" ) )
    {
      PMT_QETabPropLoc = param<std::string>( "RichNominalPmtQuantumEffTableName" );
    }
    else
    {
      throw GaudiException( "Cannot find PMT QE location RichNominalPmtQuantumEffTableName",
                            "DeRich", StatusCode::FAILURE );
    }
  }

  SmartDataPtr<TabulatedProperty> tabQE ( dataSvc(), PMT_QETabPropLoc );
  if ( !tabQE )
  {
    throw GaudiException( "No information on PMT Quantum Efficiency",
                          "DeRich", StatusCode::FAILURE );
  }
  else
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Loaded PMT QE from: " << PMT_QETabPropLoc << endmsg;
    nominalPMTQuantumEff = new Rich::TabulatedProperty1D( tabQE );
    if ( !nominalPMTQuantumEff->valid() )
    {
      throw GaudiException( "Invalid PMT QE RichTabulatedProperty1D for " + tabQE->name(),
                            "DeRich", StatusCode::FAILURE );
    }
  }

  return nominalPMTQuantumEff;
}

//=========================================================================

Rich::MirrorSegPosition DeRich::sphMirrorSegPos( const int mirrorNumber ) const
{

  Rich::MirrorSegPosition mirrorPos;

  if ( m_positionInfo )
  {
    int row = mirrorNumber / m_sphMirrorSegCols;
    if ( row >= m_sphMirrorSegRows ) row -= m_sphMirrorSegRows;
    mirrorPos.setRow( row );
    mirrorPos.setColumn( mirrorNumber % m_sphMirrorSegCols );
  }
  else 
  {
    error() << "No position information for mirrors" << endmsg;
  }

  return mirrorPos;
}

//=========================================================================

Rich::MirrorSegPosition DeRich::secMirrorSegPos( const int mirrorNumber ) const
{

  Rich::MirrorSegPosition mirrorPos;

  if ( m_positionInfo )
  {
    int row = mirrorNumber / m_secMirrorSegCols;
    if ( row >= m_secMirrorSegRows ) row -= m_secMirrorSegRows;
    mirrorPos.setRow( row );
    mirrorPos.setColumn( mirrorNumber % m_secMirrorSegCols );
  }
  else
  {
    error() << "No position information for mirrors" << endmsg;
  }

  return mirrorPos;

}

//=========================================================================
// Return a RichSmartID for a particular point in the LHCb coord system
//=========================================================================
int DeRich::sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const
{
  return ( pdPanel(side(globalPoint))->sensitiveVolumeID( globalPoint ) );
}

//=========================================================================
// Access the name for a given panel
//=========================================================================
const std::string DeRich::panelName( const Rich::Side /* panel */ ) const
{
  throw GaudiException( "Not implemented for DeRich class"+myName(),
                        "DeRich::panelName", StatusCode::FAILURE );
}

//=========================================================================
// Access PD Panels on demand
//=========================================================================
DeRichPDPanel * DeRich::pdPanel( const Rich::Side panel ) const
{
  if ( !m_PDPanels[panel] )
  {
    const std::string pName = panelName(panel);

    DeRichPDPanel* phdePanel = nullptr;

    if      ( RichPhotoDetConfig() == Rich::HPDConfig )
    {
      SmartDataPtr<DeRichHPDPanel> aHpdPanel( dataSvc(), pName );
      phdePanel = aHpdPanel;
    }
    else if ( RichPhotoDetConfig() == Rich::PMTConfig )
    {
      SmartDataPtr<DeRichPMTPanel> aPmtPanel( dataSvc(), pName );
      phdePanel = aPmtPanel;
    }

    if ( !phdePanel )
    {
      std::ostringstream mess;
      mess << "Failed to load DeRichPDPanel at " << pName;
      throw GaudiException( mess.str(), "DeRich::PhDetPanel", StatusCode::FAILURE );
    }

    m_PDPanels[panel] = phdePanel;
  }

  return m_PDPanels[panel];
}

//=============================================================================


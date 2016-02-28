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
DeRich::DeRich( const std::string & name )
  : DeRichBase              ( name  ),
    m_PDPanels              ( Rich::NRiches )
{
  m_PDPanels[Rich::Rich1]  = nullptr;
  m_PDPanels[Rich::Rich2]  = nullptr;
}

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

RichMirrorSegPosition DeRich::sphMirrorSegPos( const int mirrorNumber ) const
{

  RichMirrorSegPosition mirrorPos;

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

RichMirrorSegPosition DeRich::secMirrorSegPos( const int mirrorNumber ) const
{

  RichMirrorSegPosition mirrorPos;

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

StatusCode
DeRich::alignMirrors ( const std::vector<const ILVolume*>& mirrorContainers,
                       const std::string& mirrorID,
                       const SmartRef<Condition>& mirrorAlignCond,
                       const std::string& Rvector ) const
{

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Misaligning " << mirrorID << " in " << myName() << endmsg;

  std::map<int, IPVolume*> mirrors;

  for ( std::vector<const ILVolume*>::const_iterator contIter = mirrorContainers.begin();
        contIter != mirrorContainers.end(); ++contIter )
  {
    for ( unsigned int i=0; i<(*contIter)->noPVolumes(); ++i )
    {
      const IPVolume* cpvMirror = (*contIter)->pvolume(i);
      IPVolume* pvMirror = const_cast<IPVolume*>(cpvMirror);

      // find mirrors that match mirrorID
      const std::string mirrorName = pvMirror->name();
      if ( mirrorName.find(mirrorID) != std::string::npos )
      {
        const std::string::size_type mpos = mirrorName.find(':');
        if ( std::string::npos == mpos )
        {
          fatal() << "A mirror without a number!" << endmsg;
          return StatusCode::FAILURE;
        }
        const int mirrorNumber = atoi( mirrorName.substr(mpos+1).c_str() );
        mirrors[mirrorNumber] = pvMirror;
      }
    }
  }
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Found " << mirrors.size() << " mirrors" << endmsg;

  const std::vector<double>& rotX = mirrorAlignCond->paramVect<double>("RichMirrorRotX");
  const std::vector<double>& rotY = mirrorAlignCond->paramVect<double>("RichMirrorRotY");

  // make sure the numbers match
  if ( rotX.size() != rotY.size() )
  {
    fatal() << "Mismatch in X and Y rotations in Condition:"
            << mirrorAlignCond->name() << endmsg;
    return StatusCode::FAILURE;
  }
  if ( rotX.size() != mirrors.size() )
  {
    fatal() << "Number of parameters does not match mirrors in:"
            << mirrorAlignCond->name() << endmsg;
    return StatusCode::FAILURE;
  }

  const std::vector<double> & Rs = paramVect<double>(Rvector);
  if ( rotX.size() != Rs.size() )
  {
    fatal() << "Number of Rs does not match mirrors in:"
            << mirrorAlignCond->name() << endmsg;
    return StatusCode::FAILURE;
  }

  for ( unsigned int mNum=0; mNum<rotX.size(); ++mNum )
  {
    // create transformation matrix to rotate the mirrors around their centre
    Gaudi::TranslationXYZ pivot( -Rs[mNum], 0.0, 0.0 );
    Gaudi::Transform3D transl( pivot );
    // the x rotation is around z because of the way the mirror is created
    Gaudi::Rotation3D rot =  Gaudi::RotationZ(rotX[mNum]) * Gaudi::RotationY(rotY[mNum]);
    Gaudi::Transform3D matrixInv = transl.Inverse() * (Gaudi::Transform3D(rot) * transl);
    Gaudi::Transform3D matrix( matrixInv.Inverse() );
    // Apply the mis alignment
    mirrors[mNum]->applyMisAlignment( matrix );
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Aligned " << mirrors.size() << " of type:" << mirrorID << endmsg;

  return StatusCode::SUCCESS;
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


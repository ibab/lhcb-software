
/** @file DeRich.cpp
 *
 *  Implementation file for detector description class : DeRich
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"

// DetDesc
#include "DetDesc/AlignmentCondition.h"

// local
#include "RichDet/DeRich.h"
#include "RichDet/DeRichHPDPanel.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DeRich
//
// 2004-06-30 : Antonis Papanestis
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRich::DeRich(const std::string & name)
  : DeRichBase             (name),
    m_gasWinRefIndex        ( 0 ),
    m_gasWinAbsLength       ( 0 ),
    m_nominalHPDQuantumEff  ( 0 ),
    m_nominalSphMirrorRefl  ( 0 ),
    m_nominalSecMirrorRefl  ( 0 ),
    m_sphMirAlignCond       (),
    m_secMirAlignCond       ()
{ }

//=============================================================================
// Destructor
//=============================================================================
DeRich::~DeRich() {

  if ( m_gasWinRefIndex )       delete m_gasWinRefIndex;
  if ( m_gasWinAbsLength )      delete m_gasWinAbsLength;
  if ( m_nominalHPDQuantumEff ) delete m_nominalHPDQuantumEff;
  if ( m_nominalSphMirrorRefl ) delete m_nominalSphMirrorRefl;
  if ( m_nominalSecMirrorRefl ) delete m_nominalSecMirrorRefl;

}


//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRich::initialize ( )
{
  debug() << "Initialize " << name() << endmsg;

  if ( exists( "SphMirrorSegRows" ) )
  {
    m_sphMirrorSegRows  = param<int>( "SphMirrorSegRows" );
    m_sphMirrorSegCols  = param<int>( "SphMirrorSegColumns" );
    m_positionInfo      = true;
  }

  if ( exists( "SecMirrorSegRows" ) ) {
    m_secMirrorSegRows = param<int>( "SecMirrorSegRows" );
    m_secMirrorSegCols = param<int>( "SecMirrorSegColumns" );
  } else if ( exists( "FlatMirrorSegRows" ) ){
    m_secMirrorSegRows = param<int>( "FlatMirrorSegRows" );
    m_secMirrorSegCols = param<int>( "FlatMirrorSegColumns" );
  }

  // find the HPD quantum efficiency
  std::string HPD_QETabPropLoc;
  if ( exists( "RichHpdQETableName" ) )
    HPD_QETabPropLoc = param<std::string>( "RichHpdQETableName" );
  else
  {
    if ( exists( "RichNominalHpdQuantumEffTableName" ) )
    {
      HPD_QETabPropLoc = param<std::string>( "RichNominalHpdQuantumEffTableName" );
    }
    else
    {
      fatal() << "Cannot find HPD QE location" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  SmartDataPtr<TabulatedProperty> tabQE (dataSvc(), HPD_QETabPropLoc);
  if ( !tabQE )
    error() << "No info on HPD Quantum Efficiency" << endmsg;
  else {
    debug() << "Loaded HPD QE from: " << HPD_QETabPropLoc << endmsg;
    m_nominalHPDQuantumEff = new Rich::TabulatedProperty1D( tabQE );
    if ( !m_nominalHPDQuantumEff->valid() )
    {
      error() << "Invalid HPD QE RichTabulatedProperty1D for " << tabQE->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//
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
  else {
    error() << "No position information for mirrors" << endmsg;
  }

  return mirrorPos;
}

//=========================================================================
//
//=========================================================================
RichMirrorSegPosition DeRich::secMirrorSegPos( const int mirrorNumber ) const
{

  RichMirrorSegPosition mirrorPos;

  if ( m_positionInfo ) {
    int row = mirrorNumber / m_secMirrorSegCols;
    if ( row >= m_secMirrorSegRows ) row -= m_secMirrorSegRows;
    mirrorPos.setRow( row );
    mirrorPos.setColumn( mirrorNumber % m_secMirrorSegCols );
  }
  else {
    error() << "No position information for mirrors" << endmsg;
  }

  return mirrorPos;

}
//=========================================================================
//  misalignSphMirrors
//=========================================================================
StatusCode DeRich::alignMirrors ( std::vector<const ILVolume*> mirrorContainers,
                                  const std::string& mirrorID,
                                  SmartRef<Condition> mirrorAlignCond,
                                  const std::string& Rvector ) const {

  verbose() << "Misaligning " << mirrorID << " in " << myName() << endmsg;

  std::map<int, IPVolume*> mirrors;
  const IPVolume* cpvMirror;

  std::vector<const ILVolume*>::const_iterator contIter;
  for (contIter = mirrorContainers.begin(); contIter != mirrorContainers.end();
       ++contIter) {
    for (unsigned int i=0; i<(*contIter)->noPVolumes(); ++i) {
      cpvMirror = (*contIter)->pvolume(i);
      IPVolume* pvMirror = const_cast<IPVolume*>(cpvMirror);

      // find mirrors that match mirrorID
      std::string mirrorName = pvMirror->name();
      if (mirrorName.find(mirrorID) != std::string::npos ) {
        //msg << MSG::VERBOSE << "Matched mirror " << mirrorName << endmsg;
        const std::string::size_type mpos = mirrorName.find(':');
        if ( std::string::npos == mpos ) {
          fatal() << "A mirror without a number!" << endmsg;
          return StatusCode::FAILURE;
        }
        int mirrorNumber = atoi( mirrorName.substr(mpos+1).c_str() );
        mirrors[mirrorNumber] = pvMirror;
      }
    }
  }
  verbose() << "Found " << mirrors.size() << " mirrors" << endmsg;

  std::vector<double> rotX = mirrorAlignCond->paramVect<double>("RichMirrorRotX");
  std::vector<double> rotY = mirrorAlignCond->paramVect<double>("RichMirrorRotY");

  // make sure the numbers match
  if ( rotX.size() != rotY.size() ) {
    fatal() << "Mismatch in X and Y rotations in Condition:"
            << mirrorAlignCond->name() << endmsg;
    return StatusCode::FAILURE;
  }
  if ( rotX.size() != mirrors.size() ) {
    fatal() << "Number of parameters does not match mirrors in:"
            << mirrorAlignCond->name() << endmsg;
    return StatusCode::FAILURE;
  }

  std::vector<double> Rs = paramVect<double>(Rvector);
  if ( rotX.size() != Rs.size() ) {
    fatal() << "Number of Rs does not match mirrors in:"
            << mirrorAlignCond->name() << endmsg;
    return StatusCode::FAILURE;
  }

  for (unsigned int mNum=0; mNum<rotX.size(); ++mNum) {

    // create transformation matrix to rotate the mirrors around their centre
    Gaudi::TranslationXYZ pivot( -Rs[mNum], 0.0, 0.0 );
    Gaudi::Transform3D transl( pivot );
    // the x rotation is around z because of the way the mirror is created
    Gaudi::Rotation3D rot =  Gaudi::RotationZ(rotX[mNum]) * Gaudi::RotationY(rotY[mNum]);
    Gaudi::Transform3D matrixInv = transl.Inverse() * (Gaudi::Transform3D(rot) * transl);
    Gaudi::Transform3D matrix( matrixInv.Inverse() );

    mirrors[mNum]->applyMisAlignment( matrix );
  }

  debug() << "Aligned " << mirrors.size() << " of type:" << mirrorID << endmsg;
  return StatusCode::SUCCESS;

}

//=========================================================================
//  return a RichSmartID for a particular point in the LHCb coord system
//=========================================================================
int DeRich::sensitiveVolumeID(const Gaudi::XYZPoint& globalPoint) const
{
  return ( m_HPDPanels[side(globalPoint)]->sensitiveVolumeID( globalPoint ) );
}

//=============================================================================

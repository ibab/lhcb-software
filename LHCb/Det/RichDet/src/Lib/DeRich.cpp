
/** @file DeRich.cpp
 *
 *  Implementation file for detector description class : DeRich
 *
 *  $Id: DeRich.cpp,v 1.14 2006-02-01 16:20:49 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// DetDesc
#include "DetDesc/AlignmentCondition.h"

// local
#include "RichDet/DeRich.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DeRich
//
// 2004-06-30 : Antonis Papanestis
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRich::DeRich()
  : m_gasWinRefIndex        ( 0 ),
    m_gasWinAbsLength       ( 0 ),
    m_HPDQuantumEff         ( 0 ),
    m_nominalSphMirrorRefl  ( 0 ),
    m_nominalSecMirrorRefl  ( 0 ),
    m_sphMirAlignCond       (),
    m_secMirAlignCond       (),
    m_name                  ( "DeRich" )
{ }

//=============================================================================
// Destructor
//=============================================================================
DeRich::~DeRich() {}


//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRich::initialize ( ) 
{
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
    MsgStream msg( msgSvc(), myName() );
    msg << MSG::ERROR << "No position information for mirrors" << endmsg;
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
    MsgStream msg ( msgSvc(), myName() );
    msg << MSG::ERROR << "No position information for mirrors" << endmsg;
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

  MsgStream msg( msgSvc(), myName() );
  msg << MSG::VERBOSE << "Misaligning " << mirrorID << " in " 
      << myName() <<  endmsg;

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
          msg << MSG::FATAL << "A mirror without a number!" << endmsg;
          return StatusCode::FAILURE;
        }
        int mirrorNumber = atoi( mirrorName.substr(mpos+1).c_str() );
        mirrors[mirrorNumber] = pvMirror;
      }
    }
  }
  msg << MSG::VERBOSE << "Found " << mirrors.size() << " mirrors" << endmsg;
  
  std::vector<double> rotX = mirrorAlignCond->paramVect<double>("RichMirrorRotX");
  std::vector<double> rotY = mirrorAlignCond->paramVect<double>("RichMirrorRotY");

  // make sure the numbers match
  if ( rotX.size() != rotY.size() ) {
    msg << MSG::FATAL << "Mismatch in X and Y rotations in Condition:" 
        << mirrorAlignCond->name() << endmsg;
    return StatusCode::SUCCESS;
  }
  if ( rotX.size() != mirrors.size() ) {
    msg << MSG::FATAL << "Number of parameters does not match mirrors in:" 
        << mirrorAlignCond->name() << endmsg;
    return StatusCode::SUCCESS;
  }    

  std::vector<double> Rs = paramVect<double>(Rvector);
  if ( rotX.size() != Rs.size() ) {
    msg << MSG::FATAL << "Number of Rs does not match mirrors in:" 
        << mirrorAlignCond->name() << endmsg;
    return StatusCode::SUCCESS;
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
  
  msg << MSG::DEBUG << "Aligned " << mirrors.size() << " " << mirrorID << endmsg;
  return StatusCode::SUCCESS;

}

//=============================================================================

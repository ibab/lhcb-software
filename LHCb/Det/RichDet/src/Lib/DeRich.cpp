
/** @file DeRich.cpp
 *
 *  Implementation file for detector description class : DeRich
 *
 *  $Id: DeRich.cpp,v 1.12 2005-12-14 09:34:52 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

// Gaudi
#include "GaudiKernel/MsgStream.h"

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

  //  m_vectorNames = paramVectors();
  m_paramNames  = paramNames();

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
//=============================================================================


/** @file DeRich.cpp
 *
 *  Implementation file for detector description class : DeRich
 *
 *  $Id: DeRich.cpp,v 1.10 2005-05-13 16:11:37 marcocle Exp $
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
    m_nominalFlatMirrorRefl ( 0 ),
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
    m_flatMirrorSegRows = param<int>( "FlatMirrorSegRows" );
    m_flatMirrorSegCols = param<int>( "FlatMirrorSegColumns" );
    m_positionInfo      = true;
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
RichMirrorSegPosition DeRich::flatMirrorSegPos( const int mirrorNumber ) const
{

  RichMirrorSegPosition mirrorPos;

  if ( m_positionInfo ) {
    int row = mirrorNumber / m_flatMirrorSegCols;
    if ( row >= m_flatMirrorSegRows ) row -= m_flatMirrorSegRows;
    mirrorPos.setRow( row );
    mirrorPos.setColumn( mirrorNumber % m_flatMirrorSegCols );
  }
  else {
    MsgStream msg ( msgSvc(), myName() );
    msg << MSG::ERROR << "No position information for mirrors" << endmsg;
  }

  return mirrorPos;

}
//=============================================================================

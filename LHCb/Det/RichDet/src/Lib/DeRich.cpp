
/** @file DeRich.cpp
 *
 *  Implementation file for detector description class : DeRich
 *
 *  CVS Log :-
 *  $Id: DeRich.cpp,v 1.6 2004-10-20 16:16:36 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.5  2004/10/18 11:17:44  papanest
 *  mirror reflectivity
 *
 *  Revision 1.4  2004/10/18 09:21:49  jonrob
 *  Minor updates to functions (adding const etc.)
 *
 *  Revision 1.3  2004/09/01 15:20:19  papanest
 *  added functions for TabProps
 *
 *  Revision 1.2  2004/07/27 08:55:23  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

// Gaudi
#include "GaudiKernel/MsgStream.h"
//#include "Kernel/CLHEPStreams.h"

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
DeRich::DeRich(  ) {

}
//=============================================================================
// Destructor
//=============================================================================
DeRich::~DeRich() {};


//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRich::initialize ( ) {

  //  StatusCode fail = StatusCode::FAILURE;

  MsgStream log(msgSvc(), "DeRich" );
  log << MSG::DEBUG << "Starting initialisation for DeRich" << endmsg;

  m_gasWinRefIndex = 0;
  m_gasWinAbsLength = 0;
  m_HPDQuantumEff = 0;
  m_nominalSphMirrorRefl = 0;
  m_nominalFlatMirrorRefl = 0;

  m_vectorNames = paramVectors();
  m_paramNames  = params();

  if ( hasParam( "SphMirrorSegRows" ) ) {
    m_sphMirrorSegRows = paramAsInt( "SphMirrorSegRows" );
    m_sphMirrorSegCols = paramAsInt( "SphMirrorSegColumns" );
    m_flatMirrorSegRows = paramAsInt( "FlatMirrorSegRows" );
    m_flatMirrorSegCols = paramAsInt( "FlatMirrorSegColumns" );

    m_positionInfo = true;
  }

  log << MSG::DEBUG << "Finished initialisation for DeRich" << endmsg;
  return StatusCode::SUCCESS;
}


//=========================================================================
//  find a parameter vector
//=========================================================================
bool DeRich::hasParamVector ( const std::string & vectorName ) const
{
  return ( m_vectorNames.end() != std::find(m_vectorNames.begin(),m_vectorNames.end(),vectorName) );
}

//=========================================================================
//  find a parameter
//=========================================================================
bool DeRich::hasParam ( const std::string & paramName ) const
{
  return ( m_paramNames.end() != std::find(m_paramNames.begin(),m_paramNames.end(),paramName) );
}


//=========================================================================
//
//=========================================================================
RichMirrorSegPosition DeRich::sphMirrorSegPos( const int mirrorNumber ) const
{

  RichMirrorSegPosition mirrorPos;

  if ( m_positionInfo ) {
    int row = mirrorNumber / m_sphMirrorSegCols;
    if ( row >= m_sphMirrorSegRows ) row -= m_sphMirrorSegRows;

    mirrorPos.setRow( row );
    mirrorPos.setColumn( mirrorNumber % m_sphMirrorSegCols );
  }
  else {
    MsgStream log(msgSvc(), "DeRich" );
    log << MSG::ERROR << "No position information for mirrors" << endmsg;
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
    MsgStream log(msgSvc(), "DeRich" );
    log << MSG::ERROR << "No position information for mirrors" << endmsg;
  }

  return mirrorPos;

}
//=============================================================================

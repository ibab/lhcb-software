
/** @file DeRich.cpp
 *
 *  Implementation file for detector description class : DeRich
 *
 *  CVS Log :-
 *  $Id: DeRich.cpp,v 1.2 2004-07-27 08:55:23 jonrob Exp $
 *  $Log: not supported by cvs2svn $
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
bool DeRich::hasParamVector (std::string vectorName ) {

  for (strings::const_iterator it = m_vectorNames.begin();
       it != m_vectorNames.end(); ++it) {
    if ( (*it) == vectorName ) return true;
  }
  
  return false;
}

//=========================================================================
//  find a parameter
//=========================================================================
bool DeRich::hasParam (std::string paramName ) {

  for (strings::const_iterator it = m_paramNames.begin();
       it != m_paramNames.end(); ++it) {
    if ( (*it) == paramName ) return true;
  }
  
  return false;
}


//=========================================================================
//  
//=========================================================================
RichMirrorSegPosition DeRich::sphMirrorSegPos( int mirrorNumber ) {

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
RichMirrorSegPosition DeRich::flatMirrorSegPos( int mirrorNumber ) {

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

// $Id: AsctAlgorithm.cpp,v 1.3 2004-06-11 15:26:15 phicharp Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "AsctAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AsctAlgorithm
//
// 2002-07-16 : Philippe Charpentier
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AsctAlgorithm::AsctAlgorithm( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) {

  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable = "" );
}

//=============================================================================
// Destructor
//=============================================================================
AsctAlgorithm::~AsctAlgorithm() {}; 


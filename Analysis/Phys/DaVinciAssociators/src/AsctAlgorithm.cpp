// $Id: AsctAlgorithm.cpp,v 1.2 2003-05-26 11:38:37 phicharp Exp $
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
  : Algorithm ( name , pSvcLocator ) {

  declareProperty( "InputData", m_inputData );
  declareProperty( "OutputTable", m_outputTable );
}

//=============================================================================
// Destructor
//=============================================================================
AsctAlgorithm::~AsctAlgorithm() {}; 


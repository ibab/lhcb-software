// $Id: L0FromRawBase.cpp,v 1.4 2009-10-29 11:17:00 jucogan Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "L0Base/L0FromRawBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0FromRawBase
//
// 2009-10-20 : Julien Cogan
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0FromRawBase::L0FromRawBase( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty("WriteProcData"  , m_writeProcData   = false);
  declareProperty("WriteOnTES"  , m_writeOnTES   = true);
  declareProperty("L0Context"   , m_l0context   = "");
}
//=============================================================================
// Destructor
//=============================================================================
L0FromRawBase::~L0FromRawBase() {} 


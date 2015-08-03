// $Id: L0AlgBase.cpp,v 1.4 2010-01-20 16:03:20 odescham Exp $
// Include files 

// local
#include "L0Base/L0AlgBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0AlgBase
//
// 2009-10-20 : Julien Cogan
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0AlgBase::L0AlgBase( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{

  declareProperty("WriteBanks"  , m_writeBanks   = true);
  declareProperty("WriteOnTES"  , m_writeOnTES   = false);
  declareProperty("L0Context"   , m_l0context    = "");

}
//=============================================================================
// Destructor
//=============================================================================
L0AlgBase::~L0AlgBase() {}

#define  CALODET_CALOPIN_CPP 1 
// ============================================================================
// Include files
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
#include "CaloDet/CaloPin.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloPin 
//
// 15/11/2006 : Olivier Deschamps
//-----------------------------------------------------------------------------


//=============================================================================
// Standard creator, initializes variables
//=============================================================================
CaloPin::CaloPin(LHCb::CaloCellID id )
  : m_id(id),
    m_side(-1),
    m_index(-1),
    m_area(),
    m_fCol(),
    m_fRow(),
    m_lCol(),
    m_lRow(),
    m_leds(),
    m_cells()
{
  std::stringstream reg("Region ") ;
  m_region = reg.str();
  std::stringstream cod("Code ");
  m_code = cod.str();

};

//=============================================================================
// destructor 
//=============================================================================
CaloPin::~CaloPin() {};

//=============================================================================

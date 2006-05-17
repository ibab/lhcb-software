// ============================================================================
// $Id: CellParam.cpp,v 1.5 2006-05-17 16:01:55 cattanem Exp $
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
#define  CALODET_CELLPARAM_CPP 1 
// ============================================================================
// Include files
// CaloDet 
#include "CaloDet/DeCalorimeter.h"
#include "CaloDet/CellParam.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CellParam
//
// 02/07/2001 : Ivan Belyaev
//-----------------------------------------------------------------------------


//=============================================================================
// Standard creator, initializes variables
//=============================================================================
CellParam::CellParam( const LHCb::CaloCellID& id ) 
  : m_cellID        (     id           )
  , m_size          (      0.0 * Gaudi::Units::meter )
  , m_center        ( -99999.0 * Gaudi::Units::meter , 
                      -99999.0 * Gaudi::Units::meter , 
                      0.0      * Gaudi::Units::meter )
  , m_sine          (     0.0 )
  , m_gain          (     0.0 )
  , m_time          (     0.0 )
  , m_cardNumber    (     -1  )
  , m_cardRow       (     -1  )
  , m_cardColumn    (     -1  )
  , m_neighbors     (         )
  , m_zsupNeighbors (         )
{};

//=============================================================================
// destructor 
//=============================================================================
CellParam::~CellParam() {};

//=============================================================================

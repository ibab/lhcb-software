// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/01/23 18:39:42  ocallot
// Fix unitialized variables in CellParam constructor
//
// Revision 1.2  2001/07/17 17:26:52  ibelyaev
// modifications to improve Doxygen documentation
//
// Revision 1.1  2001/07/02 17:17:59  ibelyaev
// improvements in readability of DeCalorimeter.h
//
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
  , m_size          (      0.0 * meter )
  , m_center        ( -99999.0 * meter , 
                      -99999.0 * meter , 
                      0.0      * meter )
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

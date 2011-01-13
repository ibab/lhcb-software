// ============================================================================
// $Id: $ 
// ============================================================================
#define  CALODET_CARDPARAM_CPP 1 
// ============================================================================
/// Include files
/// local
#include "CaloDet/DeCalorimeter.h"
#include "CaloDet/CardParam.h"


//-----------------------------------------------------------------------------
// Implementation file for class : CardParam
//
// 02/07/2001 : Ivan Belyaev
//-----------------------------------------------------------------------------

// ============================================================================
// Standard creator, initializes variables
// ============================================================================
CardParam::CardParam( int iArea , 
                      int iRow  , 
                      int iCol  ,
                      int ilRow  , 
                      int ilCol  ,
                      int iNum  ,
                      int iCrate,
                      int iSlot) 
  : m_Area           ( iArea )
    , m_Row            ( iRow )
    , m_Col            ( iCol )
    , m_lRow            ( ilRow )
    , m_lCol            ( ilCol )
    , m_DownNumber     ( -1 )
    , m_LeftNumber     ( -1 )
    , m_CornerNumber   ( -1 ) 
    , m_PreviousNumber ( -1 )
    , m_validationNumber( -1 )
    ,m_isPin (false)
    ,m_number( iNum)
    ,m_crate( iCrate)
    ,m_slot( iSlot)
    ,m_tell1 (-1)
    ,m_selectionType(-1)
    ,m_mapping( CardParam::None )
{
  m_ids.reserve( 64 );
  m_ids.clear();
}

// ============================================================================
// Destructor
// ============================================================================
CardParam::~CardParam(){}

// ============================================================================

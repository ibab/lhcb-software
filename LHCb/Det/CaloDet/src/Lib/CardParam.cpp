// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2007/07/19 20:37:01  odescham
// fix CardParam initialisation for PIN FEB
//
// Revision 1.4  2007/06/14 16:10:16  odescham
// fix FEBoard connections
//
// Revision 1.3  2007/02/22 23:17:18  odescham
// Define the monitoring system in DeCalorimeter
//
// Revision 1.2  2001/07/17 17:26:52  ibelyaev
// modifications to improve Doxygen documentation
//
// Revision 1.1  2001/07/02 17:17:59  ibelyaev
// improvements in readability of DeCalorimeter.h
//
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
{
  m_ids.reserve( 64 );
} ;

// ============================================================================
// Destructor
// ============================================================================
CardParam::~CardParam(){};

// ============================================================================


  

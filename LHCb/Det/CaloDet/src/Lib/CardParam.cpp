// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
                      int iCol  ) 
  : m_Area           ( iArea )
  , m_Row            ( iRow )
  , m_Col            ( iCol )
  , m_DownNumber     ( -1 )
  , m_LeftNumber     ( -1 )
  , m_CornerNumber   ( -1 ) 
  , m_PreviousNumber ( -1 )
{} ;

// ============================================================================
// Destructor
// ============================================================================
CardParam::~CardParam(){};

// ============================================================================


  

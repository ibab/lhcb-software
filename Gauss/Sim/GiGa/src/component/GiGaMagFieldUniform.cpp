// $Id: GiGaMagFieldUniform.cpp,v 1.10 2002-05-07 12:21:35 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaMACROs.h"
/// local 
#include "GiGaMagFieldUniform.h"

/** @file 
 *  
 *  implementation of class GiGaMagFieldUniform 
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

// ============================================================================
/// Factory
// ============================================================================
IMPLEMENT_GiGaFactory( GiGaMagFieldUniform) ;
// ============================================================================

// ============================================================================
/** standard constructor 
 *  @see GiGaMagFieldBase
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaMagFieldUniform::GiGaMagFieldUniform
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent )
  : GiGaMagFieldBase ( type , name , parent)
  , m_Bx( 0 ) 
  , m_By( 0 ) 
  , m_Bz( 0 ) 
{
  declareProperty("Bx" , m_Bx );
  declareProperty("By" , m_By );
  declareProperty("Bz" , m_Bz );
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaMagFieldUniform::~GiGaMagFieldUniform(){};
// ============================================================================

// ============================================================================
/** get the field value 
 *  @see G4MagneticField
 *  @param point point  
 *  @param B   value of magnetic filed in poitnt 
 */
// ============================================================================
void GiGaMagFieldUniform::GetFieldValue 
( const double [4] /* point */    , 
  double*             B           ) const 
{
  *(B+0) = (double) m_Bx ;
  *(B+1) = (double) m_By ;
  *(B+2) = (double) m_Bz ;
  *(B+2) = (double) m_Bz ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================


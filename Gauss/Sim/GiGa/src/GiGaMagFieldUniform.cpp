/// GaudiKernel
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/MsgStream.h"
/// GiGa
#include "GiGa/GiGaMagFieldFactory.h"
/// local 
#include "GiGaMagFieldUniform.h"
///

///////////////////////////////////////////////////////////////////////////////////////
static const  GiGaMagFieldFactory<GiGaMagFieldUniform>                              s_GiGaMagFieldUniformFactory ;
const        IGiGaMagFieldFactory&                     GiGaMagFieldUniformFactory = s_GiGaMagFieldUniformFactory ; 
///////////////////////////////////////////////////////////////////////////////////////
GiGaMagFieldUniform::GiGaMagFieldUniform( const std::string& nick , ISvcLocator* loc) 
  : GiGaMagFieldBase ( nick , loc )
  , m_Bx( 0 ) 
  , m_By( 0 ) 
  , m_Bz( 0 ) 
{
  declareProperty("Bx" , m_Bx );
  declareProperty("By" , m_By );
  declareProperty("Bz" , m_Bz );
};
///////////////////////////////////////////////////////////////////////////////////////
GiGaMagFieldUniform::~GiGaMagFieldUniform(){};
///////////////////////////////////////////////////////////////////////////////////////
void GiGaMagFieldUniform::GetFieldValue ( const double Point[3], double *B  ) const 
{
  ///
  *(B+0) = (double) m_Bx ;
  *(B+1) = (double) m_By ;
  *(B+2) = (double) m_Bz ;
  ///
};
///////////////////////////////////////////////////////////////////////////////////////









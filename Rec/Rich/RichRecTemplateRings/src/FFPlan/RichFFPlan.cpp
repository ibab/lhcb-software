// $Id: RichFFPlan.cpp,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"


#include "fftw3.h"

// #include <math.h>


// local
#include "RichFFPlan.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichFFPlan
//
// 2007-06-15 : Sajan EASO
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichFFPlan );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichFFPlan::RichFFPlan( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  :  RichRingRecToolBase ( type, name , parent )
{
  declareInterface<IRichFFPlan>(this);

}
void RichFFPlan::InitFFPlan(){
  int nx = RConst()->NumR();  
  int ny = RConst()->NumW();

  setDimensionPolar(nx,ny);
  
}

void RichFFPlan::setDimensionPolar(int nx, int ny ){
   m_fNx=  nx;                 //m_dimx_2drTocFPol
   m_fNy=  ny;                // m_dimy_2drTocFPol
   m_fNxy =  (nx*ny) ;        //m_dimxy_2drTocFPol
   m_fNyh = ( (ny/2)  + 1) ;  //m_dimhalfy_2drTocFPol


   m_iNx = nx;               // m_dimx_2dcTocInvPol
   m_iNyh = m_fNyh;           // m_dimy_2dcTocInvPol
   m_iNxyh =  m_iNx* m_iNyh ;  // m_dimxy_2dcTocInvPol

}

VD  RichFFPlan::ConvertToFF2d( VD A, VD B){
  // normally A is log ploar radius and B is theta for log polar setup. This 
  // means A=x and B=y in the notations of this class.
  // here we assume both A and B have the same size.
  // aType 0 is for log polar and the aType 1 is for cartisian.
  // aType=1 is not used at the moment; only one type ie. Log-polar  used in this version.


  int aSize= (int) A.size();

  // create and reset to zero  the arrays for FF

  double* m_Input2drTocF = (double* ) calloc(m_fNxy , sizeof(double));
  fftw_complex * m_Output2drTocF = (fftw_complex* ) calloc(m_fNxy , sizeof(fftw_complex));
  

  // setup a plan   

  fftw_plan  m_plan2drTocForward = fftw_plan_dft_r2c_2d( m_fNx,m_fNy,
                                   m_Input2drTocF ,m_Output2drTocF ,FFTW_MEASURE);


   
  for(int i=0; i< aSize; ++i ) {
    int cbin= ((int) A [i]) * m_fNy + ((int) B [i]);
    if( cbin <  m_fNxy ) {  
              m_Input2drTocF [cbin ]= 1.0 ; 
    }
  }

  // execute the plan

  fftw_execute(m_plan2drTocForward);



  // get the output

  VD aOut (m_fNx* m_fNyh  * 2) ;
  for(int ix=0; ix < m_fNx; ++ix ) {
     for(int iy=0; iy< m_fNyh; ++iy) {
       int ioi= ix* m_fNyh + iy;
       aOut[ ioi*2 ]    = m_Output2drTocF [ioi] [0];
       aOut[ ioi*2 +1 ] = m_Output2drTocF [ioi] [1];
     }
  }


  
  // clear the FF arrays.

  fftw_destroy_plan(m_plan2drTocForward);
  fftw_free(m_Input2drTocF);
  fftw_free(m_Output2drTocF);


  
  return aOut;
  
  
}

VVD  RichFFPlan::ConvertToInvFF2d(VD F ){
  int fsize = (int) ( F.size())/2;

  // create and reset to zero the arrays for Inv FF.

  fftw_complex* m_Input2dcTocInverse = (fftw_complex* ) calloc(m_iNxyh ,  sizeof(fftw_complex));
  fftw_complex* m_Output2dcTocInverse = (fftw_complex* ) calloc(m_iNxyh ,  sizeof(fftw_complex));

  // setup a plan

  fftw_plan m_plan2dcTocInverse = fftw_plan_dft_2d(m_iNx, m_iNyh,
                                  m_Input2dcTocInverse,m_Output2dcTocInverse,-1,FFTW_MEASURE);



  for(int k=0; k<fsize;++k) {
    m_Input2dcTocInverse [k] [0]= F[2*k];
    m_Input2dcTocInverse [k] [1]= F[2*k+1];
  } 


  // execute the plan
  fftw_execute(m_plan2dcTocInverse);


  // get the output

  VVD aInvOut(   m_iNx, VD (m_iNyh));
  for(int i=0; i< m_iNx; i++ ) {
    for(int j=0; j<m_iNyh; ++j ) {
            aInvOut [i] [j] = ( m_Output2dcTocInverse [i*m_iNyh +j ] [0])/m_iNxyh  ;       
    } 
  }

  // clear the FF arrays

  fftw_destroy_plan(m_plan2dcTocInverse );
  fftw_free(m_Input2dcTocInverse);
  fftw_free(m_Output2dcTocInverse);
  
  return aInvOut;
    
  
}

//=============================================================================
// Destructor
//=============================================================================
RichFFPlan::~RichFFPlan() {} 

//=============================================================================

// $Id: CaloCorr2TANH.cpp,v 1.3 2002-06-21 11:02:46 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2002/04/30 20:37:56  ibelyaev
//  new option files and bettwer doxygen documentation
//
// Revision 1.1  2002/04/07 18:15:00  ibelyaev
//  preliminary version ('omega'-release)
//
// ============================================================================
// Include files
#include <cmath>
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// local
#include "CaloCorr2TANH.h"

// ============================================================================
/** @file CaloCorr2TANH.h
 *  
 *  Implementation file for class : CaloCorr2TANH
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru;
 *  @date 27/03/2002 
 */
// ============================================================================

// ============================================================================
/** @var CaloCorr2TANHFactory
 *  Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloCorr2TANH>         s_factory ;
const        IToolFactory&CaloCorr2TANHFactory = s_factory ;

namespace DoubleTanh
{
  
  /** S-correction function
   *
   *  @see CaloCorrection
   *  @see CaloCorrection::Function
   *  @param  cluster    uncirrected position of cluster 
   *  @param  seed       position of center of the seed
   *  @param  size       size of seed cell 
   *  @param  parameters parametrisation
   *  @return corrected 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 27/03/2002 
   *
   */
  double 
  doubleTanh ( const double                      cluster    ,
               const double                      seed       , 
               const double                      size       , 
               const CaloCorrection::Parameters& parameters ) 
  {
    // redefine parameters
    const double dx0 = ( cluster - seed ) /size        ;
    /// s-curve as 2 hyperbolic tangents 
    const double xc  = 0.5 * size * 
      ( parameters[0] * tanh ( parameters[1] * dx0 ) +
        parameters[2] * tanh ( parameters[3] * dx0 ) ) ;
    
    return ( xc + seed ) * parameters[4] + parameters[5] ;
  };
  
  /** derivative of s-correction function
   *  
   *  @see CaloCorrection
   *  @see hollebeck
   *  @see CaloCorrection::Function
   *
   *  @param  cluster    uncirrected position of cluster 
   *  @param  seed       position of center of the seed
   *  @param  size       size of seed cell 
   *  @param  parameters parametrisation
   *  @return corrected 
   *
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date 27/03/2002 
   */
  double 
  doubleTanhPrime ( const double                      cluster    ,
                    const double                      seed       , 
                    const double                      size       , 
                    const CaloCorrection::Parameters& parameters ) 
  {
    // redefine parameters
    const double dx0 = ( cluster - seed ) /size        ;
    //
    const double arg1  = parameters[1] * dx0 ;
    const double arg2  = parameters[3] * dx0 ;
    //
    const double ch1   = 1. / cosh( arg1 ) ;
    const double ch2   = 1. / cosh( arg2 ) ;
    // derivative 
    const double prime = 0.5 * size *
      (  parameters[0] * parameters[1] / size * ch1 * ch1 +
         parameters[2] * parameters[3] / size * ch2 * ch2 ) ;
    //
    return prime * parameters[4] ;
  };  
};

//=============================================================================
/** Standard constructor
 *  @param  type   type of the tool (?)
 *  @param  name   the name of this concrete instance 
 *  @param  parent the tool's parent
 */
//=============================================================================
CaloCorr2TANH::CaloCorr2TANH( const std::string& type   ,
                              const std::string& name   ,
                              const IInterface*  parent )
  : CaloCorrection ( type, name , parent ) 
{  
  setFx  ( DoubleTanh::doubleTanh      );
  setFxp ( DoubleTanh::doubleTanhPrime );
  setFy  ( DoubleTanh::doubleTanh      );
  setFyp ( DoubleTanh::doubleTanhPrime );
};

// ============================================================================
// The End 
// ============================================================================

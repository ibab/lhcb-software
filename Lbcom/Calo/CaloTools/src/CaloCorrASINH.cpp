// $Id: CaloCorrASINH.cpp,v 1.4 2002-06-13 12:32:38 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/04/30 20:37:56  ibelyaev
//  new option files and bettwer doxygen documentation
//
// Revision 1.2  2002/04/23 10:49:03  ibelyaev
//  fix compiler problems on Win2K
//
// Revision 1.1  2002/04/07 18:15:00  ibelyaev
//  preliminary version ('omega'-release)
//
// ============================================================================
// Include files
// STD & STL 
#include <cmath>
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// local
#include "CaloCorrASINH.h"

// ============================================================================
/** @file CaloCorrASINH.h
 *  
 *  Implementation file for class : CaloCorrASINH
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru;
 *  @date 27/03/2002 
 */
// ============================================================================

// ============================================================================
/** Declaration of the Tool Factory
 */
// ============================================================================
static const  ToolFactory<CaloCorrASINH>         s_factory ;
const        IToolFactory&CaloCorrASINHFactory = s_factory ;

namespace Hollebeek 
{ 
  
	/**  temporary fix for absence of "asinh" function on WIN32 platform
	 *   @author Vanya Belyaev Ivan.Belyaev@itep.ru
	 *   @date 32 Apr 2002
	 */
	long double
  Asinh( long double x )
	{
#ifndef WIN32
    return asinh( x );
#else
    return log( x + sqrt( x * x + 1. ) );
#endif 
	};
  
  /** @brief s-correction function
   *
   *  The function and parameters from 
   *  Olivier Deschamps LPC/Clermont  <odescham@in2p3.fr>
   *
   *
   *  @f[
   *  \begin{large}
   *   \begin{array}{l}
   *   x^{\prime} = x_0 + p_0 \times {\mathrm{asinh}} 
   *   \left( \frac{\displaystyle \Delta x }{\displaystyle S / 2 } 
   *   \times {\mathrm{sinh}} 
   *   \left( \frac {\displaystyle S/2 } 
   *   { \displaystyle p_0 } \right)\right), \\
   *  {\mathrm{where}} \ 
   *  \Delta x = \frac{\displaystyle x - x_0 }{\displaystyle S}, 
   *  \ x_0 \ - \ {\mathrm{barycenter}}, 
   *  \ x_0 \ - \ {\mathrm{seed}},  
   *  \ S \ - \ {\mathrm{cell size}}
   *   \end{array}
   *  \end{large}
   *  @f]
   *  
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
  hollebeek 
  ( const double                      cluster    ,
    const double                      seed       , 
    const double                      size       , 
    const CaloCorrection::Parameters& parameters ) 
  {
    // redefine parameters
    const long double Xo = cluster - seed       ;
    const long double D  = 0.5 * size           ;
    const long double b  = parameters[0] * size ;
    // use formula! 
    const long double Xc = b * Asinh( Xo / D * sinh( D/b ) ) ;
    // adjust the center again 
    return seed + Xc ;
  };
  
  /** @brief derivative of s-correction function
   *  
   *  The function and parameters from 
   *  Olivier Deschamps LPC/Clermont  <odescham@in2p3.fr>
   *
   *  @see CaloCorrection
   *  @see hollebeek
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
  hollebeekPrime 
  ( const double                      cluster    ,
    const double                      seed       , 
    const double                      size       , 
    const CaloCorrection::Parameters& parameters ) 
  {
    // redefine parameters
    const long double Xo = cluster - seed        ;
    const long double D  = 0.5 * size            ;
    const long double b  = parameters[0]  * size ;
    //
    const long double arg   = Xo / D * sinh( D / b ) ;
    const long double arg2  = sqrt( arg * arg + 1. ) ;
    const long double prime = b / D * sinh( D / b ) 
      * ( 1.0 + arg / arg2 ) / ( arg + arg2 ) ;
    /// 
    return prime ;
  };
  
};

//=============================================================================
/** Standard constructor
 *  @param  type   type of the tool (?)
 *  @param  name   the name of this concrete instance 
 *  @param  parent the tool's parent
 */
//=============================================================================
CaloCorrASINH::CaloCorrASINH( const std::string& type   ,
                              const std::string& name   ,
                              const IInterface*  parent )
  : CaloCorrection ( type, name , parent ) 
{  
  setFx  ( Hollebeek::hollebeek      );
  setFxp ( Hollebeek::hollebeekPrime );
  setFy  ( Hollebeek::hollebeek      );
  setFyp ( Hollebeek::hollebeekPrime );
};

// ============================================================================
// The End 
// ============================================================================

// $Id: CaloCorrASINH.cpp,v 1.1 2002-04-07 18:15:00 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
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

/** @fn hollebeck 
 *  @brief s-correction function
 *
 *  The function and parameters from 
 *  Olivier Deschamps LPC/Clermont  <odescham@in2p3.fr>
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
static inline double 
hollebeck ( const double                      cluster    ,
            const double                      seed       , 
            const double                      size       , 
            const CaloCorrection::Parameters& parameters ) 
{
  // redefine parameters
  const double Xo = cluster - seed ;
  const double D  = 0.5 * size     ;
  const double b  = parameters[0]  ;
  // use formula! 
  const double Xc = b * asinh( Xo / D * sinh( D/b ) ) ;
  // adjast teh center again 
  return seed + Xc ;
};

/** @fn hollebeckPrime 
 *  @brief derivative of s-correction function
 *  
 *  The function and parameters from 
 *  Olivier Deschamps LPC/Clermont  <odescham@in2p3.fr>
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
static inline double 
hollebeckPrime ( const double                      cluster    ,
                 const double                      seed       , 
                 const double                      size       , 
                 const CaloCorrection::Parameters& parameters ) 
{
  // redefine parameters
  const double Xo = cluster - seed ;
  const double D  = 0.5 * size     ;
  const double b  = parameters[0]  ;
  //
  const double arg   = Xo / D * sinh( D / b ) ;
  const double arg2  = sqrt( arg * arg + 1. ) ;
  const double prime = b / D * sinh( D / b ) 
    * ( 1.0 + arg / arg2 ) / ( arg + arg2 ) ;
  /// 
  return prime ;
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
  setFx ( hollebeck      );
  setFxp( hollebeckPrime );
  setFy ( hollebeck      );
  setFyp( hollebeckPrime );
};

// ============================================================================
// The End 
// ============================================================================

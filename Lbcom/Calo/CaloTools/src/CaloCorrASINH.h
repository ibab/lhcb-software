// $Id: CaloCorrASINH.h,v 1.2 2002-04-30 20:37:56 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/07 18:15:00  ibelyaev
//  preliminary version ('omega'-release)
// 
// ============================================================================
#ifndef CALOTOOLS_CALOCORRASINH_H 
#define CALOTOOLS_CALOCORRASINH_H 1
// Include files
// from STL
#include <string>
// local 
#include "CaloCorrection.h"

/** @class CaloCorrASINH CaloCorrASINH.h
 *  
 *  concrete S-correction tool, 
 *  which uses Hollebeek's parametristaion
 * 
 * "Topics in calorimetry for HEP', 
 *  Hollebeek, SLAC Summer School Lectures.
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
 *   \left( \frac {\displaystyle S/2 } { \displaystyle p_0 } \right)\right), \\
 *  {\mathrm{where}} \ 
 *  \Delta x = \frac{\displaystyle x - x_0 }{\displaystyle S}, 
 *  \ x_0 \ - \ {\mathrm{barycenter}}, 
 *  \ x_0 \ - \ {\mathrm{seed}},  
 *  \ S \ - \ {\mathrm{cell size}}
 *   \end{array}
 *  \end{large}
 *  @f]
 *  
 *  @see Hollebeek 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   27/03/2002
 */
class CaloCorrASINH : 
  public CaloCorrection 
{
  /// frined factory for instantiation 
  friend ToolFactory<CaloCorrASINH>;
  ///
protected:
  
  /** Standard constructor
   *  @param  type   type of the tool (?)
   *  @param  name   the name of this concrete instance 
   *  @param  parent the tool's parent
   */
  CaloCorrASINH( const std::string& type   ,  
                 const std::string& name   ,
                  const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~CaloCorrASINH(){};
  
private:
  
  /** default constructor is private
   */
  CaloCorrASINH();
  
  /** copy constructor is private
   *  @param copy object to be copied 
   */
  CaloCorrASINH( const CaloCorrASINH& copy );
  
  /** asignement operator is private
   *  @param copy object to be copied 
   */
  CaloCorrASINH& operator = ( const CaloCorrASINH& copy );
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOTOOLS_CALOCORRASINH_H
// ============================================================================

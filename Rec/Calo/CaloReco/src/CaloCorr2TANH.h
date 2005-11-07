// $Id: CaloCorr2TANH.h,v 1.3 2005-11-07 12:12:42 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2003/06/23 13:11:54  ibelyaev
//  fix for gcc3.2
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// Revision 1.2  2002/04/30 20:37:56  ibelyaev
//  new option files and bettwer doxygen documentation
//
// Revision 1.1  2002/04/07 18:15:00  ibelyaev
//  preliminary version ('omega'-release)
// 
// ============================================================================
#ifndef CALOTOOLS_CALOCORR2TANH_H 
#define CALOTOOLS_CALOCORR2TANH_H 1
// Include files
// from STL
#include <string>
// local 
#include "CaloCorrection.h"

/** @class CaloCorr2TANH CaloCorr2TANH.h
 *  
 *  concrete S-correction tool, 
 *  which uses "tanh" parametrization, used by 
 *
 *  - Ivan Korolko,       ITEP/Moscow  Ivan.Korolko@cern.ch

 *  - Frederic Machefert, LAL/Orsay    frederic.machefert@in2p3.fr and 

 *  - Konstantin Belous, IHEP/Protvino  belous@mx.ihep.su
 *  
 *  The formula used is:
 *
 *  @f[
 *  \begin{large}
 *  \begin{array}{l}
 *  x^{\prime} = \frac{\displaystyle S}{\displaystyle 2} \left[ 
 *  p_0 \tanh \left( p_1 \Delta x \right) + 
 *  p_2 \tanh \left( p_3 \Delta x \right) \right] + p_4 x_0 + p_5, \\ 
 *  {\mathrm{where}} \ 
 *  \Delta x = \frac{\displaystyle x - x_0 }{\displaystyle S}, 
 *  \ x_0 \ - \ {\mathrm{barycenter}}, 
 *  \ x_0 \ - \ {\mathrm{seed}},  
 *  \ S \ - \ {\mathrm{cell size}}
 *  \end{array}
 *  \end{large}
 *  @f]
 *
 *  @see ::doubleTanh  
 *
 *  Configuration via job-options:
 *
 *  - Calo2tanh0.XParameters = { vector of parameters for x-correction } ;
 *
 *  - Calo2tanh0.YParameters = { vector of parameters for y-correction } ;
 *
 *  - Calo2tanh0.CellSize    = the cell size
 *
 *  - Calo2tanh0.NewZ        = new z position of cluster  
 *
 *  - Calo2tanh0.UpdateZ     = flag for updating the z position
 *
 *  
 *  @author Vanya Belyaev Ivan.Belyaev.itep.ru
 *  @date   27/03/2002
 */
class CaloCorr2TANH : 
  public CaloCorrection 
{
  /// frined factory for instantiation 
  friend class ToolFactory<CaloCorr2TANH>;
  ///
protected:
  
  /** Standard constructor
   *  @param  type   type of the tool (?)
   *  @param  name   the name of this concrete instance 
   *  @param  parent the tool's parent
   */
  CaloCorr2TANH( const std::string& type   ,  
                 const std::string& name   ,
                  const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~CaloCorr2TANH(){};
  
private:
  
  /** default constructor is private
   */
  CaloCorr2TANH();
  
  /** copy constructor is private
   *  @param copy object to be copied 
   */
  CaloCorr2TANH( const CaloCorr2TANH& copy );
  
  /** asignement operator is private
   *  @param copy object to be copied 
   */
  CaloCorr2TANH& operator = ( const CaloCorr2TANH& copy );
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOTOOLS_CALOCORR2TANH_H
// ============================================================================

// $Id: CaloCorr2TANH.h,v 1.1 2002-04-07 18:15:00 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
 *  Ivan Korolko ITEP/Moscow Ivan.Korolko@cern.ch,
 *  Frederic Machefert LAL/Orsay frederic.machefert@in2p3.fr and 
 *  Konstantin Belous IHEP/Protvino  <belous@mx.ihep.su>
 *  
 *  @author Vanya Belyaev Ivan.Belyaev.itep.ru
 *  @date   27/03/2002
 */
class CaloCorr2TANH : 
  public CaloCorrection 
{
  /// frined factory for instantiation 
  friend ToolFactory<CaloCorr2TANH>;
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

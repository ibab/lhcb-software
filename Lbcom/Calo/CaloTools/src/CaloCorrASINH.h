// $Id: CaloCorrASINH.h,v 1.1 2002-04-07 18:15:00 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
 *  @author Vanya Belyaev Ivan.Belyaev.itep.ru
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

// $Id: IGiGaInterface.h,v 1.1 2002-05-07 12:21:30 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGA_IGIGAINTERFACE_H 
#define GIGA_IGIGAINTERFACE_H 1
// Include files
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/IAlgTool.h"
// GiGa
#include "GiGa/IIDIGiGaInterface.h"

/** @class IGiGaInterface IGiGaInterface.h GiGa/IGiGaInterface.h
 *  
 *  Intermediate virual base for GiGa objects 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   06/05/2002
 */

class IGiGaInterface: 
  public virtual IInterface , 
  public virtual IAlgTool          /* to make the instantiation trivial */
{
  
public:

  /** retrieve the unique interface identifier (static)
   *  @see IID_IGiGaInterface
   *  @return unique interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_IGiGaInterface ; }

protected:

  /// destructor (virtual and protected 
  virtual ~IGiGaInterface() {};
  
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif // GIGA_IGIGAINTERFACE_H
// ============================================================================

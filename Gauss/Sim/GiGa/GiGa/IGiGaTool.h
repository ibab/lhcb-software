// $Id: IGiGaTool.h,v 1.2 2004-02-20 18:58:18 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/06 18:49:46  ibelyaev
//  see $GIGAROOT/doc/release.notes
// 
// ============================================================================
#ifndef GIGA_IGIGATOOL_H 
#define GIGA_IGIGATOOL_H 1
// ============================================================================
// Include files
#include "GiGa/IGiGaInterface.h"

/** @class IGiGaTool IGiGaTool.h GiGa/IGiGaTool.h
 *
 *  An abstract interface for generic "unspecified" GiGa Tool
 *
 *  @author Vanay BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
class IGiGaTool : virtual public IGiGaInterface 
{
public:
  
  /** uniqie interface identification
   *  @see IInterface 
   *  @see InterfaceID 
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() ;
  
  /** the only one essential method 
   *  @param par parameters 
   *  @return status code
   */
  virtual StatusCode process ( const std::string& parameter = "" ) const = 0 ;
  
protected:
  
  /// destructor 
  virtual ~IGiGaTool() ;
  
};
// ============================================================================

// ============================================================================
// The END
// ============================================================================
#endif // GIGA_IGIGATOOL_H
// ============================================================================

// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.2  2001/07/26 14:24:46  ibelyaev
/// new base class for Run Action
///
/// Revision 1.1  2001/07/26 13:26:02  ibelyaev
/// new abstract interface
/// 
// ============================================================================
#ifndef GIGA_IGIGARUNACTION_H 
#define GIGA_IGIGARUNACTION_H 1
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/IInterface.h"
/// GiGa 
#include "GiGa/IIDIGiGaRunAction.h"
/// G4 
#include "G4UserRunAction.hh"

/** @class IGiGaRunAction IGiGaRunAction.h GiGa/IGiGaRunAction.h
 *  
 *  definition of presudo-abstract interface for 
 *    GiGa Run Action
 *
 *  @author Ivan Belyaev
 *  @date   26/07/2001
 */

class IGiGaRunAction: virtual public G4UserRunAction ,  
                      virtual public IInterface          
{
  
public:
  
  /** retrieve the uniques interface ID
   *  @return uniqie interface ID 
   */
  static const InterfaceID& interfaceID ()  { return IID_IGiGaRunAction ; }
  
  /** identification 
   *  @return name of concrete inteface instance 
   */
  virtual const std::string& name        () const = 0 ;

  /** initialization method 
   *  @return status code 
   */
  virtual StatusCode         initialize  ()       = 0 ;
  
  /** finalization method 
   *  @return status code 
   */
  virtual StatusCode         finalize    ()       = 0 ;
  
  ///  virtual destructor   
  virtual ~IGiGaRunAction(){};
  
private:

};

// ============================================================================
#endif ///< GIGA_IGIGARUNACTION_H
// ============================================================================

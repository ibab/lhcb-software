// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/23 13:11:45  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef    GIGA_IGiGaPhysListFACTORY_H
#define    GIGA_IGiGaPhysListFACTORY_H 1 
// ============================================================================
// STL 
#include <string>
// GaudiKernel
#include "GaudiKernel/IFactory.h"
// GiGa 
#include "GiGa/IIDIGiGaPhysListFactory.h"
//
class ISvcLocator;
class IGiGaPhysList;
// 

/** @class IGiGaPhysListFactory IGiGaPhysListFactory.h 
 *
 *  definition of (pseudu)abstract (pseudo)interface 
 *   for factory for Geant 4 Physics List class
 *
 *  @author Vanya Belyaev
 */


class IGiGaPhysListFactory: virtual public IFactory
{
  ///
 public:

  /** access to the uniqie interface identifier 
   *  @return the uniqie interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_IGiGaPhysListFactory; }

  /** instantiate the concrete Physics List Object 
   *  @param name name of the object
   *  @param lov  pointer to service locator 
   *  @return pointer to instantiated Phys List object 
   */
  virtual IGiGaPhysList* instantiate( const std::string& name , 
                                      ISvcLocator*       loc  ) const = 0;
  
  /** access to the physics list type
   *  @return the physics list type
   */
  virtual const std::string& physListType()    const  = 0;

  /// destructor 
  virtual ~IGiGaPhysListFactory(){};
  ///
};



// ============================================================================
#endif  ///< GIGA_IGiGaPhysListFACTORY_H
// ============================================================================

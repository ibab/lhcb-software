// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/23 13:11:45  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef    GIGA_IGIGAMagFieldFACTORY_H
#define    GIGA_IGIGAMagFieldFACTORY_H 1 
// ============================================================================
//
#include <string>
// 
#include "GaudiKernel/IFactory.h"
//
#include "GiGa/IIDIGiGaMagFieldFactory.h"
//
class ISvcLocator;
class IGiGaMagField;
// 


/** @class IGiGaMagFieldFactory IGiGaMagFieldFactory.h 
 *
 *  definition of (pseudu)abstract (pseudo)interface 
 *  for factory for creation of Geant 4 Magnetic field class
 *
 *  @author Vanya Belyaev
 */


class IGiGaMagFieldFactory: virtual public IFactory
{
  ///
public:
  
  /** retrieve the unique interface identifier 
   *  @return the unique interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_IGiGaMagFieldFactory; }
  
  /** instantiate concrete Mag Field Object
   *  @param name object name
   *  @param svc  pointer to service locator 
   *  @return pointer to concrete Mag Field object 
   */
  virtual IGiGaMagField* instantiate( const std::string& name , 
                                      ISvcLocator*       loc  ) const = 0;
  
  /** access to the magnetic field type
   *  @return the magnetic field type
   */
  virtual const std::string& magFieldType()    const  = 0;
  
  /// destructor 
  virtual ~IGiGaMagFieldFactory(){};

};



// ============================================================================
#endif  ///< GIGA_IGIGAMagFieldFACTORY_H
// ============================================================================

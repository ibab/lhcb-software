// $Id: GiGaFieldMgr.h,v 1.2 2003-09-22 13:59:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/06 19:07:33  ibelyaev
//  update foe newer GiGa, add new tools
// 
// ============================================================================
#ifndef GAUSSTOOLS_GIGAFIELDMGR_H 
#define GAUSSTOOLS_GIGAFIELDMGR_H 1
// Include files
#include "GiGa/GiGaFieldMgrBase.h"
class G4ChordFinder;
 
/** @class GiGaFieldMgr GiGaFieldMgr.h 
 *  
 *  The concrete field manager for Gauss 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-03
 */
class GiGaFieldMgr : public GiGaFieldMgrBase
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GiGaFieldMgr> ;

public:
  
  /** retrieve the magnetic field 
   *  @see IGiGaFieldMgr 
   *  @see G4MagneticField 
   *  @see G4Field
   *  @return poniter to magnetic field 
   */
  virtual G4MagneticField*        field    () const ;
  
  /** initialize the object 
   *  @see GiGaFieldMgrBase
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   initialize     () ; 
  
  /** finalize   the object 
   *  @see GiGaFieldMgrBase
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   finalize       () ; 
  
protected:
  
  /** standard constructor
   *  @see GiGaFieldMgrBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaFieldMgr 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ; 
  
  // destructor 
  virtual ~GiGaFieldMgr( ); ///< Destructor
  
private:
  
  std::string             m_fieldType   ;
  IGiGaFieldMgr*          m_field       ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_GIGAFIELDMGR_H
// ============================================================================

// $Id: GiGaFieldMgr.h,v 1.4 2007-01-12 15:36:45 ranjard Exp $
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
  //friend class GiGaFactory<GiGaFieldMgr> ;

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
  
  //protected:
public:

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

// $Id: ZeroFieldMgr.h,v 1.2 2007-01-12 15:36:58 ranjard Exp $
#ifndef GAUSSTOOLS_ZEROFIELDMGR_H 
#define GAUSSTOOLS_ZEROFIELDMGR_H 1

// Include files
#include "GiGa/GiGaFieldMgrBase.h"
class G4ChordFinder;
 
/** @class ZeroFieldMgr ZeroFieldMgr.h 
 *  
 *  The concrete field manager for Gauss 
 *
 *  @author Witek Pokorski
 *  @date   2003-06-04
 */
class ZeroFieldMgr : public GiGaFieldMgrBase
{
  /// friend factory for instantiation 
  //friend class GiGaFactory<ZeroFieldMgr> ;

public:
  
  /** retrieve the magnetic field 
   *  @see IGiGaFieldMgr 
   *  @see G4MagneticField 
   *  @see G4Field
   *  @return poniter to magnetic field 
   */
  virtual G4MagneticField*        field    () const ;
  
  /** initialize the object 
   *  @see ZeroFieldMgrBase
   *  @see GiGaBase
   *  @see  AlgTool
   *  @see IAlgTool
   *  @return status code 
   */
  virtual StatusCode   initialize     () ; 
  
  //protected:
  
  /** standard constructor
   *  @see GiGaFieldMgrBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  ZeroFieldMgr 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ; 
  
  // destructor 
  virtual ~ZeroFieldMgr( ); ///< Destructor
  
private:
  
  IGiGaFieldMgr*          m_field       ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_ZEROFIELDMGR_H
// ============================================================================

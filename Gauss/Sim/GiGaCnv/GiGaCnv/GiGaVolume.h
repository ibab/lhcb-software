// $Id: GiGaVolume.h,v 1.1 2002-01-22 18:24:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GIGACNV_GIGAVOLUME_H 
#define GIGACNV_GIGAVOLUME_H 1
// Include files
class G4LogicalVolume  ;   ///< from Geant4 
class GiGaAssembly     ;   ///< from GiGaCnv

/** @class GiGaVolume GiGaVolume.h GiGaCnv/GiGaVolume.h
 *  
 *  Helpful "intermediate" class to minimize teh difference between 
 *  Logical volume and logical assembly
 *
 *  @author Ivan Belyaev
 *  @date   17/01/2002
 */
class GiGaVolume 
{

public:

  /** Standard constructor
   *  @param volume  pointer to logical volume 
   *  @param assembly pointer to logical assembly 
   */
  GiGaVolume( G4LogicalVolume* volume   = 0 ,
              GiGaAssembly*    assembly = 0 )
    : m_volume   ( volume   ) 
    , m_assembly ( assembly )
  {};
  
  /** destructor
   */
  ~GiGaVolume(){}; 
  
  /** accessor to logical volume 
   *  @return pointer to logical volume 
   */
  inline G4LogicalVolume*  volume   () const { return m_volume   ; }
  
  /** accessor to logical assembly
   *  @return pointer to logical assembly 
   */
  inline GiGaAssembly*     assembly () const { return m_assembly ; }
  
  /** valid?  
   *  only volume or assembly must be valid! 
   */
  inline bool              valid    () const  
  {
    return 
      0 == volume () && 0 == assembly () ? false :
      0 != volume () && 0 != assembly () ? false : true ;
  };
  
  /** set/reset  the new value for logical volume 
   *  @param value new value for logical volume 
   */
  inline void setVolume   ( G4LogicalVolume* value = 0 ) 
  { m_volume  = value ; }
  
  /** set/reset  the new value for logical assembly 
   *  @param value new value for logical assembly 
   */
  inline void setAssembly ( GiGaAssembly*    value = 0 ) 
  { m_assembly = value ; }
  
private:
  
  G4LogicalVolume* m_volume     ;
  GiGaAssembly*    m_assembly   ;
  
};

// ============================================================================
// End 
// ============================================================================
#endif // GIGACNV_GIGAVOLUME_H
// ============================================================================

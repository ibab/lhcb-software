// $Id: GiGaVolumePair.h,v 1.3 2007-07-02 08:48:54 gcorti Exp $
#ifndef GIGACNV_GIGAVolumePair_H 
#define GIGACNV_GIGAVolumePair_H 1

// Include files
// from Gaudi
#include "GaudiKernel/Transform3DTypes.h"

// GiGaCnv 
#include "GiGaCnv/GiGaVolume.h"

/** @class GiGaVolumePair GiGaVolumePair.h GiGaCnv/GiGaVolumePair.h
 *  
 *  Helpful class for representation of sub-element for GiGaAssembly 
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   17/01/2002
 */

class GiGaVolumePair 
{
  
public:
  
  /** Standard constructor
   *  @param volume pointer to GiGa volume
   *  @param matrix transformation matrix 
   */ 
  GiGaVolumePair( const GiGaVolume&     volume = GiGaVolume     () , 
                  const Gaudi::Transform3D& matrix = Gaudi::Transform3D () )
    : m_volume( volume ) 
    , m_matrix( matrix ) 
  {};
  
  /** destructor 
   */
  ~GiGaVolumePair(){};
  
  /** accessor to the pointer to volume 
   *  @return pointer to volume 
   */
  inline const GiGaVolume&     volume () const { return m_volume ; }
  
  /** accessor to the transformation matrix 
   *  @return transformation matrix 
   */
  inline const Gaudi::Transform3D& matrix () const { return m_matrix ; }
  
  /** set new value for volume 
   *  @param value new value for the volume 
   */
  inline void setVolume( const GiGaVolume& value ) 
  { m_volume = value ; }
  
  /** set/reset new value for transformation matrix 
   *  @param value new value of transformation matrix  
   */
  inline void setMatrix( const Gaudi::Transform3D& value = Gaudi::Transform3D() ) 
  { m_matrix = value ; }
  
private:
  
  GiGaVolume      m_volume ;
  Gaudi::Transform3D  m_matrix ;
  
};

// ============================================================================
// End 
// ============================================================================
#endif // GIGACNV_GIGAVolumePair_H
// ============================================================================

// $Id: VeloHit.h,v 1.2 2002-12-18 11:08:55 witoldp Exp $
#ifndef TRACKING_VELOHIT_H 
#define TRACKING_VELOHIT_H 1

// Include files
#include "TrackerHit.h"


/** @class VeloHit VeloHit.h Tracking/VeloHit.h
 *  
 *
 *  @author Witold Pokorski
 *  @date   2002-08-16
 */
class VeloHit : public TrackerHit {
public:
  /// Standard constructor
  VeloHit();
  
  ~VeloHit( ); ///< Destructor

  void *operator new(size_t);
  void operator delete(void *aHit);

  /// Retrieve Sensor Number
  G4long Sensor() const; 

  /// Update Sensor Number
  void SetSensor(G4long value);


protected:

private:

  G4long m_sensor; ///< Sensor Number

};

inline G4long VeloHit::Sensor() const 
{
  return m_sensor;
}

inline void VeloHit::SetSensor(G4long value)
{
  m_sensor = value; 
}

typedef G4THitsCollection<VeloHit> VeloHitsCollection;

extern G4Allocator<VeloHit> VeloHitAllocator;

#endif // TRACKING_VELOHIT_H

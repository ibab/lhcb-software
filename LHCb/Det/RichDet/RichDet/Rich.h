
#ifndef RICHDET_RICH_H
#define RICHDET_RICH_H 1

#include <vector>
#include <cassert>
#include "RichDet/ParticleCode.h"

#include "RichDet/PhotonReflector.h"
#include "RichDet/PhotonRadiator.h"
#include "RichDet/PhotonDetector.h"
#include "RichDet/RichParameters.h"


// Represents a detector. The main purpose is to establish
// a RICH identity and to provide access to the components

class Rich {

public:
  typedef int RichID;   // in principle, "enum RichID {Rich1, Rich2}" would
  enum {Rich1, Rich2};  // be nicer, but C++ is not very happy with
                        // looping over enumerations...

  Rich (const RichID id);

  virtual ~Rich();

  virtual void update(const RichParameters &parameter);

  const RichID id() const;

  int nrRadiator() const;

  const PhotonRadiator& 
  radiator(const PhotonRadiator::RadiatorID radiator) const;

  const PhotonReflector& reflector() const;

  const PhotonDetector& detector() const;

  static double particleMass(const ParticleCode code);

protected:

private:
  Rich();

  Rich(const Rich &right);

  Rich & operator=(const Rich &right);

  // Data Members for Class Attributes

  const RichID m_id;

private:
  // Data Members for Associations

  std::vector<PhotonRadiator*> m_radiator;

  PhotonReflector *m_reflector;

  PhotonDetector *m_detector;

};


inline const Rich::RichID Rich::id () const
{
  return m_id;
}

inline int Rich::nrRadiator () const
{
  return m_radiator.size();
}

inline const PhotonRadiator& 
Rich::radiator(const PhotonRadiator::RadiatorID radiator) const
{
  assert( m_radiator[radiator] != NULL );
  return *m_radiator[radiator];
}

inline const PhotonReflector & Rich::reflector () const
{
  assert( m_reflector != NULL );
  return *m_reflector;
}

inline const PhotonDetector & Rich::detector () const
{
  assert( m_detector != NULL );
  return *m_detector;
}

#endif // RICHDET_RICH_H

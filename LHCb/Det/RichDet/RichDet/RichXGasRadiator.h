#ifndef RICHDET_RICHXGASRADIATOR_H
#define RICHDET_RICHXGASRADIATOR_H 1

#include <vector>
//#include "RichGlobal/Definitions.h"
using namespace std;

// PhotonRadiator
#include "RichDet/PhotonRadiator.h"

class Rich;
class PhotonSpectrum;
class PhotonReflector;
class PhotonDetector;
class RichParameters;
class ActivePixel;
class Photon;
class TrackSegment;

class RichXGasRadiator : public PhotonRadiator {

public:
  RichXGasRadiator (const RadiatorID id, const Rich &rich);

  virtual ~RichXGasRadiator();

  void update (const RichParameters &parameter);

  double refractiveIndex (const double energy) const;

  double refractiveIndex () const;

  bool leaves (const Trajectory &trajectory, 
               double &distance) const;

  Photon generatePhoton (const ParticleCode code, 
                         TrackSegment &segment) const;

  Photon generatePhoton (const double thetaCherenkov, 
                         const double phiCherenkov, 
                         const double distCherenkov, 
                         TrackSegment &segment) const;

  Photon reconstructPhoton (TrackSegment &segment,
                            const ActivePixel &pixel) const;

  double scatterFraction (const ParticleCode particle, 
                          const TrackSegment &segment,
                          const double theta,
                          const double area) const;

  // Additional Public Declarations

protected:
  // Additional Protected Declarations

private:
  RichXGasRadiator();

  // Data Members for Class Attributes
  vector<double> m_parGas;

  mutable double m_avgRefIndex;

  // Additional Private Declarations

private:
  // Additional Implementation Declarations

};

#endif // RICHDET_RICHXGASRADIATOR_H

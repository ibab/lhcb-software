#ifndef RICHDET_RICHXGASRADIATOR_H
#define RICHDET_RICHXGASRADIATOR_H 1

#include <vector>

#include "RichDet/Rich.h"
#include "RichDet/PhotonRadiator.h"
#include "RichDet/PhotonSpectrum.h"
#include "RichDet/PhotonReflector.h"
#include "RichDet/PhotonDetector.h"
#include "RichDet/RichParameters.h"

class RichXGasRadiator : public PhotonRadiator {

public:
  RichXGasRadiator (const RadiatorID id, const Rich &rich);

  virtual ~RichXGasRadiator();

  void update (const RichParameters &parameter);

  double refractiveIndex (const double energy) const;

  double refractiveIndex () const;

  bool leaves (const Trajectory &trajectory, 
               double &distance) const;

  double scatterFraction (const double theta,
                          const double area) const;

private:
  RichXGasRadiator();

  std::vector<double> m_parGas;

  mutable double m_avgRefIndex;

};

#endif // RICHDET_RICHXGASRADIATOR_H

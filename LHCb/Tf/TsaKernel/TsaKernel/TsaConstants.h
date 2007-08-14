#ifndef _TsaConstants_H
#define _TsaConstants_H 1

// CLHEP
#include "GaudiKernel/PhysicalConstants.h"

namespace Tf
{
  namespace Tsa
  {

    /** @namespace TsaConstants TsaConstants.h Tsa/TsaConstants.h
     *
     *  Namespace for some useful type definitions and constants
     *
     *  @author M.Needham
     *  @date   13/3/2002
     */
    namespace TsaConstants
    {

      const double zMagnet = 530.0*Gaudi::Units::cm;
      const double oneOverZMagnet = 1.0/(530.0*Gaudi::Units::cm);
      const double endTracker = 950.0*Gaudi::Units::cm;
      const double beginTracker = 700.0*Gaudi::Units::cm;

      const double pMax = 500.0*Gaudi::Units::GeV;
      const double pMin = 100.0*Gaudi::Units::MeV;
      const double z0 = 0.0*Gaudi::Units::mm;
      const double tilt = 0.0036;
      const double sth = 0.087156;
      const double tth  = 0.08749;

    }

  }
}

#endif //  _TsaConstants_H

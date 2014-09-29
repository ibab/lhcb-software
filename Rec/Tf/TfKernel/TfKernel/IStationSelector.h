#ifndef TFKERNEL_ISTATIONSELECTOR_H
#define TFKERNEL_ISTATIONSELECTOR_H

#include "TfKernel/RegionSelectors.h"

namespace Tf
{
  /** @class IStationSelector TStationHitManager.h TfKernel/TStationHitManager.h
   *
   *  Interface to region selectors
   */
  class IStationSelector {
  public:
    /// Returns the search window for a given z position
    virtual XYSearchWindow searchWindow(double z) const = 0;
    /// Get the max and min z region of validity
    virtual void getValidity(double& zmin, double& zmax) const = 0;
    /// Virtual destructor
    virtual ~IStationSelector() { }
  };
}

#endif

// $Id: SiZuriFunction.h,v 1.2 2009-01-16 16:28:15 mneedham Exp $
#ifndef SiZuriFunction_H
#define SiZuriFunction_H 1

// Gaudi
#include "SiAmplifierResponseBase.h"
#include "GaudiMath/GaudiMath.h"

#include <utility>

/** @class SiZuriFunction SiZuriFunction.h
 *
 *  Class for estimating Beetle response using second order
 *  CR-RC shaper function. Shape is good up to ~ 30 ns after the peak
 *  Undershoot is overestimated however. This is expected however 
 *  since the Beetle has 'active' undershoot cancellation
 *  See: \li <a href = "http://cdsweb.cern.ch/record/837194?ln=en"> LHCb-2005-029 </a>
 * 
 *
 *  @author M.Needham
 *  @date   15/1/2009
 */

class SiZuriFunction : public SiAmplifierResponseBase {

public: 

  /** Constructer */
  SiZuriFunction(const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  /** initialize */
  StatusCode initialize() override;


 private:

  /// the actual Zurich function
  double zuriFun(double t) const;

  std::pair<double,double> findMax() const;

  /// fit par[2] ~ risetime
  double m_riseTime; 

  /// range in time
  double m_tRange;

  /// coupling offset
  double m_couplingOffset;

};

#endif // SiZuriFunction_H

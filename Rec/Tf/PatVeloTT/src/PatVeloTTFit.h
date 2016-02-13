#ifndef INCLUDE_PATVELOTTFIT_H
#define INCLUDE_PATVELOTTFIT_H 1

#include "GaudiAlg/GaudiTool.h"
#include "PatVeloTTTool.h"
#include "TrackInterfaces/IPatVeloTTFit.h"

static const InterfaceID IID_PatVeloTTFit("PatVeloTTFit", 1, 0);

// forward declarations
class PatVeloTool;

/** @class PatVeloTTFit PatVeloTTFit.h
 *  
 * provide a convenient interface to the internal fit used in the PatVeloTT
 * algorithm in the pattern reco
 *
 * @author Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
 * @date   2009-03-10
 */
class PatVeloTTFit : public GaudiTool, virtual public IPatVeloTTFit {

public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PatVeloTTFit; }

  /// Standard Constructor
  PatVeloTTFit(const std::string& type, const std::string& name,
	       const IInterface* parent);

  ~PatVeloTTFit() override; ///< Destructor

  StatusCode initialize() override; ///< Tool initialization

  StatusCode fitVTT( LHCb::Track& track) const override;

private:
  PatVeloTTTool* m_patVTTTool = nullptr;
  DeSTDetector* m_ttDet = nullptr;
  PatTTMagnetTool*    m_PatTTMagnetTool = nullptr;  ///< Multipupose tool for Bdl and deflection
};
#endif // INCLUDE_PATVELOTTFIT_H


// $Id: PatVeloTTFit.h,v 1.1 2009-03-11 15:30:53 smenzeme Exp $
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

  virtual ~PatVeloTTFit(); ///< Destructor

  virtual StatusCode initialize(); ///< Tool initialization
  virtual StatusCode   finalize(); ///< Tool finalize

  virtual StatusCode fitVTT( LHCb::Track& track) const;

private:
  PatVeloTTTool* m_patVTTTool;
  DeSTDetector* m_ttDet;

  PatTTMagnetTool*    m_PatTTMagnetTool;  ///< Multipupose tool for Bdl and deflection
};
#endif // INCLUDE_PATVELOTTFIT_H


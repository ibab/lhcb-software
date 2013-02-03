// $Id: PatVeloUTFit.h,v 1.1 2009-03-11 15:30:53 smenzeme Exp $
#ifndef INCLUDE_PATVELOUTFIT_H
#define INCLUDE_PATVELOUTFIT_H 1

#include "GaudiAlg/GaudiTool.h"
#include "PatVeloUTTool.h"
#include "TrackInterfaces/IPatVeloUTFit.h"

static const InterfaceID IID_PatVeloUTFit("PatVeloUTFit", 1, 0);

// forward declarations
class PatVeloTool;

/** @class PatVeloUTFit PatVeloUTFit.h
 *  
 * provide a convenient interface to the internal fit used in the PatVeloUT
 * algorithm in the pattern reco
 *
 * @author Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
 * @date   2009-03-10
 */
class PatVeloUTFit : public GaudiTool, virtual public IPatVeloUTFit {

public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PatVeloUTFit; }

  /// Standard Constructor
  PatVeloUTFit(const std::string& type, const std::string& name,
	       const IInterface* parent);

  virtual ~PatVeloUTFit(); ///< Destructor

  virtual StatusCode initialize(); ///< Tool initialization
  virtual StatusCode   finalize(); ///< Tool finalize

  virtual StatusCode fitVUT( LHCb::Track& track) const;

private:
  PatVeloUTTool* m_patVUTTool;
  DeSTDetector* m_ttDet;

  PatUTMagnetTool*    m_PatUTMagnetTool;  ///< Multipupose tool for Bdl and deflection
};
#endif // INCLUDE_PATVELOUTFIT_H


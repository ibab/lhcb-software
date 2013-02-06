// $Id: PrVeloUTFit.h,v 1.1 2009-03-11 15:30:53 smenzeme Exp $
#ifndef INCLUDE_PRVELOUTFIT_H
#define INCLUDE_PRVELOUTFIT_H 1

#include "GaudiAlg/GaudiTool.h"
#include "PrVeloUTTool.h"
#include "TrackInterfaces/IPrVeloUTFit.h"

static const InterfaceID IID_PrVeloUTFit("PrVeloUTFit", 1, 0);

// forward declarations
class PrVeloTool;

/** @class PrVeloUTFit PrVeloUTFit.h
 *  
 * provide a convenient interface to the internal fit used in the PrVeloUT
 * algorithm in the pattern reco
 *
 * @author Pavel Krokovny <krokovny@physi.uni-heidelberg.de>
 * @date   2009-03-10
 */
class PrVeloUTFit : public GaudiTool, virtual public IPrVeloUTFit {

public:

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_PrVeloUTFit; }

  /// Standard Constructor
  PrVeloUTFit(const std::string& type, const std::string& name,
	       const IInterface* parent);

  virtual ~PrVeloUTFit(); ///< Destructor

  virtual StatusCode initialize(); ///< Tool initialization
  virtual StatusCode   finalize(); ///< Tool finalize

  virtual StatusCode fitVUT( LHCb::Track& track) const;

private:
  PrVeloUTTool* m_patVUTTool;
  DeSTDetector* m_ttDet;

  PrUTMagnetTool*    m_PrUTMagnetTool;  ///< Multipupose tool for Bdl and deflection
};
#endif // INCLUDE_PRVELOUTFIT_H


// $Id: ProducePairPhotons.h,v 1.1.1.1 2003-12-11 11:10:25 gcorti Exp $
#ifndef PRODUCEPAIRPHOTONS_H
#define PRODUCEPAIRPHOTONS_H 1

// Include files
// from STL
#include <string>

// from DaVinci
#include "DaVinciTools/DVAlgorithm.h"

// from RecoTools
#include "RecoTools/ITrVeloCharge.h"

/** @class ProducePairPhotons ProducePairPhotons.h
 *
 *  Produce Photons with pair (selected with mass,...) and store them in TES
 *
 *  @author Richard Beneyton
 *  @date   2003-01-16
 */

class ProducePairPhotons : public DVAlgorithm {

public:
  /// Standard constructor
  ProducePairPhotons( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ProducePairPhotons( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
	// MessageService Level
	bool m_IsDebugOutputLevel;

	std::string m_veloChargeName; ///<  Name of TrVeloCharge Tool
	ITrVeloCharge* m_veloCharge; ///<  TrVeloCharge Tool

	int m_electronID; ///< electron particleID
	ParticleID m_electronParticleID;
	int m_positronID; ///< positron particleID
	ParticleID m_positronParticleID;
	double m_electronMass; ///< electron mass
	int m_gammaID; ///< gamma particleID
	ParticleID m_gammaParticleID;

	double m_MinimalProbabilityCut; ///< Minimal Probability for TrVeloCharge

};
#endif // PRODUCEPAIRPHOTONS_H

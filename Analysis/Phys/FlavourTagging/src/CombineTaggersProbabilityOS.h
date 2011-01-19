#ifndef COMBINETAGGERSPROBABILITYOS_H 
#define COMBINETAGGERSPROBABILITYOS_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "ICombineTaggersTool.h"            // Interface

/** @class CombineTaggersProbabilityOS CombineTaggersProbabilityOS.h CombineTaggersProbabilityOS.h
 *  
 *  v1.
 *  @author Marc Grabalosa
 *  @date   2011-01-17
 */

class CombineTaggersProbabilityOS : public GaudiTool, 
	virtual public ICombineTaggersTool {
public:

  /// Standard constructor
  CombineTaggersProbabilityOS( const std::string& type, 
			     const std::string& name,
			     const IInterface* parent );

  virtual ~CombineTaggersProbabilityOS( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  int combineTaggers(LHCb::FlavourTag& theTag, std::vector<LHCb::Tagger*>& );

private:

  double m_ProbMin_OS, m_omegascale_OS, m_omegamaxbin_OS;
  double m_P0_Cal_OS, m_P1_Cal_OS, m_Eta_Cal_OS;

};
#endif // COMBINETAGGERSPROBABILITY_H

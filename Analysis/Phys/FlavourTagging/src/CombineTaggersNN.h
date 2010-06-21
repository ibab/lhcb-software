#ifndef COMBINETAGGERSNN_H 
#define COMBINETAGGERSNN_H 1
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "ICombineTaggersTool.h"            // Interface

#include "TMVA/Reader.h" //TMVA      

/** @class CombineTaggersNN CombineTaggersNN.h CombineTaggersNN.h
 *  
 *  v1.3
 *  @author Marc Grabalosa
 *  @date   2010-06-10
 */

class CombineTaggersNN : public GaudiTool, 
	virtual public ICombineTaggersTool {
public:

  /// Standard constructor
  CombineTaggersNN( const std::string& type, 
		    const std::string& name,
		    const IInterface* parent );

  virtual ~CombineTaggersNN( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /// Finalize
  StatusCode finalize();
  
  int combineTaggers(LHCb::FlavourTag& theTag, std::vector<LHCb::Tagger*>& );

private:

  double m_ProbMin, m_ProbMin_NN;
  double m_P0_NN, m_P1_NN;
  double m_omegascale, m_omegamaxbin;

  std::string getPath(std::string);
  std::string m_TmvaMethod;
  std::string m_WeightsFile;
  TMVA::Reader* m_reader_comb;
  float pmu, pe, pk, pss, pvtx;

};
#endif // COMBINETAGGERSNN_H

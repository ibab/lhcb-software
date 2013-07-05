#ifndef COMBINETAGGERSNN_H 
#define COMBINETAGGERSNN_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "ICombineTaggersTool.h"            // Interface

//nnet
#include "NeuralNet/NNcomb.cxx"

/** @class CombineTaggersNN CombineTaggersNN.h
 *
 *
 *  @author Marc Grabalosa Gandara
 *  @date   2010-10-13
 */

class CombineTaggersNN : public GaudiTool,
                         virtual public ICombineTaggersTool {

public:

  /// Standard constructor
  CombineTaggersNN( const std::string& type,
                    const std::string& name,
                    const IInterface* parent);

  virtual ~CombineTaggersNN( ); ///< Destructor

  int combineTaggers(LHCb::FlavourTag& theTag, std::vector<LHCb::Tagger*>&, int,  bool  );

private:

  double m_P0_NN, m_P1_NN, m_P2_NN, m_ProbMin;
  float pmu, pe, pk, pss, pvtx;

};
#endif // COMBINETAGGERSNN_H

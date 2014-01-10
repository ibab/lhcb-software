// $Id$
#ifndef COMBINEDV_H 
#define COMBINEDV_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IParticleCombiner.h"
#include "GaudiKernel/AlgFactory.h"

/** @class CombineDV CombineDV.h
 *  Merge DV candidates into one candidate
 *
 *  @author Veerle Heijne
 *  @date   2011-10-12
 */
class CombineDV : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CombineDV( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CombineDV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_candidateLocation;  //input TES location input candidates to be merged
  std::string m_outputLocation;     //output TES location of merged candidate
  IParticleCombiner*  m_combiner;   //Combiner tool
  std::string  m_combinerName;      //MomentumCombiner
  

 
};
#endif // COMBINEDV_H

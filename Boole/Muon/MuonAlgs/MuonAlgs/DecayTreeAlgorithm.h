// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Muon/MuonAlgs/MuonAlgs/DecayTreeAlgorithm.h,v 1.1.1.1 2002-06-23 16:56:22 asatta Exp $
#ifndef TUTORIAL_DECAYTREEALGORITHM_H
#define TUTORIAL_DECAYTREEALGORITHM_H

// Framework include files
#include "GaudiKernel/Algorithm.h"  // Required for inheritance

// Forward declarations


/** User example objects: DecayTreeAlgorithm

    Description:
    The base class for the Gaudi tutorial.

    @author:  M.Frank
    @version: 1.0
*/
class DecayTreeAlgorithm : public Algorithm {
public:
  /// Constructor: A constructor of this form must be provided.
  DecayTreeAlgorithm(const std::string& name, ISvcLocator* pSvcLocator); 
  /// Standard Destructor
  virtual ~DecayTreeAlgorithm();
  /// Override Base class implementationInitialize
  virtual StatusCode initialize();
  /// Finalize
  virtual StatusCode finalize();
  /// Event callback
  virtual StatusCode execute();
  void printDecayTree(long , const std::string&, const MCParticle*);
private:
  long m_depth;
  std::string m_partName;
  IParticlePropertySvc* m_ppSvc;
  long m_partID;
};

#endif // TUTORIAL_DECAYTREEALGORITHM_H

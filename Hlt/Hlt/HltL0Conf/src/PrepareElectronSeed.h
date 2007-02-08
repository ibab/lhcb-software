// $Id: 
#ifndef PREPAREELECTRONSEED_H 
#define PREPAREELECTRONSEED_H 1

// Include files for service retrieval
#include "GaudiAlg/GaudiTool.h"

//#include "IPrepareElectronSeed.h"            // Interface
#include "HltBase/IPrepareCaloSeed.h"  

#include "Event/L0CaloCandidate.h"
#include "Event/State.h"

/** @class PrepareElectronSeed PrepareElectronSeed.h
 *  
 *  Tool to prepare state for TsaCollector for L0 confirmation
 *
 *  2007-01-11 Johannes Albrecht: some adaptions
 *
 *  2006 Herve Terrier
 */

class PrepareElectronSeed : public GaudiTool, virtual public IPrepareCaloSeed
{
public:

  // Standard constructor
  PrepareElectronSeed( const std::string& type,const std::string& name,const IInterface* parent);

  /// Standard Destructor
  virtual ~PrepareElectronSeed( ); 

  /// Initialize method
  virtual StatusCode initialize();

//   /// Finalize method
//   virtual StatusCode finalize();

  virtual StatusCode prepareSeed( const LHCb::L0CaloCandidate& eL0Cand,
                                  LHCb::State& seedStatePos,
                                  LHCb::State& seedStateNeg );
  

private:

  bool m_debugInfo; 
  

  // electron cand pos, energy
  double e_et;
  double e_x1,e_x2,e_x3,e_x4;
  double e_y1,e_y2,e_y3,e_y4;
  double e_s1,e_s2,e_s3,e_s4;
  double e_e1,e_e2,e_e3,e_e4;

  // ECAL
  DeCalorimeter* m_ecal;
  CellNeighbour m_neighbour;

  StatusCode SetCandPosAndE(const LHCb::L0CaloCandidate& cand,
                            const LHCb::CaloDigits* calodig,
                            double& x1, double& x2, double& x3, double& x4,
                            double& y1, double& y2, double& y3, double& y4,
                            double& e1, double& e2, double& e3, double& e4,
                            double& s1, double& s2, double& s3, double& s4 );
  


  
 
};


  
#endif // PREPAREELECTRONSEED_H

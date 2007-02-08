// $Id: 
#ifndef PREPAREHADRONSEED_H 
#define PREPAREHADRONSEED_H 1

// Include files for service retrieval
#include "GaudiAlg/GaudiTool.h"

#include "HltBase/IPrepareCaloSeed.h"            // Interface



/** @class PrepareHadronSeed PrepareHadronSeed.h
 *  
 *  Tool to prepare state for TsaCollector for L0 confirmation
 *
 *  2007-01-15 Johannes Albrecht some adaptions
 *
 *  2006 Herve Terrier
 */

class PrepareHadronSeed : public GaudiTool, virtual public IPrepareCaloSeed
{
public:

  // Standard constructor
  PrepareHadronSeed( const std::string& type,const std::string& name,const IInterface* parent);

  /// Standard Destructor
  virtual ~PrepareHadronSeed( ); 

  /// Initialize method
  virtual StatusCode initialize();

//   /// Finalize method
//   virtual StatusCode finalize();
  
  virtual StatusCode prepareSeed(const LHCb::L0CaloCandidate& hadL0Cand,
                                 LHCb::State& seedStatePos,
                                 LHCb::State& seedStateNeg );
  
private:
  
 // electron cand pos, energy
  double had_et;
  double had_x1,had_x2,had_x3,had_x4;
  double had_y1,had_y2,had_y3,had_y4;
  double had_s1,had_s2,had_s3,had_s4;
  double had_e1,had_e2,had_e3,had_e4;

  // some methods
  StatusCode SetCandPosAndE( const LHCb::L0CaloCandidate& cand, 
                             const LHCb::CaloDigits* hcaldig,
                             double& x1, double& x2, double& x3, double& x4,
                             double& y1, double& y2, double& y3, double& y4,
                             double& e1, double& e2, double& e3, double& e4,
                             double& s1, double& s2, double& s3, double& s4 );
  
  int ECALpart( float x , float y );
  
  // ECAL
  DeCalorimeter* m_ecal;
  CellNeighbour m_neighbour_ecal;
  
  // HCAL
  DeCalorimeter* m_hcal;
  CellNeighbour m_neighbour_hcal;

  bool m_debugInfo;
  
  
};


  
#endif // PREPAREHADRONSEED_H

// $Id: 
#ifndef ELECTRONSEEDTOOL_H 
#define ELECTRONSEEDTOOL_H 1

// Include files for service retrieval
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ILHCbMagnetSvc.h"

#include "HltBase/ICaloSeedTool.h"    // Interface

#include "Event/L0CaloCandidate.h"

//local
#include "L0ConfDataStore.h"

namespace LHCb 
{
  class State;
};

/** @class ElectronSeedTool ElectronSeedTool.h
 *  
 *  Tool to prepare state for TsaCollector for L0 confirmation
 *
 *  2007-01-11 Johannes Albrecht: some adaptions
 *
 *  2006 Herve Terrier
 */

class ElectronSeedTool : public GaudiTool, virtual public ICaloSeedTool
{
public:

  // Standard constructor
  ElectronSeedTool( const std::string& type,const std::string& name,const IInterface* parent);

  /// Standard Destructor
  virtual ~ElectronSeedTool( ); 

  /// Initialize method
  virtual StatusCode initialize();

  virtual StatusCode makeTrack( const LHCb::L0CaloCandidate& eL0Cand,
                                LHCb::Track& seedTrack );
  

private:
  bool m_debugMode, m_fieldOff;
  
  std::vector<double> m_sigmaX2;
  std::vector<double> m_sigmaY2;
  std::vector<double> m_sigmaTx2;
  std::vector<double> m_sigmaTy2;
  
  double parIP[2], parMP[2], parOP[2];
  double cellSize[3];
  double sParIP[2], sParMP[2], sParOP[2];
  

  // electron cand pos, energy
  double e_et;
  double e_x1,e_x2,e_x3,e_x4;
  double e_y1,e_y2,e_y3,e_y4;
  double e_s1,e_s2,e_s3,e_s4;
  double e_e1,e_e2,e_e3,e_e4;

  // ECAL
  DeCalorimeter* m_ecal;
  ILHCbMagnetSvc*  m_magFieldSvc;

  StatusCode SetCandPosAndE(const LHCb::L0CaloCandidate& cand,
                            double& x1, double& x2, double& x3, double& x4,
                            double& y1, double& y2, double& y3, double& y4,
                            double& e1, double& e2, double& e3, double& e4,
                            double& s1, double& s2, double& s3, double& s4 );
  
  
  ICaloDataProvider* m_caloDaq;
  
  //debug information
  L0ConfDataStore* m_DataStore;

};


  
#endif // ELECTRONSEEDTOOL_H

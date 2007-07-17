// $Id: HadronSeedTool.h,v 1.1 2007-07-17 14:05:08 albrecht Exp $
#ifndef HADRONSEEDTOOL_H 
#define HADRONSEEDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/ICaloSeedTool.h"            // Interface

//local
#include "L0ConfDataStore.h"

/** @class HadronSeedTool HadronSeedTool.h
 *  
 *  Tool to prepare L0HadronCand to a track using information
 *  in the ECal if possible
 *
 *  @author Johannes Albrecht
 *  @date   2007-07-17
 *
 *  2007-05-31 Johannes Albrecht: Hadron Seed preparation completely rewritten and optimized, 
 *                                uses 2x2 HCal cluster and 2x2 ECal cluster   
 *
 *  2007-01-15 Johannes Albrecht some adaptions
 *
 *  2006 Herve Terrier

*/
class HadronSeedTool : public GaudiTool, virtual public ICaloSeedTool {
public: 
  /// Standard constructor
  HadronSeedTool( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~HadronSeedTool( ); ///< Destructor

  /// Initialize method
  virtual StatusCode initialize();

  virtual StatusCode makeTrack( const LHCb::L0CaloCandidate& hadL0Cand,
                                LHCb::Track& seedTrack );
  

protected:

private:
  //calculate the position of a 2x2 HCal cluster around the L0Cand
  StatusCode getHCalBarycenter( const LHCb::L0CaloCandidate& cand, double& clusterEnergy , 
                                double& xBarycenter , double& yBarycenter , int& reg );

  StatusCode getECalBarycenter( double xHCal , double yHCal , double& nCells , 
                                double&  clusterEnergy, double& xECal, double& yECal , int& region  );
  
  int ECALpart( double x , double y );
  
  //calorimneters
  DeCalorimeter* m_ecal;
  DeCalorimeter* m_hcal;
  
  //calo data on demand tools 
  ICaloDataProvider* m_ecalDaq;
  ICaloDataProvider* m_hcalDaq;

  //tool to store debug information
  L0ConfDataStore* m_DataStore;  

  /*
   *  search windows:
   *  first three are Ecal (IP,MP,OP) , last two HCal (IP,MP)
   *  x,y windows are in mm, tx, ty in rad
   */
  std::vector<double> m_sigmaX2;
  std::vector<double> m_sigmaY2;
  std::vector<double> m_sigmaTx2;
  std::vector<double> m_sigmaTy2;
  
  //boolean if information in dataStore tool should be saved
  bool m_debugMode;

  double cellSizeECal[3];
  double zECal, zHCal, zT3;
  


};
#endif // HADRONSEEDTOOL_H

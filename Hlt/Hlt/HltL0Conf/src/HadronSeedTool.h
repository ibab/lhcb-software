// $Id: HadronSeedTool.h,v 1.2 2008-03-20 14:17:07 albrecht Exp $
#ifndef HADRONSEEDTOOL_H 
#define HADRONSEEDTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "HltBase/ICaloSeedTool.h"            // Interface

#include "IL0ConfExtrapolator.h"

//local
#include "L0ConfDataStore.h"

// forward declaration
namespace LHCb {
  class CaloCellID;
}

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
  StatusCode getHCalBarycenter(const LHCb::L0CaloCandidate& cand,
                               double&x, double& y, double& e, int& region ) const;

  StatusCode getECalBarycenter(double& x, double& y, double& z, double& e,
                               int minCells, int& region ) const;

  int ECALpart( double x , double y ) const;
  int HCALpart ( const LHCb::CaloCellID& cell ) const;
  
  //calorimneters
  DeCalorimeter* m_ecal;
  DeCalorimeter* m_hcal;
  
  //calo data on demand tools 
  ICaloDataProvider* m_ecalDaq;
  ICaloDataProvider* m_hcalDaq;
  IL0ConfExtrapolator* m_l0ConfExtrapolator;
  

  //tool to store debug information
  L0ConfDataStore* m_DataStore;  

  /*
   *  search windows:
   *  first three are Ecal (IP,MP,OP) , last two HCal (IP,MP)
   *  x,y windows are in mm, tx, ty in rad
   */
  //resolution w/o decoded calo cells
  std::vector<double> m_l0SigmaX2;
  std::vector<double> m_l0SigmaY2;
  std::vector<double> m_l0SigmaTx2;
  std::vector<double> m_l0SigmaTy2;
  //better resolution with decoded calo cells
  std::vector<double> m_sigmaX2;
  std::vector<double> m_sigmaY2;
  std::vector<double> m_sigmaTx2;
  std::vector<double> m_sigmaTy2;
 
  
  //boolean if information in dataStore tool should be saved
  bool m_debugMode, m_decodeCalos;
  static const double zECal, zHCal, zT3;
};
#endif // HADRONSEEDTOOL_H

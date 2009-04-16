// $Id: Calo2Calo.h,v 1.3 2009-04-16 13:07:43 odescham Exp $
#ifndef CALO2CALO_H 
#define CALO2CALO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloInterfaces/ICalo2Calo.h"            // Interface
#include "CaloDet/DeCalorimeter.h"

/** @class Calo2Calo Calo2Calo.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-05-29
 */

class Calo2Calo : public GaudiTool, virtual public ICalo2Calo {
public: 
  /// Standard constructor
  Calo2Calo( const std::string& type, 
             const std::string& name,
             const IInterface* parent);

  virtual ~Calo2Calo( ); ///< Destructor
  StatusCode initialize();
  // CaloCellIDs
  const std::vector<LHCb::CaloCellID>& cellIDs(LHCb::CaloCluster fromCluster, std::string toCalo);
  const std::vector<LHCb::CaloCellID>& cellIDs(LHCb::CaloCellID  fromId     , std::string toCalo, bool init=true);
  const std::vector<LHCb::CaloCellID>& cellIDs(){return m_cells;};
  // Digits
  const std::vector<LHCb::CaloDigit*>& digits(LHCb::CaloCellID   fromId     , std::string toCalo);
  const std::vector<LHCb::CaloDigit*>& digits(LHCb::CaloCluster  fromCluster, std::string toCalo);
  const std::vector<LHCb::CaloDigit*>& digits(){return m_digits;};
  // Energy
  double energy(LHCb::CaloCellID   fromId     , std::string toCalo);
  double energy(LHCb::CaloCluster  fromCluster, std::string toCalo);
  double energy(){return m_energy;};
  // multiplicity
  int multiplicity(LHCb::CaloCellID   fromId     , std::string toCalo);
  int multiplicity(LHCb::CaloCluster  fromCluster, std::string toCalo);
  int multiplicity(){return m_count;};

  // Additional
  bool isLocalMax(LHCb::CaloDigit digit);

protected:
  void reset();
  const std::vector<LHCb::CaloCellID>& addCell( LHCb::CaloCellID id, std::string toCalo);
  // Calo Maps
  std::map<std::string,DeCalorimeter*> m_det;
  std::map<std::string,std::string> m_loc;
  std::map<std::string,double>m_refSize;
  //
  std::vector<LHCb::CaloCellID> m_cells;
  std::vector<LHCb::CaloDigit*>  m_digits;
  double m_energy;
  std::string m_fromCalo;
  int m_count;
private:
  std::string m_toCalo;
  bool m_geo;
};
#endif // CALO2CALO_H






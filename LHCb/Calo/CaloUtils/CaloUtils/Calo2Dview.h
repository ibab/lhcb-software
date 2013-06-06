#ifndef CALODAQ_CALO2DVIEW_H 
#define CALODAQ_CALO2DVIEW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"
//from LHCb
#include "CaloDet/DeCalorimeter.h"
#include "Kernel/CaloCellID.h"
// from Event
#include "Event/MCCaloHit.h"
#include "Event/MCCaloDigit.h"
#include "Event/CaloDigit.h"
#include "Event/CaloAdc.h"
#include "Event/L0CaloAdc.h"
#include "Event/L0PrsSpdHit.h"
#include "Event/L0CaloCandidate.h"
#include "Event/CaloCluster.h"
#include "GaudiKernel/Point3DTypes.h"
#include <TH2.h> 
#include <TProfile2D.h> 
#include <TH1.h> 

/** @class Calo2Dview Calo2Dview.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2008-01-18
 */
class Calo2Dview : public GaudiHistoAlg {
public: 
  /// Standard constructor
  Calo2Dview( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~Calo2Dview( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization

  void bookCalo2D(const HistoID& unit,const std::string title, std::string name ,int area =-1);
  void bookCalo2D(const HistoID& unit,const std::string title, unsigned  int calo , int area = -1);
  void fillCalo2D(const HistoID& unit, LHCb::MCCaloHit mchit, const std::string title="");
  void fillCalo2D(const HistoID& unit, LHCb::MCCaloDigit mcdigit, const std::string title="");
  void fillCalo2D(const HistoID& unit, LHCb::CaloDigit digit, const std::string title="");
  void fillCalo2D(const HistoID& unit, LHCb::CaloAdc adc, const std::string title="");
  void fillCalo2D(const HistoID& unit, LHCb::L0CaloAdc l0adc, const std::string title="");
  void fillCalo2D(const HistoID& unit, LHCb::L0PrsSpdHit hit, const std::string title="");
  void fillCalo2D(const HistoID& unit, LHCb::L0CaloCandidate l0calo, const std::string title="");
  void fillCalo2D(const HistoID& unit, LHCb::CaloCluster cluster, const std::string title="");  
  void fillCalo2D(const HistoID& unit, const LHCb::CaloCellID& id , double value, const std::string title="");
  void fillCaloPin2D(const HistoID& unit,const LHCb::CaloCellID& id , double value, const std::string title="");
  void reset(const HistoID& unit,std::string title="");
  void resetTitle(const HistoID& unit,std::string title);
  void getCaloParam(unsigned int calo);
  void setThreshold(double threshold){m_threshold=threshold;};
  void setPinView(bool pin){m_pin=pin ;};
  void setActualSize(bool dim){m_dim=dim ;};
  void setL0ClusterView(bool l0){m_l0=l0 ;};
  void setOffset(double offset){m_offset = offset;};
  void setFlux(bool flux){m_flux = flux ;}
  void setEnergyWeighted(bool energyWeighted){m_energyWeighted = energyWeighted ;}
  void setGeometricalView(bool geo){m_geo = geo;}
  void setOneDimension(bool dim){m_1d = dim;}
  void setSplit(bool split){m_split = split;}
  bool split(){return m_split;}
  const HistoID getUnit(const HistoID& unit, int calo, int area)const;  
  
protected:
  // attributes
  DeCalorimeter* m_calo;
  int m_caloType;
  unsigned int m_reg;
  unsigned int m_centre;
  std::vector<double> m_xsize;
  std::vector<double> m_ysize;
  std::vector<LHCb::CaloCellID> m_refCell;
  int m_fCard;
  int m_lCard;
  int m_nChan;
  double  m_threshold;
  double  m_offset;
  bool m_dim;
  bool m_l0;
  bool m_pin;
  bool m_energyWeighted;
  bool m_flux;
  bool m_geo;
  bool m_split;
  bool m_1d;
  int m_bin1d;
  std::string m_lab;
  bool m_profile;
  std::string m_prof;
private:
  int centre(int x , int area);
  std::string getTitle(std::string title, int calo, int area);

  std::map<int,DeCalorimeter*> m_caloMap;
  std::map<int,std::vector<LHCb::CaloCellID> > m_refCellMap;
  std::map<int,int> m_centreMap;
  std::map<int,int> m_regMap;
  std::map<int,int> m_nChanMap;
  std::map<int,int> m_fCardMap;
  std::map<int,int> m_lCardMap;
  //
  //
  std::map<HistoID,unsigned int> caloViewMap;

  // 
  AIDA::IHistogram1D* m_storeH1;
  AIDA::IHistogram2D* m_storeH2;
  AIDA::IProfile1D*   m_storeP1;
  AIDA::IProfile2D*   m_storeP2;
  HistoID m_storeLun;
  
};
#endif // CALODAQ_CALO2DVIEW_H

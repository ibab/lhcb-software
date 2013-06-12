 #include <vector>
#include "GaudiAlg/GaudiTupleAlg.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"
#include "GaudiKernel/IEventTimeDecoder.h"
// ============================================================================

class CaloPi0Ntp : public GaudiTupleAlg{
  /// friend factory for instantiation
  friend class AlgFactory<CaloPi0Ntp>;
public: 
 /// standard algorithm initialization
  CaloPi0Ntp( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~CaloPi0Ntp();
  virtual StatusCode initialize();
  virtual StatusCode execute();
protected:
  /** Standard constructor
   *  @param   name        algorithm name
   *  @param   pSvcLocator pointer to service locator
   */
  /// destructor (virtual and protected)
private:
  /// default  constructor is private
  CaloPi0Ntp();
  /// copy     constructor is private
  CaloPi0Ntp( const CaloPi0Ntp& );
  /// assignement operator is private
  CaloPi0Ntp &operator=( const CaloPi0Ntp& );
private:
  bool inRange( std::pair<double,double> range, double value);
  bool inRange( std::pair<int, int > range, int value);
  std::vector<double> toVector(const Gaudi::XYZPoint& point);
  std::vector<double> toVector(const Gaudi::LorentzVector& vec);

  void hTuning(std::string base, int spd,double prs1, double prs2,
               const Gaudi::LorentzVector c1 , const LHCb::CaloCellID id1, 
               const Gaudi::LorentzVector c2 , const LHCb::CaloCellID id2,int nVert);

  std::pair<double,double> m_ppt;
  std::pair<double,double> m_isol;
  std::pair<int,int> m_conv;
  std::pair<double,double> m_prsE;
  std::pair<double,double> m_pt;
  std::pair<double,double> m_e;
  std::pair<double,double> m_mass;
  bool m_bkg;

  double m_leBin;
  double m_etBin;
  double m_thBin;

  double m_hMin;
  double m_hMax;
  int m_hBin;
  IEventTimeDecoder* m_odin;
  bool m_tuple;
  bool m_histo;
  bool m_trend;
  std::string m_vertLoc;
  int m_spdBin;
  int m_spdMult;
  bool m_usePV3D;


  DeCalorimeter* m_calo;
  ICaloHypo2Calo* m_toSpd ;   
  ICaloHypo2Calo* m_toPrs ;   

};

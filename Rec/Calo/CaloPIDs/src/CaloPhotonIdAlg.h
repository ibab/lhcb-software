// $Id: CaloPhotonIdAlg.h,v 1.1 2010-03-08 01:31:34 odescham Exp $
#ifndef CALOPHOTONIDALG_H 
#define CALOPHOTONIDALG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// from LHCb
#include "CaloUtils/CaloAlgUtils.h"
#include "Event/Track.h"
#include "CaloUtils/Calo2Track.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"
#include "Relations/Relation1D.h"
#include "Relations/IRelationWeighted.h"
#include "CaloUtils/ClusterFunctors.h"
#include "CaloUtils/CaloMomentum.h"
#include "boost/static_assert.hpp"

/** @class CaloPhotonIdAlg CaloPhotonIdAlg.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2010-02-27
 */
class CaloPhotonIdAlg : public GaudiAlgorithm {
public: 
  /// Standard constructor
  CaloPhotonIdAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloPhotonIdAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  double likelihood (const LHCb::CaloHypo* hypo ) const ;
  inline unsigned int bin(const double value,
                          const std::vector<double> vect) const {
    unsigned int index=0;
    if (value>=vect[ vect.size()-1 ]) {return vect.size();}
    while ((index<vect.size()) && (value>=vect[ index ])) {index++;}
    return index;
  }
  double evalChi2(const LHCb::CaloCluster*) const ;
  void evalParam(const LHCb::CaloHypo* ,double &,double &,double &,double &,double &,unsigned int &) const ;					       
  double evalLikelihood(double ,double ,double ,double ,double ,double ,unsigned int ) const ;				       
 private:



  bool m_tracking;   // use tracking
  bool m_extrapol;
  bool m_seed;
  bool m_neig;
  
  // Relations 
  std::string      m_tableLocation ;
  // 
  ICaloHypo2Calo* m_toCalo;


  std::string m_type;
  std::vector<std::string> m_inputs;
  std::string m_output;


// Photon Pdf

  typedef std::vector<double>  Data    ;
  
  Data                m_binEPrs;
  Data                m_binChi2;
  Data                m_binSeed;

  Data                m_ePrsXBin;
  Data                m_chi2XBin;
  Data                m_seedXBin;

  Data                m_ePrsYBin;
  Data                m_chi2YBin;
  Data                m_seedYBin;

  Data                m_signalEPrsData_0     ;
  Data                m_signalEPrsSpdData_0  ;
  Data                m_backgrEPrsData_0     ;
  Data                m_backgrEPrsSpdData_0  ;

  Data                m_signalChi2Data_0     ;
  Data                m_signalChi2SpdData_0  ;
  Data                m_backgrChi2Data_0     ;
  Data                m_backgrChi2SpdData_0  ;

  Data                m_signalSeedData_0     ;
  Data                m_signalSeedSpdData_0  ;
  Data                m_backgrSeedData_0     ;
  Data                m_backgrSeedSpdData_0  ;

  Data                m_signalEPrsData_1     ;
  Data                m_signalEPrsSpdData_1  ;
  Data                m_backgrEPrsData_1     ;
  Data                m_backgrEPrsSpdData_1  ;

  Data                m_signalChi2Data_1     ;
  Data                m_signalChi2SpdData_1  ;
  Data                m_backgrChi2Data_1     ;
  Data                m_backgrChi2SpdData_1  ;

  Data                m_signalSeedData_1     ;
  Data                m_signalSeedSpdData_1  ;
  Data                m_backgrSeedData_1     ;
  Data                m_backgrSeedSpdData_1  ;

  Data                m_signalEPrsData_2     ;
  Data                m_signalEPrsSpdData_2  ;
  Data                m_backgrEPrsData_2     ;
  Data                m_backgrEPrsSpdData_2  ;

  Data                m_signalChi2Data_2     ;
  Data                m_signalChi2SpdData_2  ;
  Data                m_backgrChi2Data_2     ;
  Data                m_backgrChi2SpdData_2  ;

  Data                m_signalSeedData_2     ;
  Data                m_signalSeedSpdData_2  ;
  Data                m_backgrSeedData_2     ;
  Data                m_backgrSeedSpdData_2  ;


};
#endif // CALOPHOTONIDALG_H

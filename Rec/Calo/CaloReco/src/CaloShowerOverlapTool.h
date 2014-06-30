#ifndef CALOSHOWEROVERLAPTOOL_H 
#define CALOSHOWEROVERLAPTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloCluster.h"
#include "CaloCorrectionBase.h"
#include "CaloInterfaces/ICaloHypoTool.h"
#include "CaloInterfaces/ICaloShowerOverlapTool.h"            // Interface



/** @class CaloShowerOverlapTool CaloShowerOverlapTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2014-06-02
 */
class CaloShowerOverlapTool : public GaudiTool, virtual public ICaloShowerOverlapTool {
public: 
  /// Standard constructor
  CaloShowerOverlapTool( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual StatusCode initialize();
  virtual ~CaloShowerOverlapTool( ); ///< Destructor

  StatusCode setProfile(std::string);
  void process(const LHCb::CaloCluster* c1, const LHCb::CaloCluster* c2,
               int spd=0, int niter=10,bool propagateInitialWeights=false);

protected:
  void storeInitialWeights(const LHCb::CaloCluster* cl1,const LHCb::CaloCluster* cl2);
  double getInitialWeight(const LHCb::CaloCellID id);
  double fraction(LHCb::CaloCluster* c, LHCb::CaloDigit* d,int flag);
  void subtract(LHCb::CaloCluster* c1,LHCb::CaloCluster* c2,bool propagateInitialWeights);
  double showerFraction(double d3d,unsigned int area,int spd);
  void evaluate(LHCb::CaloCluster* c,bool hypoCorrection = true);
private:
  int m_a1;
  int m_a2;
  int m_s1;
  int m_s2;
  std::string m_detLoc;
  const DeCalorimeter* m_det;
  std::string m_pcond;
  std::string m_type;
  ICaloHypoTool* m_stool;
  ICaloHypoTool* m_ltool;
  CaloCorrectionBase* m_shape;
  std::map<const LHCb::CaloCellID,double> m_weights;
  bool m_verbose;
  unsigned int m_minSize;
};
#endif // CALOSHOWEROVERLAPTOOL_H

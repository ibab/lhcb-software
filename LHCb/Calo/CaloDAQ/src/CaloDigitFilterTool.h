// $Id: $
#ifndef CALODIGITFILTERTOOL_H 
#define CALODIGITFILTERTOOL_H 1

// Include files
// from Gaudi
class StatusCode;
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Kernel/CaloCellID.h"
#include "CaloUtils/CaloCellIDAsProperty.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/Incident.h" 
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"
#include "CaloDAQ/ICaloDigitFilterTool.h"            // Interface
#include "CaloUtils/CaloAlgUtils.h"
#include "Event/RecVertex.h"

/** @class CaloDigitFilterTool CaloDigitFilterTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2010-12-13
 */
class CaloDigitFilterTool : public GaudiTool, virtual public ICaloDigitFilterTool, virtual public IIncidentListener {
public: 
  /// Standard constructor
  CaloDigitFilterTool( const std::string& type,  
                       const std::string& name,
                       const IInterface* parent);

  virtual ~CaloDigitFilterTool( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  //
  bool setDet(std::string det);
  void getOffsetMap(std::string det);
  int getMask(std::string det);
  double getOffset(LHCb::CaloCellID id, int scale);
  void setMaskMap(std::map<std::string,int> maskMap);
  int getScale();
  bool cleanDigits(std::string det, bool substr=true, bool mask = true);
  int method(std::string det){
    if( det != m_caloName)setDet( det );
    return m_scalingMethod;
  }
  unsigned int nVertices();
  unsigned int nSpd();
  double offset(LHCb::CaloCellID id);    
  double offsetRMS(LHCb::CaloCellID id);    
    
  virtual void handle(const Incident& /* inc */ ) { 
    debug() << "IIncident Svc reset" << endmsg;
    m_reset = true ;  
  } 

protected:
  void cleanDigit(LHCb::CaloDigit* digit, bool substr=true,int scale = -1, bool mask=true);

private:
  std::map<std::string,int> m_maskMap;
  int m_mask;
  std::map<LHCb::CaloCellID,double> m_offsets;
  std::map<LHCb::CaloCellID,double> m_offsetsRMS;
  LHCb::CaloDigits* m_digits;
  DeCalorimeter* m_calo;
  std::string m_caloName;
  int m_scalingMethod;
  bool m_useCondDB;
  
  std::map<LHCb::CaloCellID,double> m_ecalOffset;
  std::map<LHCb::CaloCellID,double> m_hcalOffset;
  std::map<LHCb::CaloCellID,double> m_prsOffset;

  std::map<LHCb::CaloCellID,double> m_ecalOffsetRMS;
  std::map<LHCb::CaloCellID,double> m_hcalOffsetRMS;
  std::map<LHCb::CaloCellID,double> m_prsOffsetRMS;

  int    m_scalingBin;
  double m_scalingMin;
  double m_mOffs;
  int m_nMask;
  std::string m_vertLoc;
  bool m_usePV3D;
  std::string m_scaling;
  double m_offsetRMS;
  bool m_reset;
  int m_scale;
};
#endif // CALODIGITFILTERTOOL_H

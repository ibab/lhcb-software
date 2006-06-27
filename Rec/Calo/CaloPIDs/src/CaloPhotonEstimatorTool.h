#ifndef CALOPHOTONESTIMATORTOOL_H
#define CALOPHOTONESTIMATORTOOL_H 1
// Include files
// Gaudi
#include "GaudiAlg/GaudiTool.h"
// Kernel
#include "Kernel/Point3DTypes.h"
#include "Kernel/Vector3DTypes.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
// CaloInterfaces
#include "CaloInterfaces/ICaloHypoLikelihood.h"
// Event
#include "Event/Track.h"
#include "Event/CaloCluster.h"
// Associator
#include "Relations/IRelationWeighted.h"

/** @class CaloPhotonEstimatorTool CaloPhotonEstimatorTool.h
 *
 *  Photon Selection Tool
 *  (LHCb 2004-03)
 *
 *  @author Frédéric Machefert frederic.machefert@in2p3.fr
 *  @date   2004-15-04
 */

// Forward declarations
class DeCalorimeter   ;
class LHCb::CaloHypo  ;
class IDataProviderSvc;
class IIncidentSvc    ;
class IHistogram1D    ;

class CaloPhotonEstimatorTool :  public         GaudiTool,
                                 virtual public ICaloHypoLikelihood{
public:
  CaloPhotonEstimatorTool
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );

  virtual ~CaloPhotonEstimatorTool();

  virtual StatusCode initialize ();
  virtual StatusCode finalize   ();
  virtual const LHCb::CaloHypo::Hypothesis& hypothesis () const ;
  virtual double operator() (const LHCb::CaloHypo* hypo ) const ;
  virtual double likelihood (const LHCb::CaloHypo* hypo ) const ;


protected:
  inline unsigned int bin(const double value,
                          const std::vector<double> vect) const {
    unsigned int index=0;
    if (value>=vect[ vect.size()-1 ]) {return vect.size();}
    while ((index<vect.size()) && (value>=vect[ index ])) {index++;}
    return index;
  }

 private:

  


// Detector Information
  DeCalorimeter *m_detEcal;
  DeCalorimeter *m_detPrs;
  DeCalorimeter *m_detSpd;

  std::string m_nameOfECAL;
  std::string m_nameOfPRS;
  std::string m_nameOfSPD;

  Gaudi::Plane3D m_planePrs      ;
  Gaudi::Plane3D m_planeSpd      ;

  // Hypothesis
  LHCb::CaloHypo::Hypothesis       m_hypothesis;

// Relations 
  std::string      m_tableLocation ;
  mutable LHCb::Calo2Track::IClusTrTable*  m_table  ;

//Vertex
  Gaudi::XYZPoint m_vertex    ;

// Photon Pdf

  typedef std::vector<double>  Data    ;

  Data                m_eBin;
  Data                m_binningEPrs;
  Data                m_binningChi2;
  Data                m_binningShap;

  Data                m_ePrsBin;
  Data                m_chi2Bin;
  Data                m_shapBin;

  Data                m_signalEPrsData     ;
  Data                m_signalEPrsSpdData     ;
  Data                m_backgrEPrsData     ;
  Data                m_backgrEPrsSpdData     ;

  Data                m_signalChi2Data     ;
  Data                m_signalChi2SpdData     ;
  Data                m_backgrChi2Data     ;
  Data                m_backgrChi2SpdData     ;

  Data                m_signalShapeData     ;
  Data                m_signalShapeSpdData     ;
  Data                m_backgrShapeData     ;
  Data                m_backgrShapeSpdData     ;

/*
  // Services
  IHistogramSvc*                   m_histoSvc  ;

  std::vector<IHistogram1D*>    m_signalEPrs     ;
  std::vector<IHistogram1D*>    m_backgrEPrs     ;
  std::vector<IHistogram1D*>    m_signalChi2     ;
  std::vector<IHistogram1D*>    m_backgrChi2     ;
  std::vector<IHistogram1D*>    m_signalShape     ;
  std::vector<IHistogram1D*>    m_backgrShape     ;

  std::vector<IHistogram1D*>    m_signalEPrsSpd     ;
  std::vector<IHistogram1D*>    m_backgrEPrsSpd     ;
  std::vector<IHistogram1D*>    m_signalChi2Spd     ;
  std::vector<IHistogram1D*>    m_backgrChi2Spd     ;
  std::vector<IHistogram1D*>    m_signalShapeSpd     ;
  std::vector<IHistogram1D*>    m_backgrShapeSpd     ;

  IHistogram1D*                 m_likelihood;
  bool                          m_monitoring;
*/

  bool                          m_extrapol;
 };

#endif // CALOPHOTONESTIMATORTOOL_H





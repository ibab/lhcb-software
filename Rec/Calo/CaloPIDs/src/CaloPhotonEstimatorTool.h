#ifndef CALOPHOTONESTIMATORTOOL_H
#define CALOPHOTONESTIMATORTOOL_H 1
// Include files
// STD & STL
#include <string>
#include <vector>
// CLHEP
#include "CLHEP/Geometry/Point3D.h"
// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
// CaloInterfaces
#include "Kernel/CaloHypotheses.h"
#include "CaloInterfaces/ICaloHypoLikelihood.h"
// CaloKernel
#include "CaloKernel/CaloTool.h"
// Event
#include "Event/TrStoredTrack.h"
#include "Event/CaloCluster.h"
// Associator
#include "Relations/IAssociatorWeighted.h"


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
class CaloHypo        ;
class IDataProviderSvc;
class IIncidentSvc    ;
class IHistogram1D    ;

class CaloPhotonEstimatorTool :
  public virtual IIncidentListener ,
  public virtual ICaloHypoLikelihood,
  public         CaloTool
{
  /// friend class for instantiation
  friend class ToolFactory<CaloPhotonEstimatorTool>;

public:
  virtual StatusCode initialize ();
  virtual StatusCode finalize   ();
  virtual const CaloHypotheses::Hypothesis& hypothesis () const ;
  virtual double operator() (const CaloHypo* hypo ) const ;
  virtual double likelihood (const CaloHypo* hypo ) const ;
  virtual void handle( const Incident& incident ) ;

/*
  std::vector<IHistogram1D*> makeHisto(unsigned int, double, double,
                                       unsigned int, std::string,std::string,
                                       std::vector<double>,
                                       unsigned int);
*/

protected:
  CaloPhotonEstimatorTool
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );

  virtual ~CaloPhotonEstimatorTool();

  inline unsigned int bin(const double value,
                          const std::vector<double> vect) const {
    unsigned int index=0;
    if (value>=vect[ vect.size()-1 ]) {return vect.size();}
    while ((index<vect.size()) && (value>=vect[ index ])) {index++;}
    return index;
  }

 private:

  typedef IAssociatorWeighted<CaloCluster,TrStoredTrack,float> IMatch ;
  typedef IMatch::DirectType                                   Table  ;
  typedef Table::Range                                         Range  ;

// Detector Information
  DeCalorimeter *m_detEcal;
  DeCalorimeter *m_detPrs;
  DeCalorimeter *m_detSpd;

  std::string m_nameOfECAL;
  std::string m_nameOfPRS;
  std::string m_nameOfSPD;

  double m_zPrs      ;
  double m_zSpd      ;
  double m_shiftSpd  ;

  // Hypothesis
  CaloHypotheses::Hypothesis       m_hypothesis;

// Associator
  std::string                      m_matchType         ;
  std::string                      m_matchName         ;
  IMatch*                          m_match             ;

  HepPoint3D                       m_vertex    ;

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





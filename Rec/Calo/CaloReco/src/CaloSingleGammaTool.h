#ifndef CALOSINGLEGAMMATOOL_H
#define CALOSINGLEGAMMATOOL_H 1

// Include files
// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
// CaloInterfaces
#include "Kernel/CaloHypotheses.h" 
#include "CaloInterfaces/ICaloHypoLikelihood.h"
// CaloKernel
#include "CaloKernel/CaloTool.h"
// from CaloEvent/Event
#include "Event/CaloDigit.h"
// from CaloUtils
#include "CaloUtils/ClusterFunctors.h"

// Forward declarations
class DeCalorimeter;
class CaloHypo;
class IDataProviderSvc;
class IIncidentSvc ; 


class CaloSingleGammaTool : 
  public virtual IIncidentListener ,
  public virtual ICaloHypoLikelihood ,
  public         CaloTool
{
  /// friend class for instantiation
  friend class ToolFactory<CaloSingleGammaTool>;
  
public:
  
  virtual StatusCode initialize ();
  
  virtual StatusCode finalize   ();
  
  virtual const CaloHypotheses::Hypothesis& hypothesis () const ; 

  virtual double operator() (const CaloHypo* hypo ) const ;
  
  virtual double likelihood (const CaloHypo* hypo ) const ;

  virtual void handle( const Incident& incident ) ;
  
protected:

  CaloSingleGammaTool
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent );
  
  virtual ~CaloSingleGammaTool();

 private:
  DeCalorimeter *m_detEcal;
  DeCalorimeter *m_detPrs;
  DeCalorimeter *m_detSpd;

  IIncidentSvc*                    m_incSvc    ;

  double                           m_z         ;
  double                           m_zSpd      ;
  double                           m_zPrs      ;
  
  std::string m_nameOfECAL;
  std::string m_nameOfSPD;
  std::string m_nameOfPRS;
  
  CaloHypotheses::Hypothesis m_hypothesis;

  HepPoint3D                       m_vertex    ;
  bool m_extrapolation;
  
  double m_shiftSpd;

 };

#endif // CALOSINGLEGAMMATOOL_H





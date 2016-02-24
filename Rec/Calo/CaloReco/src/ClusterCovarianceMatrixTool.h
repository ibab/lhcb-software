// $Id: ClusterCovarianceMatrixTool.h,v 1.5 2010-03-08 01:19:40 odescham Exp $
// ============================================================================
#ifndef CLUSTERCOVARIANCEMATRIXTOOL_H 
#define CLUSTERCOVARIANCEMATRIXTOOL_H 1
// Include files
#include  "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/Incident.h" 
#include  "CaloUtils/CovarianceEstimator.h"
#include  "CaloInterfaces/ICaloClusterTool.h"
#include  "CaloCorrectionBase.h"


/** @class ClusterCovarianceMatrixTool 
 *         ClusterCovarianceMatrixTool.h
 *  
 *  Concrete tool for calculation of covariance matrix 
 *  for the whole cluster object 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

namespace CaloCovariance{
  enum Parameters{
    Stochastic      = 0 ,  // stochastig term     Cov(EE)_i +=  [ S  * sqrt(E_i_GeV) ]^2
    GainError       = 1 ,  // constant term       Cov(EE)_i +=  [ G  * E_i           ]^2
    IncoherentNoise = 2 ,  // noise (inc.) term   Cov(EE)_i +=  [ iN * gain_i        ]^2 
    CoherentNoise   = 3 ,  // noise (coh.) term   Cov(EE)_i +=  [ cN * gain_i        ]^2
    ConstantE       = 4 ,  // additional term     Cov(EE) +=  [ cE               ]^2
    ConstantX       = 5 ,  // additional term     Cov(XX) +=  [ cX               ]^2
    ConstantY       = 6 ,  // additional term     Cov(XX) +=  [ cY               ]^2
    Last
  };  
  static const int nParams=Last+1;
  static const std::string ParameterName[nParams] = {"Stochastic","GainError","IncoherentNoise","CoherentNoise",
                                                     "ConstantE" ,"ConstantX","ConstantY"};
  static const std::string ParameterUnit[nParams] = {"Sqrt(GeV)" , "" , "ADC" , "ADC" , "MeV" , "mm" , "mm"};
  typedef std::map<std::string,std::vector<double> > ParameterMap;
  typedef SimplePropertyRef< ParameterMap >          ParameterProperty;
}

    
      
    


class ClusterCovarianceMatrixTool: public virtual ICaloClusterTool , public GaudiTool , virtual public IIncidentListener{
  friend struct ToolFactory<ClusterCovarianceMatrixTool>;

public:
  
  virtual StatusCode initialize ();
  virtual StatusCode finalize   ();
  virtual StatusCode process    ( LHCb::CaloCluster* cluster ) const ;  
  virtual StatusCode operator() ( LHCb::CaloCluster* cluster ) const ;


  virtual void handle(const Incident&  ) { 
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << "IIncident Svc reset" << endmsg; 
    setEstimatorParams(); // reset estimator when parameters change
  } 

  
protected:

  StatusCode getParamsFromOptions();
  StatusCode getParamsFromDB();
  void setEstimatorParams(bool init=false);

  ClusterCovarianceMatrixTool( const std::string& type   ,
                               const std::string& name   ,
                               const IInterface*  parent );
  
  /// destructor, virtual and protected 
  virtual ~ClusterCovarianceMatrixTool();

private:
  
  /// default constructor  is private 
  ClusterCovarianceMatrixTool();
  /// copy    constructor  is private 
  ClusterCovarianceMatrixTool
  ( const ClusterCovarianceMatrixTool& );  
  /// assignement operator is private 
  ClusterCovarianceMatrixTool& operator=( const ClusterCovarianceMatrixTool& );

private:
  
  CovarianceEstimator     m_estimator ;
  CaloCovariance::ParameterMap m_parameters;
  std::string m_detData;
  const DeCalorimeter* m_det;
  bool m_useDB;
  CaloCorrectionBase*      m_dbAccessor;
  std::string m_conditionName;
  std::map<unsigned int,std::string> m_source;
  }; ///< end of class ClusterCovarianceMatrixTool
// ============================================================================
#endif // CLUSTERCOVARIANCEMATRIXTOOL_H

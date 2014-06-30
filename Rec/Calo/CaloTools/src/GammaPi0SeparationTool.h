// $Id: $
#ifndef GAMMAPI0SEPARATIONTOOL_H 
#define GAMMAPI0SEPARATIONTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloInterfaces/IGammaPi0SeparationTool.h"

// Math
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"

//using namespace LHCb;

#include "TMV_MLP_inner.C"
#include "TMV_MLP_middle.C"
#include "TMV_MLP_outer.C"

//static const InterfaceID IID_GammaPi0SeparationTool ( "GammaPi0SeparationTool", 1, 0 );

/** @class GammaPi0SeparationTool GammaPi0SeparationTool.h
 *  
 *
 *  @author Miriam Calvo Gomez
 *  @date   2010-03-29
 */
class GammaPi0SeparationTool : public GaudiTool, virtual public IGammaPi0SeparationTool{
public: 

  // Return the interface ID
  //  static const InterfaceID& interfaceID() { return IID_GammaPi0SeparationTool; }

  /// Standard constructor
  GammaPi0SeparationTool( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~GammaPi0SeparationTool( ); ///< Destructor

  StatusCode initialize();
  StatusCode finalize();

  //double isPhoton(const LHCb::Particle* gamma);
  double isPhoton(const LHCb::CaloHypo* hypo);
  
  bool ClusterVariables(const LHCb::CaloHypo* hypo,
                        double& fr2, double& fasym, double& fkappa, double& fr2r4, double& etot,
                        double& Eseed, double& E2, int& area);
  
  bool PrsVariables(const LHCb::CaloHypo* hypo,
                    double& r2PS, double& asymPS, double& kappaPS, double& r2r4PS, 
                    double& eSumPS, double& ePrs, double& eMaxPS, double& e2ndPS, double& ecornerPS, 
                    int& multiPS, int& multiPS15, int& multiPS30, int& multiPS45);

  double inputData(std::string data){
    // try ecal data
    std::map<std::string,double>::iterator it = m_data.find(data);
    if( it != m_data.end() )return it->second;
    // else try prs data :
    std::map<std::string,double>::iterator itp = m_prsdata.find(data);
    if( itp != m_prsdata.end() )return itp->second;
    // yapa
    return 0.;
  }
  std::map<std::string,double> inputDataMap(){return m_data;}
  std::map<std::string,double> inputPrsDataMap(){return m_prsdata;}
  double isPhoton(const double* v){
    return photonDiscriminant(int(v[0]),
                              v[1],v[2],v[3],
                              v[4],v[5],v[6],
                              v[7],v[8],v[9],v[10],
                              int(v[11]),int(v[12]),int(v[13]),int(v[14]));
  }


protected:

private:

  double m_minPt;

  std::auto_ptr<IClassifierReader> m_reader0;
  std::auto_ptr<IClassifierReader> m_reader1;
  std::auto_ptr<IClassifierReader> m_reader2;

  const DeCalorimeter* m_ecal;
  const DeCalorimeter* m_prs;

  Gaudi::XYZPoint m_vertex;
  Gaudi::Plane3D m_planePrs;

  double photonDiscriminant(int area,  
                            double r2, double r2r4, double asym, 
                            double kappa, double Eseed, double E2,
                            double r2PS, double asymPS, double eMaxPS, double e2ndPS,
                            int multiPS, int multiPS15, int multiPS30, int multiPS45); 

  std::map<std::string,double> m_data;
  std::map<std::string,double> m_prsdata;
  double m_def;
};
#endif // GAMMAPI0SEPARATIONTOOL_H

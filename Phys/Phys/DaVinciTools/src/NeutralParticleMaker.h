#ifndef NEUTRALPARTICLEMAKER_H 
#define NEUTRALPARTICLEMAKER_H 1

// Include files
// from STL
#include <string>
#include <utility>

// from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "DaVinciTools/IParticleMaker.h"

// Forward declarations
class DeCalorimeter; 
class IParticlePropertySvc;

/** @class NeutralParticleMaker NeutralParticleMaker.h
 *  Produces Neutral Particles from Neutral ProtoParticles
 *
 * @author Frederic Machefert
 * @date 24/07/02/2002 
*/

class NeutralParticleMaker : public AlgTool,
                             virtual public IParticleMaker {
public:
  /// Standard constructor
  NeutralParticleMaker( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);

  virtual ~NeutralParticleMaker( ); ///< Destructor

  /// Initialize
  StatusCode initialize();
  
  /// The standard event data service. Method not in IParticleMaker.h.
  IDataProviderSvc* eventSvc() const;

  /// The standard event data service. Method not in IParticleMaker.h.
  IDataProviderSvc* detSvc() const;

  /// The Particle property service.
  IParticlePropertySvc* ppSvc() const;
  
  /// Selection
  virtual double applySelection( SmartRef<ProtoParticle> );

  /// Functional method to make particles.
  virtual StatusCode makeParticles( ParticleVector& );
  
  /// Finalize
  StatusCode finalize();

  /// internal method
  StatusCode fillParticle( const ProtoParticle* protoCand, 
                           int protoID, double protoCL,
                           Particle* particle );
 
protected:
 
private:
  
  std::string m_input;
  
  std::vector<std::string>  m_particleNames; ///< Names of Particles to make
  std::vector< std::pair<int,double> > m_ids; ///< PDGid of Particles to make
  std::vector<double> m_confLevels;  ///< Cut Value on the CL of pid Hypothesis
	
    //  std::vector<double> m_vertexPosError; // Error on the Vertex Position for Error 
    //	                                // Matrix Calculations
  double m_x0;
  double m_y0;
  double m_z0;
  double m_vertexErr_x;
  double m_vertexErr_y;
  double m_vertexErr_z;
  
  IParticlePropertySvc* m_ppSvc;  ///<  Reference to Particle Property Service
  IDataProviderSvc* m_EDS;        ///<  Reference to Event Data Service
  IDataProviderSvc* m_DDS;        ///<  Reference to Detector Data Service
	      
  int m_pi0ID;
  int m_gammaID;

  DeCalorimeter *m_detEcal;
  DeCalorimeter *m_detPrs;
  DeCalorimeter *m_detSpd;
  
  double                           m_zEcal     ;
  double                           m_zSpd      ;
  double                           m_zPrs      ;

  double                           m_shiftSpd;
  
  std::string m_nameEcal;
  std::string m_nameSpd;
  std::string m_namePrs;

  ClusterFunctors::ClusterFromCalo m_calo                        ;

  HepPoint3D                       m_vertex;

};
#endif // NEUTRALPARTICLEMAKER_H

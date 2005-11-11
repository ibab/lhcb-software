#ifndef LAGRANGEGLOBALFITTER_H
#define LAGRANGEGLOBALFITTER_H 1

// Include files from STL
#include <vector>
#include <string>
#include <map>

// Include files from Gaudi
#include "GaudiKernel/AlgTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "CLHEP/Vector/LorentzVector.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Matrix/DiagMatrix.h"

// Include files from Kernel
#include "Kernel/IGlobalFitter.h" 
#include "Kernel/IVertexFitter.h"
#include "Kernel/IParticleTransporter.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"
#include "Kernel/IParticleStuffer.h"

// Forward declarations
class IParticlePropertySvc;
class IVertexFitter;
class IParticleTransporter;
class IParticleStuffer;

/** @class LagrangeGlobalFitter LagrangeGlobalFitter.h 
 *  
 * Description: Perform a kinematic and geometric fit for every decay topology
 *
 * @author G.Balbi A.Carbone V. Vagnoni S.Vecchi
 *
 * @date 20/10/2005
 *
 */

//--------------------- LAGRANGEGLOBALFITTER CLASS ---------------//

class LagrangeGlobalFitter : public GaudiTool, virtual public IGlobalFitter {
  
public:
	
  //constructor and destructor
  LagrangeGlobalFitter( const std::string& type, const std::string& name, const IInterface* parent);
  virtual ~LagrangeGlobalFitter() { }
  
  StatusCode initialize();    
  StatusCode finalize();
  
  //Fit Methods
  StatusCode fit(Vertex &WorkingVertex);  
  StatusCode fit(Vertex &WorkingVertex, double&, double&);  
  StatusCode fit(Particle &WorkingParticle);
	
  //Retrieve Pulls
  StatusCode getFitPull(Particle &previousP, Particle &afterP, HepVector &pull);
  StatusCode getFitPull(Vertex &previousV, Vertex &afterP, HepVector &pull);

  //Set mass constraint	
  void setMassConstraint(Vertex &v);

  //Set which particle to compute the lifetime
  void setWhichParticleLifetime(Particle *);
		
		
#define M_C_AT_VTX 0x80
  
private:

  /// Particle to compute the lifetime
  
  
  Particle *SelectParticle;
  Vertex* ProductionVertex;
  Vertex* DecayVertex;  

  /// Compute the lifetime and its error
	  
  StatusCode LagrangeGlobalFitter::lifeTimeCalculator(Vertex &outputVertex, 
                                                      HepMatrix &V, int& num_m, 
                                                      double &lftime, double &lftimeerr);
		
  /// General internal fit method
  StatusCode fitFromEverything(Vertex &inVertex,Particle &inParticle, 
                               bool isFromVertex, HepMatrix &V, int& num_m);//
		
  bool isParticleResonance(Particle &p);
	
  bool hasMassConstraint(Vertex &v);

  bool isVertexMeasured(Vertex &v);

  bool isParticleMeasured(Particle &p);

  StatusCode findParticleinTree(Vertex&,int&);
		
  /// Compute degree of freedom  
  StatusCode computeDoF(Vertex &inputVertex, int &nm, int &nu, int &nc);
  StatusCode computeDoF(Particle &inputParticle, int &nm, int &nu, int &nc);

  StatusCode doComputeDoF(Vertex &inputVertex, int &nm, int &nu, int &nc, bool isStartingVertex);
  StatusCode doComputeDoF(Particle &inputParticle, int &nm, int &nu, int &nc);

  /// Estimate unmeasured amounts and transport
  StatusCode estimateAndTransport(HepSymMatrix &Ce, HepVector &e, HepVector &u, Vertex &inputVertex);
  StatusCode estimateAndTransport(HepSymMatrix &Ce, HepVector &e, HepVector &u, Particle &inputParticle);

  StatusCode estimateVertex(Vertex &inputVertex, Particle &pAtVtx);
  StatusCode estimateParticle(Particle &p);
  StatusCode transport(Vertex &inputVertex);
  //DoEmbed
  void doEmbed(Vertex &inputVertex, int &e_embed, int &u_embed, HepSymMatrix &Ce, HepVector &e, HepVector &u);
  void doEmbed(Particle &inputParticle, int &e_embed, int &u_embed, HepSymMatrix &Ce, HepVector &e, HepVector &u);
  //Some Checks
  StatusCode checkMomenta(Vertex &inputVertex, HepVector &e, HepVector &u);
  StatusCode checkMomenta(Particle &inputParticle, HepVector &e, HepVector &u);
  StatusCode doCheck(Vertex &inputVertex, HepVector &e, HepVector &u, int &e_embed, int &u_embed);

		
		
  struct mVect {
    double tx, ty, p, m;
    int i;
    bool f;
  };

  /// Compute the constraints and iterate
  bool computeConstraint(HepVector &e, HepVector &u, Vertex &workingVertex, HepVector &constraint);
  bool computeConstraint(HepVector &e, HepVector &u, Particle &workingParticle, HepVector &constraint);


  void doComputeConstraint(HepVector &e, HepVector &u,  Vertex &workingVertex,
                           int &e_embed, int &u_embed, int &cindex, HepVector &constraint,
                           struct mVect &vp, bool &conv, bool isStartingVertex);

  StatusCode iterateWithLinearConstraint(HepSymMatrix &Ce, HepVector &e, HepVector &u, int &nm, int &nu, int &nc,
                                         Vertex &workingVertex, HepVector &constraint, 
                                         HepMatrix &B, HepMatrix &C, HepMatrix &Minv);
  StatusCode iterateWithLinearConstraint(HepSymMatrix &Ce, HepVector &e, HepVector &u, int &nm, int &nu, int &nc,
                                         Particle &workingParticle, HepVector &constraint, 
                                         HepMatrix &B, HepMatrix &C, HepMatrix &Minv);
 
  /// Solve Linear System with update
  StatusCode solveSystem(HepSymMatrix &Ce, HepMatrix &B, HepMatrix &C,
                         int &nm, int &nu, int &nc, HepVector &constraint, HepVector &de, HepVector &du, HepMatrix &Minv);


  void doIteration(HepVector &e, HepVector &u, HepMatrix &B, HepMatrix &C,
                   Vertex &workingVertex,
                   int &e_embed, int &u_embed,
                   int &cindex, struct mVect &vp, bool isStartingVertex);

  void updateCovariance(HepSymMatrix &Ce, HepSymMatrix &Cu,
                        HepMatrix &Minv, int &nm, int &nu);

  StatusCode checkCovariance(HepSymMatrix &Ce, int &nm);
  StatusCode checkCovariance(HepSymMatrix &Ce, HepSymMatrix &Cu, int &nm, int &nu);

  StatusCode getChiSquare(HepVector &e0, HepVector &e, HepSymMatrix &Ce, double &chis);


  StatusCode fillDecayGraph(HepSymMatrix &Ce0, HepVector &e0, HepSymMatrix &Ce, HepVector &e, HepSymMatrix &Cu,
                            HepVector &u, Vertex &workingVertex, int ndof);
  StatusCode fillDecayGraph(HepSymMatrix &Ce0, HepVector &e0, HepSymMatrix &Ce, HepVector &e, HepSymMatrix &Cu,
                            HepVector &u, Particle &workingParticle, int ndof);

  void doFill(double &chis, HepSymMatrix &Ce, HepVector &e, HepSymMatrix &Cu, 
              HepVector &u, Vertex &workingVertex, int &e_embed, int &u_embed, int ndof);


		
  double lifetime,lifetimeErr;
  std::map< Particle*, int> MParStore; ///< Meausured Particle Pointers Map
  std::map< Particle*, int> UNParStore; ///< UNMeausured Particle Pointers Map
  std::map< Vertex*, int> MVtxStore;  ///<Meausured Vertex Poiters Map
  std::map< Vertex*, int> UNVtxStore;  ///<UNMeausured Vertex Poiters Map
   		
		
  /// Cut on the Width in order to use the daughters ot not
  IParticlePropertySvc* m_ppSvc;        ///< Reference to ParticlePropertySvc
  IToolSvc* m_pToolSvc;
  IVertexFitter* m_pVertexUnconst;      ///< Reference to VertexFitter
  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use      
  IParticleStuffer* m_pStuffer;         ///< Reference to ParticleStuffer
 
  int m_maxIterations; ///< maximum number of linearized iterations
  double m_toleranceResonance; ///< tolerance for distance**2 between origin and end vertex for resonance (in mm**2)
  double m_toleranceMass; ///< tolerance for M**2-m**2 (in MeV**2)
  double m_toleranceVertex; ///< tolerance for x and y of vertex (in mm)
  double m_toleranceMomentum; ///< tolerance for px,py,pz conservation at vertex (in MeV)
  double m_toleranceCorrelation; ///< tolerance for correlation coefficients to exceed 1 
  ///< due to rounding problems in case of 100% correlated variables
 
};

#endif // LAGRANGEGLOBALFITTER_H


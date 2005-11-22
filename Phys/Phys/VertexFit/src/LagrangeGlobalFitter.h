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
 *  
 *  @date   11/16/2005
 *
 * @brief This tool allows the fit of a generic decay tree topology 
 * (Up to now we don't handle the photons) It includes 3 methods to invoke the globalfitting: 
 * <ol> 
 * <li> FIT from Particle </li>  
 * <li> FIT from Vertex </li> 
 * <li> FIT from Vertex with Lifetime </li>
 * </ol>
 * 
 *<em> <b> Examples: </b> </em>
 *
 * <b> Bd --> pi+ pi-  </b>
 *
 * In this example we have 2 final states (pions) coming from the secondary vertex. This vertex is the B's endVertex.
 * So we can build the tree in this way: 
 * <ol> 
 * <li> <b> FIT from Particle: </b> <br>
 * Create the secondary vertex:
 * @code	
 *	Vertex SV;
 * @endcode
 *
 * Add the 2 pions:
 *
 * @code 
 *	Particle mypi[2];
 *	int i=0;
 *	for(ParticleVector::const_iterator iP=StartParts.begin();iP!=StartParts.end();iP++) {
 *		mypi[i]=*(*iP); 
 *		SV.addToProducts(&mypi[i++]); //Very important! We have to pass the copies of the particles
 *	}
 * @endcode
 * Set the mass constraint in the secondary Vertex:
 * @code
 *	globalFitter->setMassConstraint(SV);
 * @endcode
 *
 * Create the B particle with its mass, its PID and its endVertex:
 * @code
 *	Particle newB;
 *	newB.setParticleID(m_BID);
 *	newB.setMass(m_BMass);
 *	newB.setEndVertex(&SV);
 * @endcode
 *
 * Then we can invoke the method fit(Particle &inParticle)
 * @code
 *	StatusCode sc=globalFitter()->fit(newB);
 * @endcode
 * </li>
 * <li> <b> FIT from Vertex: </b> <br>
 * Now we add the information about the primary vertex, in this way we increase the number of constraints:
 * 
 * @code
 *	Vertex *priVtx=0;
 *	//loop on the primary vertices
 *	for(Vertices::const_iterator ivert = Verts.begin(); ivert != Verts.end(); ivert++) {
 *		if((*ivert)->type() == Vertex::Primary) {
 *			nPriVtxDir++;
 *			if (nPriVtxDir==1) priVtx = (*ivert);
 *		}
 *	} 
 *	Vertex Primary=*priVtx;
 * @endcode
 *	
 * Attach the B to the primary vertex 
 * @code
 *	Primary.addToProducts(&newB);
 * @endcode
 * 
 * In this configuration we can invoke the method  fit(Vertex &inVertex)
 *
 * @code
 *	StatusCode sc=globalFitter()->fit(Primary);
 * @endcode
 * </li>
 * <li> <b> FIT from Vertex with Lifetimes: </b> <br>
 * At last we should want to retrieve the B lifetime with its error
 *
 * Select the particle in the tree which we'd like to know its propertime:
 * @code
 *	globalFitter()->setWhichParticleLifetime(&newB);
 * @endcode
 * Then invoke the globalfitting
 * @code
 * vector < double > lf,lferr; 
 * sc=globalFitter()->fit(Primary,lf,lferr);
 * @endcode 
 * </li>
 * </ol>
 *
 * <b> Bd --> J/psi(mu+,mu-), Ks(pi+,pi-)  </b>
 *
 * In this example we have 4 final states (pions and muons) coming from the J/psi and the Ks vertices. 
 The Ks and J/psi come from the endVertex (secondary vertex) of the B. This vertex is the B's endVertex. 
 We have to set the mass constraints in the 3 vertices. Also the Jpsi is a resonance so we have to set a 
 further constraint.We build the tree: <br> 
 * Create the vertices:
 * @code	
 *	Vertex SV, KsVtx, JpsiVtx;
 * @endcode
 *
 * Attach the 2 pions and the 2 muons to relative vertices:
 *
 * @code 
 *	Particle mypi[2];
 *	int i=0;
 *	for(ParticleVector::const_iterator iP=piParts.begin();iP!=piParts.end();iP++) {
 *		mypi[i]=*(*iP); 
 *		KsVtx.addToProducts(&mypi[i++]); 
 *	}
 *	Particle mymu[2];
 *	i=0;
 *	for(ParticleVector::const_iterator iP=muParts.begin();iP!=muParts.end();iP++) {
 *		mymu[i]=*(*iP); 
 *		JpsiVtx.addToProducts(&mymu[i++]); 
 *	}

 * @endcode
 *
 * Make a copy of the particles for the Pull computations
 * @code
 *	Particle mypiprev[2];
 *	mypiprev[0]=mypi[0];
 *	mypiprev[1]=mypi[1];
 *
 *	Particle mymuprev[2];
 *	mymuprev[0]=mymu[0];
 *	mymuprev[1]=mymu[1];
 * @endcode
 * Set the mass constraints in the Vertices:
 * @code
 *	globalFitter->setMassConstraint(SV);
 *	globalFitter->setMassConstraint(KsVtx);
 *	globalFitter->setMassConstraint(JpsiVtx);
 * @endcode
 *
 * Create the B particle with its mass, its PID and its endVertex:
 * @code
 *	Particle newB;
 *	newB.setParticleID(m_BID);
 *	newB.setMass(m_BMass);
 *	newB.setEndVertex(&SV);
 * @endcode
 *
 * Create the Ks particle with its mass, its PID and its endVertex:
 * @code
 *	Particle inKs;
 *	inKs.setParticleID(m_KsID);
 *	inKs.setMass(m_KsMass);
 *	inKs.setEndVertex(&KsVtx);
 * @endcode
 *
 * Create the Jpsi particle with its mass, its PID and its endVertex:
 * @code
 *	Particle inJpsi;
 *	inJpsi.setParticleID(m_JpsiID);
 *	inJpsi.setMass(m_JpsiMass);
 *	inJpsi.setEndVertex(&JpsiVtx);
 * @endcode
 *
 * The Jpsi is a resonance, so:
 * @code 
 *	inJpsi.setIsResonance(true);
 * @endcode
 * Then attach the Ks and Jpsi to the secondary vertex
 * @code
 *	SV.addToProducts(&inJpsi);
 *	SV.addToProducts(&inKs);
 * @endcode
 *
 * We take the primary vertex from MCTruth
 * @code
 *	Vertex *priVtx=0;
 *	//loop on the primary vertices
 *	for(Vertices::const_iterator ivert = Verts.begin(); ivert != Verts.end(); ivert++) {
 *		if((*ivert)->type() == Vertex::Primary) {
 *			nPriVtxDir++;
 *			if (nPriVtxDir==1) priVtx = (*ivert);
 *		}
 *	} 
 *	Vertex Primary=*priVtx;
 * @endcode
 *	
 * Attach the B to the primary vertex 
 * @code
 *	Primary.addToProducts(&newB);
 * @endcode
 *
 * Select the particles in the tree which we'd like to know its propertime:
 * @code
 *	globalFitter()->setWhichParticleLifetime(&newB);
 *	globalFitter()->setWhichParticleLifetime(&inKs);
 * @endcode
 * Then invoke the globalfitting
 * @code
 *	vector < double > lf,lferr; 
 *	sc=globalFitter()->fit(Primary,lf,lferr);
 * @endcode 
 * 
 * At last retrieve the information about Pulls for the pions and muons before and after the fitting:
 *
 * @code
 *	HepVector ParticlePull1(5), ParticlePull2(5); //pi
 *	HepVector ParticlePull3(5), ParticlePull4(5); //muon
 *
 *	sc=globalFitter()->getFitPull(mypiprev[0], mypi[0], ParticlePull1);
 *	if(sc.isFailure()) {
 *		error() << "getFitPull failed" << endreq;
 *		return StatusCode::SUCCESS;
 *	}
 *
 *	sc=globalFitter()->getFitPull(mypiprev[1], mypi[1], ParticlePull2);
 *	if(sc.isFailure()) {
 *		error() << "getFitPull failed" << endreq;
 *		return StatusCode::SUCCESS;
 *	}
 *
 *	sc=globalFitter()->getFitPull(mymuprev[0], mymu[0], ParticlePull3);
 *	if(sc.isFailure()) {
 *		error() << "getFitPull failed" << endreq;
 *		return StatusCode::SUCCESS;
 *	}
 *
 *	sc=globalFitter()->getFitPull(mymuprev[1], mymu[1], ParticlePull4);
 *	if(sc.isFailure()) {
 *		error() << "getFitPull failed" << endreq;
 *		return StatusCode::SUCCESS;
 *	}
 * @endcode
 */



class LagrangeGlobalFitter : public GaudiTool, virtual public IGlobalFitter {
  
public:
	
  ///Standard constructor 
  LagrangeGlobalFitter( const std::string& type, const std::string& name, const IInterface* parent);
		
  ///Destructor
  ~LagrangeGlobalFitter() { 
    MParStore.clear();
    UNParStore.clear();
    MVtxStore.clear();
    UNVtxStore.clear();
    SelectParticle.clear();
    ProductionVertex.clear();
    DecayVertex.clear();
  }
  
  StatusCode initialize();    
  StatusCode finalize();
  
		
  StatusCode fit(Vertex &WorkingVertex);  ///< Fit from a Vertex
  ///Fit from a Vertex and retrieve the propertime and its error of the particle/particles selected 
  StatusCode fit(Vertex &WorkingVertex, std::vector < double >&, std::vector < double >&); 
  StatusCode fit(Particle &WorkingParticle); ///< Fit from a Particle
	
  //Retrieve Pulls
  ///Retrieve the function FitPULL for measured particle computed with all the track parameters transported in the z  
  StatusCode getFitPull(Particle &previousP, Particle &afterP, HepVector &pull);
  /// Retrieve the function FitPULL for a measured particle  
  StatusCode getFitPull(Vertex &previousV, Vertex &afterP, HepVector &pull); 

  ///Set mass constraint	
  void setMassConstraint(Vertex &v);

  ///Set which particle to compute the lifetime
  void setWhichParticleLifetime(Particle *);
		
		
#define M_C_AT_VTX 0x80
  
private:

  //Particle to compute the lifetime
  
  std::vector < Particle* > SelectParticle;
  std::vector < Vertex* > ProductionVertex;
  std::vector < Vertex* > DecayVertex;  

  //Compute the lifetime and its error
	  
  StatusCode LagrangeGlobalFitter::lifeTimeCalculator(Vertex &outputVertex, HepMatrix &V, int indexwhichParticle,
                                                      int& num_m, double &lftime, double &lftimeerr);
		
  //General internal fit method
  StatusCode fitFromEverything(Vertex &inVertex,Particle &inParticle, 
                               bool isFromVertex, HepMatrix &V, int& num_m);//
		
  bool isParticleResonance(Particle &p); ///< is a resonance?
	
  bool hasMassConstraint(Vertex &v); ///< has this vertex a mass constraint?

  bool isVertexMeasured(Vertex &v); ///< is measured?

  bool isParticleMeasured(Particle &p); ///< is meausered?

  /// find in the tree the particle and set 1 the variable f
  StatusCode findParticleinTree(Vertex&,int indexwhichParticle, int& f); 
		
  ///Compute degree of freedom  
  StatusCode computeDoF(Vertex &inputVertex, int &nm, int &nu, int &nc); 
  StatusCode computeDoF(Particle &inputParticle, int &nm, int &nu, int &nc); 

  StatusCode doComputeDoF(Vertex &inputVertex, int &nm, int &nu, int &nc, bool isStartingVertex);
  StatusCode doComputeDoF(Particle &inputParticle, int &nm, int &nu, int &nc);

  ///Estimate unmeasured amounts and transport
  StatusCode estimateAndTransport(HepSymMatrix &Ce, HepVector &e, HepVector &u, Vertex &inputVertex); 
  StatusCode estimateAndTransport(HepSymMatrix &Ce, HepVector &e, HepVector &u, Particle &inputParticle);

  StatusCode estimateVertex(Vertex &inputVertex, Particle &pAtVtx);
  StatusCode estimateParticle(Particle &p);
  StatusCode transport(Vertex &inputVertex);
  //DoEmbed
  void doEmbed(Vertex &inputVertex, int &e_embed, int &u_embed, HepSymMatrix &Ce, HepVector &e, HepVector &u);
  void doEmbed(Particle &inputParticle, int &e_embed, int &u_embed, HepSymMatrix &Ce, HepVector &e, HepVector &u);
  ///Some Checks
  StatusCode checkMomenta(Vertex &inputVertex, HepVector &e, HepVector &u);
  StatusCode checkMomenta(Particle &inputParticle, HepVector &e, HepVector &u);
  StatusCode doCheck(Vertex &inputVertex, HepVector &e, HepVector &u, int &e_embed, int &u_embed);

		
		
  struct mVect {
    double tx, ty, p, m;
    int i;
    bool f;
  };

  ///Compute the constraints and iterate
  bool computeConstraint(HepVector &e, HepVector &u, Vertex &workingVertex, HepVector &constraint);
  bool computeConstraint(HepVector &e, HepVector &u, Particle &workingParticle, HepVector &constraint);


  void doComputeConstraint(HepVector &e, HepVector &u,  Vertex &workingVertex,
                           int &e_embed, int &u_embed, int &cindex, HepVector &constraint,
                           struct mVect &vp, bool &conv, bool isStartingVertex);

  StatusCode iterateWithLinearConstraint(HepSymMatrix &Ce, HepVector &e, HepVector &u, int &nm, int &nu, int &nc,
                                         Vertex &workingVertex, HepVector &constraint, HepMatrix &B, 
                                         HepMatrix &C, HepMatrix &Minv);
  StatusCode iterateWithLinearConstraint(HepSymMatrix &Ce, HepVector &e, HepVector &u, int &nm, int &nu, int &nc,
                                         Particle &workingParticle, HepVector &constraint, HepMatrix &B, 
                                         HepMatrix &C, HepMatrix &Minv);
 
  ///Solve Linear System with update
  StatusCode solveSystem(HepSymMatrix &Ce, HepMatrix &B, HepMatrix &C,
                         int &nm, int &nu, int &nc, HepVector &constraint, HepVector &de, 
                         HepVector &du, HepMatrix &Minv);


  void doIteration(HepVector &e, HepVector &u, HepMatrix &B, HepMatrix &C,
                   Vertex &workingVertex,
                   int &e_embed, int &u_embed,
                   int &cindex, struct mVect &vp, bool isStartingVertex);

  void updateCovariance(HepSymMatrix &Ce, HepSymMatrix &Cu,
                        HepMatrix &Minv, int &nm, int &nu);

  StatusCode checkCovariance(HepSymMatrix &Ce, int &nm);
  StatusCode checkCovariance(HepSymMatrix &Ce, HepSymMatrix &Cu, int &nm, int &nu);

  StatusCode getChiSquare(HepVector &e0, HepVector &e, HepSymMatrix &Ce, double &chis);

  /// Update the entire decay tree	
  StatusCode fillDecayGraph(HepSymMatrix &Ce0, HepVector &e0, HepSymMatrix &Ce, HepVector &e, 
                            HepSymMatrix &Cu,HepVector &u, Vertex &workingVertex, int ndof);
  StatusCode fillDecayGraph(HepSymMatrix &Ce0, HepVector &e0, HepSymMatrix &Ce, HepVector &e,
                            HepSymMatrix &Cu,HepVector &u, Particle &workingParticle, int ndof); 

  void doFill(double &chis, HepSymMatrix &Ce, HepVector &e, HepSymMatrix &Cu, 
              HepVector &u, Vertex &workingVertex, int &e_embed, int &u_embed, int ndof);


  std::map< Particle*, int> MParStore; ///< Meausured Particle Pointers Map
  std::map< Particle*, int> UNParStore; ///< UNMeausured Particle Pointers Map
  std::map< Vertex*, int> MVtxStore;  ///< Meausured Vertex Poiters Map
  std::map< Vertex*, int> UNVtxStore;  ///< UNMeausured Vertex Poiters Map
   		
		
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
  /// tolerance for correlation coefficients to exceed 1 due to rounding problems in case of 100% correlated variables
  double m_toleranceCorrelation; 
 
};

#endif // LAGRANGEGLOBALFITTER_H

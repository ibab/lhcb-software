// $Id: IGlobalFitter.h,v 1.2 2005-11-22 10:05:50 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IGLOBALFITTER_H 
#define DAVINCIKERNEL_IGLOBALFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class Vertex;
class Particle;
class StatusCode;
class IParticlePropertySvc;


static const InterfaceID IID_IGlobalFitter("IGlobalFitter", 2 , 1);

/** @interface IGlobalFitter
 *  Interface Class for fitting an arbitrary decay topology.
 *
 *  @author G.Balbi, A. Carbone, V. Vagnoni, S.Vecchi  
 *  
 *  @date   11/11/2005
 *
 * @brief This tool allows the fit of a generic decay tree topology (Up to now we don't handle the photons) 
 It includes 3 methods to invoke the globalfitting: 
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
 So we can build the tree in this way: 
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
 * In this example we have 4 final states (pions and muons) coming from the J/psi and the Ks vertices. The Ks and J/psi come 
 from the endVertex (secondary vertex) of the B. This vertex is the B's endVertex. We have to set the mass constraints in the 
 3 vertices. Also the Jpsi is a resonance so we have to set a further constraint. We build the tree: <br> 
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

class IGlobalFitter : virtual public IAlgTool {
  
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGlobalFitter; }
  
  virtual StatusCode fit(Vertex &inVertex)=0;  ///< Fit from a Vertex
  /// Fit from a Vertex and retrieve the propertime and its error of the particle/particles selected
  virtual StatusCode fit(Vertex &inVertex, std::vector < double >& time, std::vector < double >& timeerr)=0; 
  virtual StatusCode fit(Particle &inParticle)=0; ///< Fit from a Particle

  virtual StatusCode getFitPull(Vertex&, Vertex&, HepVector&)=0; ///< Retrieve the function FitPULL for 2 vertices
  virtual StatusCode getFitPull(Particle&, Particle&, HepVector&)=0; ///< Retrieve the function FitPULL for 2 particles

  virtual void setMassConstraint(Vertex &v)=0; ///< Set the mass constraint in the vertex v
  ///	Mark in the tree which particles, one at a time, we want to retrieve the propertime and the error 
  virtual void setWhichParticleLifetime(Particle*)=0; 
};

#endif // DAVINCIKERNEL_IGLOBALFITTER_H

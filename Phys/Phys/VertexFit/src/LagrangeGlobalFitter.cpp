// Exps program $

// Include files from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IParticlePropertySvc.h"

#include "Kernel/CLHEPStreams.h"

// local file
#include "LagrangeGlobalFitter.h"
#include "Kernel/IParticleTransporter.h"
#include <iostream>
#include <iomanip>

//--------------------------------------------------------------------
//
//  ClassName  : LagrangeGlobalFitter
// 
//  Description: Perform a kinematic and geometric fit for every decay topology
//  @Author     : G.Balbi A.Carbone V. Vagnoni S.Vecchi
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<LagrangeGlobalFitter> s_factory;
const IToolFactory& LagrangeGlobalFitterFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
LagrangeGlobalFitter::LagrangeGlobalFitter(const std::string& type, 
                                           const std::string& name, 
                                           const IInterface* parent) 
  : GaudiTool( type, name, parent ),
    m_ppSvc(0),
    m_pVertexUnconst(0),
    m_pTransporter(0),
    m_transporterType("CombinedTransporter"),
    m_maxIterations(30),
    m_toleranceResonance(1e-4), // in mm distanza fra i 2 vertici della risonanza
    m_toleranceMass(1), // in MeV**2
    m_toleranceVertex(1e-4), // in mm
    m_toleranceMomentum(1e-4), // in MeV
    m_toleranceCorrelation(1e-5) {
	
	declareInterface<IGlobalFitter>(this);
	
	declareProperty("Transporter", m_transporterType);
	declareProperty("maxIterations", m_maxIterations);
	declareProperty("toleranceResonance", m_toleranceResonance);
	declareProperty("toleranceMass", m_toleranceMass);
	declareProperty("toleranceVertex", m_toleranceVertex);
	declareProperty("toleranceMomentum", m_toleranceMomentum);
	declareProperty("toleranceCorrelation", m_toleranceCorrelation);
	
}

//==================================================================
// Initialize
//==================================================================
StatusCode LagrangeGlobalFitter::initialize() {
	
	
	// This tool needs to use internally the ParticlePropertySvc to retrieve the
	// mass to be used
	StatusCode sc = StatusCode::FAILURE;
	sc = service( "ParticlePropertySvc", m_ppSvc );
	if( sc.isFailure ()) {
		fatal() << "ParticlePropertySvc Not Found" << endmsg;
		return StatusCode::FAILURE;
	}
	
	sc = toolSvc()->retrieveTool(m_transporterType, 
                               m_pTransporter, this);
	if(sc.isFailure()) {
		fatal() << " Unable to retrieve ParticleTransporter tool" ;
		return sc;
	}
	
	sc = toolSvc()->retrieveTool("UnconstVertexFitter",
                               m_pVertexUnconst,this);
	if(sc.isFailure()) {
		fatal() << "Unable to retrieve Uncosntr. Fitter tool"
            << endmsg ;
		return sc;
	}
	
	sc = toolSvc()->retrieveTool("ParticleStuffer", m_pStuffer, this);
	if( sc.isFailure() ) {
		fatal() << "Unable to retrieve ParticleStuffer tool" << endmsg;
		return StatusCode::FAILURE;
	}
	return StatusCode::SUCCESS;
	
}

//=========================================================================
// Finalize
//=========================================================================
StatusCode LagrangeGlobalFitter::finalize() {
	
	debug() << "Release tools" << endmsg;
	if( m_pTransporter   ) toolSvc()->releaseTool( m_pTransporter );
	if( m_pVertexUnconst ) toolSvc()->releaseTool( m_pVertexUnconst );
	if( m_pStuffer )       toolSvc()->releaseTool( m_pStuffer );
	
	return StatusCode::SUCCESS;
}


//========================================================================
//
//========================================================================

StatusCode LagrangeGlobalFitter::lifeTimeCalculator(Vertex &outputVertex, HepMatrix &Minv, int indexwhichParticle, int& nm, 
                                                    double &lftime, double &lftimeerr) {
	
	
	int ind_prod=0; 
	int ind_dec=0;
	int ind_par=0;
	int f=1;

	if (findParticleinTree(outputVertex, indexwhichParticle, f).isFailure())
	{
		error() << "Failed during finding in tree "<< endmsg;
		return StatusCode::FAILURE;
	}

	if (!f)
		debug() << " SelectedParticle found " << endmsg;
	else
	{
		error() << " SelectedParticle not found " << endmsg;	
		return StatusCode::FAILURE;	
	}

	
	std::map <Vertex*, int>::iterator iV=MVtxStore.find(ProductionVertex[indexwhichParticle]);
	if (iV==MVtxStore.end())
  {
    iV=UNVtxStore.find(ProductionVertex[indexwhichParticle]);
    if (iV==UNVtxStore.end())
    {
      error() << "Lifetime Computation: Production Vertex not found" << endmsg;	
      return StatusCode::FAILURE;
    }	
    ind_prod+=nm;
  }
	ind_prod+=(iV->second);
	debug() << "Production vertex starting at index:" << ind_prod<< endmsg;

	
	debug() << "I'm looking for vertex "<< DecayVertex[indexwhichParticle] << endmsg;
	iV=MVtxStore.find(DecayVertex[indexwhichParticle]);
	if (iV==MVtxStore.end())
	{
		iV=UNVtxStore.find(DecayVertex[indexwhichParticle]);
		if (iV==UNVtxStore.end())
		{
			error() << "Lifetime Computation: Decay Vertex not found" << endmsg;	
			return StatusCode::FAILURE;
		}
		ind_dec+=nm;
	}
	ind_dec+=(iV->second);
	debug() << "Decay vertex starting at index:" << ind_dec<< endmsg;

	debug() << "I'm looking for particle "<< SelectParticle[indexwhichParticle] << endmsg;
	std::map <Particle*, int>::iterator iP=MParStore.find(SelectParticle[indexwhichParticle]);
	if (iP==MParStore.end())
	{
		
		iP=UNParStore.find(SelectParticle[indexwhichParticle]);
		if (iP==UNParStore.end())
		{
			error() << "Lifetime Computation: Particle not found" << endmsg;	
			return StatusCode::FAILURE;
		}
		ind_par+=nm;
	}
	ind_par+=(iP->second);
	debug() <<  "Particle starting at index:" << ind_par<< endmsg;
	
	HepPoint3D PVtx=ProductionVertex[indexwhichParticle]->position();	
	HepPoint3D SVtx=DecayVertex[indexwhichParticle]->position();	
	double tx,ty,p_p;
	
	tx=SelectParticle[indexwhichParticle]->slopeX();
	ty=SelectParticle[indexwhichParticle]->slopeY();
	p_p=SelectParticle[indexwhichParticle]->p();
	
	//Some numerical tricks
	double zz=sqrt(1+(tx*tx)+(ty*ty));
	double z1=1./p_p;
	double z2=1./zz;
	double z4=1./(1+(tx*tx)+(ty*ty));
	
	HepPoint3D mom=(SelectParticle[indexwhichParticle]->momentum()).vect();
	
	double massa=SelectParticle[indexwhichParticle]->mass();
	lftime=massa*(SVtx-PVtx).dot(mom)*z1*z1; 		
	
	debug() <<"Lifetime by GlobalFitter " << lftime <<endmsg;
	
	
	//Error on Lifetime	
	
	double l_pvz = -massa*z1*z2;
	double l_pvx = l_pvz*tx;
	double l_pvy = l_pvz*ty;
	
	
	double l_svx =-l_pvx;
	double l_svy =-l_pvy;
	double l_svz =-l_pvz;
	
	double l_p=-lftime*z1;
	double step1_tx=-massa*(PVtx[0]-SVtx[0])*z1*z2;	
	double step1_ty=-massa*(PVtx[1]-SVtx[1])*z1*z2;
	double step12=( PVtx[2]+tx*(PVtx[0]-SVtx[0])+ty*(PVtx[1]-SVtx[1])-SVtx[2] );
	double step2_tx=z1*massa*tx*step12*z4*z2;	
	double step2_ty=z1*massa*ty*step12*z4*z2;	
	
	debug() << " step1_tx " <<step1_tx << endmsg;  
	debug() << " step1_ty " <<step1_ty << endmsg;  
	debug() << " step12 " << step12 << endmsg;
	debug() << " step2_tx " << step2_tx << endmsg;
	debug() << " step2_ty  " << step2_ty << endmsg;
	
	double l_tx=step1_tx+step2_tx;
	double l_ty=step1_ty+step2_ty;
	
	debug() << " tx " <<tx << endmsg;  
	debug() << " ty " <<ty << endmsg;  
	debug() << " p_p " <<p_p << endmsg;
	debug() << " massa " << massa << endmsg;
	debug() << " zz  " << zz << endmsg;
	
	debug() << " PVtx[0] "  << PVtx[0] << endmsg;  
	debug() << " PVtx[1] "  << PVtx[1] << endmsg;  
	debug() << " PVtx[2] "  << PVtx[2] << endmsg;
	
	debug() << " SVtx[0] "  << SVtx[0] << endmsg;  
	debug() << " SVtx[1] "  << SVtx[1] << endmsg;  
	debug() << " SVtx[2] "  << SVtx[2] << endmsg;
	
	debug() << " Derivative on PVx" << l_pvx << endmsg;
	debug() << " Derivative on PVy" << l_pvy << endmsg;
	debug() << " Derivative on PVz" << l_pvz << endmsg;
	
	debug() << " Derivative on SVx" << l_svx << endmsg;
	debug() << " Derivative on SVy" << l_svy << endmsg;
	debug() << " Derivative on SVz" << l_svz << endmsg;  
	
	debug()  << " Derivative on tx" << l_tx  << endmsg;
	debug()  << " Derivative on ty" << l_ty  << endmsg;
	debug()  << " Derivative on p"  << l_p   << endmsg;
	
	
	HepMatrix J(1,9);
	
	// Derivatives Matrix
	J(1,1)=l_pvx;
	J(1,2)=l_pvy;
	J(1,3)=l_pvz;        
	
	J(1,4)=l_svx;
	J(1,5)=l_svy;
	J(1,6)=l_svz;
	
	J(1,7)=l_tx;
	J(1,8)=l_ty;
	J(1,9)=l_p;
	
	HepMatrix V(9,9);
	
	
	V.sub(1,1,Minv.sub(ind_prod,ind_prod+2,ind_prod,ind_prod+2)); //ok PV   
	V.sub(4,4,Minv.sub(ind_dec,ind_dec+2,ind_dec,ind_dec+2));//ok ** SV 	  
  V.sub(7,7,Minv.sub(ind_par+3,ind_par+3+2,ind_par+3,ind_par+3+2));//ok ** BS'
			
  V.sub(4,1,Minv.sub(ind_dec,ind_dec+2,ind_prod,ind_prod+2));//ok ** A2 //correlation between PV and SV
  V.sub(7,1,Minv.sub(ind_par+3,ind_par+3+2,ind_prod,ind_prod+2));//ok A1 //correlation between PV and the tx,ty,p of the Particle
  V.sub(4,7,Minv.sub(ind_dec,ind_dec+2,ind_par+3,ind_par+3+2));//ok B1 //correlation between SV and the tx,ty,p of the Particle
  V.sub(7,4,(Minv.sub(ind_dec,ind_dec+2,ind_par+3,ind_par+3+2)).T());//ok ** B1.T() 
  V.sub(1,7,(Minv.sub(ind_par+3,ind_par+3+2,ind_prod,ind_prod+2)).T());//ok ** A1.T() 
  V.sub(1,4,(Minv.sub(ind_dec,ind_dec+2,ind_prod,ind_prod+2)).T());//ok A2.T()
									
  //------------ Output for MATHEMATICA check ------------------------------
  /*
    cout << "--------=-=-=-====-=-=-=-=-=-=-=-=-=-=-==-="<<endl;	
    std::cout << " { ";
    for(int i=0; i<V.num_row(); i++){
    std::cout << " { ";
    for(int ii=0; ii<(V.num_col()-1); ii++ ) std::cout << fixed << V[i][ii] << ",  ";
    std::cout << V[i][V.num_col()-1] << "}, " << endl;     
    }		
    cout << "--------=-=-=-====-=-=-=-=-=-=-=-=-=-=-==-="<<endl;
  */
  //------------End Output for MATHEMATICA check ------------------------------
									
  lftimeerr=sqrt((J*V*(J.T()))(1,1));
  debug() << "Error on lifetime " << lftimeerr << endmsg;
									
  return StatusCode::SUCCESS;
}

StatusCode LagrangeGlobalFitter::getFitPull(Particle &previousP, Particle &afterP, HepVector &pull) {
	Particle transParticle;
	StatusCode sc = m_pTransporter->transport(previousP, afterP.pointOnTrack().z(), transParticle);
	if(sc.isFailure()) return sc;
	pull(1)=(transParticle.pointOnTrack().x()-afterP.pointOnTrack().x())/
		sqrt(transParticle.pointOnTrackErr()(1,1)-afterP.pointOnTrackErr()(1,1)); // x
	pull(2)=(transParticle.pointOnTrack().y()-afterP.pointOnTrack().y())/
		sqrt(transParticle.pointOnTrackErr()(2,2)-afterP.pointOnTrackErr()(2,2)); // y
	pull(3)=(transParticle.slopeX()-afterP.slopeX())/
		sqrt(transParticle.slopesMomErr()(1,1)-afterP.slopesMomErr()(1,1)); // tx
	pull(4)=(transParticle.slopeY()-afterP.slopeY())/
		sqrt(transParticle.slopesMomErr()(2,2)-afterP.slopesMomErr()(2,2)); // ty
	pull(5)=(transParticle.p()-afterP.p())/
		sqrt(transParticle.slopesMomErr()(3,3)-afterP.slopesMomErr()(3,3)); // p
	return StatusCode::SUCCESS;
}

StatusCode LagrangeGlobalFitter::getFitPull(Vertex &previousV, Vertex &afterV, HepVector &pull) {
	pull(1)=(previousV.position().x()-afterV.position().x())/
    sqrt(previousV.positionErr()(1,1)-afterV.positionErr()(1,1)); // x
	pull(2)=(previousV.position().y()-afterV.position().y())/
		sqrt(previousV.positionErr()(2,2)-afterV.positionErr()(2,2)); // y
	pull(3)=(previousV.position().z()-afterV.position().z())/
		sqrt(previousV.positionErr()(3,3)-afterV.positionErr()(3,3)); // z
	return StatusCode::SUCCESS;
}


//========================================================================
//
//========================================================================
StatusCode LagrangeGlobalFitter::fit(Vertex &inVertex)  {
    
	//Fit From Vertex
	HepMatrix Minv;
	int nm;
	Particle dummy;  
	StatusCode sc=fitFromEverything(inVertex, dummy, true, Minv, nm);
	if (sc.isFailure()) return StatusCode::FAILURE;
	
	return StatusCode::SUCCESS;
	
}

StatusCode LagrangeGlobalFitter::fit(Vertex &inVertex, std::vector < double > &lifeTime, std::vector < double > &lifeTimeErr)  
{ 
	//Fit from Vertex and retrieve lifetime and its error
	double lfTime, lfTimeErr;
	HepMatrix Minv;
	int nm;
	Particle dummy;
	StatusCode sc=fitFromEverything(inVertex, dummy, true, Minv, nm);
	if (sc.isFailure()) return StatusCode::FAILURE;
	
	for (unsigned int i=0; i< SelectParticle.size(); i++)
	{
		lifeTimeCalculator(inVertex, Minv, i, nm, lfTime, lfTimeErr);
		lifeTime.push_back(lfTime);
		lifeTimeErr.push_back(lfTimeErr);
	}
	return StatusCode::SUCCESS;
}

StatusCode LagrangeGlobalFitter::fit(Particle &inParticle)  {
	debug() << "fit from Particle..." << endmsg;
	HepMatrix Minv;
	int nm;
	Vertex dummy;
	return fitFromEverything(dummy, inParticle, false, Minv, nm);
	
}


StatusCode LagrangeGlobalFitter::fitFromEverything(Vertex &inVertex, Particle &inParticle, bool isFromVertex, 
                                                   HepMatrix &V, int& num_m) {
	
	info() << "Starting global fit ...." << endmsg;
	
	StatusCode sc;
	
	Vertex *workingVertex=&inVertex;
	Particle *workingParticle=&inParticle;
	
	debug() << "Compute NdF" << endmsg;
	// then compute number of measurements, of unmeasurements, of contraints;
	int nm, nu, nc;
	if(isFromVertex) {
		sc = computeDoF(*workingVertex, nm, nu, nc);
	} else {
		sc = computeDoF(*workingParticle, nm, nu, nc);
	}
	if(sc.isFailure()) {
		error() << "cannot compute DoF" << endmsg;
		return StatusCode::FAILURE;
	}
	
	debug() << "Number of measured variables   " << nm << endmsg;
	debug() << "Number of unmeasured variables " << nu << endmsg;
	debug() << "Number of constraints           " << nc << endmsg;
	debug() << "Number of degrees of freedom    " << nc-nu << endmsg;
	
	num_m=nm;
	
	// trivial check if constraints are sufficient to close the problem (not conclusive...)
	if(nc-nu==0) {
		warning()<<"The problem with 0DoF can be simply a transformation from measured to unmeasured variables.Is it what you want?"<<
      endmsg;
	} else if(nc-nu<0) {
		error() << "No DoF to close the problem... giving up" << endmsg;
		return StatusCode::FAILURE;
	}
	
	
	// Second get initial estimate of unmeasured variables and transport measurements to approximate vertices
	// (in the meanwhile fill vector of measurements, its covariance and vector of unmeasured 
	HepSymMatrix Ce(nm, 0);
	HepVector e(nm,0);
	HepVector u(nu,0);
	if(isFromVertex) {
		sc = estimateAndTransport(Ce, e, u, *workingVertex);
	} else {
		sc = estimateAndTransport(Ce, e, u, *workingParticle);
	}
	if(sc.isFailure()) {
		error() << "cannot get initial estimates" << endmsg;
		return StatusCode::FAILURE;
	}
	sc = checkCovariance(Ce, nm);
	if (sc.isFailure() ) {
		error() << "covariance matrix after transport check failed... something fishy" << endmsg;
		return StatusCode::FAILURE;
	}
	
	debug()<< "e " << e << endmsg;
	debug()<< "u " << u << endmsg;
	
	// copy parameter vector to save initial values (later used to compute chi2)
	HepVector e0(e); 
	// save covariance matrix to be used for chi2 calculation
	HepSymMatrix Ce0(Ce);
	
	HepMatrix B_m(nc, nm, 0);
	HepMatrix C_m(nc, nu, 0);
	HepMatrix Minv_m(nm+nu+nc, nm+nu+nc, 0);
	
	// now start loop of linearized constraints
	bool final = false; // convergence flag
	int  icount= 0; // number of iterations counter
  // iterate with linearized constraints until convergence
  //---------------------------------------------------------------------------- iterate over icount
	while (icount < m_maxIterations) {
		
		// get constraint unmbalance and check them
		HepVector constraint(nc,0); // vector of non linear constraints
		if(isFromVertex) {
			final=computeConstraint(e, u, *workingVertex, constraint);
		} else {
			final=computeConstraint(e, u, *workingParticle, constraint);
		}
		
		// check that momenta are physical
		if(isFromVertex) {
			sc = checkMomenta(*workingVertex, e, u);
		} else {
			sc = checkMomenta(*workingParticle, e, u);
		}
		
		if ( sc.isFailure() ) 
		{
			error() << "momenta out of range " << endmsg;
			return StatusCode::FAILURE;
		}
		
		
		// converged !!
		if(final) break;
		
		
		// update variables e and u with linearized constraints
		HepMatrix B(nc, nm, 0);
		HepMatrix C(nc, nu, 0);
		HepMatrix Minv(nm+nu+nc, nm+nu+nc, 0);
		if(isFromVertex) {
			sc = iterateWithLinearConstraint(Ce, e, u, nm, nu, nc, *workingVertex, constraint, B, C, Minv);
		} else {
			sc = iterateWithLinearConstraint(Ce, e, u, nm, nu, nc, *workingParticle, constraint, B, C, Minv);
		}
		if ( sc.isFailure() ) {
			error() << "iteration " << icount+1 << " failed" << endmsg;
			return StatusCode::FAILURE;  
		}
		
		
		// save maxtrices for subsequent usage
		B_m=B;
		C_m=C;
		Minv_m=Minv;
		
		
		icount = icount + 1;
		
		debug()<< "Treasure Map" << endmsg;
		debug()<< "Measured Particles " << endmsg;
		for (std::map<Particle*,int>::iterator i=MParStore.begin();i!=MParStore.end();i++)
			debug() << i->second << " " << " " << i->first << endmsg;
		
		debug()<< "UnMeasured Particles " << endmsg;
		for (std::map<Particle*,int>::iterator i=UNParStore.begin();i!=UNParStore.end();i++)
			debug() << i->second << " " << " " << i->first << endmsg;
		
		debug()<< "Measured Vertexes " << endmsg;
		for (std::map<Vertex*,int>::iterator i=MVtxStore.begin();i!=MVtxStore.end();i++)
			debug() << i->second << " " << " " << i->first << endmsg;
		
		debug()<< "UnMeasured Vertexes " << endmsg;
		for (std::map<Vertex*,int>::iterator i=UNVtxStore.begin();i!=UNVtxStore.end();i++)
			debug() << i->second << " " << " " << i->first << endmsg;		
		
	}
	
	
	//---------------------------------------------------------------------------- iterate over icount
	
	
	// if not converged, return
	if(!final) {
		error() << "Sorry, no convergence" << endmsg;
		error()<< "Reached maximum number of iterations = " << icount << endmsg;
		error() << "If you think this event should be reconstructed and if you have a huge number of constraints,";
    error() << "try to increase maxIterations in job options" << endmsg;
		error() << "If it doesn't help, please contact experts" << endmsg;
		return StatusCode::FAILURE; 
	} else {
		info() << "GlobalFitter converged after " << icount << " iterations!" << endmsg;
	}
	
	
	// update the covariance matrix
	HepSymMatrix Cu(nu,0);
	
	updateCovariance(Ce, Cu, Minv_m, nm, nu);
	
	// check the covariance matrix
	sc = checkCovariance(Ce, Cu, nm, nu);
	if (sc.isFailure() ) {
		error() << "covariance matrix check failed... something fishy" << endmsg;
		return StatusCode::FAILURE;
	}
	
	// build up output graph
	
	if(isFromVertex) {
		////SV
		for(SmartRefVector<Particle>::iterator it=workingVertex->products().begin();
        it!=workingVertex->products().end();
        it++) {
			debug() << " z Particle" << (*it)->pointOnTrack().z()<<" zo "<< u(3) <<endmsg;
		}
		
		sc=fillDecayGraph(Ce0, e0, Ce, e, Cu, u, *workingVertex, nc-nu);
		if(sc.isFailure()) return StatusCode::FAILURE;
		
	} else {
		sc=fillDecayGraph(Ce0, e0, Ce, e, Cu, u, *workingParticle, nc-nu);
		if(sc.isFailure()) return StatusCode::FAILURE;
	}
	
	V=Minv_m;
	// exit with deserved success!
	return StatusCode::SUCCESS;
}





void LagrangeGlobalFitter::setMassConstraint(Vertex &v) {
	
	v.setType(Vertex::VertexType(v.type()|M_C_AT_VTX));
}

bool LagrangeGlobalFitter::hasMassConstraint(Vertex &v) {
	
	if(v.type()&M_C_AT_VTX) return true;
	return false;
}

bool LagrangeGlobalFitter::isVertexMeasured(Vertex &v) {
	
	return ((v.type()&(0xff-M_C_AT_VTX))!=Vertex::Unknown);
}

bool LagrangeGlobalFitter::isParticleMeasured(Particle &p) {
	
	return (p.origin()!=NULL);
}



bool LagrangeGlobalFitter::isParticleResonance(Particle &p) {
	return(p.isResonance());
}


StatusCode LagrangeGlobalFitter::computeDoF(Vertex &inputVertex, int &nm, int &nu, int &nc) {
	
	nm=nu=nc=0;
	return doComputeDoF(inputVertex, nm, nu, nc, true);
	
}

StatusCode LagrangeGlobalFitter::computeDoF(Particle &inputParticle, int &nm, int &nu, int &nc) {
	
	nm=nu=nc=0;
	return doComputeDoF(inputParticle, nm, nu, nc);
	
}


StatusCode LagrangeGlobalFitter::doComputeDoF(Particle &inputParticle, int &nm, int &nu, int &nc) {
	
	
	
	if(!inputParticle.particleID().pid()) {
		error() << "Particle without PID in decay graph. Fishy topology. Giving up." << endmsg;
		return StatusCode::FAILURE;         
	}
	
	if(inputParticle.mass()<0) {
		error() << "Particle without mass in decay graph. Fishy topology. Giving up." << endmsg;
		return StatusCode::FAILURE;         
	}
	
	if(!inputParticle.endVertex()) {
		error() << "A starting particle must have an end vertex. Unimplemented feature or fishy topology. Giving up." << endmsg;
		return StatusCode::FAILURE;
	}  
	
	if(isParticleMeasured(inputParticle)) {
		error() << "A measured particle cannot have an end vertex. Unimplemented feature or fishy topology. Giving up." << endmsg;
		return StatusCode::FAILURE;
	}
	
	nu+=5; // particle is just created, i.e. unmeasured
	
	// force particle to end vertex
	nc+=2;
	
	
	// go ahead
	doComputeDoF(*inputParticle.endVertex(), nm, nu, nc, false);
	
	return StatusCode::SUCCESS;
}

StatusCode LagrangeGlobalFitter::doComputeDoF(Vertex &inputVertex, int &nm, int &nu, int &nc, bool isStartingVertex) {
	
	
	
	debug() << "entering doComputeDoF" << endmsg;
	
	if(!isVertexMeasured(inputVertex)) {
		if(inputVertex.products().size()<2) {
			error() << "An unmeasured vertex must have more than one particle out of it. Fishy topology. Giving up." << endmsg;
			return StatusCode::FAILURE;
		}
		nu+=3; // vertex is just created, i.e. unmeasured
	} else {
		if(!isStartingVertex) {
			error() << "A measured vertex can only be the starting vertex of the graph.";
      error() << "Unimplemented feature or fishy topology.Giving up." << endmsg;
			return StatusCode::FAILURE;
		}
		nm+=3; // vertex is measured
		if(inputVertex.parent()) {
			error() << "You are passing a persistent Vertex instead of your own copy. Cannot overwrite. Giving up" << endmsg;
			return StatusCode::FAILURE;
		}
	}
	
	if(!isStartingVertex) {
		if(hasMassConstraint(inputVertex)) {
			nc++; // mass constraint is applied in the vertex add 1 constraint
		}
		nc+=3; // add conservation of 3-momentum
	} else if(hasMassConstraint(inputVertex)){
		error() << "Cannot apply mass constraint on starting vertex. Use fitFromParticle instead. Giving up." << endmsg;
		return StatusCode::FAILURE;    
	}
	
	debug() << nc << endmsg;
	
	for(SmartRefVector<Particle>::iterator it=inputVertex.products().begin();
      it!=inputVertex.products().end();
      it++) {
		
		if(!(*it)->particleID().pid()) {
			error() << "Particle without PID in decay graph. Fishy topology. Giving up." << endmsg;
			return StatusCode::FAILURE;         
		}
		
		if((*it)->mass()<0) {
			error() << "Particle without mass in decay graph. Fishy topology. Giving up." << endmsg;
			return StatusCode::FAILURE;         
		}
		
		if(!isParticleResonance(**it))     nc+=2; // constrain particle to come from the vertex (only NOT RESONANCES)
		
		if(isParticleMeasured(*(*it))) {
			if((*it)->endVertex()) {
				error() << "A measured particle cannot have an end vertex. Unimplemented feature or fishy topology. Giving up." << endmsg;
				return StatusCode::FAILURE;
			}
			nm+=5; // particle is measured
			if((*it)->parent()) {
				error() << "You are passing a persistent Particle instead of your own copy. Cannot overwrite. Giving up" << endmsg;
				return StatusCode::FAILURE;
			}
		}
		else {
			if(!(*it)->endVertex()) {
				error() << "A unmeasured particle must have an end vertex. Unimplemented feature or fishy topology. Giving up." << endmsg;
				return StatusCode::FAILURE;
			}
			nu+=5; // particle is unmeasured (just created)
		}
		
		Vertex *endVertex=(*it)->endVertex();
		
		if(isParticleResonance(**it)) { // translate knwoledge of resonance into null distance between vertices
			if(!endVertex) {
				error() << "Found resonance in decay graph without end vertex. Fishy topology. Giving up." << endmsg;
				return StatusCode::FAILURE;
			}
			nc+=3; // add null distance constraint
		}
		
		if(endVertex) {
			
			nc+=2; // constrain particle to go into the vertex
			
			StatusCode sc=doComputeDoF(*endVertex, nm, nu, nc, false);
			if(sc.isFailure()) return sc;
		}
	}
	
	return StatusCode::SUCCESS;
} 


StatusCode LagrangeGlobalFitter::estimateAndTransport(HepSymMatrix &Ce, HepVector &e, HepVector &u, 
                                                      Vertex &inputVertex) {
	
	
	
	
	debug() << "entering estimateAndTransport" << endmsg;
	
	Particle dummy;
	dummy.setParticleID(0);
	
	StatusCode sc=estimateVertex(inputVertex, dummy);
	if(sc.isFailure()) return StatusCode::FAILURE;
	
	int e_embed, u_embed;
	e_embed=u_embed=1;
	doEmbed(inputVertex, e_embed, u_embed, Ce, e, u);
	
	return StatusCode::SUCCESS;
}



StatusCode LagrangeGlobalFitter::estimateAndTransport(HepSymMatrix &Ce, HepVector &e, HepVector &u, 
                                                      Particle &inputParticle) {
	
	
	
	
	debug() << "entering estimateAndTransport" << endmsg;
	
	StatusCode sc=estimateParticle(inputParticle);
	if(sc.isFailure()) return StatusCode::FAILURE;
	
	int e_embed, u_embed;
	e_embed=u_embed=1;
	doEmbed(inputParticle, e_embed, u_embed, Ce, e, u);
	
	return StatusCode::SUCCESS;
}



StatusCode LagrangeGlobalFitter::estimateVertex(Vertex &inputVertex, Particle &pAtVtx) {
	
	
	
	debug() << "entering Estimate Vertex" << endmsg;
	
	if(!isVertexMeasured(inputVertex)) { // vertex is just created, i.e. unmeasured
		
		ParticleVector pList;
		
		for(SmartRefVector<Particle>::iterator it=inputVertex.products().begin();
        it!=inputVertex.products().end();
        it++) {
			
			if(!isParticleMeasured(**it)) { 
				// estimate unmeasured particle
				StatusCode sc=estimateParticle(**it);
				if(sc.isFailure()) return StatusCode::FAILURE;
				pList.push_back(*it);
			} else {
				// particle is measured, and since at the moment it can't decay, don't go ahead in the tree
				pList.push_back(*it);
			}
		}
		
		if(pList.size()<2) { // less than two determinable particle at vertex
      // this can never happen in the current implementation. Check included for future development
			error() << "Less than two determinable particles at vertex. Fishy decay topology. Giving up." << endmsg;
			return StatusCode::FAILURE;
		}
		
		
		Vertex uncVertex;
		StatusCode sc = m_pVertexUnconst->fitVertex(pList,uncVertex);
		if(sc.isFailure()) {
			error() << "Unconstrained vertex failure" << endmsg;
			return StatusCode::FAILURE;
		}
		
		debug() << "vertex unconstrained position " << uncVertex.position().x() 
            << " " << uncVertex.position().y()  <<  " "   << uncVertex.position().z() << endmsg;  
		
		debug() << "vertex unconstrained position " << uncVertex.position().z() << " " << uncVertex.positionErr()(3,3)<< endmsg;  
		
		// save the estimate of initial unknown vertex position
		inputVertex.setPosition(uncVertex.position());
		inputVertex.setPositionErr(uncVertex.positionErr());
		
		// fill particle at vertex
		sc=m_pStuffer->fillParticle(uncVertex,pAtVtx,pAtVtx.particleID()); 
		if(sc.isFailure()) {
			error() << " particle stuffer failure " << endmsg;
			return StatusCode::FAILURE;
		}
		
		
		// vertex is known now... transport measured variables at vertex
		sc=transport(inputVertex);
		if( sc.isFailure() ) return StatusCode::FAILURE;      
		
	} else { // vertex is measured
		
		debug() << "vertex is measured" << endmsg;
		
		// vertex is known. transport measured variables at vertex
		StatusCode sc=transport(inputVertex);
		if(sc.isFailure()) return StatusCode::FAILURE;
		
		// go ahead in the tree
		for(SmartRefVector<Particle>::iterator it=inputVertex.products().begin();
        it!=inputVertex.products().end();
        it++) {
			if(!isParticleMeasured(**it)) { // since a measured particle cannot decay at the moment only estimate unmeasured
				StatusCode sc=estimateParticle(**it);
				if(sc.isFailure()) return StatusCode::FAILURE; 
			}
		}
		
	}
	
	
	return StatusCode::SUCCESS;
	
} 


StatusCode LagrangeGlobalFitter::estimateParticle(Particle &part) {
	
	
	
	Particle p;
	p.setParticleID(part.particleID());
	
	StatusCode sc=estimateVertex(*(part.endVertex()),p);
	if(sc.isFailure()) return StatusCode::FAILURE;
	
	
	// set estimate of unknown particle
	part.setMomentum(p.momentum());
	part.setPointOnTrack(p.pointOnTrack());
	part.setPointOnTrackErr(p.pointOnTrackErr());
	part.setPosSlopesCorr(p.posSlopesCorr());
	part.setSlopesMomErr(p.slopesMomErr());
	
	return StatusCode::SUCCESS;
}


StatusCode LagrangeGlobalFitter::transport(Vertex &inputVertex) {
	
	
	
	debug() << "entering transport" << endmsg;
	
	for(SmartRefVector<Particle>::iterator it=inputVertex.products().begin();
      it!=inputVertex.products().end();
      it++) {
		
		if(isParticleMeasured(*(*it))) { // particle is measured
			
			// transport it to origin vertex
			Particle transParticle;
			StatusCode sc = m_pTransporter->transport(**it, inputVertex.position().z(), transParticle);
			
			if (sc.isFailure()) {
				error() << "Track extrapolation failed" << endmsg;
				return sc;
			}
			
			(*it)->setMomentum(transParticle.momentum());
			(*it)->setPointOnTrack(transParticle.pointOnTrack());
			(*it)->setPointOnTrackErr(transParticle.pointOnTrackErr());
			(*it)->setPosSlopesCorr(transParticle.posSlopesCorr());
			(*it)->setSlopesMomErr(transParticle.slopesMomErr());
			
		}
	}
	
	return StatusCode::SUCCESS;
	
}


void LagrangeGlobalFitter::doEmbed(Particle &inputParticle, int &e_embed, int &u_embed, HepSymMatrix &Ce, HepVector &e,
                                   HepVector &u) {
	
	
	debug() << "Entering doEmbed particle " << endmsg;
	
	u(u_embed) = inputParticle.pointOnTrack().x();
	u(u_embed+1) = inputParticle.pointOnTrack().y();
	u(u_embed+2) = inputParticle.slopeX();
	u(u_embed+3) = inputParticle.slopeY();
	
	double px = inputParticle.momentum().px();
	double py = inputParticle.momentum().py();
	double pz = inputParticle.momentum().pz();
	u(u_embed+4) = sqrt(pow(px,2)+pow(py,2)+pow(pz,2));
	
	u_embed+=5;
	
	UNParStore[&inputParticle]=u_embed;
	
	doEmbed(*inputParticle.endVertex(), e_embed, u_embed, Ce, e, u);  
	
}

void LagrangeGlobalFitter::doEmbed(Vertex &inputVertex, int &e_embed, int &u_embed, HepSymMatrix &Ce, HepVector &e, HepVector &u){
	
    
	
  debug() << "Entering doEmbed Vertex" << endmsg;
  // embed vertex
  if(isVertexMeasured(inputVertex)) {
		e(e_embed)   = inputVertex.position().x()/mm;
		e(e_embed+1) = inputVertex.position().y()/mm;
		e(e_embed+2) = inputVertex.position().z()/mm;
		Ce.sub(e_embed,inputVertex.positionErr()/(mm*mm));
		MVtxStore[&inputVertex]=e_embed;
		e_embed+=3;		
		
  } else {
		u(u_embed)   = inputVertex.position().x()/mm;
		u(u_embed+1) = inputVertex.position().y()/mm;
		u(u_embed+2) = inputVertex.position().z()/mm;
		UNVtxStore[&inputVertex]=u_embed;
		u_embed+=3;
		
		
  }
	
  debug() << "vx,vy,vz " << inputVertex.position().x()<< " " << inputVertex.position().y()<< " " << 
    inputVertex.position().z()<< endmsg;
	
  for(SmartRefVector<Particle>::iterator it=inputVertex.products().begin();
      it!=inputVertex.products().end();
      it++) {
		
		
		if(isParticleMeasured(**it)) {
			e(e_embed)   = (*it)->pointOnTrack().x()/mm;
			e(e_embed+1) = (*it)->pointOnTrack().y()/mm;
			e(e_embed+2) = (*it)->slopeX();
			e(e_embed+3) = (*it)->slopeY();
			MParStore[&(**it)]=e_embed;
			
			double px = (*it)->momentum().px(); ///GeV;
			double py = (*it)->momentum().py(); ///GeV;
			double pz = (*it)->momentum().pz(); ///GeV;
			e(e_embed+4) = sqrt(pow(px,2)+pow(py,2)+pow(pz,2));
			
			HepSymMatrix newpointErr = (*it)->pointOnTrackErr();
			HepSymMatrix newSlpMomCorrErr = (*it)->slopesMomErr();
			HepMatrix newPointSlpMomCorrErr = (*it)->posSlopesCorr();
			
			HepSymMatrix CeTemp(5,1);
			
			CeTemp(1,1) = newpointErr(1,1)/(mm*mm);             // x-x
			CeTemp(1,2) = newpointErr(2,1)/(mm*mm);             // x-y
			CeTemp(2,2) = newpointErr(2,2)/(mm*mm);             // y-y
			
			CeTemp(1,3) = newPointSlpMomCorrErr(1,1)/mm;   // x-sx
			CeTemp(1,4) = newPointSlpMomCorrErr(2,1)/mm;   // x-sy
			CeTemp(1,5) = newPointSlpMomCorrErr(3,1)/(mm); //*GeV);    // x-p
			CeTemp(2,3) = newPointSlpMomCorrErr(1,2)/mm;   // y-sx
			CeTemp(2,4) = newPointSlpMomCorrErr(2,2)/mm;   // y-sy
			CeTemp(2,5) = newPointSlpMomCorrErr(3,2)/(mm); //*GeV);    // y-p
			
			CeTemp(3,3) = newSlpMomCorrErr(1,1);                 // sx-sx
			CeTemp(3,4) = newSlpMomCorrErr(2,1);                  // sx-sy
			CeTemp(3,5) = newSlpMomCorrErr(3,1); ///GeV;             // sx-p
			CeTemp(4,4) = newSlpMomCorrErr(2,2);                 // sy-sy
			CeTemp(4,5) = newSlpMomCorrErr(3,2); ///GeV;             // sy-p
			CeTemp(5,5) = newSlpMomCorrErr(3,3); ///(GeV*GeV);       // p-p 
			
			
			// embed the one track matrix in the Ce matrix
			Ce.sub(e_embed,CeTemp);
			
			e_embed+=5;
			
    } else {
		  u(u_embed)   = (*it)->pointOnTrack().x()/mm;
		  u(u_embed+1) = (*it)->pointOnTrack().y()/mm;
		  u(u_embed+2) = (*it)->slopeX();
		  u(u_embed+3) = (*it)->slopeY();
		  
		  double px = (*it)->momentum().px(); ///GeV;
		  double py = (*it)->momentum().py(); ///GeV;
		  double pz = (*it)->momentum().pz(); ///GeV;
		  u(u_embed+4) = sqrt(pow(px,2)+pow(py,2)+pow(pz,2));
		  UNParStore[&(**it)]=u_embed;
		  u_embed+=5;
		  
		  
    }
		
		
		if((*it)->endVertex()) {
			doEmbed(*((*it)->endVertex()), e_embed, u_embed, Ce, e, u);
		}
		
  }
	
}



StatusCode LagrangeGlobalFitter::checkMomenta(Vertex &inputVertex, HepVector &e, HepVector &u) {
	
	debug() << "Entering checkMomenta " << endmsg;
	int e_embed=1;
	int u_embed=1;
	
	return doCheck(inputVertex, e, u, e_embed, u_embed);
}

StatusCode LagrangeGlobalFitter::checkMomenta(Particle &inputParticle, HepVector &e, HepVector &u) {
	
	int e_embed=1;
	int u_embed=1;
	
	debug() << "checkMomenta unmeasured p=" << u(u_embed+4) << endmsg;
	if(u(u_embed+4)<0) return StatusCode::FAILURE;
	u_embed+=5;
	return doCheck(*inputParticle.endVertex(), e, u, e_embed, u_embed);
}

StatusCode LagrangeGlobalFitter::doCheck(Vertex &inputVertex, HepVector &e, HepVector &u, int &e_embed, int &u_embed) {
	
	debug() << "Entering doCheck " << endmsg;
	// embed vertex
	
	if(isVertexMeasured(inputVertex)) {
		e_embed+=3;    
	} else {
		u_embed+=3;
	}
	for(SmartRefVector<Particle>::iterator it=inputVertex.products().begin();
      it!=inputVertex.products().end();
      it++) {
		
		
		if(isParticleMeasured(**it)) {
			debug() << "checkMomenta measured p=" << e(e_embed+4) << endmsg;
			if(e(e_embed+4)<0) return StatusCode::FAILURE;
			e_embed+=5;
		} else {
			debug() << "checkMomenta unmeasured p=" << u(u_embed+4) << endmsg;
			if(u(u_embed+4)<0) return StatusCode::FAILURE;
			u_embed+=5;
		}
		
		if((*it)->endVertex()) {
			StatusCode sc=doCheck(*((*it)->endVertex()), e, u, e_embed, u_embed);
			if(sc.isFailure()) return sc;
		}
		
	}
	
	return StatusCode::SUCCESS;
	
}



bool LagrangeGlobalFitter::computeConstraint(HepVector &e, HepVector &u, Vertex &workingVertex, HepVector &constraint) {
	
	debug() << "Entering computeConstraint " << endmsg;
	int e_embed=1;
	int u_embed=1;
	int cindex=1;
	struct mVect dummy_mVect;
	bool conv=true;
	doComputeConstraint(e, u, workingVertex, e_embed, u_embed, cindex, constraint, dummy_mVect, conv, true);
	
	debug() << "exited doComputeConstraint " << endmsg;
	
	return conv;
}


bool LagrangeGlobalFitter::computeConstraint(HepVector &e, HepVector &u, Particle &workingParticle, HepVector &constraint) {
	
	debug() << "Entering computeConstraint " << endmsg;
	int e_embed=1;
	int u_embed=1;
	int cindex=1;
	bool conv=true;
	
	double x=u(u_embed);
	double y=u(u_embed+1);
	double tx=u(u_embed+2);
	double ty=u(u_embed+3);
	double p=u(u_embed+4);
	u_embed+=5;
	
	struct mVect v;
	v.tx=tx;
	v.ty=ty;
	v.p=p;
	v.m=workingParticle.mass(); ///GeV;
	
	
	
	double xe=u(u_embed);
	double ye=u(u_embed+1);
	double ze=u(u_embed+2);
	
	
	// being unmeasured the point on track is the end vertex
	double z=workingParticle.pointOnTrack().z()/mm;
	
	// constrain unmeasured particle to end vertex
	debug() << "Constraint Particle " << workingParticle.particleID().pid() << " to endVertex; cindex (2)"  << cindex << endmsg;
	
	constraint(cindex) = xe-tx*(ze-z)-x;
	constraint(cindex+1) = ye-ty*(ze-z)-y;
	
	if(fabs(constraint(cindex))>m_toleranceVertex) conv=false;
	if(fabs(constraint(cindex+1))>m_toleranceVertex) conv=false;
	
	cindex+=2;
	
	doComputeConstraint(e, u, *workingParticle.endVertex(), e_embed, u_embed, cindex, constraint, v, conv, false);
	
	debug()<< "exited doComputeConstraint " << endmsg;
	
	return conv;
}



void LagrangeGlobalFitter::doComputeConstraint(HepVector &e, HepVector &u, Vertex &workingVertex,
                                               int &e_embed, int &u_embed, int &cindex, HepVector &constraint, 
                                               struct mVect &vp, bool &conv, bool isStartingVertex) {
	
	debug() << "entering doComputeConstraint " << endmsg;
	
	// origin vertex
	double xo;
	double yo;
	double zo;
	if(isVertexMeasured(workingVertex)) {
		xo=e(e_embed);
		yo=e(e_embed+1);
		zo=e(e_embed+2);
		e_embed+=3;
	} else {
		xo=u(u_embed);
		yo=u(u_embed+1);
		zo=u(u_embed+2);
		u_embed+=3;
	}
	
	debug() << "Origin Vertex: xo " << xo << " " << yo << " " << zo << endmsg;
	
	std::vector<struct mVect> momentumList;
	
	for(SmartRefVector<Particle>::iterator it=workingVertex.products().begin();
      it!=workingVertex.products().end();
      it++) {
		
		double x;
		double y;
		double tx;
		double ty;
		double p;
		
		if(isParticleMeasured(**it)) {
			x=e(e_embed);
			y=e(e_embed+1);
			tx=e(e_embed+2);
			ty=e(e_embed+3);
			p=e(e_embed+4);
			e_embed+=5;
		} else {
			x=u(u_embed);
			y=u(u_embed+1);
			tx=u(u_embed+2);
			ty=u(u_embed+3);
			p=u(u_embed+4);
			u_embed+=5;
		}
		
		debug() << "particle parameters " << x << " " << y << " " << tx << " " << ty << " " << p << " ID " << 
      (*it)->particleID().pid() << endmsg;
		
		struct mVect v;
		v.tx=tx;
		v.ty=ty;
		v.p=p;
		v.m=(*it)->mass(); ///GeV;
		momentumList.push_back(v);
		
		double z=(*it)->pointOnTrack().z()/mm;
		
		// constrain particle to origin vertex
		if(!isParticleResonance(**it)) {
			
			debug() << "Constraint Particle " << (**it).particleID().pid() << " to Origin Vertex; cindex (2)"  << cindex << endmsg;
			constraint(cindex) = xo-tx*(zo-z)-x;
			constraint(cindex+1) = yo-ty*(zo-z)-y;
			
			if(fabs(constraint(cindex))>m_toleranceVertex) conv=false;
			if(fabs(constraint(cindex+1))>m_toleranceVertex) conv=false;
			
			cindex+=2;
		}
		
		if((*it)->endVertex()) {
			// constrain particle to end vertex
			double xe;
			double ye;
			double ze;
			if(isVertexMeasured(*(*it)->endVertex())) {
				xe=e(e_embed);
				ye=e(e_embed+1);
				ze=e(e_embed+2);
			} else {
				xe=u(u_embed);
				ye=u(u_embed+1);
				ze=u(u_embed+2);
			}
			
			debug() << " Decay Vertex coordinates -x->" << xe << " " << x << "-y->" << ye << " " << y <<  "-z->" << ze << 
        " " << z << endmsg;
			
			debug() << "Constraint Particle " << (**it).particleID().pid() << " to end Vertex; cindex (2)"  << cindex << endmsg;
			constraint(cindex) = xe-tx*(ze-z)-x;
			constraint(cindex+1) = ye-ty*(ze-z)-y;
			
			if(fabs(constraint(cindex))>m_toleranceVertex) conv=false;
			if(fabs(constraint(cindex+1))>m_toleranceVertex) conv=false;
			
			cindex+=2;
			
			if(isParticleResonance(**it)) { // force distance between vertices to be zero
				constraint(cindex)   = (xe-xo); 
				constraint(cindex+1) = (ye-yo); 
				constraint(cindex+2) = (ze-zo);
				debug() << "Constraint SV = PV; cindex (3)"  << cindex << endmsg;
				
				if(fabs(constraint(cindex)  )>m_toleranceResonance) conv=false;
				if(fabs(constraint(cindex+1))>m_toleranceResonance) conv=false;
				if(fabs(constraint(cindex+2))>m_toleranceResonance) conv=false;
				
				cindex+=3;
			}
			
			// go ahead
			doComputeConstraint(e, u, *((*it)->endVertex()), e_embed, u_embed, cindex, constraint, v, conv, false);
		}
		
	}
	
	debug() << "momentumList size " << momentumList.size() << endmsg;
	
	if(!isStartingVertex) {
		// mass constraint and add 3-momentum conservation
		int i=0;
		for(std::vector<struct mVect>::iterator pl1=momentumList.begin(); pl1!=momentumList.end(); pl1++,i++) {
			double p1=pl1->p;
			double tx1=pl1->tx;
			double ty1=pl1->ty;
			double m1=pl1->m;
			double E1=sqrt(pow(p1,2)+pow(m1,2));
			double T1=sqrt(1+pow(tx1,2)+pow(ty1,2));  // p/pz=1/sin(theta)
			
			debug() << "Constraint momenta conservation " << m1 << "; cindex (3)"  << cindex << endmsg;
			
			constraint(cindex) += p1*tx1/T1;   //px
			constraint(cindex+1) += p1*ty1/T1; //py
			constraint(cindex+2) += p1/T1;     //pz
			
			if(hasMassConstraint(workingVertex)) {
				debug() << "Constraint mass conservation " << m1 << "; cindex (1)"  << cindex+3 << endmsg;
				constraint(cindex+3) += pow(m1,2);
				
				int j=0;
				for(std::vector<struct mVect>::iterator pl2=momentumList.begin(); j<i&&pl2!=momentumList.end(); pl2++,j++) {
					
					double p2=pl2->p;
					double tx2=pl2->tx;
					double ty2=pl2->ty;
					double m2=pl2->m;
					double E2=sqrt(pow(p2,2)+pow(m2,2));
					double T2=sqrt(1+pow(tx2,2)+pow(ty2,2));
					double T12=1+tx1*tx2+ty1*ty2;
					
					constraint(cindex+3)+=2*E1*E2-2*p1*p2*T12/T1/T2;
					
				} 
			}
		}
		
		debug() << "px,py,pz " <<  constraint(cindex)<< " " <<  constraint(cindex+1)<< " " <<  constraint(cindex+2) << endmsg;
		
		double T=sqrt(1+pow(vp.tx,2)+pow(vp.ty,2));
		// conserve px, py, pz
		constraint(cindex)-=vp.p*vp.tx/T;
		constraint(cindex+1)-=vp.p*vp.ty/T;
		constraint(cindex+2)-=vp.p/T;
		
		
		if(fabs(constraint(cindex))>m_toleranceMomentum) conv=false;
		if(fabs(constraint(cindex+1))>m_toleranceMomentum) conv=false;
		if(fabs(constraint(cindex+2))>m_toleranceMomentum) conv=false;
		cindex+=3;
		
		if(hasMassConstraint(workingVertex)) {
			// invariant mass
			constraint(cindex)-=pow(vp.m,2);
			if(fabs(constraint(cindex))>m_toleranceMass) conv=false;
			cindex++;
		}
	}
}



StatusCode LagrangeGlobalFitter::iterateWithLinearConstraint(HepSymMatrix &Ce, HepVector &e, HepVector &u, int &nm, int &nu, 
                                                             int &nc,Particle &workingParticle, HepVector &constraint, 
                                                             HepMatrix &B, HepMatrix &C, HepMatrix &Minv) {
	
	debug() << "entering iterateWithLinearConstraint " << endmsg;
	
	// matrix of constraint derivative
	HepVector de(nm,0);       // correction measured  
	HepVector du(nu,0);       // correction unmeasured
	
	int e_embed=1;
	int u_embed=1;
	int cindex=1;
	
	
	double tx=u(u_embed+2);
	double ty=u(u_embed+3);
	double p=u(u_embed+4);
	int indexp=u_embed;
	
	u_embed+=5;
	
	struct mVect v;
	v.tx=tx;
	v.ty=ty;
	v.p=p;
	v.m=workingParticle.mass(); ///GeV;
	v.i=indexp;
	v.f=false;
	
	
	double ze=u(u_embed+2);
	
	
	// being unmeasured the point on track is the end vertex
	double z=workingParticle.pointOnTrack().z()/mm;
	
	
	
	// constrain unmeasured particle to end vertex
	C(cindex,u_embed)=1;  // derivative wrt xe
	C(cindex,u_embed+2)=-tx;  // derivative wrt ze
	C(cindex+1,u_embed+1)=1;  // derivative wrt ye
	C(cindex+1,u_embed+2)=-ty;  // derivative wrt ze
	C(cindex,indexp)=-1;  // derivative wrt x
	C(cindex,indexp+2)=-(ze-z);  // derivative wrt tx
	C(cindex+1,indexp+1)=-1;  // derivative wrt y
	C(cindex+1,indexp+3)=-(ze-z);  // derivative wrt ty
	cindex+=2;
	
	// go ahead
	doIteration(e, u, B, C, *workingParticle.endVertex(), e_embed, u_embed, cindex, v, false);
	
	StatusCode sc=solveSystem(Ce, B, C, nm, nu, nc, constraint, de, du, Minv);
	if(sc.isFailure()) return sc;
	
	e = e + de;
	
	u = u + du;
	
	return StatusCode::SUCCESS;
}

StatusCode LagrangeGlobalFitter::iterateWithLinearConstraint(HepSymMatrix &Ce, HepVector &e, HepVector &u, int &nm, int &nu, 
                                                             int &nc, Vertex &workingVertex, HepVector &constraint, HepMatrix &B, 
                                                             HepMatrix &C, HepMatrix &Minv) {
	
	
	debug() << "entering iterateWithLinearConstraint " << endmsg;
	
	
	// matrix of constraint derivative
	HepVector de(nm,0);       // correction measured  
	HepVector du(nu,0);       // correction unmeasured
	
	int e_embed=1;
	int u_embed=1;
	int cindex=1;
	struct mVect dummy_mVect;
	doIteration(e, u, B, C, workingVertex, e_embed, u_embed, cindex, dummy_mVect, true);  // calcola B,C
	
	StatusCode sc=solveSystem(Ce, B, C, nm, nu, nc, constraint, de, du, Minv);  // Determina le correzioni de du e da' Minv
	if(sc.isFailure()) return sc;
	debug()<< " Vettore correzioni de " << de <<endmsg;
	debug()<< " Vettore correzioni du " << du <<endmsg;
	
	e = e + de;
	
	u = u + du;
	
	return StatusCode::SUCCESS;
}




StatusCode LagrangeGlobalFitter::solveSystem(HepSymMatrix &Ce, HepMatrix &B, HepMatrix &C, 
                                             int &nm, int &nu, int &nc, HepVector &constraint, HepVector &de, HepVector &du, 
                                             HepMatrix &Minv) {
	
	debug() << "entering solveSystem" << endmsg;
	
	
	int inv;
	
	HepSymMatrix G = Ce.inverse(inv);
	if ( inv != 0 ){
		error() << "Ce inverse failure" << endmsg;     
		return StatusCode::FAILURE;
	}
	
	HepMatrix Bt=B.T();   // Transpose
	HepMatrix Ct=C.T();
	HepMatrix M(nm+nu+nc,nm+nu+nc,0);
	
	M.sub(1,1,G);
	M.sub(nm+nu+1,1,B);
	M.sub(nm+nu+1,nm+1,C);
	M.sub(1,nm+nu+1,Bt);
	M.sub(nm+1,nm+nu+1,Ct);
	
	
	
	Minv=M.inverse(inv);
	
	if ( inv != 0 ){
		
		/*Only for MATHEMATICA CHECK
		//---------------------------------------------------------
		std::cout << " Matrice M *******************"<<endl;
		std::cout << " { ";
		for(int i=0; i<M.num_row(); i++){
    std::cout << " { ";
    for(int ii=0; ii<(M.num_col()-1); ii++ ) std::cout << fixed << M[i][ii] << ",  ";
    std::cout << M[i][M.num_col()-1] << "}, " << endl;     
		}		
		*/
		
		error() << "M inverse failure" << endmsg;     
		return StatusCode::FAILURE;
	}
	
	HepVector Z(nm+nu+nc,0);
	Z.sub(nm+nu+1,-constraint);
	
	HepVector Y=Minv*Z;
	
	de = Y.sub(1,nm);
	du = Y.sub(nm+1,nm+nu);
	
	debug() << "Exit Solving System" << endmsg;
	
	return StatusCode::SUCCESS;
	
}


void LagrangeGlobalFitter::doIteration(HepVector &e, HepVector &u, HepMatrix &B, HepMatrix &C,
                                       Vertex &workingVertex,
                                       int &e_embed, int &u_embed, 
                                       int &cindex, struct mVect &vp, bool isStartingVertex) {

	debug() << "entering iterateWithLinearConstraint" << endmsg;
	
	// origin vertex
	double xo;  
	double yo;
	double zo;
	int indexo;
	if(isVertexMeasured(workingVertex)) {
		xo=e(e_embed);
		yo=e(e_embed+1);
		zo=e(e_embed+2);
		indexo=e_embed;
		e_embed+=3;
	} else {
		xo=u(u_embed);
		yo=u(u_embed+1);
		zo=u(u_embed+2);
		indexo=u_embed;
		u_embed+=3;
	}
	
	debug() << "x,y,z " << xo << " " << yo << " " << zo <<endmsg;
	
	
	std::vector<struct mVect> momentumList;
	
	for(SmartRefVector<Particle>::iterator it=workingVertex.products().begin();
      it!=workingVertex.products().end();
      it++) {
		
		double x;
		double y;
		double tx;
		double ty;
		double p;
		
		int indexp;
		if(isParticleMeasured(**it)) {
			x=e(e_embed);
			y=e(e_embed+1);
			tx=e(e_embed+2);
			ty=e(e_embed+3);
			p=e(e_embed+4);
			indexp=e_embed;
			e_embed+=5;
		} else {
			x=u(u_embed);
			y=u(u_embed+1);
			tx=u(u_embed+2);
			ty=u(u_embed+3);
			p=u(u_embed+4);
			indexp=u_embed;
			u_embed+=5;
		}
		
		
		struct mVect v;
		v.tx=tx;
		v.ty=ty;
		v.p=p;
		v.m=(*it)->mass(); ///GeV;
		v.i=indexp;
		v.f=isParticleMeasured(**it);
		momentumList.push_back(v);
		
		debug() << "mass " << (*it)->mass() << endmsg;
		
		double z=(*it)->pointOnTrack().z()/mm;
		
		
		if(!isParticleResonance(**it)) {      
			// constrain particle to origin vertex
			if(isVertexMeasured(workingVertex)) {
				B(cindex,indexo)=1;  // derivative wrt xo
				B(cindex,indexo+2)=-tx;  // derivative wrt zo
				B(cindex+1,indexo+1)=1;  // derivative wrt yo
				B(cindex+1,indexo+2)=-ty;  // derivative wrt zo
			} else {
				C(cindex,indexo)=1;  // derivative wrt xo
				C(cindex,indexo+2)=-tx;  // derivative wrt zo
				C(cindex+1,indexo+1)=1;  // derivative wrt yo
				C(cindex+1,indexo+2)=-ty;  // derivative wrt zo
			}
			
			if(isParticleMeasured(**it)) {
				B(cindex,indexp)=-1;  // derivative wrt x
				B(cindex,indexp+2)=-(zo-z);  // derivative wrt tx
				B(cindex+1,indexp+1)=-1;  // derivative wrt y
				B(cindex+1,indexp+3)=-(zo-z);  // derivative wrt ty      
				
			} else {
				C(cindex,indexp)=-1;  // derivative wrt x
				C(cindex,indexp+2)=-(zo-z);  // derivative wrt tx
				C(cindex+1,indexp+1)=-1;  // derivative wrt y
				C(cindex+1,indexp+3)=-(zo-z);  // derivative wrt ty
			}   
			cindex+=2;
		}
		
		if((*it)->endVertex()) {
			// constrain particle to end vertex
			double xe;
			double ye;
			double ze;
			
			if(isVertexMeasured(*((*it)->endVertex()))) {
				xe=e(e_embed);
				ye=e(e_embed+1);
				ze=e(e_embed+2);
			} else {
				xe=u(u_embed);
				ye=u(u_embed+1);
				ze=u(u_embed+2);
			}
			
			// constrain particle to end vertex
			if(isVertexMeasured(*((*it)->endVertex()))) {
				B(cindex,e_embed)=1;  // derivative wrt xe
				B(cindex,e_embed+2)=-tx;  // derivative wrt ze
				B(cindex+1,e_embed+1)=1;  // derivative wrt ye
				B(cindex+1,e_embed+2)=-ty;  // derivative wrt ze
			} else {
				C(cindex,u_embed)=1;  // derivative wrt xe
				C(cindex,u_embed+2)=-tx;  // derivative wrt ze
				C(cindex+1,u_embed+1)=1;  // derivative wrt ye
				C(cindex+1,u_embed+2)=-ty;  // derivative wrt ze
			}
			if(isParticleMeasured(**it)) {
				B(cindex,indexp)=-1;  // derivative wrt x
				B(cindex,indexp+2)=-(ze-z);  // derivative wrt tx
				B(cindex+1,indexp+1)=-1;  // derivative wrt y
				B(cindex+1,indexp+3)=-(ze-z);  // derivative wrt ty
			} else {
				C(cindex,indexp)=-1;  // derivative wrt x
				C(cindex,indexp+2)=-(ze-z);  // derivative wrt tx
				C(cindex+1,indexp+1)=-1;  // derivative wrt y
				C(cindex+1,indexp+3)=-(ze-z);  // derivative wrt ty
			}
			cindex+=2;
			
			if(isParticleResonance(**it)) { // force distance between vertices to be zero
				
				debug() << "res " << u_embed << " " << indexo << " " << cindex << endmsg;
				
				if(!isVertexMeasured(*( (*it)->endVertex() ) )) { 
					C(cindex,u_embed) = 1; // wrt xe
					C(cindex+1,u_embed+1) = 1; // wrt ye
					C(cindex+2,u_embed+2) = 1; // wrt ze
				} else {   
					error() << "Resonance endVertex measured!! ++++++++++++++++++++ CHECK your code" << endmsg;
				}
				if(isVertexMeasured(workingVertex)) {
					B(cindex,indexo) = -1; // wrt xo
					B(cindex+1,indexo+1) = -1; // wrt yo
					B(cindex+2,indexo+2) = -1; // wrt zo
				} else {
					C(cindex,indexo) = -1; // wrt xo
					C(cindex+1,indexo+1) = -1; // wrt yo
					C(cindex+2,indexo+2) = -1; // wrt zo
				}
				cindex+=3;
			}
			
			
			// go ahead
			doIteration(e, u, B, C, *((*it)->endVertex()), e_embed, u_embed, cindex, v, false);
			
		}
		
	}
	
	debug() << "size " << momentumList.size() << endmsg;
	
	if(!isStartingVertex) {
		// mass constraint and 3-momentum conservation
		int i=0;
		for(std::vector<struct mVect>::iterator pl1=momentumList.begin(); pl1!=momentumList.end(); pl1++,i++) {
			double p1=pl1->p;
			double tx1=pl1->tx;
			double ty1=pl1->ty;
			double m1=pl1->m;
			int    i1=pl1->i;
			//      double E1=sqrt(pow(p1,2)+pow(m1,2));
			double T1=sqrt(1+pow(tx1,2)+pow(ty1,2));
			
			debug() << "cindex= " <<cindex<< endmsg;
			
			debug() << p1 << " " << tx1 << " " << ty1 << " " << m1 << endmsg;
			
			// constraint px at vertex (p1*tx1/T1)
			if(pl1->f) { // is measured
				B(cindex,i1+2) += p1/T1-p1*tx1*tx1/T1/T1/T1; // derivative wrt tx1
				B(cindex,i1+3) += -p1*tx1*ty1/T1/T1/T1; // derivative wrt ty1
				B(cindex,i1+4) += tx1/T1; // derivative wrt p1
				
				// constraint py at vertex (p1*ty1/T1)
				B(cindex+1,i1+2) += -p1*tx1*ty1/T1/T1/T1; // derivative wrt tx1
				B(cindex+1,i1+3) += p1/T1-p1*ty1*ty1/T1/T1/T1; // derivative wrt ty1
				B(cindex+1,i1+4) += ty1/T1; // derivative wrt p1
				
				// constraint pz at vertex (p1/T1)
				B(cindex+2,i1+2) += -p1*tx1/T1/T1/T1; // derivative wrt tx1
				B(cindex+2,i1+3) += -p1*ty1/T1/T1/T1; // derivative wrt ty1
				B(cindex+2,i1+4) += 1/T1; // derivative wrt p1
			} else {
				C(cindex,i1+2) += p1/T1-p1*tx1*tx1/T1/T1/T1; // derivative wrt tx1
				C(cindex,i1+3) += -p1*tx1*ty1/T1/T1/T1; // derivative wrt ty1
				C(cindex,i1+4) += tx1/T1; // derivative wrt p1
				
				// constraint py at vertex (p1*ty1/T1)
				C(cindex+1,i1+2) += -p1*tx1*ty1/T1/T1/T1; // derivative wrt tx1
				C(cindex+1,i1+3) += p1/T1-p1*ty1*ty1/T1/T1/T1; // derivative wrt ty1
				C(cindex+1,i1+4) += ty1/T1; // derivative wrt p1
				
				// constraint pz at vertex (p1/T1)
				C(cindex+2,i1+2) += -p1*tx1/T1/T1/T1; // derivative wrt tx1
				C(cindex+2,i1+3) += -p1*ty1/T1/T1/T1; // derivative wrt ty1
				C(cindex+2,i1+4) += 1/T1; // derivative wrt p1
			}
			
			
			if(hasMassConstraint(workingVertex)) {
				
				int j=0;
				for(std::vector<struct mVect>::iterator pl2=momentumList.begin(); j<i&&pl2!=momentumList.end(); pl2++,j++) {
					
					double p2=pl2->p;
					double tx2=pl2->tx;
					double ty2=pl2->ty;
					double m2=pl2->m;
					int    i2=pl2->i;
					//          double E2=sqrt(pow(p2,2)+pow(m2,2));
					double T2=sqrt(1+pow(tx2,2)+pow(ty2,2));
					double T12=1+tx1*tx2+ty1*ty2;
					
					// constraint invariant mass (2*E1*E2-2*p1*p2*T12/T1/T2)
					if(pl1->f) { // is measured
						B(cindex+3,i1+2)+=-2*p1*p2*(tx2*T1*T2-T12*tx1*T2/T1)/pow(T1*T2,2); // derivative wrt tx1
						B(cindex+3,i1+3)+=-2*p1*p2*(ty2*T1*T2-T12*ty1*T2/T1)/pow(T1*T2,2); // derivative wrt ty1
						B(cindex+3,i1+4)+=2*p1*sqrt(pow(p2,2)+pow(m2,2))/sqrt(pow(p1,2)+pow(m1,2))-2*p2*T12/T1/T2; // derivative wrt p1
					} else {
						C(cindex+3,i1+2)+=-2*p1*p2*(tx2*T1*T2-T12*tx1*T2/T1)/pow(T1*T2,2); // derivative wrt tx1
						C(cindex+3,i1+3)+=-2*p1*p2*(ty2*T1*T2-T12*ty1*T2/T1)/pow(T1*T2,2); // derivative wrt ty1
						C(cindex+3,i1+4)+=2*p1*sqrt(pow(p2,2)+pow(m2,2))/sqrt(pow(p1,2)+pow(m1,2))-2*p2*T12/T1/T2; // derivative wrt p1
					}
					
					if(pl2->f) {
						B(cindex+3,i2+2)+=-2*p1*p2*(tx1*T1*T2-T12*tx2*T1/T2)/pow(T1*T2,2); // derivative wrt tx2
						B(cindex+3,i2+3)+=-2*p1*p2*(ty1*T1*T2-T12*ty2*T1/T2)/pow(T1*T2,2); // derivative wrt ty2
						B(cindex+3,i2+4)+=2*p2*sqrt(pow(p1,2)+pow(m1,2))/sqrt(pow(p2,2)+pow(m2,2))-2*p1*T12/T1/T2; // derivative wrt p2
					} else {
						C(cindex+3,i2+2)+=-2*p1*p2*(tx1*T1*T2-T12*tx2*T1/T2)/pow(T1*T2,2); // derivative wrt tx2
						C(cindex+3,i2+3)+=-2*p1*p2*(ty1*T1*T2-T12*ty2*T1/T2)/pow(T1*T2,2); // derivative wrt ty2
						C(cindex+3,i2+4)+=2*p2*sqrt(pow(p1,2)+pow(m1,2))/sqrt(pow(p2,2)+pow(m2,2))-2*p1*T12/T1/T2; // derivative wrt p2
					}
				}    
			}
		}
		
		double T=sqrt(1+pow(vp.tx,2)+pow(vp.ty,2));
		if(vp.f) { // vp is measured
      // constraint px at vertex (p*tx/T)
			B(cindex,vp.i+2) = -vp.p/T+vp.p*vp.tx*vp.tx/T/T/T; // derivative wrt tx
			B(cindex,vp.i+3) = vp.p*vp.tx*vp.ty/T/T/T; // derivative wrt ty
			B(cindex,vp.i+4) = -vp.tx/T; // derivative wrt p
			
			// constraint py at vertex (p*ty/T)
			B(cindex+1,vp.i+2) = vp.p*vp.tx*vp.ty/T/T/T; // derivative wrt tx
			B(cindex+1,vp.i+3) = -vp.p/T+vp.p*vp.ty*vp.ty/T/T/T; // derivative wrt ty
			B(cindex+1,vp.i+4) = -vp.ty/T; // derivative wrt p
			
			// constraint pz at vertex (p/T)
			B(cindex+2,vp.i+2) = vp.p*vp.tx/T/T/T; // derivative wrt tx
			B(cindex+2,vp.i+3) = vp.p*vp.ty/T/T/T; // derivative wrt ty
			B(cindex+2,vp.i+4) = -1/T; // derivative wrt p
		} else {
			// constraint px at vertex (p*tx/T)
			C(cindex,vp.i+2) = -vp.p/T+vp.p*vp.tx*vp.tx/T/T/T; // derivative wrt tx
			C(cindex,vp.i+3) = vp.p*vp.tx*vp.ty/T/T/T; // derivative wrt ty
			C(cindex,vp.i+4) = -vp.tx/T; // derivative wrt p
			
			// constraint py at vertex (p*ty/T)
			C(cindex+1,vp.i+2) = vp.p*vp.tx*vp.ty/T/T/T; // derivative wrt tx
			C(cindex+1,vp.i+3) = -vp.p/T+vp.p*vp.ty*vp.ty/T/T/T; // derivative wrt ty
			C(cindex+1,vp.i+4) = -vp.ty/T; // derivative wrt p
			
			// constraint pz at vertex (p/T)
			C(cindex+2,vp.i+2) = vp.p*vp.tx/T/T/T; // derivative wrt tx
			C(cindex+2,vp.i+3) = vp.p*vp.ty/T/T/T; // derivative wrt ty
			C(cindex+2,vp.i+4) = -1/T; // derivative wrt p
		}
		cindex+=3;
		
		if(hasMassConstraint(workingVertex)) {
			cindex++;
		}
	}
}


void LagrangeGlobalFitter::updateCovariance(HepSymMatrix &Ce, HepSymMatrix &Cu, 
                                            HepMatrix &Minv, int &nm, int &nu) {
	
	
	HepMatrix H=Minv.sub(nm+1,nm+nu,nm+1,nm+nu);
	H=(H + H.T())/2;
	Cu.assign(H) ;   //Patch Stefania Vecchi  
	
	HepMatrix H2=Minv.sub(1,nm,1,nm);
	H2=(H2 + H2.T())/2.;   //Patch Stefania Vecchi  
	Ce.assign(H2);
	
}



StatusCode LagrangeGlobalFitter::checkCovariance(HepSymMatrix &Ce,  int &nm) {

	debug() << "Entering check Covariance"<< endmsg;
	
	int nu=0;
	HepSymMatrix dummy(1);
	return checkCovariance(Ce, dummy, nm, nu);
	debug() << "Exiting from check Covariance"<< endmsg;
}

StatusCode LagrangeGlobalFitter::checkCovariance(HepSymMatrix &Ce, HepSymMatrix &Cu,  int &nm, int &nu) {

	debug() << "Entering check Covariance"<< endmsg;
	
	
	
	// check the covariance matrix
	//   all elements in diagonal positive
	for (int i=1;i<=nm;i++){
		if (Ce(i,i)<0) {
			error() << "Problem with Ce covariance matrix: " << i << ","
              << i << " " << Ce (i,i) << endmsg;
			return StatusCode::FAILURE;
		}
	}
	//   correlation rho(i,j) between -1 and 1
	for (int i=1; i<=nm; i++){
		for (int j=i+1; j<=nm; j++){
			if ( fabs (Ce(i,j))/sqrt(Ce(i,i)*Ce(j,j)) > 1+m_toleranceCorrelation) {
				error() << "Problem with Ce covariance matrix: "<< i << ","
                << j << " "<< Ce(i,j)/sqrt(Ce(i,i)*Ce(j,j)) << endmsg;
				return StatusCode::FAILURE;
			}
		}
	}
	
	//   all elements in diagonal positive
	for (int i=1;i<=nu;i++){
		if (Cu(i,i)<0) {
			error() << "Problem with Cu covariance matrix: " << i << ","
              << i << " " << Cu (i,i) << endmsg;
			return StatusCode::FAILURE;
		}
	}
	//   correlation rho(i,j) between -1 and 1
	for (int i=1; i<=nu; i++){
		for (int j=i+1; j<=nu; j++){
			if ( fabs (Cu(i,j))/sqrt(Cu(i,i)*Cu(j,j)) > 1+m_toleranceCorrelation) {
				error() << "Problem with Cu covariance matrix: "<< i << ","
                << j << " "<< Cu(i,j)/sqrt(Cu(i,i)*Cu(j,j)) << endmsg;
				return StatusCode::FAILURE;
			}
		}
	}
	
	debug() << "Exiting from check Covariance"<< endmsg;
	
	return StatusCode::SUCCESS;
	
}


StatusCode LagrangeGlobalFitter::fillDecayGraph(HepSymMatrix &Ce0, HepVector &e0, HepSymMatrix &Ce, HepVector &e, HepSymMatrix &Cu
                                                ,HepVector &u, Particle &workingParticle, int ndof) {
	
	
	
	debug() << "fill DecayGraph"<< endmsg;
	
	
	int e_embed=1;
	int u_embed=1;
	
	// Get the chisq of the fit.
	double chis;
	StatusCode sc = getChiSquare(e0, e, Ce0, chis);
	if ( sc.isFailure() ) {
		return StatusCode::FAILURE;  
	}
	
	// starting particle must be unmeasured in the current implementation
	workingParticle.setPointOnTrack(HepPoint3D(u(u_embed)*mm,u(u_embed+1)*mm,workingParticle.pointOnTrack().z()));
	double T=sqrt(1+pow(u(u_embed+2),2)+pow(u(u_embed+3),2));
	double px=u(u_embed+2)*u(u_embed+4)/T; //*GeV;
	double py=u(u_embed+3)*u(u_embed+4)/T; //*GeV;
	double pz=u(u_embed+4)/T; //*GeV;
	
	workingParticle.setMomentum(HepLorentzVector(px,py,pz,sqrt(pow(u(u_embed+4),2)+pow(workingParticle.mass(),2))));
	
	HepSymMatrix newpointErr(3,0);
	HepSymMatrix newSlpMomCorrErr(3,0);
	HepMatrix newPointSlpMomCorrErr(3,3,0);
	
	newpointErr(1,1)=Cu(u_embed,u_embed)*mm*mm;             // x-x
	newpointErr(1,2)=Cu(u_embed,u_embed+1)*mm*mm;           // x-y
	newpointErr(2,2)=Cu(u_embed+1,u_embed+1)*mm*mm;         // y-y
	newpointErr(3,3)=0;                               // z-z
	
	newPointSlpMomCorrErr(1,1)=Cu(u_embed,u_embed+2)*mm;   // x-sx
	newPointSlpMomCorrErr(2,1)=Cu(u_embed,u_embed+3)*mm;   // x-sy
	newPointSlpMomCorrErr(3,1)=Cu(u_embed,u_embed+4)*mm; //*GeV;    // x-p
	newPointSlpMomCorrErr(1,2)=Cu(u_embed+1,u_embed+2)*mm; // y-sx
	newPointSlpMomCorrErr(2,2)=Cu(u_embed+1,u_embed+3)*mm;   // y-sy
	newPointSlpMomCorrErr(3,2)=Cu(u_embed+1,u_embed+4)*mm; //*GeV;   // y-p
	
	
	newSlpMomCorrErr(1,1)=Cu(u_embed+2,u_embed+2);       // sx-sx
	newSlpMomCorrErr(2,1)=Cu(u_embed+2,u_embed+3);       // sx-sy
	newSlpMomCorrErr(3,1)=Cu(u_embed+2,u_embed+4); //*GeV;       // sx-p
	newSlpMomCorrErr(2,2)=Cu(u_embed+3,u_embed+3);      // sy-sy
	
	newSlpMomCorrErr(3,2)=Cu(u_embed+3,u_embed+4); //*GeV;       // sy-p
	newSlpMomCorrErr(3,3)=Cu(u_embed+4,u_embed+4); //*GeV*GeV;        // p-p
	
	workingParticle.setPointOnTrackErr(newpointErr);
	workingParticle.setSlopesMomErr(newSlpMomCorrErr);
	workingParticle.setPosSlopesCorr(newPointSlpMomCorrErr);
	
	u_embed+=5;
	
	doFill(chis, Ce, e, Cu, u, *workingParticle.endVertex(), e_embed, u_embed, ndof);
	
	return StatusCode::SUCCESS;
	
}


StatusCode LagrangeGlobalFitter::fillDecayGraph(HepSymMatrix &Ce0, HepVector &e0, HepSymMatrix &Ce, HepVector &e, 
                                                HepSymMatrix &Cu,HepVector &u, Vertex &workingVertex, int ndof) {
	debug() << "fill DecayGraph"<< endmsg;
	
	int e_embed=1;
	int u_embed=1;
	
	// Get the chisq of the fit.
	double chis;
	StatusCode sc = getChiSquare(e0, e, Ce0, chis);
	if ( sc.isFailure() ) {
		return StatusCode::FAILURE;  
	}
	
	doFill(chis, Ce, e, Cu, u, workingVertex, e_embed, u_embed, ndof);
	
	return StatusCode::SUCCESS;
	
}

void LagrangeGlobalFitter::doFill(double &chis, HepSymMatrix &Ce, HepVector &e, HepSymMatrix &Cu, 
                                  HepVector &u, Vertex &inputVertex, int &e_embed, int &u_embed, int ndof) {
	
	
    
  debug() << "doFill " <<endmsg;
    
  // deembed vertex
  if(isVertexMeasured(inputVertex)) {
		inputVertex.setPosition(HepPoint3D(e(e_embed)*mm,e(e_embed+1)*mm,e(e_embed+2)*mm));
		inputVertex.setPositionErr(Ce.sub(e_embed,e_embed+2)*mm*mm);
		e_embed+=3;    
  } else {
		inputVertex.setPosition(HepPoint3D(u(u_embed)*mm,u(u_embed+1)*mm,u(u_embed+2)*mm));
		inputVertex.setPositionErr(Cu.sub(u_embed,u_embed+2)*mm*mm);
		u_embed+=3;
  }
	
  if(hasMassConstraint(inputVertex)) {
		inputVertex.setType(Vertex::DecayWithMass);
  } else {
		inputVertex.setType(Vertex::Decay);
  }
	
  inputVertex.setNDoF(ndof);
  inputVertex.setChi2(chis);
	
  for(SmartRefVector<Particle>::iterator it=inputVertex.products().begin();
      it!=inputVertex.products().end();
      it++) {
		
		
		if(isParticleMeasured(**it)) {
			(*it)->setPointOnTrack(HepPoint3D(e(e_embed)*mm,e(e_embed+1)*mm,(*it)->pointOnTrack().z()));  
			double T=sqrt(1+pow(e(e_embed+2),2)+pow(e(e_embed+3),2)); 
			double px=e(e_embed+2)*e(e_embed+4)/T; //*GeV;
			double py=e(e_embed+3)*e(e_embed+4)/T; //*GeV;
			double pz=e(e_embed+4)/T; //*GeV;
			
			(*it)->setMomentum(HepLorentzVector(px,py,pz,sqrt(pow(e(e_embed+4),2)+pow((*it)->mass(),2))));
			
			HepSymMatrix newpointErr(3,0);
			HepSymMatrix newSlpMomCorrErr(3,0);
			HepMatrix newPointSlpMomCorrErr(3,3,0);
			
			newpointErr(1,1)=Ce(e_embed,e_embed)*mm*mm;             // x-x
			newpointErr(1,2)=Ce(e_embed,e_embed+1)*mm*mm;           // x-y
			newpointErr(2,2)=Ce(e_embed+1,e_embed+1)*mm*mm;         // y-y
			newpointErr(3,3)=0;                               // z-z
			
			
			newPointSlpMomCorrErr(1,1)=Ce(e_embed,e_embed+2)*mm;   // x-sx
			newPointSlpMomCorrErr(2,1)=Ce(e_embed,e_embed+3)*mm;   // x-sy
			newPointSlpMomCorrErr(3,1)=Ce(e_embed,e_embed+4)*mm; //*GeV;   // x-p
			newPointSlpMomCorrErr(1,2)=Ce(e_embed+1,e_embed+2)*mm; // y-sx
			newPointSlpMomCorrErr(2,2)=Ce(e_embed+1,e_embed+3)*mm;   // y-sy
			newPointSlpMomCorrErr(3,2)=Ce(e_embed+1,e_embed+4)*mm; //*GeV;   // y-p
			
			newSlpMomCorrErr(1,1)=Ce(e_embed+2,e_embed+2);       // sx-sx
			newSlpMomCorrErr(2,1)=Ce(e_embed+2,e_embed+3);       // sx-sy
			newSlpMomCorrErr(3,1)=Ce(e_embed+2,e_embed+4); //*GeV;       // sx-p
			newSlpMomCorrErr(2,2)=Ce(e_embed+3,e_embed+3);       // sy-sy
			newSlpMomCorrErr(3,2)=Ce(e_embed+3,e_embed+4); //*GeV;       // sy-p
			newSlpMomCorrErr(3,3)=Ce(e_embed+4,e_embed+4); //*GeV*GeV;       // p-p 
			
			(*it)->setPointOnTrackErr(newpointErr);
			(*it)->setSlopesMomErr(newSlpMomCorrErr);
			(*it)->setPosSlopesCorr(newPointSlpMomCorrErr);
			
			debug()<<" PointONTrackErr " << (*it)->pointOnTrackErr() << " slopes " << (*it)->slopesMomErr() << endmsg;
			
			e_embed+=5;
			
		} else {
			
			(*it)->setPointOnTrack(HepPoint3D(u(u_embed)*mm,u(u_embed+1)*mm,(*it)->pointOnTrack().z()));
			double T=sqrt(1+pow(u(u_embed+2),2)+pow(u(u_embed+3),2));
			double px=u(u_embed+2)*u(u_embed+4)/T; //*GeV;
			double py=u(u_embed+3)*u(u_embed+4)/T; //*GeV;
			double pz=u(u_embed+4)/T; //*GeV;
			
			(*it)->setMomentum(HepLorentzVector(px,py,pz,sqrt(pow(u(u_embed+4),2)+pow((*it)->mass(),2))));
			
			HepSymMatrix newpointErr(3,0);
			HepSymMatrix newSlpMomCorrErr(3,0);
			HepMatrix newPointSlpMomCorrErr(3,3,0);
			
			newpointErr(1,1)=Cu(u_embed,u_embed)*mm*mm;             // x-x
			newpointErr(1,2)=Cu(u_embed,u_embed+1)*mm*mm;           // x-y
			newpointErr(2,2)=Cu(u_embed+1,u_embed+1)*mm*mm;         // y-y
			newpointErr(3,3)=0;                               // z-z
			
			
			newPointSlpMomCorrErr(1,1)=Cu(u_embed,u_embed+2)*mm;   // x-sx
			newPointSlpMomCorrErr(2,1)=Cu(u_embed,u_embed+3)*mm;   // x-sy
			newPointSlpMomCorrErr(3,1)=Cu(u_embed,u_embed+4)*mm; //*GeV;   // x-p
			newPointSlpMomCorrErr(1,2)=Cu(u_embed+1,u_embed+2)*mm; // y-sx
			newPointSlpMomCorrErr(2,2)=Cu(u_embed+1,u_embed+3)*mm;   // y-sy
			newPointSlpMomCorrErr(3,2)=Cu(u_embed+1,u_embed+4)*mm; //*GeV;   // y-p
			
			newSlpMomCorrErr(1,1)=Cu(u_embed+2,u_embed+2);       // sx-sx
			newSlpMomCorrErr(2,1)=Cu(u_embed+2,u_embed+3);       // sx-sy
			newSlpMomCorrErr(3,1)=Cu(u_embed+2,u_embed+4); //*GeV;        // sx-p
			newSlpMomCorrErr(2,2)=Cu(u_embed+3,u_embed+3);       // sy-sy
			newSlpMomCorrErr(3,2)=Cu(u_embed+3,u_embed+4); //*GeV;       // sy-p
			newSlpMomCorrErr(3,3)=Cu(u_embed+4,u_embed+4); //*GeV*GeV;       // p-p 
			
			(*it)->setPointOnTrackErr(newpointErr);
			(*it)->setSlopesMomErr(newSlpMomCorrErr);
			(*it)->setPosSlopesCorr(newPointSlpMomCorrErr);
			
			u_embed+=5;
			
		}
		
		
		if((*it)->endVertex()) {
			doFill(chis, Ce, e, Cu, u, *((*it)->endVertex()), e_embed, u_embed, ndof);
		}
		
	}
	
}


StatusCode LagrangeGlobalFitter::getChiSquare(HepVector &e0, HepVector &e, HepSymMatrix &Ce, double &chis) {
	
	HepVector ed = e0 - e;
	
	int inv;
	HepSymMatrix Ge = Ce.inverse(inv);
	
	if ( inv == 0 ) {
		chis = dot(ed, (Ge*ed));
	}
	else {
		error() << " matrix inversion problem " << endmsg;
		return StatusCode::FAILURE;
	}
	debug() << "chi =  " << chis << endmsg;
	
	// check sign of chi2
	if(chis<0) {
		error() << "negative chi2. Failure." << endmsg;
		return StatusCode::FAILURE;
	}
	
	return StatusCode::SUCCESS;
}



void LagrangeGlobalFitter::setWhichParticleLifetime(Particle *inP){
	debug() << "setting Which Particle " <<  inP <<endmsg;
	SelectParticle.push_back(&(*inP));
	debug() << "Done " <<  *inP <<endmsg;

}


StatusCode LagrangeGlobalFitter::findParticleinTree(Vertex &LastV,int indexwhichParticle, int& found)
{
	debug() << " Looking for particle ptr:"<< SelectParticle[indexwhichParticle] << endmsg;
	debug() << " Entering vertex ptr:"<< &LastV << endmsg;
	for ( SmartRefVector<Particle>::iterator iLast = (LastV.products()).begin(); iLast!=(LastV.products()).end();iLast++)
	{
		debug()  << " Particle ptr" << &(*(*iLast))<<endmsg;
		if (&(**iLast)==(SelectParticle[indexwhichParticle])) {
			found=0;
			ProductionVertex.push_back(&LastV);
			//SelectParticle=*iLast;
			DecayVertex.push_back((**iLast).endVertex());
			debug() << "I've found it!" <<endmsg;
			debug() << "Father Vertex ptr " << &LastV << endmsg;
			debug() << "Decay Vertex ptr"<< (**iLast).endVertex() << endmsg;
			return StatusCode::SUCCESS;
		}
		if ((*(*iLast)).endVertex())
			findParticleinTree(*((*(*iLast)).endVertex()), indexwhichParticle, found);
	}

	return StatusCode::SUCCESS;
}


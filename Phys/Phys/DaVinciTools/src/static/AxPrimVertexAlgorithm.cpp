// $Id: AxPrimVertexAlgorithm.cpp,v 1.1.1.1 2001-07-09 09:28:42 gcorti Exp $
#define DAVINCI_AXPRIMVERTEXALGORITHM_CPP 


// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "LHCbEvent/Event.h"
#include "CLHEP/Units/PhysicalConstants.h"
#include "PhysEvent/VtxCandidate.h"
#include "AxPrimVertexAlgorithm.h"

//--------------------------------------------------------------------
//  Implementation file for class : AXPrimVertexAlgorithm
//  
//  Description : This algorithm fills a VtxCandidate class
//                relative to the primary vertex.
//                Internally it delegates the job to the fortran 
//                routine axpv (used as well by axprim that in addition
//                fills a bank)
//
//  25/05/2001 : Gloria Corti
//
//--------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const AlgFactory<AxPrimVertexAlgorithm>  Factory;
const IAlgFactory& AxPrimVertexAlgorithmFactory =   Factory;

// Wrapping of Fortran routines
extern "C" {
#ifdef WIN32
  void __stdcall AXPV( float* pv, float* covmat, float* chi2out, int* ngood,
                       int* ierr, const int* ntra, const int* trae );
#else
  void axpv_( float* pv, float* covmat, float* chi2out, int* ngood, int* ierr,
              const int* ntra, const int* trae );

#define AXPV axpv_
#endif
}

//=============================================================================
// Standard creator, no variables to initialize
//=============================================================================
AxPrimVertexAlgorithm ::AxPrimVertexAlgorithm (const std::string& name, 
                                               ISvcLocator* pSvcLocator) 
 : Algorithm(name, pSvcLocator) {
}

//=============================================================================
// Standard destructor
//=============================================================================
AxPrimVertexAlgorithm ::~AxPrimVertexAlgorithm() { };

//=============================================================================
// Initialisation. Is empty.
//=============================================================================
StatusCode AxPrimVertexAlgorithm ::initialize() {
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
// Inside the computation is delegated to the Fortran routine in
// axreclib
//=============================================================================
StatusCode AxPrimVertexAlgorithm ::execute() {

  MsgStream         log( msgSvc(), name() );
  log << MSG::INFO << ">>> Executing " << endreq;

  // Delegate the job to the fortran world : set input variables
  int ntrae = 0;
  int trae[2] = { 0, 0 };
  // Declare output variables and initialize them to zeros
  float pv[3] = { 0.0, 0.0, 0.0 }; 
  float covmat[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
  float chi2out = 0;
  int ngood=0, ierr= 0;
  AXPV( pv, covmat, &chi2out, &ngood, &ierr, &ntrae, trae );

  if( 0 != ierr ) {
    log << MSG::WARNING << "    Primary vertex calculation failed" 
        << "AXPV Error = " << ierr << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << "    Now creating Primary Vertex" << endreq;

  VtxCandidateVector* createdVertices = new VtxCandidateVector();
  VtxCandidate* vertex = new VtxCandidate();

  // The computation in the fortran world is done assuming cm, so we
  // need to convert to the Gaudi convention of mm
  vertex->setPosition( HepPoint3D( pv[0]*cm, pv[1]*cm, pv[2]*cm ) );
  vertex->setChiSquare( chi2out );
  HepSymMatrix error(3,0);
  error(1,1) = covmat[0]*cm2;
  error(2,1) = covmat[1]*cm2;
  error(2,2) = covmat[2]*cm2;
  error(3,1) = covmat[3]*cm2;
  error(3,2) = covmat[4]*cm2;
  error(3,3) = covmat[5]*cm2;
  vertex->setCovariance(error);
  vertex->setCreatorID(1);
  // Put vertex in container
  // remeber to remove the following line it is here only for debugging
  std::cout << *vertex << std::endl;
  createdVertices->push_back(vertex);

  // Now register the container in the store
  StatusCode sc = 
   eventSvc()->registerObject("/Event/Phys","PrimaryVertices",createdVertices);
  if ( sc.isFailure() ) {
    // Unsuccesfull: delete the allocated container
    delete createdVertices;
    log << MSG::ERROR  
        << "    Unable to register /Event/Phys/PrimaryVertices" << endreq;
    return StatusCode::FAILURE;
  }


  return StatusCode::SUCCESS;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode AxPrimVertexAlgorithm ::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::INFO << ">>> Finalize" << endreq;

  return StatusCode::SUCCESS;
}







// $Id: AddFourMom.cpp,v 1.1 2002-02-08 18:25:41 gcorti Exp $

// Include files
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from Event
#include "LHCbEvent/AxPartCandidate.h"

// local
#include "AddFourMom.h"

//--------------------------------------------------------------------
//
//  ClassName  : AddFourMom
// 
//  Description: This method returns the four momentum and the charge
//               obtained combining a list of AxPartCandidates
//
//  Author     : G.Corti  
//
//--------------------------------------------------------------------

//==================================================================
// Instantiation of a static factory class used by clients to create
// instances of this service
//==================================================================
static ToolFactory<AddFourMom> s_factory;
const IToolFactory& AddFourMomFactory = s_factory;

//==================================================================
// Standard Constructor
//==================================================================
AddFourMom::AddFourMom(const std::string& type, const std::string& name, 
                       const IInterface* parent) 
    : AlgTool( type, name, parent ) {

  // This tool needs to use internally the ParticlePropertySvc to retrieve the
  // mass to be used
  m_ppSvc = 0;
  if( serviceLocator() ) {
    StatusCode sc = StatusCode::FAILURE;
    sc = serviceLocator()->service( "ParticlePropertySvc", m_ppSvc );
  }  

  declareInterface<IAddFourMom>(this);
}

//==================================================================
// Create a candidate particle combining the particles in the list
// add four-momentum according to condition chosen in 
//==================================================================
StatusCode AddFourMom::calculate( const std::vector<AxPartCandidate*> pList,
                                 HepLorentzVector& pOut, double charge ) {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "Combining particles charge and 4 momentum" << endreq;
 
  if( 0 == m_ppSvc ) return StatusCode::FAILURE;

  // Reset output arguments
  pOut = HepLorentzVector(0.0, 0.0, 0.0, 0.0);
  charge = 0;
  int icount = 0;

  // now do the addition
  std::vector<AxPartCandidate*>::const_iterator iterP;
  
  for( iterP = pList.begin(); iterP != pList.end(); iterP++ ) {
    ParticleProperty* pp = 0;
    long pID = (*iterP)->particleID().id();
    pp = m_ppSvc->findByStdHepID( pID );
    if( pp == 0 ) { 
      log << MSG::ERROR << "Not found particle with requested pID = " 
          << pID << endreq;
      return StatusCode::FAILURE;
    }

    icount++;
    pOut    += (*iterP)->fourMomentum( pp->mass() );
    charge  += (*iterP)->charge();
  }
  
  log << MSG::DEBUG << "Mass of " << icount << " particles = " 
      << pOut.m() << endreq;
  return StatusCode::SUCCESS;

}




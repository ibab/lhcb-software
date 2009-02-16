// $Id: Dstar2D0Pi.cpp,v 1.1.1.1 2009-02-16 14:11:47 jonrob Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "Dstar2D0Pi.h"

using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : Dstar2D0Pi
//
// 2009-02-09 : Andrew Stephen Powell
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Dstar2D0Pi );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Dstar2D0Pi::Dstar2D0Pi( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
{
  //Declare Algorithm Properties:
  declareProperty( "DstarMassWindow", m_DstarMassWin = 300.0*MeV );
  declareProperty( "DstarVertexChi2Cut", m_VertexChi2Dstar = 100.0 );
  declareProperty( "DstarMinPtCut", m_DstarPtCut = 0.0*MeV );
  declareProperty( "DstarD0MassDiff",  m_diffDstarD0 = 200.*MeV );
}
//=============================================================================
// Destructor
//=============================================================================
Dstar2D0Pi::~Dstar2D0Pi() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Dstar2D0Pi::initialize() {
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // Retrieve Geant3 ID Numbers for Each Particle 
  const LHCb::ParticleProperty* partProp;
  partProp = ppSvc()->find( "D0" );
  m_D0ID = LHCb::ParticleID(partProp->pdgID());
  m_D0Mass = partProp->mass();
  partProp = ppSvc()->find( "D*(2010)+" );
  m_DstarPlusID = LHCb::ParticleID(partProp->pdgID());
  partProp = ppSvc()->find( "D*(2010)-" );
  m_DstarMinusID  =  LHCb::ParticleID(partProp->pdgID());
  m_DstarMass = partProp->mass();
  m_D0DstarMassDiff = m_DstarMass-m_D0Mass;
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Dstar2D0Pi::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  setFilterPassed(false);  // Mandatory. Set to true if event is accepted. 

  counter("Events")++;
  
  // The Algorithm Code:
  LHCb::Particle::ConstVector daughters = desktop()->particles();
  sc = MakeDstar2D0Pi(daughters);
  if(!sc) return sc;
  return StatusCode::SUCCESS;
}

//=========================================================================
//  
//=========================================================================
StatusCode Dstar2D0Pi::MakeDstar2D0Pi(const LHCb::Particle::ConstVector& daughters ) {

  // Event Number Now Processing
  debug()<< "Processing event number " 
	<< counter("Events").nEntries() 
	<< endmsg;

  // Obtain Primary Vertices:
  const LHCb::RecVertex::Container* vPrimaryVertices = 
    desktop()->primaryVertices();
  const LHCb::Vertex::ConstVector vSecondaryVertices = 
    desktop()->secondaryVertices();
  
  // Isolate D0s and Pions
  LHCb::Particle::ConstVector vPions, vD0s;
  LHCb::RecVertex::ConstVector::const_iterator ivert; 

  StatusCode sc = StatusCode::SUCCESS;
  sc = particleFilter()->filterByPID( daughters, vD0s, "D0" ); 
  // CC Done by Default
  sc = particleFilter()->filterByPID( daughters, vPions, "pi+");
  // CC Done by Defualt

  debug()<< "Number of D0s found = " 
	 << vD0s.size() << endmsg;
  debug()<< "Number of Pions found = " 
	 << vPions.size() << endmsg;

  if ( (vD0s.size() == 0) || (vPions.size() == 0) ) {
    debug() << "vD0 empty: No D0 found" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Find All Two Particle Combinations to Form D*
  counter("DstarCandsInEvent").reset();
  LHCb::Particle::ConstVector::const_iterator  i1, i2;
  debug()<< "Finding All Possible D*->D0Pi Combinations" << endmsg;
  
  // Loop Over All D0 Candidates
  for ( i1 = vD0s.begin(); i1 != vD0s.end(); i1++ ) {

    // Loop Over All Pi Candidates
    for ( i2 = vPions.begin(); i2 != vPions.end(); i2++ ) {

      // Check Overlap (Ensure Pi is not already used to make the D0 )
      const SmartRefVector<LHCb::Particle> & Combin
        = (*i1)->endVertex()->outgoingParticles() ;
      for ( SmartRefVector<LHCb::Particle>::const_iterator rit 
	      = Combin.begin();
            rit != Combin.end(); rit++ ) {
        if ( (*rit)->key() == (*i2)->key() ) 
	  verbose() << "Overlap!"<< endmsg;
	  continue;
      }
      
      // Ensure D0Pi+ and D0barPi- Combinations
      if ( (*i2)->particleID().pid()>0 && (*i1)->particleID().pid() > 0)
	{
	  m_DstarPID=m_DstarPlusID;
	}
      else if( (*i2)->particleID().pid()<0 && (*i1)->particleID().pid()< 0) 
	{
	  m_DstarPID=m_DstarMinusID;
	}
      else
	continue;

	// Find the Invariant Mass of the D0/pi Combination
	Gaudi::LorentzVector D0PionComb =
          (*i1)->momentum() + (*i2)->momentum() ;
        double mass  = D0PionComb.M()/MeV;
        verbose()<<"Nominal D Mass: "<<m_D0Mass<<endmsg;
        verbose()<<"Invariant Mass of Candidate: "<<mass<<endmsg;
        
	// Apply Mass Window Cut
        if ( fabs( mass - m_DstarMass ) < m_DstarMassWin ) {
          debug() << "Invariant Mass of D0/Pi Combination = "
		 << mass << " MeV/c^2 : " 
		 << "Passed "
		 << m_DstarMassWin
		 <<" MeV/c^2 Mass Window Cut" << endmsg;
	  
	  // Apply Mass Difference Cut
	  double difference = mass - ( (*i1)->momentum().M()/MeV );
	  if ( ( difference >=0 ) 
	    && (fabs(difference - m_D0DstarMassDiff) < m_diffDstarD0) ){
	  debug()<<"Measured Mass Difference = "<<difference<<"MeV/c^2"<<endmsg;
	  debug()<<"PDG Mass Difference ="<<m_D0DstarMassDiff<<"MeV/c^2"<<endmsg;
	  debug()<<"Passed "
		 <<m_diffDstarD0
		 <<" MeV/c^2 Mass Difference Window" << endmsg;

	  // Perform Unconstrained Vertex Fit:
          LHCb::Vertex D0PionVertex_NoPid;
          LHCb::Particle candDstar(m_DstarPID);
          StatusCode SCD0PionVertex_NoPid=
            vertexFitter()->fit(*(*i1),
                                *(*i2),
                                candDstar,
                                D0PionVertex_NoPid);
          
          if (!SCD0PionVertex_NoPid) {
            Warning("Fit error",StatusCode::SUCCESS,1).ignore();
	    continue;
          }

	  // Apply Vertex Chi2 Cut
          if ( SCD0PionVertex_NoPid
               && (D0PionVertex_NoPid.chi2() < m_VertexChi2Dstar) ) 
	    {
	      info()<< "Chi2 of Unconstrained D0/Pi Vertex Fit = "
		    << D0PionVertex_NoPid.chi2() 
		    << " : Passed "
		    << m_VertexChi2Dstar
		    <<" cut" << endmsg;
          
	      // Apply D* Pt and Mass Window Cuts
	      if( ( (((&candDstar)->pt()) > m_DstarPtCut) && 
		    (fabs( ((&candDstar)->momentum().M())- m_DstarMass ) <
		     m_DstarMassWin)) ) {

		double difference1 =  ((&candDstar)->momentum().M())
                  - ( (*i1)->momentum().M()/MeV );
                if ((difference1 >=0) && 
		    (fabs(difference1 - m_D0DstarMassDiff) < m_diffDstarD0)){
		  
		  setFilterPassed(true);
                  if ( m_DstarPID == m_DstarPlusID ) counter("DstarPlus")++;
                  else counter("DstarMinus")++;
		  counter("DstarCandsInEvent")++;

		  // D* Candidate Passed All Cuts! - Save to Desktop
                  desktop()->keep(&candDstar);
		  info()<<"Saved a D* Candidate to the Physics Desktop "
			 <<counter("DstarCandsInEvent").nEntries()
			 <<endmsg;

		  /*plot((&candDstar)->momentum().M(),
		       "D* Mass", 
		       "Mass",
		       m_DstarMass-m_DstarMassWin,
		       m_DstarMass+m_DstarMassWin);*/
		  
		  
		  const SmartRefVector<LHCb::Particle> & Combin2
		    = (&candDstar)->endVertex()->outgoingParticles() ;
		  SmartRefVector<LHCb::Particle>::const_iterator dstit;
		  for ( dstit = Combin2.begin(); dstit != Combin2.end(); dstit++ ) {
		    if ( fabs((*dstit)->particleID().pid()) == 421){
		      
		      /*plot( (*dstit)->momentum().M(),
			    "D0 Mass",
			    "Mass",
			    m_D0Mass-25/MeV,
			    m_D0Mass+25/MeV);*/
		    }
		  }
		} 
		  
	      } // Closes if ( DstPt ... )
            } // Closes if ( unconstVertexFit...) 
	  } // Closes if ( difference...)
	} 
    } // Closes for ( iPions = vPions.begin()...) 
  } // Closes for ( iD0s = vD0s.begin()...)
  return desktop()->saveDesktop();

  // Confirm No Overlaps
  const LHCb::Particle::ConstVector parts = desktop()->particles();
  LHCb::Particle::ConstVector vDstar;
  sc = StatusCode::SUCCESS;
  sc = particleFilter()->filterByPID(parts, vDstar, "D*(2010)+");
  // CC Done by Default
  LHCb::Particle::ConstVector::const_iterator oit;
  for (oit = vDstar.begin();  oit !=  vDstar.end(); oit++ ) {
    
    if ( checkOverlap()->foundOverlap(*oit) ){
      debug()<<"Found Overlap!!"<<endmsg;
      counter("FoundOverlapp")++;
    }
  }
   
  // Log number of combinations found:
  info() << "Number of D0/Pi Combinations: " 
	 << counter("DstarCountPerEvent").nEntries() << endmsg;

  // Save desktop entries to TES:
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Dstar2D0Pi::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DVAlgorithm::finalize();
}

//=============================================================================

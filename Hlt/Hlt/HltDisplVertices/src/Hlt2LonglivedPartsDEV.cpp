/************* TODO ***********
 * Add a cut on the number of Particle ID, with sepcial cuts...
 ******************************/

// $Id: Hlt2LonglivedPartsDEV.cpp,v 1.1 2010-01-28 15:20:11 ngueissa Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "Hlt2LonglivedPartsDEV.h"

using namespace Gaudi::Units ;
using namespace LHCb;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : Hlt2LonglivedPartsDEV
//
// 2010-01-22 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Hlt2LonglivedPartsDEV );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Hlt2LonglivedPartsDEV::Hlt2LonglivedPartsDEV( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_nbevent(0)
    , m_nbpassed(0)
{
  declareProperty("MinNbParts", m_MinNbParts = 1 );
  declareProperty("MinNbTracks", m_MinNbtrks = 0 );
  declareProperty("RMin", m_RMin = 0.5 );
  declareProperty("MinMass", m_MinMass = 6.5*GeV );
  declareProperty("MinSumpt", m_MinSumpt = 6*GeV );
  declareProperty("MinRecpt", m_MinRecpt = 0*GeV );
  declareProperty("MinX", m_MinX = -10.*m );
  declareProperty("MaxX", m_MaxX = 10.*m );
  declareProperty("MinY", m_MinY = -10.*m );
  declareProperty("MaxY", m_MaxY = 10.*m );
  declareProperty("MinZ", m_MinZ = -10.*m );
  declareProperty("MaxZ", m_MaxZ = 100*m );
}
//=============================================================================
// Destructor
//=============================================================================
Hlt2LonglivedPartsDEV::~Hlt2LonglivedPartsDEV() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Hlt2LonglivedPartsDEV::initialize() {
  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if(msgLevel(MSG::DEBUG)){
    debug() <<"==> Initialize Hlt2LonglivedPartsDE"<< endmsg;
    debug() <<"Min number of candidates       : "<< m_MinNbParts << endmsg;
    debug() <<"---------------- CUTS on candidates --------------"<< endmsg;
    debug() <<"Min radial displacement        "<< m_RMin <<" mm"<< endmsg;
    debug() <<"Min measured mass              "<< m_MinMass/GeV 
	    <<" GeV"<< endmsg;
    debug() <<"Min measured pT                "<< m_MinRecpt/GeV 
	    <<" GeV"<< endmsg;
    debug() <<"Min sum of all daughter tracks "<< m_MinSumpt/GeV 
	    <<" GeV"<< endmsg;
    debug() <<"Min X position                 "<< m_MinX/mm <<" mm"<< endmsg;
    debug() <<"Max X position                 "<< m_MaxX/mm <<" mm"<< endmsg;
    debug() <<"Min Y position                 "<< m_MinY/mm <<" mm"<< endmsg;
    debug() <<"Max Y position                 "<< m_MaxY/mm <<" mm"<< endmsg;
    debug() <<"Min Z position                 "<< m_MinZ/mm <<" mm"<< endmsg;
    debug() <<"Max Z position                 "<< m_MaxZ/mm <<" mm"<< endmsg;
    debug() <<"Min number of tracks           "<< m_MinNbtrks << endmsg;
    debug() <<"BEWARE : not all daughters may be saved !"<< endmsg;
    debug() <<"------------------------------------"<< endmsg;
  }

  m_BeamLine = new Particle();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Hlt2LonglivedPartsDEV::execute() {

  ++m_nbevent;
  if( msgLevel(MSG::DEBUG) ) debug() << "==> Execute event "
				      << m_nbevent  << endmsg;
  setFilterPassed(false);

  //A container to save all candidates
  Particle::ConstVector RecParts;

  //Get the Beam line
  if( exist<Particles>( "HLT/Hlt2LineDisplVertices/BeamLine" ) ){
    Particles* BL = get<Particles>( "HLT/Hlt2LineDisplVertices/BeamLine" );
    m_BeamLine = *(BL->begin());
    if( msgLevel(MSG::DEBUG) )    
      debug()<<"Beam line position "<< m_BeamLine->referencePoint()
	     <<", direction " << m_BeamLine->momentum() << endmsg;
  } else { desktop()->saveTrees( RecParts ); }

  //Get the candidates
  const Particle::ConstVector & Parts = desktop()->particles();
  if(msgLevel(MSG::DEBUG)) 
    debug()<<"Retrieve "<< Parts.size() <<" particles" << endmsg;
  if( Parts.size() < m_MinNbParts ){
    if(msgLevel(MSG::DEBUG)) 
      debug()<<"Insufficent nb of candidates, event rejected !"<< endmsg; 
    return desktop()->saveTrees( RecParts );
  }
  //Loop on the candidates
  Particle::ConstVector::const_iterator ip = Parts.begin();
  for( ; ip != Parts.end(); ++ip ){
    const Particle * p = *ip;

    //Get rid of non-reconstructed particles, i.e. with no daughters.
    if( p->isBasicParticle() ){ 
      debug()<<"Basic particle !" << endmsg; 
      continue;
    }

    double mass = p->measuredMass();
    int nbtrks = p->daughtersVector().size();
    double chi = p->endVertex()->chi2PerDoF();
    Gaudi::XYZPoint pos = p->endVertex()->position();
    double rho = RadDist( pos );
    Gaudi::LorentzVector mom = p->momentum();

    //Print out
    if( msgLevel(MSG::DEBUG) ){
      debug()<<"Candidate with mass " << mass/GeV 
	     <<" GeV, nb of tracks " << nbtrks << ", Chi2/ndof " 
	     << chi <<", R "<< rho <<", pos of end vtx " 
	     << pos << endmsg;
    }

    if( mass < m_MinMass || nbtrks < m_MinNbtrks || rho <  m_RMin || 
	pos.x() < m_MinX || pos.x() > m_MaxX || pos.y() < m_MinY || 
	pos.y() > m_MaxY || pos.z() < m_MinZ || pos.z() > m_MaxZ 
	// || chi > m_MaxChi2OvNDoF 
	){
      if( msgLevel( MSG::DEBUG ) )
        debug()<<"Particle do not pass the cuts"<< endmsg; continue; 
    }
    RecParts.push_back( p );
  }

  unsigned int NbCandidates = RecParts.size(); 
  if( NbCandidates >= m_MinNbParts ){
    if(msgLevel(MSG::DEBUG))
      debug()<<"Event satisfied criteria ! "
	     <<"Nb of candidates "<< NbCandidates << endmsg;
    setFilterPassed(true);
    ++m_nbpassed;
  } else { if(msgLevel(MSG::DEBUG)) debug()<<"Event rejected !"<< endmsg; }


  return desktop()->saveTrees( RecParts );;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Hlt2LonglivedPartsDEV::finalize() {

  if ( msgLevel(MSG::DEBUG) ){ debug() << "==> Finalize Hlt2LonglivedPartsDEV" 
				       << endmsg;
    if(m_nbevent == 0) m_nbevent++;
    double err = 10.*std::sqrt( static_cast<double>(m_nbpassed/m_nbevent) );
    debug() << "------------- Efficiency -----------"<< endmsg;
    debug() << "| Accepted event (%)     "<< 100.*m_nbpassed/m_nbevent 
	    <<"+-" << err
	    << " ( "<< m_nbpassed <<" )" << endmsg;
    debug() << "| Total number of events " << m_nbevent << endmsg;
    debug() << "------------------------------------"<< endmsg;
  }

  //delete m_BeamLine;

  return DVAlgorithm::finalize();
}

//============================================================================
// Distance of a XYZPoint to the beam interaction line
//============================================================================
double Hlt2LonglivedPartsDEV::RadDist( const Gaudi::XYZPoint& p ){
  
  //intersection of the beam line with the XY plane, 
  //find the lambda parameter of the line.
  double lambda = (p.z() - m_BeamLine->referencePoint().z()) /
    m_BeamLine->momentum().z();

  //find x and y of intersection point
  double x = m_BeamLine->referencePoint().x() 
    + lambda * m_BeamLine->momentum().x();
  double y = m_BeamLine->referencePoint().y() 
    + lambda * m_BeamLine->momentum().y();
  
  //return distance to the intersection point 
  x -= p.x(); y -= p.y();
  return sqrt( x*x + y*y );
}

//=============================================================================

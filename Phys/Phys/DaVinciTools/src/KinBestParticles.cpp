// $Id: KinBestParticles.cpp,v 1.1 2004-07-16 11:20:48 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "KinBestParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : KinBestParticles
//
// 2004-07-15 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<KinBestParticles>          s_factory ;
const        IToolFactory& KinBestParticlesFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
KinBestParticles::KinBestParticles( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IBestParticles>(this);

  declareProperty( "byPt", m_byPt = true );  
  declareProperty( "byP", m_byP = false );  
  declareProperty( "byE", m_byE = false );  
  declareProperty( "number", m_number = 1 );  
}
//=============================================================================
// Destructor
//=============================================================================
KinBestParticles::~KinBestParticles() {}; 

//=============================================================================
// Initialize
//=============================================================================
StatusCode KinBestParticles::initialize() {

  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }

  // P and E are exclusive
  if (( m_byP ) && ( m_byE )){
    err() << "Cannot sort by momentum and energy at the same time" << endreq;
    return StatusCode::FAILURE;
  }
  // Pt and (E or P) are exclusive too, but Pt is on by default -> switch off
  if (( m_byPt ) && (( m_byE ) || ( m_byP ))){
    warning() 
      << "Switching off (default) sorting by Pt since P or E is requested" 
      << endreq;
    m_byPt = false ; 
  }
  // Need at least one
  if (!(( m_byPt ) || ( m_byE ) || ( m_byP ))){
    err() << "No sorting variable defined" << endreq; 
    return StatusCode::FAILURE;
  }
  // No particles requested
  if (m_number < 1){
    err() << "Required " << m_number << " Particles to output" << endreq;
    return StatusCode::FAILURE;    
  }  
  // Now it's OK
  info() << "Will get the " << m_number << " particles with highest ";
  if (m_byPt) info() << "Pt" ;
  else if (m_byE) info() << "E" ;
  else if (m_byP) info() << "P" ;
  info() << endreq;

  return StatusCode::SUCCESS;
  
}
// ============================================================================
// ============================================================================
StatusCode KinBestParticles::finalize(){
  debug() << "Finalizing KinBestParticles" << endreq;
  return GaudiTool::finalize() ;
}
//=============================================================================
/// Get m_number candidates with highest <var>
//=============================================================================
StatusCode KinBestParticles::Candidates( const ParticleVector & input,
                                         ParticleVector& output){
  
  StatusCode sc = getCandidates( input, output, m_number);  
  return sc ;
}
 
//=============================================================================
/// Get the candidate with highest <var>
//=============================================================================
StatusCode KinBestParticles::BestCandidate( ParticleVector& input,
                                            Particle* output){
  ParticleVector PV;
  StatusCode sc = getCandidates( input, PV, 1);
  if (!sc)  return sc ;
  output = PV[0];
  
  return sc ;
}
//=============================================================================
/// Get the n candidates with highest <var>
//=============================================================================
StatusCode KinBestParticles::getCandidates( const ParticleVector & input,
                                         ParticleVector& output, const int& n){
  std::vector<PResult> PandV;
  ParticleVector::const_iterator p ;
  debug() << "::getCandidates - vector size " << input.size() << endreq;
  int nt = 0 ;
  for ( p = input.begin() ; p != input.end() ; p++ ){
    nt++ ;
    double var = -1.;
    if ( m_byPt ) var = (*p)->pt();
    else if ( m_byP ) var = (*p)->p();
    else if ( m_byE ) var = (*p)->momentum().e();
    debug() << "Track " << nt << ", value is " << var << endreq;
    PResult PR((*p),var);
    if (  PandV.size() == 0 ){
      PandV.push_back(PR); // insert first
      verbose() << "Pushing back first particle" << endreq;
    } else {
      std::vector<PResult>::iterator r = PandV.begin();
      int i=0;
      while (( (*r).getV() > var ) && ( i < n )){
        verbose() << "Loop: Stored value " << i << " is " << 
          (*r).getV() << endreq;
        r++ ; i++;
        if ( r == PandV.end() ) break;      
      } 
      if (i < n) {
        PandV.insert(r,PR); // insert here
        verbose() << "Inserted particle " << nt << " at position " 
                  << i << endreq;
      } else verbose() << "Did not insert particle " << nt << endreq; 
    }
  }
  // copy the output  
  std::vector<PResult>::iterator r ;
  int i = 0 ;
  for ( r = PandV.begin() ; r != PandV.end() ; r++){
    double var = (*r).getV();
    Particle* P = (*r).getP();
    if ( i < n ){
      debug() << "Good candidate " << i << "  with V= " << var
              << " and P= " << P->momentum()  << endreq;
      output.push_back(P);
    } else verbose() << "Ignored candidate with V= " << var
                     << "and P= " <<  P->momentum()  << endreq;    
    i++;
  }

  return StatusCode::SUCCESS;
}

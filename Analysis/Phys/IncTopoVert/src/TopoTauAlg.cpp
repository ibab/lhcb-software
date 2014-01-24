// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "TopoTauAlg.h"
#include "Event/Particle.h"
#include "Event/VertexBase.h"
#include "Event/RecVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TopoTauAlg
// 
// 2013-12-17 : Julien Cogan, Giampiero Mancinelli
//-----------------------------------------------------------------------------

using namespace Gaudi;
using namespace LHCb;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( TopoTauAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TopoTauAlg::TopoTauAlg( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : DaVinciAlgorithm ( name , pSvcLocator ),
  m_nEvents(0),m_nAccepted(0),m_nCandidates(0)
{
  //declareProperty("TrackInputLocation", m_tracksInputLocation = "Rec/Track/Best");
  declareProperty("VFParticlesInputLocation", m_VFparticlesInputLocation = "Phys/StdAllNoPIDsPions/Particles");
  //declareProperty("ParticlesInputLocation", m_particlesInputLocation);
  declareProperty("OutputLocation", m_outputLocation);
//   declareProperty("cut_ips"     , m_cut_ips  = 10.     ); // 
//   declareProperty("cut_ips_VF"     , m_cut_ips_VF  = 20.     ); // 
//   declareProperty("cut_ghost"     , m_cut_ghost  = 0.3     ); //
  declareProperty("cut_ntrk"     , m_cut_ntrk  = 150     ); //
}
//=============================================================================
// Destructor
//=============================================================================
TopoTauAlg::~TopoTauAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TopoTauAlg::initialize() {
  StatusCode sc = DaVinciAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by DaVinciAlgorithm

  debug() << "==> Initialize" << endmsg;

  m_topoVertexTool = tool<ITopoVertexTool>("TopoVertexTool",this);
  m_combiner = tool<IParticleCombiner>("LoKi::FastVertexFitter");
  if ( !m_topoVertexTool ) return Error("Topo vertex tool not found",StatusCode::FAILURE);
  if ( !m_combiner ) return Error("Particle combiner tool not found",StatusCode::FAILURE);
  m_Geom = tool<IDistanceCalculator>("LoKi::DistanceCalculator", this);
   if ( ! m_Geom ) {
    fatal() << "DistanceCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }
 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode TopoTauAlg::execute() {

  debug() << "==> Execute" << endmsg;
  ++m_nEvents;  // Increment event counter
  bool passed(false);
  
  setFilterPassed( false );
  
  //   double cut_ips = m_cut_ips;
  //   double cut_ips_VF = m_cut_ips_VF;
  //   double cut_ghost = m_cut_ghost;
  double cut_ntrk = m_cut_ntrk;
  //   double ipsall;
  
  // Get tracks for vertex function 
  /*
    LHCb::Tracks* input_tracks = get<LHCb::Tracks *>(m_tracksInputLocation);
    std::vector<const LHCb::Track *> tracks_for_vfunc;
    tracks_for_vfunc.reserve(input_tracks->size());
    for (LHCb::Tracks::iterator it_ptrk = input_tracks->begin(); it_ptrk<input_tracks->end(); ++it_ptrk){
    tracks_for_vfunc.push_back(*it_ptrk);
    }
  */
  
  LHCb::Particles * parts_for_vfunc = get<LHCb::Particles>(m_VFparticlesInputLocation);
  std::vector<const LHCb::Track *> tracks_for_vfunc;
  tracks_for_vfunc.reserve(parts_for_vfunc->size());
  const LHCb::Particle::ConstVector& parts_for_vfind = this->i_particles();
  std::map<const LHCb::Track *,const LHCb::Particle *> tracks_to_part_map;
  std::vector<const LHCb::Track *> tracks_for_vfind;
  if (parts_for_vfind.size()>cut_ntrk) return StatusCode::SUCCESS;
  tracks_for_vfind.reserve(parts_for_vfind.size());

  /*  
      RecVertex::Container* verts = NULL;  
      if(exist<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary)){    
      verts = get<LHCb::RecVertex::Container>(LHCb::RecVertexLocation::Primary);    
      }  
      else {    
      warning()<<"No primary vertex at location"<<endreq;    
      return StatusCode::SUCCESS;    
      }

      for ( LHCb::Particles::const_iterator it_part = parts_for_vfunc->begin(); 
            it_part != parts_for_vfunc->end() ; ++it_part) {
      const LHCb::Track* it_ptrk = (*it_part)->proto()->track();
      
      ipsall=100000.;
      double imp, impchi2;
      RecVertex::Container::const_iterator iv;
      for ( iv = verts->begin(); iv != verts->end(); iv++) {
      StatusCode sc2 = m_Geom->distance((it_ptrk),(*iv),imp,impchi2);
      if (sc2.isFailure() ) 
      {    
      Warning("m_Geom problems",StatusCode::SUCCESS,20);    
      continue;
      }
      if (impchi2<ipsall) {
      ipsall = impchi2;
      }
      }
      
      double ghostProb;
      ghostProb = (*it_ptrk).ghostProbability();
      ;  
      if (ipsall>cut_ips_VF && ghostProb <cut_ghost) {
      
      tracks_for_vfunc.push_back(it_ptrk);
      
      }
      
      }
      
      
      // Get tracks for vertex finding and store track <-> particle correspondance
      //LHCb::Particles * parts_for_vfind = get<LHCb::Particles>(m_particlesInputLocation);
      //    const LHCb::Particle::ConstVector& parts_for_vfind = this->i_particles();
      //   std::map<const LHCb::Track *,const LHCb::Particle *> tracks_to_part_map;
      //  std::vector<const LHCb::Track *> tracks_for_vfind;
      //  tracks_for_vfind.reserve(parts_for_vfind.size());
      
      for ( LHCb::Particle::ConstVector::const_iterator it_part = parts_for_vfind.begin();  
            it_part != parts_for_vfind.end() ; ++it_part) {
      const LHCb::Track* it_ptrk = (*it_part)->proto()->track();
      
      ipsall=100000.;
      double imp, impchi2;
      RecVertex::Container::const_iterator iv;
      for ( iv = verts->begin(); iv != verts->end(); iv++) {
      StatusCode sc2 = m_Geom->distance((it_ptrk),(*iv),imp,impchi2);
      if (sc2.isFailure() ) 
      {    
      Warning("m_Geom problems",StatusCode::SUCCESS,20);    
      continue;
      }
      if (impchi2<ipsall) {
      ipsall = impchi2;
      }
      }
      
      double ghostProb;
      ghostProb = (*it_ptrk).ghostProbability();
      
      if (ipsall>cut_ips && ghostProb <cut_ghost) {  
      
      tracks_for_vfind.push_back(it_ptrk);
      tracks_to_part_map[it_ptrk]=*it_part;
      
      }
      
  }
  
  // Make the inclusive vertexing
    
  //      StatusCode  sc1 = m_topoVertexTool->findVertices(tracks_for_vfind,tracks_for_vfunc);
  */
  for ( LHCb::Particle::ConstVector::const_iterator it_part = parts_for_vfind.begin();  
        it_part != parts_for_vfind.end() ; ++it_part) {
    const LHCb::Track* it_ptrk = (*it_part)->proto()->track();
    tracks_for_vfind.push_back(it_ptrk);
    tracks_to_part_map[it_ptrk]=*it_part;
  }
  
  StatusCode  sc1 = m_topoVertexTool->findVertices(tracks_for_vfind,tracks_for_vfind);
  if ( sc1.isFailure() ) return Warning("FindVertices failed",StatusCode::SUCCESS,20);
  
  // List of output particles
  //LHCb::Particles *pmothers = new LHCb::Particles();
  //put(pmothers,m_outputLocation);
  
  // Make a Tau for each vertex with each 3 tracks vertex with charge +1 or -1
  
  std::vector<LHCb::RecVertex*> vertices = m_topoVertexTool->vertices();
  
  for (LHCb::RecVertex::Container::const_iterator it_vtx = vertices.begin(); it_vtx != vertices.end() ;++it_vtx){
    LHCb::Particle::ConstVector daughters;
    int sum_charge = 0;
    for (SmartRefVector< LHCb::Track >::const_iterator it_ptrk=(*it_vtx)->tracks().begin(); 
         it_ptrk!=(*it_vtx)->tracks().end(); ++it_ptrk){
      daughters.push_back(tracks_to_part_map[*it_ptrk]);
      sum_charge+=(*it_ptrk)->charge();
    }
    const LHCb::Particle *deskPart ;
    
    LHCb::Vertex   dummy_vtx;
    LHCb::Particle mother;
    StatusCode sc0 = m_combiner->combine(daughters,mother,dummy_vtx);
    if (mother.measuredMass() >50000) {
      deskPart = this->markTree( &(mother) );
      mother.setParticleID(LHCb::ParticleID(0));
      setFilterPassed( true );
      ++m_nCandidates;
      passed = true;
    }
    else {
      if (abs(sum_charge)!=1) continue;
      if ((*it_vtx)->tracks().size()!=3) continue;
      if (sum_charge==-1) mother.setParticleID(LHCb::ParticleID(15));
      if (sum_charge==+1) mother.setParticleID(LHCb::ParticleID(-15));
      deskPart = this->markTree( &(mother) );
      setFilterPassed( true );
      ++m_nCandidates;
      passed = true;
    }
    
    
    if( !deskPart )
      {
        err() << " Unable to save particle" << endmsg;
        return StatusCode::FAILURE;
      }
  }
  
  
  if (passed) ++m_nAccepted;   // Increment accepted event counter
  
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TopoTauAlg::finalize() {
  
  debug() << "==> Finalize" << endmsg;
  info() << "Passed " << m_nCandidates << " candidates in "
         << m_nAccepted << " accepted events among "
         << m_nEvents << " events" << endmsg ;
  
  return DaVinciAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

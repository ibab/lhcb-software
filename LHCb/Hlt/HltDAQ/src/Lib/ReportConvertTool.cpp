// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "HltDAQ/ReportConvertTool.h"

using LHCb::HltObjectSummary;
using LHCb::HltSelRepRBStdInfo;

//-----------------------------------------------------------------------------
// Implementation file for class : ReportConvertTool: ReportConvertTool.cpp
// Author: Sean Benson
// 05/06/2008
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ReportConvertTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ReportConvertTool::ReportConvertTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : GaudiTool( type, name , parent )
, m_version(-999)
{
  declareInterface<IReportConvert>(this);

  m_LatestVersion=1;
  for(it_unordered_map2 track_it = m_track_unordered_map2_Turbo.begin(); track_it!=m_track_unordered_map2_Turbo.end(); track_it++){
    if(track_it->first > m_LatestVersion) m_LatestVersion = track_it->first;
  }
}

float ReportConvertTool::floatFromInt(unsigned int i) {
  union IntFloat { unsigned int mInt; float mFloat; };
  IntFloat a; a.mInt=i;
  return a.mFloat;
}

void ReportConvertTool::setReportVersion(int version){
  m_version = version;
}

void ReportConvertTool::setReportVersionLatest(){
  m_version = m_LatestVersion;
}

int ReportConvertTool::getReportVersion(){
  return m_version;
}

void ReportConvertTool::SummaryFromRaw(HltObjectSummary::Info* info, HltSelRepRBStdInfo::StdInfo* subbank, int classID) {
  
  // Version number that we use to ensure backwards compatibility with Run 1
  // (has nothing to do with header ID).
  int run1version=-999;

  debug() << "SummaryFromRaw call for ID:" << classID << endmsg;

  // Make sure the tool is given a version to use
  if( m_version == -999 ){ 
    Warning( "I have not been told a version number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }

  debug() << "Assigning used_map with version:" << m_version << endmsg;
  
  // Which one of the member maps do we need to use?
  /*
   * Word on versions:
   * - Version 3 and greater for Run 2
   * - Versions 1 and 2 for Run 1
   * v3 is the first version in the header ID to have turbo information.
   * v1 and v2 in the header ID have nothing to do with level of information in the reports.
   * If (v1 || v2), Track, Particle, RecVertex and CaloCluster need to look for the level of information.
   * - Need to protect against someone with v1 or v2 asking for a Turbo map.
   * - Need to protect against v1 or v2 requesting a Turbo object decode (i.e. RichPID, etc.)
   */

  if((m_version<3 && m_version>0) && ( (classID!=LHCb::CLID_Track) && (classID!=LHCb::CLID_Particle) && (classID!=LHCb::CLID_RecVertex) && (classID!=LHCb::CLID_CaloCluster) ) )
      Error( std::string{ "Asked to decode Run 1 style reports with Turbo object (" } + std::to_string(classID) + std::string{ "). Please update your software" }, StatusCode::FAILURE, 100 );

  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  switch( classID )
  {
    case LHCb::CLID_Track:
      {
        if(m_version<3) {
          // Looking at Run 1 data, need to know which map to use
          if( subbank->size() == (m_track_unordered_map2.at(1)).size() ) run1version = 1;
          else if( subbank->size() == (m_track_unordered_map2.at(2)).size() ) run1version = 2;
          else Error( "Track requested in Run 1 reports, but reports are unknown" , StatusCode::FAILURE, 100 );
          used_map = m_track_unordered_map2;
          m_version=run1version;
        }
        else if( subbank->size()<(m_track_unordered_map2_Turbo.at(m_version)).size() ) used_map = m_track_unordered_map2;
        else used_map = m_track_unordered_map2_Turbo;
      }
      break;
    case LHCb::CLID_RecVertex:
      {      
        if(m_version<3) {
          // Looking at Run 1 data, need to know which map to use
          if( subbank->size() == (m_recvertex_unordered_map2.at(1)).size() ) run1version = 1;
          else if( subbank->size() == (m_recvertex_unordered_map2.at(2)).size() ) run1version = 2;
          used_map = m_particle_unordered_map2;
          m_version=run1version;
        }
        else if( subbank->size()<(m_recvertex_unordered_map2_Turbo.at(m_version)).size() ) used_map = m_recvertex_unordered_map2;
        else used_map = m_recvertex_unordered_map2_Turbo;
      }
      break;
    case LHCb::CLID_Vertex:
      {      
        if( subbank->size()<(m_vertex_unordered_map2_Turbo.at(m_version)).size() ) used_map = m_vertex_unordered_map2;
        else used_map = m_vertex_unordered_map2_Turbo;
      }
      break;
    case LHCb::CLID_Particle:
      {      
        if(m_version<3) {
          // Looking at Run 1 data, need to know which map to use
          if( subbank->size() == (m_particle_unordered_map2.at(1)).size() ) run1version = 1;
          else if( subbank->size() == (m_particle_unordered_map2.at(2)).size() ) run1version = 2;
          used_map = m_particle_unordered_map2;
          m_version=run1version;
        }
        else if( subbank->size()<(m_particle_unordered_map2_Turbo.at(m_version)).size() ) used_map = m_particle_unordered_map2;
        else used_map = m_particle_unordered_map2_Turbo;
      }
      break;
    case LHCb::CLID_ProtoParticle:
      {      
        if( subbank->size()<(m_proto_unordered_map2_Turbo.at(m_version)).size() ) used_map = m_proto_unordered_map2;
        else used_map = m_proto_unordered_map2_Turbo;
      }
      break;
    case LHCb::CLID_RichPID:
      {      
        if( subbank->size()<(m_rpid_unordered_map2_Turbo.at(m_version)).size() ) used_map = m_rpid_unordered_map2;
        else used_map = m_rpid_unordered_map2_Turbo;
      }
      break;
    case LHCb::CLID_MuonPID:
      {      
        if( subbank->size()<(m_mpid_unordered_map2_Turbo.at(m_version)).size() ) used_map = m_mpid_unordered_map2;
        else used_map = m_mpid_unordered_map2_Turbo;
      }
      break;
    case LHCb::CLID_CaloCluster:
      {      
        if(m_version<3) {
          // Looking at Run 1 data, need to know which map to use
          if( subbank->size() == (m_calo_unordered_map2.at(1)).size() ) run1version = 1;
          else if( subbank->size() == (m_calo_unordered_map2.at(2)).size() ) run1version = 2;
          used_map = m_calo_unordered_map2;
          m_version=run1version;
        }
        else if( subbank->size()<(m_calo_unordered_map2_Turbo.at(m_version)).size() ) used_map = m_calo_unordered_map2;
        else used_map = m_calo_unordered_map2_Turbo;
      }
      break;
      // NOTE THE CASE OF 1 IS TAKEN CARE OF INSIDE THE DECODER
    default:
      { 

        Warning( std::string{ " StdInfo on unsupported class type "}+ std::to_string(classID),
                  StatusCode::SUCCESS, 20 );
        int e = 0;
        for (const auto& i : (*subbank)) { 
           info->insert( std::string { "z#Unknown.unknown" } + std::to_string( e++ ),
                                  floatFromInt(i) );        
        }
        return;
      }
    }

  // If version<3 => for Track, Particle, RecVertex, and CaloCluster, need to assign version based on size

  debug() << "Inserting from bank" << endmsg;
  for(it_unordered_map object_it = (used_map.at(m_version)).begin(); object_it!=(used_map.at(m_version)).end(); object_it++){
    info->insert( object_it->first, floatFromInt( (*subbank)[ object_it->second.first ]) );
  }
}
    
// Put the information in to the HltObjectSummary
void ReportConvertTool::ParticleObject2Summary( HltObjectSummary::Info* info, const LHCb::Particle* object, bool turbo) {
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if(turbo==true) used_map = m_particle_unordered_map2_Turbo;
  else used_map = m_particle_unordered_map2;
  
  for(it_unordered_map particle_it = (used_map.at(m_version)).begin(); particle_it!=(used_map.at(m_version)).end(); particle_it++){
    switch( particle_it->second.second )
    {
      case 0: info->insert( particle_it->first, float( object->particleID().pid() ) ); break;
      case 1: info->insert( particle_it->first, float( object->measuredMass() ) ); break;
      case 2: info->insert( particle_it->first, float( object->referencePoint().z() ) ); break;
      case 3: info->insert( particle_it->first, float( object->referencePoint().x() ) ); break;
      case 4: info->insert( particle_it->first, float( object->referencePoint().y() ) ); break;
      case 5: info->insert( particle_it->first, float( object->slopes().x() ) ); break;
      case 6: info->insert( particle_it->first, float( object->slopes().y() ) ); break;
      case 7: info->insert( particle_it->first, float( 1.0/(object->p()) ) ); break;
      case 8: info->insert( particle_it->first, float( object->confLevel() ) ); break;
      case 9: info->insert( particle_it->first, float( object->measuredMassErr() ) ); break;
      case 10: info->insert( particle_it->first, float( object->momCovMatrix()(0,0) ) ); break;
      case 11: info->insert( particle_it->first, float( object->momCovMatrix()(1,1) ) ); break;
      case 12: info->insert( particle_it->first, float( object->momCovMatrix()(2,2) ) ); break;
      case 13: info->insert( particle_it->first, float( object->momCovMatrix()(3,3) ) ); break;
      case 14: info->insert( particle_it->first, float( object->momCovMatrix()(1,0) ) ); break;
      case 15: info->insert( particle_it->first, float( object->momCovMatrix()(2,0) ) ); break;
      case 16: info->insert( particle_it->first, float( object->momCovMatrix()(2,1) ) ); break;
      case 17: info->insert( particle_it->first, float( object->momCovMatrix()(3,0) ) ); break;
      case 18: info->insert( particle_it->first, float( object->momCovMatrix()(3,1) ) ); break;
      case 19: info->insert( particle_it->first, float( object->momCovMatrix()(3,2) ) ); break;
      case 20: info->insert( particle_it->first, float( object->posMomCovMatrix()(0,0) ) ); break;
      case 21: info->insert( particle_it->first, float( object->posMomCovMatrix()(1,1) ) ); break;
      case 22: info->insert( particle_it->first, float( object->posMomCovMatrix()(2,2) ) ); break;
      case 23: info->insert( particle_it->first, float( object->posMomCovMatrix()(1,0) ) ); break;
      case 24: info->insert( particle_it->first, float( object->posMomCovMatrix()(0,1) ) ); break;
      case 25: info->insert( particle_it->first, float( object->posMomCovMatrix()(2,0) ) ); break;
      case 26: info->insert( particle_it->first, float( object->posMomCovMatrix()(0,2) ) ); break;
      case 27: info->insert( particle_it->first, float( object->posMomCovMatrix()(2,1) ) ); break;
      case 28: info->insert( particle_it->first, float( object->posMomCovMatrix()(1,2) ) ); break;
      case 29: info->insert( particle_it->first, float( object->posMomCovMatrix()(3,0) ) ); break;
      case 30: info->insert( particle_it->first, float( object->posMomCovMatrix()(3,1) ) ); break;
      case 31: info->insert( particle_it->first, float( object->posMomCovMatrix()(3,2) ) ); break;
      case 32: info->insert( particle_it->first, float( object->posCovMatrix()(0,0) ) ); break;
      case 33: info->insert( particle_it->first, float( object->posCovMatrix()(1,1) ) ); break;
      case 34: info->insert( particle_it->first, float( object->posCovMatrix()(2,2) ) ); break;
      case 35: info->insert( particle_it->first, float( object->posCovMatrix()(1,0) ) ); break;
      case 36: info->insert( particle_it->first, float( object->posCovMatrix()(2,0) ) ); break;
      case 37: info->insert( particle_it->first, float( object->posCovMatrix()(2,1) ) ); break;
    }
  }

}

void ReportConvertTool::ProtoParticleObject2Summary( HltObjectSummary::Info* info, const LHCb::ProtoParticle* object, bool turbo) {
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  if(turbo==true) used_map = m_proto_unordered_map2_Turbo;
  else used_map = m_proto_unordered_map2;
  
  for(it_unordered_map proto_it = (used_map.at(m_version)).begin(); proto_it!=(used_map.at(m_version)).end(); proto_it++){
    switch( proto_it->second.second )
    {
      case 0: info->insert( proto_it->first, float( object->info( 381, -1000 ) ) ); break;
    }
  }

}

void ReportConvertTool::TrackObject2Summary( HltObjectSummary::Info* info, const LHCb::Track* object, bool turbo) {
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }

  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if(turbo==true) used_map = m_track_unordered_map2_Turbo;
  else used_map = m_track_unordered_map2;
  
  for(it_unordered_map track_it = (used_map.at(m_version)).begin(); track_it!=(used_map.at(m_version)).end(); track_it++){
    switch( track_it->second.second )
    {
      case 0: info->insert( track_it->first, float( object->states().front()->z() ) ); break;
      case 1: info->insert( track_it->first, float( object->states().front()->x() ) ); break;
      case 2: info->insert( track_it->first, float( object->states().front()->y() ) ); break;
      case 3: info->insert( track_it->first, float( object->states().front()->tx() ) ); break;
      case 4: info->insert( track_it->first, float( object->states().front()->ty() ) ); break;
      case 5: info->insert( track_it->first, float( object->states().front()->qOverP() ) ); break;
      case 6: info->insert( track_it->first, float( object->chi2PerDoF() ) ); break;
      case 7: info->insert( track_it->first, float( object->nDoF() ) ); break;
      case 8: info->insert( track_it->first, float( object->likelihood() ) ); break;
      case 9: info->insert( track_it->first, float( object->ghostProbability() ) ); break;
      case 10: info->insert( track_it->first, float( object->flags() ) ); break;
      case 11: info->insert( track_it->first, float( object->states().back()->z() ) ); break;
      case 12: info->insert( track_it->first, float( object->states().back()->x() ) ); break;
      case 13: info->insert( track_it->first, float( object->states().back()->y() ) ); break;
      case 14: info->insert( track_it->first, float( object->states().back()->tx() ) ); break;
      case 15: info->insert( track_it->first, float( object->states().back()->ty() ) ); break;
      case 16: info->insert( track_it->first, float( object->states().back()->qOverP() ) ); break;
    }
  }
}

void ReportConvertTool::RichPIDObject2Summary( HltObjectSummary::Info* info, const LHCb::RichPID* object, bool turbo) {
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  if(turbo==true) used_map = m_rpid_unordered_map2_Turbo;
  else used_map = m_rpid_unordered_map2;
  
  for(it_unordered_map rpid_it = (used_map.at(m_version)).begin(); rpid_it!=(used_map.at(m_version)).end(); rpid_it++){
    switch( rpid_it->second.second )
    {
      case 0: info->insert( rpid_it->first, float( object->pidResultCode() ) ); break;
      case 1: info->insert( rpid_it->first, float( object->particleDeltaLL( Rich::ParticleIDType::Electron ) ) ); break;
      case 2: info->insert( rpid_it->first, float( object->particleDeltaLL( Rich::ParticleIDType::Muon ) ) ); break;
      case 3: info->insert( rpid_it->first, float( object->particleDeltaLL( Rich::ParticleIDType::Pion ) ) ); break;
      case 4: info->insert( rpid_it->first, float( object->particleDeltaLL( Rich::ParticleIDType::Kaon ) ) ); break;
      case 5: info->insert( rpid_it->first, float( object->particleDeltaLL( Rich::ParticleIDType::Proton ) ) ); break;
    }
  }

}

void ReportConvertTool::MuonPIDObject2Summary( HltObjectSummary::Info* info , const LHCb::MuonPID* object, bool turbo) {
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  if(turbo==true) used_map = m_mpid_unordered_map2_Turbo;
  else used_map = m_mpid_unordered_map2;
  
  for(it_unordered_map mpid_it = (used_map.at(m_version)).begin(); mpid_it!=(used_map.at(m_version)).end(); mpid_it++){
    switch( mpid_it->second.second )
    {
      case 0: info->insert( mpid_it->first, float( object->MuonLLMu() ) ); break;
      case 1: info->insert( mpid_it->first, float( object->MuonLLBg() ) ); break;
      case 2: info->insert( mpid_it->first, float( object->nShared() ) ); break;
      case 3: info->insert( mpid_it->first, float( object->Status() ) ); break;
      case 4: info->insert( mpid_it->first, float( object->IsMuon() ) ); break;
      case 5: info->insert( mpid_it->first, float( object->IsMuonLoose() ) ); break;
      case 6: info->insert( mpid_it->first, float( object->IsMuonTight() ) ); break;
    }
  }

}

void ReportConvertTool::CaloClusterObject2Summary( HltObjectSummary::Info* info, const LHCb::CaloCluster* object, bool turbo) {
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  if(turbo==true) used_map = m_calo_unordered_map2_Turbo;
  else used_map = m_calo_unordered_map2;
  
  for(it_unordered_map calo_it = (used_map.at(m_version)).begin(); calo_it!=(used_map.at(m_version)).end(); calo_it++){
    switch( calo_it->second.second )
    {
      case 0: info->insert( calo_it->first, float( object->e() ) ); break;
      case 1: info->insert( calo_it->first, float( object->position().x() ) ); break;
      case 2: info->insert( calo_it->first, float( object->position().y() ) ); break;
      case 3: info->insert( calo_it->first, float( object->position().z() ) ); break;
    }
  }
}

void ReportConvertTool::RecVertexObject2Summary( HltObjectSummary::Info* info, const LHCb::RecVertex* object, bool turbo) {
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  if(turbo==true) used_map = m_recvertex_unordered_map2_Turbo;
  else used_map = m_recvertex_unordered_map2;
  
  for(it_unordered_map recvertex_it = (used_map.at(m_version)).begin(); recvertex_it!=(used_map.at(m_version)).end(); recvertex_it++){
    switch( recvertex_it->second.second )
    {
      case 0: info->insert( recvertex_it->first, float( object->position().x() ) ); break;
      case 1: info->insert( recvertex_it->first, float( object->position().y() ) ); break;
      case 2: info->insert( recvertex_it->first, float( object->position().z() ) ); break;
      case 3: info->insert( recvertex_it->first, float( object->chi2() ) ); break;
    }
  }

}

void ReportConvertTool::VertexObject2Summary( HltObjectSummary::Info* info, const LHCb::Vertex* object, bool turbo) {
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  if(turbo==true) used_map = m_vertex_unordered_map2_Turbo;
  else used_map = m_vertex_unordered_map2;
  
  for(it_unordered_map vertex_it = (used_map.at(m_version)).begin(); vertex_it!=(used_map.at(m_version)).end(); vertex_it++){
    switch( vertex_it->second.second )
    {
      case 0: info->insert( vertex_it->first, float( object->chi2() ) ); break;
      case 1: info->insert( vertex_it->first, float( object->nDoF() ) ); break;
      case 2: info->insert( vertex_it->first, float( object->position().x() ) ); break;
      case 3: info->insert( vertex_it->first, float( object->position().y() ) ); break;
      case 4: info->insert( vertex_it->first, float( object->position().z() ) ); break;
      case 5: info->insert( vertex_it->first, float( object->technique() ) ); break;
      case 6: info->insert( vertex_it->first, float( object->covMatrix()(0,0) ) ); break;
      case 7: info->insert( vertex_it->first, float( object->covMatrix()(1,1) ) ); break;
      case 8: info->insert( vertex_it->first, float( object->covMatrix()(2,2) ) ); break;
      case 9: info->insert( vertex_it->first, float( object->covMatrix()(1,0) ) ); break;
      case 10: info->insert( vertex_it->first, float( object->covMatrix()(2,0) ) ); break;
      case 11: info->insert( vertex_it->first, float( object->covMatrix()(2,1) ) ); break;
    }
  }

}


// Put the information in the summary back in the object
void ReportConvertTool::ParticleObjectFromSummary( const HltObjectSummary::Info* info, LHCb::Particle* object, bool turbo) {
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }

  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if(turbo==true) used_map = m_particle_unordered_map2_Turbo;
  else used_map = m_particle_unordered_map2;

  if( m_version < 3 ){
    int run1version=-999;
    // find size we care about (i.e. make sure extra info not counted)
    unsigned int Isize = 0;
    for(HltObjectSummary::Info::iterator it_info = info->begin(); it_info!=info->end(); it_info++){
      if( (it_info->first).find("#")!=std::string::npos ) Isize++;
    }
    // Looking at Run 1 data, need to know which map to use
    if( Isize == (m_particle_unordered_map2.at(1)).size() ) run1version = 1;
    else if( Isize == (m_particle_unordered_map2.at(2)).size() ) run1version = 2;
    else Error( "Particle requested in Run 1 reports, but reports are unknown" , StatusCode::FAILURE, 100 );
    m_version=run1version;
  }

  // Momentum components
  float p=0;
  float slopex=0;
  float slopey=0;
  // Reference point
  Gaudi::XYZPoint xyz;
  // Matrices
  Gaudi::SymMatrix4x4 & momCov = *(const_cast<Gaudi::SymMatrix4x4*>(&object->momCovMatrix()));
  Gaudi::Matrix4x3 & posMomCov = *(const_cast<Gaudi::Matrix4x3*>(&object->posMomCovMatrix()));
  Gaudi::SymMatrix3x3 & posCov = *(const_cast<Gaudi::SymMatrix3x3*>(&object->posCovMatrix()));

  for(it_unordered_map particle_it = (used_map.at(m_version)).begin(); particle_it!=(used_map.at(m_version)).end(); particle_it++){
    switch( std::get<1>( particle_it->second ) )
    {
      case 0: object->setParticleID( LHCb::ParticleID( (*info)[ particle_it->first ] ) ); break;
      case 1: object->setMeasuredMass( (*info)[ particle_it->first ] ); break;
      case 2: xyz.SetZ( (*info)[ particle_it->first ] ); break;
      case 3: xyz.SetX( (*info)[ particle_it->first ] );  break;
      case 4: xyz.SetY( (*info)[ particle_it->first ] ); break;
      case 5: slopex = (*info)[ particle_it->first ] ; break;
      case 6: slopey = (*info)[ particle_it->first ] ; break;
      case 7: p=1.0/( (*info)[ particle_it->first ] ); break;
      case 8: object->setConfLevel( (*info)[ particle_it->first ] ); break;
      case 9: object->setMeasuredMassErr( (*info)[ particle_it->first ] ); break;
      case 10: momCov(0,0) = (*info)[ particle_it->first ] ; break;
      case 11: momCov(1,1) = (*info)[ particle_it->first ] ; break;
      case 12: momCov(2,2) = (*info)[ particle_it->first ] ; break;
      case 13: momCov(3,3) = (*info)[ particle_it->first ] ; break;
      case 14: momCov(1,0) = (*info)[ particle_it->first ] ; break;
      case 15: momCov(2,0) = (*info)[ particle_it->first ] ; break;
      case 16: momCov(2,1) = (*info)[ particle_it->first ] ; break;
      case 17: momCov(3,0) = (*info)[ particle_it->first ] ; break;
      case 18: momCov(3,1) = (*info)[ particle_it->first ] ; break;
      case 19: momCov(3,2) = (*info)[ particle_it->first ] ; break;
      case 20: posMomCov(0,0) = (*info)[ particle_it->first ] ; break;
      case 21: posMomCov(1,1) = (*info)[ particle_it->first ] ; break;
      case 22: posMomCov(2,2) = (*info)[ particle_it->first ] ; break;
      case 23: posMomCov(1,0) = (*info)[ particle_it->first ] ; break;
      case 24: posMomCov(0,1) = (*info)[ particle_it->first ] ; break;
      case 25: posMomCov(2,0) = (*info)[ particle_it->first ] ; break;
      case 26: posMomCov(0,2) = (*info)[ particle_it->first ] ; break;
      case 27: posMomCov(2,1) = (*info)[ particle_it->first ] ; break;
      case 28: posMomCov(1,2) = (*info)[ particle_it->first ] ; break;
      case 29: posMomCov(3,0) = (*info)[ particle_it->first ] ; break;
      case 30: posMomCov(3,1) = (*info)[ particle_it->first ] ; break;
      case 31: posMomCov(3,2) = (*info)[ particle_it->first ] ; break;
      case 32: posCov(0,0) = (*info)[ particle_it->first ] ; break;
      case 33: posCov(1,1) = (*info)[ particle_it->first ] ; break;
      case 34: posCov(2,2) = (*info)[ particle_it->first ] ; break;
      case 35: posCov(1,0) = (*info)[ particle_it->first ] ; break;
      case 36: posCov(2,0) = (*info)[ particle_it->first ] ; break;
      case 37: posCov(2,1) = (*info)[ particle_it->first ] ; break;
    }
  }
  object->setReferencePoint(xyz);
  const double slopez = 1/sqrt(slopex*slopex + slopey*slopey + 1.0);
  const double pz = slopez*p;
  const double px = slopex*pz;
  const double py = slopey*pz;
  const double mm = object->measuredMass();
  const double pe = sqrt(p*p+mm*mm);
  Gaudi::LorentzVector part_mom(px,py,pz,pe);
  object->setMomentum(part_mom);
}

void ReportConvertTool::ProtoParticleObjectFromSummary( const HltObjectSummary::Info* info, LHCb::ProtoParticle* object, bool turbo) {
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if(turbo==true) used_map = m_proto_unordered_map2_Turbo;
  else used_map = m_proto_unordered_map2;
  
  for(it_unordered_map proto_it = (used_map.at(m_version)).begin(); proto_it!=(used_map.at(m_version)).end(); proto_it++){
    switch( proto_it->second.second )
    {
      case 0: object->addInfo( 381, int( (*info)[ proto_it->first ] ) ); break;
    }
  }

}

void ReportConvertTool::TrackObjectFromSummary( const HltObjectSummary::Info* info, LHCb::Track* object, bool turbo) {
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if(turbo==true) used_map = m_track_unordered_map2_Turbo;
  else used_map = m_track_unordered_map2;
  
  if( m_version < 3 ){
    int run1version=-999;
    // find size we care about (i.e. make sure extra info not counted)
    unsigned int Isize = 0;
    for(HltObjectSummary::Info::iterator it_info = info->begin(); it_info!=info->end(); it_info++){
      if( (it_info->first).find("#")!=std::string::npos ) Isize++;
    }
    // Looking at Run 1 data, need to know which map to use
    if( Isize == (m_track_unordered_map2.at(1)).size() ) run1version = 1;
    else if( Isize == (m_track_unordered_map2.at(2)).size() ) run1version = 2;
    else Error( "Track requested in Run 1 reports, but reports are unknown" , StatusCode::FAILURE, 100 );
    m_version=run1version;
  }
  
  LHCb::State* first = new LHCb::State();
  LHCb::State* last = new LHCb::State();
  
  for(it_unordered_map track_it = (used_map.at(m_version)).begin(); track_it!=(used_map.at(m_version)).end(); track_it++){
    switch( track_it->second.second )
    {
      case 0: first->setZ( (*info)[ track_it->first ] ); break;
      case 1: first->setX( (*info)[ track_it->first ] ); break;
      case 2: first->setY( (*info)[ track_it->first ] ); break;
      case 3: first->setTx( (*info)[ track_it->first ] ); break;
      case 4: first->setTy( (*info)[ track_it->first ] ); break;
      case 5: first->setQOverP( (*info)[ track_it->first ] ); break;
      case 6: object->setChi2PerDoF( (*info)[ track_it->first ] ); break;
      case 7: object->setNDoF( int( (*info)[ track_it->first ] ) ); break;
      case 8: object->setLikelihood( (*info)[ track_it->first ] ); break;
      case 9: object->setGhostProbability( (*info)[ track_it->first ] ); break;
      case 10: object->setFlags( (unsigned int)(*info)[ track_it->first ] ); break;
      case 11: last->setZ( (*info)[ track_it->first ] ); break;
      case 12: last->setX( (*info)[ track_it->first ] ); break;
      case 13: last->setY( (*info)[ track_it->first ] ); break;
      case 14: last->setTx( (*info)[ track_it->first ] ); break;
      case 15: last->setTy( (*info)[ track_it->first ] ); break;
      case 16: last->setQOverP( (*info)[ track_it->first ] ); break;
    }
  }

  object->addToStates(*first);
  // stop memory leak
  if(turbo==false) delete last;
  if(turbo==true) object->addToStates(*last);
}

void ReportConvertTool::RichPIDObjectFromSummary( const HltObjectSummary::Info*info, LHCb::RichPID* object, bool turbo) {
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if(turbo==true) used_map = m_rpid_unordered_map2_Turbo;
  else used_map = m_rpid_unordered_map2;
  
  for(it_unordered_map rpid_it = (used_map.at(m_version)).begin(); rpid_it!=(used_map.at(m_version)).end(); rpid_it++){
    switch( rpid_it->second.second )
    {
      case 0: object->setPidResultCode( (unsigned int)(*info)[ rpid_it->first ] ); break;
      case 1: object->setParticleDeltaLL( Rich::ParticleIDType::Electron, (*info)[ rpid_it->first ] ); break;
      case 2: object->setParticleDeltaLL( Rich::ParticleIDType::Muon, (*info)[ rpid_it->first ] ); break;
      case 3: object->setParticleDeltaLL( Rich::ParticleIDType::Pion, (*info)[ rpid_it->first ] ); break;
      case 4: object->setParticleDeltaLL( Rich::ParticleIDType::Kaon, (*info)[ rpid_it->first ] ); break;
      case 5: object->setParticleDeltaLL( Rich::ParticleIDType::Proton, (*info)[ rpid_it->first ] ); break;
    }
  }

}

void ReportConvertTool::MuonPIDObjectFromSummary( const HltObjectSummary::Info* info, LHCb::MuonPID* object, bool turbo) {
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if(turbo==true) used_map = m_mpid_unordered_map2_Turbo;
  else used_map = m_mpid_unordered_map2;
  
  for(it_unordered_map mpid_it = (used_map.at(m_version)).begin(); mpid_it!=(used_map.at(m_version)).end(); mpid_it++){
    switch( mpid_it->second.second )
    {
      case 0: object->setMuonLLMu( (*info)[ mpid_it->first ] ); break;
      case 1: object->setMuonLLBg( (*info)[ mpid_it->first ] ); break;
      case 2: object->setNShared( int( (*info)[ mpid_it->first ] ) ); break;
      case 3: object->setStatus( int( (*info)[ mpid_it->first ] ) ); break;
      case 4: object->setIsMuon( int( (*info)[ mpid_it->first ] ) ); break;
      case 5: object->setIsMuonLoose( int( (*info)[ mpid_it->first ] ) ); break;
      case 6: object->setIsMuonTight( int( (*info)[ mpid_it->first ] ) ); break;
    }
  }

}

void ReportConvertTool::CaloClusterObjectFromSummary( const HltObjectSummary::Info* info, LHCb::CaloCluster* object, bool turbo) {
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if(turbo==true) used_map = m_calo_unordered_map2_Turbo;
  else used_map = m_calo_unordered_map2;
 
  Gaudi::Vector3 & exy = *(const_cast<Gaudi::Vector3*>(&object->position().parameters()));
  Gaudi::Vector3 & xy = *(const_cast<Gaudi::Vector3*>(&object->position().parameters()));
  //
  double e=0;
  double x=0;
  double y=0;

  for(it_unordered_map calo_it = (used_map.at(m_version)).begin(); calo_it!=(used_map.at(m_version)).end(); calo_it++){
    switch( calo_it->second.second )
    {
      case 0: e = (*info)[ calo_it->first ]; break;
      case 1: x = (*info)[ calo_it->first ]; break;
      case 2: y = (*info)[ calo_it->first ]; break;
      case 3: object->position().setZ( (*info)[ calo_it->first ] ); break;
    }
  }
  exy(0) = e;
  exy(1) = x; xy(0) = x;
  exy(2) = y; xy(1) = y;

}

void ReportConvertTool::RecVertexObjectFromSummary( const HltObjectSummary::Info* info, LHCb::RecVertex* object, bool turbo) {
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if(turbo==true) used_map = m_recvertex_unordered_map2_Turbo;
  else used_map = m_recvertex_unordered_map2;
 
  if( m_version < 3 ){
    int run1version=-999;
    // find size we care about (i.e. make sure extra info not counted)
    unsigned int Isize = 0;
    for(HltObjectSummary::Info::iterator it_info = info->begin(); it_info!=info->end(); it_info++){
      if( (it_info->first).find("#")!=std::string::npos ) Isize++;
    }
    // Looking at Run 1 data, need to know which map to use
    if( Isize == (m_recvertex_unordered_map2.at(1)).size() ) run1version = 1;
    else if( Isize == (m_recvertex_unordered_map2.at(2)).size() ) run1version = 2;
    else Error( "Track requested in Run 1 reports, but reports are unknown" , StatusCode::FAILURE, 100 );
    m_version=run1version;
  }

  Gaudi::XYZPoint xyz;
  for(it_unordered_map recvertex_it = (used_map.at(m_version)).begin(); recvertex_it!=(used_map.at(m_version)).end(); recvertex_it++){
    switch( recvertex_it->second.second )
    {
      case 0: xyz.SetX( (*info)[ recvertex_it->first ] ); break;
      case 1: xyz.SetY( (*info)[ recvertex_it->first ] ); break;
      case 2: xyz.SetZ( (*info)[ recvertex_it->first ] ); break;
      case 3: object->setChi2( (*info)[ recvertex_it->first ] ); break;
    }
  }
  object->setPosition( xyz );

}

void ReportConvertTool::VertexObjectFromSummary( const HltObjectSummary::Info* info, LHCb::Vertex* object, bool turbo) {
  if( m_version == -999 ){ 
    Warning( "I have not been told a verision number to use, assuming the latest", StatusCode::SUCCESS, 20 );
    m_version = m_LatestVersion;
  }
  
  unordered_map<int,unordered_map<string, pair<int,int> > > used_map;
  if(turbo==true) used_map = m_vertex_unordered_map2_Turbo;
  else used_map = m_vertex_unordered_map2;
  
  Gaudi::XYZPoint xyz;
  Gaudi::SymMatrix3x3 & cov = *(const_cast<Gaudi::SymMatrix3x3*>(&object->covMatrix()));

  for(it_unordered_map vertex_it = (used_map.at(m_version)).begin(); vertex_it!=(used_map.at(m_version)).end(); vertex_it++){
    switch( vertex_it->second.second )
    {
      case 0: object->setChi2( (*info)[ vertex_it->first ] ); break;
      case 1: object->setNDoF( int( (*info)[ vertex_it->first ] ) ); break;
      case 2: xyz.SetX( (*info)[ vertex_it->first ] ); break;
      case 3: xyz.SetY( (*info)[ vertex_it->first ] ); break;
      case 4: xyz.SetZ( (*info)[ vertex_it->first ] ); break;
      case 5: object->setTechnique( static_cast<LHCb::Vertex::CreationMethod>( (*info)[ vertex_it->first ] ) ); break;
      case 6: cov(0,0) = (*info)[ vertex_it->first ] ; break;
      case 7: cov(1,1) = (*info)[ vertex_it->first ] ; break;
      case 8: cov(2,2) = (*info)[ vertex_it->first ] ; break;
      case 9: cov(1,0) = (*info)[ vertex_it->first ] ; break;
      case 10: cov(2,0) = (*info)[ vertex_it->first ] ; break;
      case 11: cov(2,1) = (*info)[ vertex_it->first ] ; break;
    }
  }
  object->setPosition( xyz );
}

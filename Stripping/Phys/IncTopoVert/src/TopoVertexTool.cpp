// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TopoVertexTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TopoVertexTool
//
// 2012-09-21 : Julien Cogan and Mathieu Perrin-Terrin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TopoVertexTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TopoVertexTool::TopoVertexTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  m_writtenOnTES = false;

  declareInterface<ITopoVertexTool>(this);

  declareProperty("VertexFunctionToolType",m_vertexFunctionToolType="VertexFunctionTool");
  declareProperty("VertexFunctionToolName",m_vertexFunctionToolName="VertexFunctionTool");

  declareProperty("DistanceCalculatorToolType",m_distanceCalculatorToolType="LoKi::DistanceCalculator");
  declareProperty("DistanceCalculatorToolName",m_distanceCalculatorToolName="DistanceCalculatorTool");

  declareProperty("TrackVertexerToolType",m_trackVertexerToolType="TrackVertexer");
  declareProperty("TrackVertexerToolName",m_trackVertexerToolName="TrackVertexerTool");

  declareProperty("TwoTracksVtxChi2Max"   ,m_twoTracksVtxChi2Max   = 20.  );
  declareProperty("TwoTracksVtxVfMin"     ,m_twoTracksVtxVfMin     =  0.1 );
  declareProperty("TwoTracksVtxVfRatioMin",m_twoTracksVtxVfRatioMin=  0.1 );
  declareProperty("TrackVtxChi2Max"       ,m_TrackVtxChi2Max       = 15.  );

  /// To be passed to the vertex function tool (temporary)
  declareProperty("MaxFinderStep",m_step=0.001);
 
  declareProperty("MaxFinderMinGradientMag",m_max_finder_min_gradient_mag=0.01);
  declareProperty("MaxFinderMaxIteration",m_max_finder_max_iteration=10000);
  declareProperty("MaxFinderMinStep",m_max_finder_min_step=0.000001);
  declareProperty("MaxFinderMaxjump",m_max_finder_max_jump=5);

  declareProperty("ResolverCut",m_resolver_cut=0.8); 
  declareProperty("ResolverMinStep",m_resolver_min_step=0.001);
  declareProperty("ResolverMaxIteration",m_resolver_max_iteration=8);
  
  declareProperty("DxForGradient",m_dx_for_gradient=0.0001);
  declareProperty("MC",m_MC = true);
  
}
//=============================================================================
// Destructor
//=============================================================================
TopoVertexTool::~TopoVertexTool() {} 

//=============================================================================
StatusCode TopoVertexTool::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;
  
  m_vertexFunction = tool<IVertexFunctionTool>(m_vertexFunctionToolType,m_vertexFunctionToolName);
  setVertexFunctionParam();
   
  m_distanceCalculator = tool<IDistanceCalculator>(m_distanceCalculatorToolType,m_distanceCalculatorToolName);
  
  m_trackVertexer = tool<ITrackVertexer>(m_trackVertexerToolType,m_trackVertexerToolName);
  
  return sc;
}


StatusCode TopoVertexTool::findVertices(std::vector<const LHCb::Track*> & tracks,
                                        std::vector<const LHCb::Track*> & vTr, int* n_sig_track_step) 
{
  if (n_sig_track_step){
  }
  
  setVertexFunctionTracks(vTr);
  return findVertices(tracks);
}
StatusCode TopoVertexTool::findVertices(std::vector<const LHCb::Particle*> & particles, int* n_sig_track_step) 
{
  if (n_sig_track_step){
  }
  std::vector<const LHCb::Track*> tracks(particles.size());
  for (std::vector<const LHCb::Particle*>::iterator ipart=particles.begin(); ipart<particles.end();++ipart){
    const LHCb::ProtoParticle * proto = (*ipart)->proto();
    if (proto==NULL) continue;
    const LHCb::Track* track = proto->track();
    tracks.push_back(track);
  }
  return findVertices(tracks);
}
StatusCode TopoVertexTool::findVertices(std::vector<const LHCb::Particle*> & particles,
                                        std::vector<const LHCb::Track*> & vTr, int* n_sig_track_step) 
{
  if (n_sig_track_step){
  }
    setVertexFunctionTracks(vTr);
    return findVertices(particles);
}

StatusCode TopoVertexTool::findVertices(std::vector<const LHCb::Track*> & tracks, int* n_sig_track_step) 
{
  //Check that the vertex function contains some tracks
  std::vector<const LHCb::Track*>  vf_tracks ;
  getVertexFunctionTracks(vf_tracks);
  if (vf_tracks.size()==0) {
    Warning("The Vertex Function has no tracks!",StatusCode::SUCCESS,20).ignore();
  }

  cleanUpResult();
  m_writtenOnTES = false;
  
  // Initialize event
  if (tracks.size()==0){
    return Warning("No input track",StatusCode::SUCCESS,20);
  }

  m_tracks = tracks;

  setVertexFunctionParam(); // Just in case some parameters where changed (?, Giampi ?) 

  // Start finding algorithm, splitted in 6 steps to ease debugging and method overriding 
  if (!step1().isSuccess()) {
    cleanUpFull(); 
    return Warning("Step1 failed",StatusCode::SUCCESS,20);
  }
  if (m_MC == true && n_sig_track_step != NULL)     n_sig_track_step[0] = getNbSignalTrack(m_2tracks_vertices);

  if (!step2().isSuccess()) {
    cleanUpFull(); 
    return Warning("Step2 failed",StatusCode::SUCCESS,20);
  }
  if (m_MC == true && n_sig_track_step != NULL )     n_sig_track_step[1] = getNbSignalTrack(m_2tracks_vertices);

  if (!step3().isSuccess()) {
    cleanUpFull(); 
    return Warning("Step3 failed",StatusCode::SUCCESS,20);
  }
  if (m_MC == true && n_sig_track_step != NULL )     n_sig_track_step[2] = getNbSignalTrack(m_2tracks_vertices);

  if (!step4().isSuccess()) {
    cleanUpFull(); 
    return Warning("Step4 failed",StatusCode::SUCCESS,20);
  }
  if (m_MC == true && n_sig_track_step != NULL )     n_sig_track_step[3] = getNbSignalTrack(m_list_of_clusters);

  if (!step5().isSuccess()) {
    cleanUpFull(); 
    return Warning("Step5 failed",StatusCode::SUCCESS,20);
  }
  if (m_MC == true && n_sig_track_step != NULL )     n_sig_track_step[4] = getNbSignalTrack(m_vertices);

  if (!step6().isSuccess()) {
    cleanUpFull(); 
    return Warning("Step6 failed",StatusCode::SUCCESS,20);
  }
  if (m_MC == true && n_sig_track_step != NULL )     n_sig_track_step[5] = getNbSignalTrack(m_vertices);

  cleanUpTemporary();

  return StatusCode::SUCCESS;
}
 

StatusCode TopoVertexTool::step1()
{
  /*
    Make 2 tracks vertices from all tracks combinations.
    Keep only the ones surviving the chi2 and Vmin cuts.
  */

  for (std::vector<const LHCb::Track*>::iterator it_ptrk=m_tracks.begin(); it_ptrk<m_tracks.end(); ++it_ptrk){
    m_2tracks_vertices_per_track[*it_ptrk]=RecVertexVector();
  }

  std::vector<const LHCb::Track*>::iterator end0 = m_tracks.end(); 
  --end0; 
  for (std::vector<const LHCb::Track*>::iterator it_ptrk0=m_tracks.begin(); it_ptrk0< end0 ; ++it_ptrk0){ 
    std::vector<const LHCb::Track *> two_tracks_container;
    two_tracks_container.push_back(*it_ptrk0);
    
    std::vector<const LHCb::Track*>::iterator begin1 = it_ptrk0; 
    ++begin1; 
    for (std::vector<const LHCb::Track*>::iterator it_ptrk1=begin1; it_ptrk1<m_tracks.end(); ++it_ptrk1){ 
      two_tracks_container.push_back(*it_ptrk1);
      LHCb::RecVertex * vertex = m_trackVertexer->fit(two_tracks_container);
      if (vertex) {
        
        two_tracks_container.pop_back();
        
        if (vertex->chi2() > (2*m_twoTracksVtxChi2Max)){
          delete vertex;
          continue;
        }
      
        if (m_vertexFunction->valueAt(*vertex) < m_twoTracksVtxVfMin){
          delete vertex;
          continue;
        }
      }
      else { 
        Warning("fit didn't converge",StatusCode::SUCCESS,0).ignore();  
        delete vertex;
        
      }
      
      m_2tracks_vertices.push_back(vertex);
      m_2tracks_vertices_per_track[*it_ptrk0].push_back(vertex);
      m_2tracks_vertices_per_track[*it_ptrk1].push_back(vertex);
    }
  }
  return StatusCode::SUCCESS;
}
StatusCode TopoVertexTool::step2()
{
  /*
    Clean up track list for every vertex, scanning the tracks and the vertices they belong to.
  */

  // Loop over tracks (using the vertices by track map)
  for (TrackVerticesMap::iterator it_trk_vertices = m_2tracks_vertices_per_track.begin(); 
       it_trk_vertices != m_2tracks_vertices_per_track.end(); ++it_trk_vertices){
    
    const LHCb::Track * track  = it_trk_vertices->first;
    RecVertexVector  vertices = it_trk_vertices->second;
    
    if (vertices.size()<2) continue ;
  
    // Get highest vertex function of all vertices containing the current track  
    m_vertexFunction->sortByValueAt(vertices);
    double highestVf = m_vertexFunction->valueAt(*(vertices.back()));
    
    // Remove this track from all vertices with vertex function lower to a fraction of the highest
    RecVertexVector::iterator it_vertex = vertices.begin(); 
    for (; it_vertex<vertices.end(); ++it_vertex){
      if (m_vertexFunction->valueAt(**it_vertex) < (highestVf*m_twoTracksVtxVfRatioMin) ) {
        (*it_vertex)->removeFromTracks(track);
      } else {
        break;
      }
    }
    
    // Remove from track list the vertices the track have been removed from above
    vertices.erase(vertices.begin(),it_vertex);
    
    // Remove track from vertices which are unresolved from better vertices
    RecVertexVector check_list;
    for (RecVertexVector::reverse_iterator it_vertex0 = vertices.rbegin(); it_vertex0 < vertices.rend(); ++it_vertex0) {
      bool new_check = true;
      for (RecVertexVector::reverse_iterator it_vertex1=check_list.rbegin(); it_vertex1<check_list.rend(); ++it_vertex1){
        if (! m_vertexFunction->areResolved( **it_vertex0, **it_vertex1) ) {
          (*it_vertex0)->removeFromTracks(track);
          new_check = false;
          break;
        }
      }
      if (new_check) check_list.push_back(*it_vertex0);
    }
  }
  return StatusCode::SUCCESS;
}
StatusCode TopoVertexTool::step3()
{
  /*
    Remove vertex with no remaining track.
    Sort list of vertices by maximum vertex function value.
  */

  RecVertexVector new_vertices;
  for (RecVertexVector::iterator it_vertex=m_2tracks_vertices.begin(); it_vertex<m_2tracks_vertices.end(); ++it_vertex){
    LHCb::RecVertex *vertex = *it_vertex;
    if ( (vertex->tracks()).size() ==0) {
      delete(vertex);
    } else {
      new_vertices.push_back(vertex);
    }
  }
  m_2tracks_vertices.clear();
  m_2tracks_vertices = new_vertices;

  m_vertexFunction->sortByValueMax(m_2tracks_vertices);
  std::reverse(m_2tracks_vertices.begin(),m_2tracks_vertices.end()); 
  return StatusCode::SUCCESS;
}
StatusCode TopoVertexTool::step4()
{
  /*
    Merge vertices into clusters.
  */
  RecVertexVector remaining_vertices = RecVertexVector(m_2tracks_vertices.begin(),m_2tracks_vertices.end());
  while (remaining_vertices.size()>0){
    RecVertexVector::iterator next_remaining_vertex = remaining_vertices.begin();
    RecVertexVector * pcluster = new RecVertexVector();
    pcluster->push_back(*next_remaining_vertex);
    m_list_of_clusters.push_back(pcluster);
    remaining_vertices.erase(next_remaining_vertex);
    addUnResolvedToCluster(*pcluster,remaining_vertices);
  }
  return StatusCode::SUCCESS;
}
StatusCode TopoVertexTool::step5()
{
  if (m_vertices.size()!=0)  m_vertices.clear();
  /*
    Fit clusters
  */
  int icl=0;
  for (VerticesList::iterator it_cluster=m_list_of_clusters.begin(); it_cluster<m_list_of_clusters.end(); ++it_cluster){
    ++icl;
    TrackVector all_tracks;
    for (RecVertexVector::iterator it_vertex=(*it_cluster)->begin(); it_vertex<(*it_cluster)->end(); ++it_vertex){
      LHCb::RecVertex * vertex  = *it_vertex;
      const SmartRefVector< LHCb::Track > vertex_tracks = vertex->tracks();
      all_tracks.insert(all_tracks.end(),vertex_tracks.begin(),vertex_tracks.end());
    }
    std::stable_sort(all_tracks.begin(),all_tracks.end());
    TrackVector::iterator tracks_end = std::unique(all_tracks.begin(),all_tracks.end());
    TrackVector tracks;
    tracks.insert(tracks.end(), all_tracks.begin(),tracks_end);

    LHCb::RecVertex * vertex = m_trackVertexer->fit(tracks);

    if (vertex)  m_vertices.push_back(vertex);
    else { 
      Warning("fit didn't converge",StatusCode::SUCCESS,0).ignore(); 
      delete vertex;   
    }    
  }
  return StatusCode::SUCCESS;
}
StatusCode TopoVertexTool::step6()
{
  /*
    Final computation and clean up.
  */

  RecVertexVector vertices;
  
  // Remove tracks contributing too much to chi2, make new vertices eventually
  for (RecVertexVector::iterator it_vertex=m_vertices.begin(); it_vertex<m_vertices.end();++it_vertex){
    LHCb::RecVertex * vertex = *it_vertex;
    vertex = removeHighChisqTracksFromVertex(vertex);
    if ((vertex->tracks()).size()==0) continue;
    vertices.push_back(vertex);
  }
  
  // Clear old vertices vector (unused vertex have already been deleted in removeHighChisqTracksFromVertex)
  m_vertices.clear();

  // Sort by Max
  m_vertexFunction->sortByValueMax(vertices);

  // Remove tracks already used in better (higher vmax) vertices; Remaining vertices with at least 1 track make the final list.
  TrackVector used_tracks;
  for (RecVertexVector::reverse_iterator it_vertex = vertices.rbegin(); it_vertex < vertices.rend(); ++it_vertex) {
    LHCb::RecVertex * vertex = *it_vertex;
    TrackVector to_be_removed;
    const SmartRefVector< LHCb::Track > vertex_tracks = vertex->tracks();
    for (SmartRefVector< LHCb::Track >::const_iterator it_vertex_track=vertex_tracks.begin(); 
         it_vertex_track<vertex_tracks.end(); ++it_vertex_track) {
      const LHCb::Track * track = (*it_vertex_track);
      if (std::find(used_tracks.begin(), used_tracks.end(),track)!=used_tracks.end()){
        to_be_removed.push_back(track);
      } else {
        used_tracks.push_back(track);
      }
    }
    for (TrackVector::iterator it_track=to_be_removed.begin(); it_track<to_be_removed.end(); ++it_track){
      vertex->removeFromTracks(*it_track);
    }
    const SmartRefVector< LHCb::Track > vertex_finaltracks = vertex->tracks();
    for (SmartRefVector< LHCb::Track >::const_iterator it_vertex_track=vertex_finaltracks.begin();
         it_vertex_track<vertex_finaltracks.end(); ++it_vertex_track) {
    }
    if ((vertex->tracks()).size()>0) {
      m_vertices.push_back(vertex);
    }
  }
  return StatusCode::SUCCESS;
}

void TopoVertexTool::addUnResolvedToCluster(RecVertexVector & cluster, RecVertexVector & vertices, unsigned int index)
{
  /*
    Add vertices to a cluster. 
    A vertex are added to a cluster if it is unresolved with ANY vertex of the cluster.
    Unresolved vertices which are added to the cluster are removed from the input list.
    All vertices in the cluster are scanned recursivly. 
  */
  RecVertexVector remaining_vertices;
  LHCb::RecVertex * vtx = cluster[index];
  for (RecVertexVector::iterator it_vertex=vertices.begin(); it_vertex<vertices.end(); ++it_vertex){
    if (! m_vertexFunction->areResolved(**it_vertex,*vtx)){
      cluster.push_back(*it_vertex);
    } else {
      remaining_vertices.push_back(*it_vertex);
    }
  }
  if ( (index+1) < cluster.size()) {
    addUnResolvedToCluster(cluster,remaining_vertices, index+1);
  }
  vertices.clear();
  vertices.insert(vertices.begin(), remaining_vertices.begin(), remaining_vertices.end());
}

LHCb::RecVertex * TopoVertexTool::removeHighChisqTracksFromVertex(LHCb::RecVertex * vertex)
{
  /*
    Remove from vertex tracks with a chi2 contribution above threshold.
    The method is called recursivly to remove one track at a time.
    A new vertex is created (fitted) each time a track is removed and the previous vertex is deleted.
  */

  if ((vertex->tracks()).size()<3) return vertex ;
  
  TrackVector updated_tracks;

  double max_chi2=0;
  const LHCb::Track * track_to_be_removed = 0;
  const SmartRefVector< LHCb::Track > tracksRef = vertex->tracks();
  for (SmartRefVector< LHCb::Track >::const_iterator it_trackRef=tracksRef.begin();it_trackRef<tracksRef.end();++it_trackRef){
    double chi2;
    double impact;
    StatusCode sc = m_distanceCalculator->distance(*it_trackRef, vertex, impact, chi2);
    if (sc.isFailure ()) {
      Error(" removeHighChisqTracksFromVertex : fail to compute distance",StatusCode::FAILURE,50).ignore();
      chi2 = max_chi2+1;
    }
    const LHCb::Track * track = *it_trackRef;
    if (chi2>max_chi2) {
      if (0!=track_to_be_removed) updated_tracks.push_back(track_to_be_removed);
      track_to_be_removed = track;
      max_chi2 = chi2;
    } else {
      updated_tracks.push_back( track);
    }
  }
  
  if ( ( max_chi2 > m_TrackVtxChi2Max ) && (updated_tracks.size()>1) ) {
    LHCb::RecVertex * new_vertex = m_trackVertexer->fit(updated_tracks);
    if (vertex) {
      
      delete(vertex);
      vertex = removeHighChisqTracksFromVertex(new_vertex);
    }
    else { 
      Warning("fit didn't converge",StatusCode::SUCCESS,0).ignore(); 
      delete vertex;   
    }
  }  
  return vertex;
  
}


void TopoVertexTool::cleanUpTemporary()
{
  // Delete RecVertices from previous search and clear lists and maps 
  for (RecVertexVector::iterator it_vertex=m_2tracks_vertices.begin(); it_vertex<m_2tracks_vertices.end(); ++it_vertex){
    delete (*it_vertex);
  }
  for (VerticesList::iterator it_clusters=m_list_of_clusters.begin(); it_clusters<m_list_of_clusters.end(); ++it_clusters){
    delete (*it_clusters);
  }
  m_2tracks_vertices.clear();
  m_2tracks_vertices_per_track.clear();
  m_list_of_clusters.clear();
}

void TopoVertexTool::cleanUpResult()
{
  if (! m_writtenOnTES) {
    for (RecVertexVector::iterator it_vertex=m_vertices.begin(); it_vertex<m_vertices.end(); ++it_vertex){
      delete (*it_vertex);
    }
  }
  
  m_vertices.clear();
}

void TopoVertexTool::cleanUpFull()
{
  cleanUpResult();
  cleanUpTemporary();
}

void TopoVertexTool::setVertexFunctionParam()
{
  
  m_vertexFunction->setParam("MaxFinderStep",m_step);
 
  m_vertexFunction->setParam("MaxFinderMinGradientMag",m_max_finder_min_gradient_mag);
  m_vertexFunction->setParam("MaxFinderMaxIteration",m_max_finder_max_iteration);
  m_vertexFunction->setParam("MaxFinderMinStep",m_max_finder_min_step);
  m_vertexFunction->setParam("MaxFinderMaxjump",m_max_finder_max_jump);

  m_vertexFunction->setParam("ResolverCut",m_resolver_cut); 
  m_vertexFunction->setParam("ResolverMinStep",m_resolver_min_step);
  m_vertexFunction->setParam("ResolverMaxIteration",m_resolver_max_iteration);
  
  //  m_vertexFunction->setParam("DxForGradient",m_dx_for_gradient);
}

StatusCode TopoVertexTool::writeOnTES(std::string location)
{
  LHCb::RecVertices * pvertices = new LHCb::RecVertices();
  put(pvertices,location);
  for (RecVertexVector::iterator it_vertex=m_vertices.begin(); it_vertex<m_vertices.end(); ++it_vertex){
    pvertices->insert(*it_vertex);
  }

  m_writtenOnTES = true;

  return StatusCode::SUCCESS;
}

int TopoVertexTool::getNbSignalTrack(RecVertexVector VecVtx){
  std::map<int, int> signal_track_map;
  for (RecVertexVector::iterator i_vtx=VecVtx.begin(); i_vtx<VecVtx.end(); ++i_vtx){
    for (SmartRefVector< LHCb::Track >::const_iterator i_tr=(*i_vtx)->tracks().begin(); i_tr!=(*i_vtx)->tracks().end(); ++i_tr){
      int mother_pid = (*i_tr)->info(0,0.0);
      if (mother_pid ==  15 || mother_pid == -15 ){
	signal_track_map.insert ( std::pair<int,int>((*i_tr)->info(2,-1),mother_pid));      
      }
    }
  }
  //info()<<"Found "<<signal_track_map.size()<<" signal tracks"<<endmsg;
  //info()<<signal_track_map<<endmsg;
  return signal_track_map.size();
}


int TopoVertexTool::getNbSignalTrack(VerticesList VtxList){
  std::map<int, int> signal_track_map;
  for (VerticesList::iterator it_cluster=VtxList.begin(); it_cluster<VtxList.end(); ++it_cluster){
    for (RecVertexVector::iterator i_vtx=(*it_cluster)->begin(); i_vtx<(*it_cluster)->end(); ++i_vtx){
      for (SmartRefVector< LHCb::Track >::const_iterator i_tr=(*i_vtx)->tracks().begin(); i_tr!=(*i_vtx)->tracks().end(); ++i_tr){
	int mother_pid = (*i_tr)->info(0,0.0);
	if (mother_pid ==  15 || mother_pid == -15 ){
	  signal_track_map.insert ( std::pair<int,int>((*i_tr)->info(2,-1),mother_pid));      
	}
      }
    }
  }
  //info()<<"Found "<<signal_track_map.size()<<" signal tracks"<<endmsg;
  //info()<<signal_track_map<<endmsg;
  return signal_track_map.size();
}

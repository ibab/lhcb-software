
// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Event/MCHeader.h"

// local
#include "MCFilterVtxTopoTracksTool.h"


//-----------------------------------------------------------------------------
// Implementation file for class : MCFilterVtxTopoTracksTool
//
// 2012-11-08 : Julien Cogan and Mathieu Perrin-Terrin
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCFilterVtxTopoTracksTool )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCFilterVtxTopoTracksTool::MCFilterVtxTopoTracksTool( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
: GaudiHistoTool ( type, name , parent )
{
  declareInterface<IFilterVtxTopoTracksTool>(this);
  std::string Default_decay = "[B_s0 -> (tau+ -> ^pi+ ^pi- ^pi+ nu_tau~ {,gamma}{,gamma}{,gamma}) (tau- -> ^pi+ ^pi- ^pi- nu_tau {,gamma}{,gamma}{,gamma}) {,gamma}{,gamma}{,gamma}]cc";
  declareProperty("DecayDescriptor",m_DecayDescriptor= Default_decay);

  declareProperty("Plot"  ,m_DoPlot = false);

  std::string Default_decay_2 = "[B_s0 -> (tau+ -> ^pi- ^pi+ nu_tau~ {,e+,pi-}{,e-,pi-}{,gamma}{,gamma}{,gamma}) (tau- -> ^pi+ ^pi- nu_tau {,e+,pi-}{,e-,pi-}{,gamma}{,gamma}{,gamma}) {,gamma}{,gamma}{,gamma}]cc";
  declareProperty("DecayDescriptor",m_DecayDescriptor= Default_decay);

}


//====================================================================
// Initialization
//====================================================================
StatusCode MCFilterVtxTopoTracksTool::initialize()
{
  const StatusCode sc = GaudiHistoTool::initialize();
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize" << endmsg;

  info() << "Looking decay: " << m_DecayDescriptor << endmsg;

  m_MCDecayFinder = tool<IMCDecayFinder>( "MCDecayFinder", this );

  m_MCDecayPrinter = tool<IPrintMCDecayTreeTool>("PrintMCDecayTreeTool", this );

  return sc;
}

//=============================================================================

//=============================================================================
std::vector<const LHCb::Track*> & MCFilterVtxTopoTracksTool::filteredTracks(Tuples::Tuple* tuple)
{
  m_tracks.clear();
  //  bool AllRec = true;
  int Reconstructed = -1;
  debug()<<"Tool "<<name()<<endmsg;
  //Get the MC particle form the TES and the find the one corresponding to the decay
  //position of the MCPart are stored in m_MC_Tracks
  bool DecayFound = findMCDecay();
  if (DecayFound==false) {
    if(tuple != NULL) (*tuple)->column("Reconstructed",Reconstructed);
    debug()<<"No Decays found in this event"<<endmsg;
    return m_tracks;
  }

  //Get the Relation Table
  LHCb::Track2MC2D* Track2MC_table  = get<LHCb::Track2MC2D>(LHCb::Track2MCLocation::Default);
  LHCb::MC2Track*   MC2Track_table = Track2MC_table->inverse() ;

  debug()<<"MCTrack size "<< m_MC_Tracks.size()<<endmsg;
  //Make the association and store the MCParticle index as extra_info
  LHCb::MCParticles::iterator i_MCPart = m_MCParts->begin();
  Reconstructed = 1;
  for (vector<TrackAncestor>::iterator i_MCTrack = m_MC_Tracks.begin(); i_MCTrack!=m_MC_Tracks.end(); ++i_MCTrack) {

    i_MCPart = m_MCParts->begin();
    advance(i_MCPart,i_MCTrack->DaughterMCIndex);
    LHCb::MC2Track::Range links = MC2Track_table->relations ( *i_MCPart ) ;

    if ( links.empty() ) {
      debug()<<"No MC->track link found, NO TRACK RETURNED!   MC Part Ancestors :  "<<endmsg;
      Reconstructed = 0;
      if( msgLevel( MSG::DEBUG ) ) m_MCDecayPrinter->printAncestor(*i_MCPart);
      //      AllRec = false;
      ++counter("Missing MC->track link "+name());
      continue;
    }

    //LHCb::Track* bestTrack = new LHCb::Track (*links.back().to()); //copy the track in a new pointer to avoid modifying the TES
    LHCb::Track* bestTrack = links.back().to(); //copy the track in a new pointer to avoid modifying the TES
    bestTrack->addInfo(0,i_MCTrack->MotherPid); //tau  pid
    bestTrack->addInfo(1,i_MCTrack->MotherMCIndex); //tau  MCindex
    bestTrack->addInfo(2,i_MCTrack->DaughterMCIndex); //pion MCindex
    bestTrack->addInfo(3,i_MCTrack->MotherZ); //pion MCindex

    m_tracks.push_back(bestTrack);

    //check if the first track has the same weight as another track

    double bestTrackWeight = links.back().weight();
    int nb_track_with_equal_weight = 0;
    for ( LHCb::MC2Track::Range::iterator link = links.begin() ; links.end() != link ; ++link ){
      if( link->weight() == bestTrackWeight){
        //debug()<<"w= "<<link->weight()<<endmsg;
        nb_track_with_equal_weight++;
      }
    }
    if(nb_track_with_equal_weight>1) warning()<<"Number of track with same weight ("<<bestTrackWeight <<")  >1 : "<<nb_track_with_equal_weight<<endmsg;
    //debug()<<"Number of track with same weight ("<<bestTrackWeight <<")  >1 : "<<nb_track_with_equal_weight<<endmsg;



    // //check if the track type is correct
    // for (unsigned int i_type=0; i_type<m_TrackTypeAllowed.size(); i_type++){
    //   if( largest->checkType( m_TrackTypeMap[m_TrackTypeAllowed[i_type]])) {
    //  double indexMC = (double) *i_MCTrack;
    //     largest->addInfo(0,indexMC);
    //     m_tracks.push_back(largest);
    //     break;
    //   }
    // }

  }
  if(tuple != NULL) (*tuple)->column("Reconstructed",Reconstructed);

  if (Reconstructed == 1)     {if(m_DoPlot)  plot (  1, "Reconstructed" , -0.5 , 2.5 , 3 ) ;}
  else                   {if(m_DoPlot)  plot (  0, "Reconstructed" , -0.5 , 2.5 , 3 ) ;}
  //if not all the track are reconstructed, return an empty vector
  //if (!AllRec) m_tracks.clear();
  // if( msgLevel( MSG::DEBUG ) ) {
  //   debug()<<"Check Extra Info in the TES "<<endmsg;
  //   LHCb::Tracks* all_tracks = get<LHCb::Tracks>("/Event/Rec/Track/Best");
  //   for ( LHCb::Tracks::const_iterator itt = all_tracks->begin(); itt != all_tracks->end() ; ++itt) {
  //     double ExtInfo = 0;
  //     if ((*itt)->hasInfo(0)){
  //  debug()<<"Has Info 0:  "<<(*itt)->info(0,ExtInfo)<<endmsg;
  //     }
  //   }
  // }
  debug()<<"Nb track retruned by Filter: "<<m_tracks.size()<<endmsg;
  return m_tracks;
}
//=============================================================================

bool MCFilterVtxTopoTracksTool::findMCDecay()
{
  long evtnum;
  m_MC_Tracks.clear();
  /// This function fills m_MC_Tracks with the index of the
  /// MCParticle in the TES corresponding to the pions (and muon).
  m_MCParts = get<LHCb::MCParticles> ( LHCb::MCParticleLocation::Default );
  if (exist<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default)){
    const LHCb::MCHeader *evtHeader = get<LHCb::MCHeader>(LHCb::MCHeaderLocation::Default);
    evtnum = evtHeader->evtNumber();
  }
  debug()<<"Number for MC Parts in the TES :"<< m_MCParts->size()<<endmsg;
  //call the decay finder
  m_MCDecayFinder->setDecay(m_DecayDescriptor);
  bool hasDecay = m_MCDecayFinder->hasDecay(*m_MCParts);
  //bool hasDecay = m_MCDecayFinder->hasDecay();
  if (hasDecay == false){
    //check if it is the decay with an electron
    m_MCDecayFinder->setDecay(m_DecayDescriptor_2);
    if( m_MCDecayFinder->hasDecay(*m_MCParts)) {
      info()<<"The event "<<evtnum<<" has decay "<< m_DecayDescriptor_2 <<"  [msg from "<< name()<<endmsg;
      if(m_DoPlot)  plot (  2, "DecayType" , -0.5 , 2.5 , 3 ) ;
    }
    else {
      if(m_DoPlot)  plot (  0, "DecayType" , -0.5 , 2.5 , 3 ) ;
      info()<<"The event "<<evtnum<<" hasn't the required decay  [msg from "<< name()<<endmsg;
      ++counter("No decay found in the event "+name());
      //    if( msgLevel( MSG::DEBUG ) )
      m_MCDecayPrinter->printAsTree(*m_MCParts);
    }
    m_MCDecayFinder->setDecay(m_DecayDescriptor);
    return hasDecay;
  }
  else{
    if(m_DoPlot)  plot (  1, "DecayType" , -0.5 , 2.5 , 3 ) ;
    debug()<<"The event "<<evtnum <<" has the required decay; from "<< name()<<endmsg;
    const LHCb::MCParticle * headOfDecay = NULL;
    m_MCDecayFinder->findDecay(*m_MCParts, headOfDecay);
    //debug()<<"findDecay   :"<< headOfDecay<<endmsg;
    if (headOfDecay){
      //debug()<<"# mother found   :"<< headOfDecay->particleID().pid()<<endmsg;
      LHCb::MCParticle::ConstVector daughters;
      m_MCDecayFinder->decayMembers   ( headOfDecay, daughters);
      //debug()<<"# daughter found :"<< daughters.size()<<endmsg;

      //now loop over the descendant to get their position in the list
      for (LHCb::MCParticle::ConstVector::const_iterator i_daughter = daughters.begin(); i_daughter!=daughters.end(); ++i_daughter){

        TrackAncestor MC_Track;
        //debug()<<"look index of :"<< (*i_daughter)->particleID().pid()<<endmsg;

        LHCb::MCParticles::iterator daughter_position =  find(m_MCParts->begin(),m_MCParts->end(),*i_daughter)   ;
        MC_Track.DaughterMCIndex = distance(m_MCParts->begin(),daughter_position);

        const LHCb::MCParticle* Mother = (*i_daughter)->mother();
        if(!Mother) {
          warning()<<"In MC matching, track has no mother"<<endmsg;
          continue;
        }
        MC_Track.MotherPid = Mother->particleID().pid();
        MC_Track.MotherZ =   (*i_daughter)->originVertex()->position().z();
        //debug()<<"MC_Track.pid mother ="<<Mother->particleID().pid()<<endmsg;
        LHCb::MCParticles::iterator mother_position = find(m_MCParts->begin(),m_MCParts->end(),Mother);
        MC_Track.MotherMCIndex = distance(m_MCParts->begin(),mother_position);

        m_MC_Tracks.push_back(MC_Track);
        LHCb::MCParticles::iterator positionCheck = m_MCParts->begin();
        positionCheck = positionCheck + MC_Track.DaughterMCIndex;
        //debug()<<"check index Daughter is correct i:"<< MC_Track.DaughterMCIndex <<"  p :"<< (*i_daughter)->p() <<"   VS   "<<(*positionCheck)->p()<<endmsg;
        positionCheck = m_MCParts->begin();
        positionCheck = positionCheck + MC_Track.MotherMCIndex;
        //debug()<<"check index Mother is correct i:"<< MC_Track.MotherMCIndex <<"  p :"<< Mother->p() <<"   VS   "<<(*positionCheck)->p()<<endmsg;


      }
    }

  }

  return hasDecay;

}



//=============================================================================
// Destructor
//=============================================================================
MCFilterVtxTopoTracksTool::~MCFilterVtxTopoTracksTool() {
  //delete m_MCParts;

}

//=============================================================================

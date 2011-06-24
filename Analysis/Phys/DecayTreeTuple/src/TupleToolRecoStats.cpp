// $Id: TupleToolRecoStats.cpp,v 1.6 2010-09-09 12:22:42 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TupleToolRecoStats.h"
#include "Event/ProtoParticle.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/STCluster.h"
#include "Event/VeloCluster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolRecoStats
//
// 2009-02-11 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolRecoStats ); 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolRecoStats::TupleToolRecoStats( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : TupleToolBase ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);
  declareProperty("LongChi2", m_chi2 = 9.);
}
//=============================================================================
// Destructor
//=============================================================================
TupleToolRecoStats::~TupleToolRecoStats() {} 

//=============================================================================
StatusCode TupleToolRecoStats::initialize() {
  if( ! TupleToolBase::initialize() ) return StatusCode::FAILURE;

  m_rawBankDecoder = tool<IOTRawBankDecoder>("OTRawBankDecoder");
  if(!m_rawBankDecoder){
    Error("unable to get the OTRawBankDecoder tool");
    return StatusCode::FAILURE;
  }
  
  m_l0BankDecoder = tool<IL0DUFromRawTool>("L0DUFromRawTool");
  //declareProperty( "L0DUFromRawTool"   , m_fromRawTool = "L0DUFromRawTool" );
  //m_l0BankDecoder = tool<IL0DUFromRawTool>( m_fromRawTool , m_fromRawTool,this );
  if(!m_l0BankDecoder){
    Error("unable to get the L0DUFromRawTool");
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode TupleToolRecoStats::fill( Tuples::Tuple& tup) 
{
  const std::string prefix=fullName();
  
  bool test = true;
  const LHCb::Track::Container* tracks =  get<LHCb::Track::Container> ( LHCb::TrackLocation::Default ) ;
  const LHCb::ProtoParticle::Container* charged = get<LHCb::ProtoParticle::Container>(LHCb::ProtoParticleLocation::Charged);
  const LHCb::ProtoParticle::Container* neutrals = get<LHCb::ProtoParticle::Container>(LHCb::ProtoParticleLocation::Neutrals);
  test &= tup->column(prefix+"ChargedProtos", charged->size());
  test &= tup->column(prefix+"NeutralProtos", neutrals->size());
  test &= tup->column(prefix+"BestTracks", tracks->size());
  if (isVerbose()){
    test &= tup->column(prefix+"MuonTracks",number<LHCb::Tracks>(LHCb::TrackLocation::Muon));
    test &= tup->column(prefix+"ITClusters",number<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters));
    test &= tup->column(prefix+"VeloLiteClusters",
                        number<LHCb::VeloLiteCluster::FastContainer>(LHCb::VeloLiteClusterLocation::Default));  
    const unsigned int nHitsInOT = m_rawBankDecoder->totalNumberOfHits();
    test &= tup->column(prefix+"OTClusters", nHitsInOT);
    
    m_l0BankDecoder->fillDataMap();
    bool l0BankDecoderOK = m_l0BankDecoder->decodeBank();
    if(!l0BankDecoderOK ){
      Error("Readout error : unable to monitor the L0DU rawBank", StatusCode::SUCCESS,StatusCode::SUCCESS).ignore();
      return StatusCode::SUCCESS;
    }
    int nSpd = m_l0BankDecoder->data("Spd(Mult)");
    test &= tup->column(prefix+"spdMult", nSpd);
    
    unsigned int nBack = 0;
    unsigned int nLong = 0;
    unsigned int nLongGood = 0;
    int veloTracks = 0;

    // Protection from empty track container 
    if( tracks!=0 && tracks->size()>0 ) {
      LHCb::Tracks::const_iterator iterT = tracks->begin();
      for(; iterT != tracks->end() ;++iterT) {
        if ((*iterT)->checkFlag( LHCb::Track::Backward)) ++nBack;
        if ((*iterT)->checkType( LHCb::Track::Long )){
          ++nLong;
          double chi2 = ((*iterT)->nDoF() > 0) ? (*iterT)->chi2()/(*iterT)->nDoF() : 999;
          if (chi2 < m_chi2) ++nLongGood;
        }
      }
      veloTracks = nVelo(tracks);
    }

    test &= tup->column(prefix+"backwardTracks", nBack);
    test &= tup->column(prefix+"veloTracks", veloTracks);
    test &= tup->column(prefix+"longTracks", nLong);
    test &= tup->column(prefix+"goodLongTracks", nLongGood);
  }
  
  return StatusCode(test) ;
    
  } 
//=============================================================================
// nVelo by Matt, with a protection by PK
//============================================================================
unsigned TupleToolRecoStats::nVelo(const LHCb::Tracks* tracks) {

  if (msgLevel(MSG::VERBOSE)) verbose() << "nVelo" << endmsg ;
    std::vector<LHCb::Track*> tmpCont;
    LHCb::Tracks::const_iterator iterT = tracks->begin();
    for(; iterT != tracks->end() ;++iterT) {
      if ((*iterT)->hasVelo() == true) tmpCont.push_back(*iterT); 
    }
    if (msgLevel(MSG::VERBOSE)) verbose() << "tmpCont " << tmpCont.size() << endmsg ;
    if (tmpCont.empty()) return 0;
    std::vector<LHCb::Track*> keepCont;
    keepCont.push_back(tmpCont.front());
    
    std::vector<LHCb::Track*>::const_iterator iterT2 = tmpCont.begin();
    for (;iterT2 != tmpCont.end(); ++iterT2 ){
      const std::vector<LHCb::LHCbID>& vids = (*iterT2)->lhcbIDs();
      std::vector<LHCb::LHCbID> veloHits; veloHits.reserve(vids.size());
      LoKi::select(vids.begin(), vids.end(), std::back_inserter(veloHits), boost::bind(&LHCb::LHCbID::isVelo,_1));
      
      if (inCloneContainer(keepCont,veloHits) == false){
        // nothing
      } 
      else {
        keepCont.push_back(*iterT2);   
      }
    } // iterT2
    if (msgLevel(MSG::VERBOSE)) verbose() << "keepCont " << keepCont.size() << endmsg ;
    return keepCont.size();
}

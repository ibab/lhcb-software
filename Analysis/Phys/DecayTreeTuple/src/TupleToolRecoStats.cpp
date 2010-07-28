// $Id: TupleToolRecoStats.cpp,v 1.3 2010-07-28 16:44:41 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TupleToolRecoStats.h"
#include "Event/ProtoParticle.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/STCluster.h"

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
  declareProperty("fillVerbose", fillVerbose=true );
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
  test &= tup->column(prefix+"ChargedProtos",number<LHCb::ProtoParticles>(LHCb::ProtoParticleLocation::Charged));
  test &= tup->column(prefix+"NeutralProtos",number<LHCb::ProtoParticles>(LHCb::ProtoParticleLocation::Neutrals));
  test &= tup->column(prefix+"BestTracks",number<LHCb::Tracks>(LHCb::TrackLocation::Default));

  if (isVerbose()){
    test &= tup->column(prefix+"MuonTracks",number<LHCb::Tracks>(LHCb::TrackLocation::Muon));
    test &= tup->column(prefix+"ITClusters",number<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters));  
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
    
    const LHCb::Track::Container* tracks =  get<LHCb::Track::Container> ( LHCb::TrackLocation::Default ) ;
    unsigned int nBack = 0;
    LHCb::Tracks::const_iterator iterT = tracks->begin();
    for(; iterT != tracks->end() ;++iterT) {
      if ((*iterT)->checkFlag( LHCb::Track::Backward) == true) ++nBack;
    }
    test &= tup->column(prefix+"backwardTracks", nBack);
    
    int veloTracks = 0;
    veloTracks = nVelo(tracks);
    test &= tup->column(prefix+"veloTracks", veloTracks);
  }
  
  return StatusCode(test) ;
    
  } 

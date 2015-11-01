// $Id: ChargedPP2MC.cpp,v 1.15 2009-08-19 14:53:58 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// from Event
#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"

// Associators
#include "Relations/RelationWeighted1D.h"

// DaVinciKernel
#include "Kernel/PP2MCLocation.h"

// local
#include "Kernel/Particle2MCLinker.h"
#include "AsctAlgorithm.h"

static const std::string&
ChargedPP2MCAsctLocation = LHCb::ProtoParticle2MCLocation::Charged;

class ChargedPP2MC : public AsctAlgorithm {

  friend class AlgFactory<ChargedPP2MC>;

public:
  /// Standard constructor
  ChargedPP2MC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ChargedPP2MC( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  /// Create on demand the linker object
  Object2MCLinker<LHCb::Track>* track2MCLink() const
  {
    if ( !m_track2MCLink )
    {
      m_track2MCLink = new Object2MCLinker<LHCb::Track>( this,
                                                         "", "",
                                                         m_trackLocations);
    }
    return m_track2MCLink;
  }
  bool m_vetoempty;

private:

  mutable Object2MCLinker<LHCb::Track>* m_track2MCLink;
  std::vector<std::string> m_trackLocations ; ///< location for tracks

};

using namespace LHCb;

// Define data types
typedef RelationWeighted1D<ProtoParticle,MCParticle,double>  Table;

//-----------------------------------------------------------------------------
// Implementation file for class : ChargedPP2MC
//
// 10/07/2002 : Philippe Charpentier
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory

DECLARE_ALGORITHM_FACTORY( ChargedPP2MC )

#define _verbose if( msgLevel(MSG::VERBOSE) ) verbose()
#define _debug if( msgLevel(MSG::DEBUG) ) debug()
#define _info if( msgLevel(MSG::INFO) ) info()

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  ChargedPP2MC::ChargedPP2MC( const std::string& name,
                              ISvcLocator* pSvcLocator)
    : AsctAlgorithm ( name , pSvcLocator ),
      m_track2MCLink ( NULL )
{
  m_inputData.push_back( ProtoParticleLocation::Charged );
  m_inputData.push_back( ProtoParticleLocation::Upstream );
  m_inputData.push_back( ProtoParticleLocation::HltCharged );
  m_outputTable = ChargedPP2MCAsctLocation;

  m_trackLocations.push_back(TrackLocation::Default);
  declareProperty("TrackLocations", m_trackLocations );
  setProperty("InputData", m_inputData );
  declareProperty("VetoEmpty", m_vetoempty=false );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedPP2MC::~ChargedPP2MC() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedPP2MC::execute() {

  _debug << "==> Execute" << endmsg;

  // flag to create a Relations table if needed,
  const bool createTable = !outputTable().empty();

  ///////////////////////////////////////
  // Loop on ProtoParticles containers //
  ///////////////////////////////////////

  for ( std::vector<std::string>::const_iterator inp = m_inputData.begin();
        m_inputData.end()!= inp; inp++ )
  {
    // Get ProtoParticles
    SmartDataPtr<ProtoParticles> protos ( evtSvc(), *inp );
    // Check here to avoid the rest running on uDSTs where Protos are missing
    //if ( !protos ) continue;
    // Though we do not want to create empty containers in Turbo processing
    if ( !protos && m_vetoempty ) continue;

    // postpone the check for pointer till linker and relations table
    // are cretsaed/locate/registered

    // Create a linker table
    const std::string linkContainer =
      *inp + Particle2MCMethod::extension[Particle2MCMethod::ChargedPP];
    // Just a fake helper class
    Object2MCLinker<LHCb::ProtoParticle> p2MCLink(this);
    auto linkerTable = p2MCLink.linkerTable( linkContainer );

    Table* table = nullptr ;
    if ( createTable )
    {
      // create new table
      table = new Table( protos ? protos->size() : 0 ) ;
      // Register the relations table on the TES using Marco's convention
      std::string loc = *inp ;
      if ( loc.compare(0,7,"/Event/") == 0 ) { loc.erase(0,7) ;}
      put ( table , "Relations/" + loc );
    }

    if ( !table && !linkerTable ) continue;

    // and only here check the input data
    if ( !protos                          ) continue;

    int npp = protos->size();
    _verbose << "    " << npp
             << " ProtoParticles retrieved from "
             << *inp << endmsg;

    // loop on Protos to match them from their tracks
    int nrel = 0 ;
    int nass = 0;
    for( ProtoParticles::const_iterator pIt=protos->begin() ;
         protos->end() != pIt; pIt++) {
      _verbose
        << "    ProtoParticle " << (*pIt)->key();
      // Follow links from this protoParticle via tracks
      const Track* track = (*pIt)->track() ;
      if( 0 != track ) {
        _verbose
          << " from track " << track->key();
        double weight = 0.;
        const MCParticle* mcPart = track2MCLink()->first(track, weight);
        if( NULL == mcPart ) {
          _verbose << " not associated to an MCPart";
        } else {
          _verbose << " associated to MCParts";
          nass++;
          do {
            _verbose << " - " << mcPart->key();
            if( NULL != table )
              table->relate( *pIt, mcPart, weight);
            if( NULL != linkerTable )
              linkerTable->link( *pIt, mcPart, weight);
            nrel++;
            mcPart = track2MCLink()->next();
          } while( NULL != mcPart );
        }
      } else {
        _verbose << " not originating from a track";
      }
      _verbose << endmsg;
    }
    _debug
      << "Out of " << npp << " Charged ProtoParts in " << *inp << ", "
      << nass << " are associated, "
      << nrel << " relations found" << endmsg;
  }

  return StatusCode::SUCCESS ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedPP2MC::finalize() {

  _debug << "==> Finalize" << endmsg;
  delete m_track2MCLink;
  m_track2MCLink = NULL;
  return GaudiAlgorithm::finalize();
}

//=============================================================================

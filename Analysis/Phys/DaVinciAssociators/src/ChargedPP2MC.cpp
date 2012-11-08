// $Id: ChargedPP2MC.cpp,v 1.15 2009-08-19 14:53:58 pkoppenb Exp $

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

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  Object2MCLinker<LHCb::Track>* m_track2MCLink;
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

#define _verbose if( msgLevel(MSG::VERBOSE) ) verbose()
#define _debug if( msgLevel(MSG::DEBUG) ) debug()
#define _info if( msgLevel(MSG::INFO) ) info()

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedPP2MC::ChargedPP2MC( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : AsctAlgorithm ( name , pSvcLocator )
{
  m_inputData.push_back( ProtoParticleLocation::Charged );
  m_inputData.push_back( ProtoParticleLocation::Upstream );
  m_inputData.push_back( ProtoParticleLocation::HltCharged );
  m_outputTable = ChargedPP2MCAsctLocation;

  m_trackLocations.push_back(TrackLocation::Default);
  declareProperty("TrackLocations", m_trackLocations );
  declareProperty("InputData", m_inputData );

}

//=============================================================================
// Destructor
//=============================================================================
ChargedPP2MC::~ChargedPP2MC() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode ChargedPP2MC::initialize() 
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( !sc.isSuccess() ) return sc;
  
  // Get a Linker class for Tr2MCP
  m_track2MCLink = new Object2MCLinker<LHCb::Track>( this,
                                                     "", "",
                                                     m_trackLocations);
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedPP2MC::execute()
{
  _debug << "==> Execute" << endreq;

  // flag to create a Relations table if needed,
  const bool createTable = ( outputTable().empty() ? false : true );

  ///////////////////////////////////////
  // Loop on ProtoParticles containers //
  ///////////////////////////////////////

  for( std::vector<std::string>::const_iterator inp = m_inputData.begin();
       m_inputData.end()!= inp; ++inp )
  {
  
    // Get ProtoParticles
    SmartDataPtr<ProtoParticles> protos ( evtSvc(), *inp );

    // postpone the check for pointer till linker and relations table
    // are created/locate/registered

    // Create a linker table
    const std::string linkContainer =
      *inp + Particle2MCMethod::extension[Particle2MCMethod::ChargedPP];
    
    Table * table = NULL ;
    if ( createTable )
    {
      // create new table
      table = new Table( 0 == protos ? 0 : protos->size() ) ;
      // Register the relations table on the TES using Marco's convention
      std::string loc = *inp ;
      if ( 0 == loc.find("/Event/") ) { loc.replace(0,7,"") ;}
      put ( table , "Relations/" + loc );
    }
    if ( NULL == table ) continue;

    // and only here check the input data, so above table is always made
    if ( 0 == protos ) continue;
    
    // Just a fake helper class
    Object2MCLinker<LHCb::ProtoParticle> p2MCLink(this);
    Object2MCLinker<LHCb::ProtoParticle>::Linker*
      linkerTable = p2MCLink.linkerTable( linkContainer );
    if ( NULL == linkerTable ) continue;

    _verbose << "    " << protos->size()
             << " ProtoParticles retrieved from "
             << *inp << endreq;

    // loop on Protos to match them from their tracks
    unsigned int nrel(0), nass(0);
    for ( ProtoParticles::const_iterator pIt = protos->begin() ;
          protos->end() != pIt; ++pIt )
    {
      _verbose
        << "    ProtoParticle " << (*pIt)->key();
      
      // Follow links from this protoParticle via tracks
      const Track* track = (*pIt)->track() ;
      if( 0 != track )
      {
        _verbose
          << " from track " << track->key();
        double weight = 0.;
        const MCParticle* mcPart = m_track2MCLink->first(track, weight);
        if ( NULL == mcPart )
        {
          _verbose << " not associated to an MCPart";
        }
        else
        {
          _verbose << " associated to MCParts";
          ++nass;
          do 
          {
            _verbose << " - " << mcPart->key();
            if( NULL != table )
              table->relate( *pIt, mcPart, weight);
            if( NULL != linkerTable )
              linkerTable->link( *pIt, mcPart, weight);
            ++nrel;
            mcPart = m_track2MCLink->next();
          } while( NULL != mcPart );
        }
      } 
      else
      {
        _verbose << " not originating from a track";
      }
      _verbose << endreq;

    } // loop over protos

    _debug
      << "Out of " << protos->size() 
      << " Charged ProtoParts in " << *inp << ", "
      << nass << " are associated, "
      << nrel << " relations found" << endreq;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedPP2MC::finalize()
{
  _debug << "==> Finalize" << endreq;
  delete m_track2MCLink;
  m_track2MCLink = NULL;
  return GaudiAlgorithm::finalize();
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedPP2MC )

//=============================================================================

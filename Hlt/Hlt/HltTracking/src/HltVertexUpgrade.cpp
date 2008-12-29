// $Id: HltVertexUpgrade.cpp,v 1.15 2008-12-29 12:40:36 graven Exp $
// Include files
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IAlgManager.h"

// local
#include "HltVertexUpgrade.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltVertexUpgrade
//
// 2006-08-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltVertexUpgrade );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltVertexUpgrade::HltVertexUpgrade( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
  , m_selections(*this)
{
  declareProperty("RecoName", m_recoName = "empty");
  
  declareProperty("TransferExtraInfo", m_transferExtraInfo = true);
  
  declareProperty( "TESOutputVerticesName",  
                   m_TESOutputVerticesName = "Hlt/Vertex/VertexUpgradeBank");

  m_selections.declareProperties();
};
//=============================================================================
// Destructor
//=============================================================================
HltVertexUpgrade::~HltVertexUpgrade() {} 

//=============================================================================// Initialization
//=============================================================================
StatusCode HltVertexUpgrade::initialize() {

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_selections.retrieveSelections();
  m_selections.registerSelection();
  
  m_tool = tool<HltTrackUpgradeTool>("HltTrackUpgradeTool",this);
  if (!m_tool) 
    fatal() << " not able to retrieve upgrade track tool " << endreq;
  
  sc = m_tool->setReco(m_recoName);
  if ( sc.isFailure() ) return sc;
  return sc;

};

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltVertexUpgrade::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  RecVertices* overtices = 
    getOrCreate<RecVertices,RecVertices>(m_TESOutputVerticesName);
  m_tool->beginExecute();

  Hlt::VertexSelection *input = m_selections.input<1>();
  for (Hlt::VertexSelection::iterator it=input->begin(); it!=input->end(); ++it) {
    RecVertex& vseed = *(*it);
    const SmartRefVector<Track>& otracks = vseed.tracks();
    Track& seed1 = (Track&) *(*otracks.begin());
    Track& seed2 = (Track&) *(*(otracks.begin()+1));
    debug() << " -- updating vertex -- " << endreq;
    if (m_debug) {
      printInfo(" seed1 ", seed1);
      printInfo(" seed2 ", seed2);
    }
    std::vector<LHCb::Track*> tracks1, tracks2;
    sc = m_tool->iupgrade(seed1,tracks1);
    if (sc.isFailure()) return sc;
    sc = m_tool->iupgrade(seed2,tracks2);
    if (sc.isFailure()) return sc;
    if (tracks1.empty() || tracks2.empty() ) continue;
    debug() << " creating a vertex " << endreq;
    for (std::vector<Track*>::iterator t1 = tracks1.begin();
         t1 != tracks1.end(); ++t1) {
      Track& track1 = *(*t1);
      for (std::vector<Track*>::iterator t2 = tracks2.begin();
           t2 != tracks2.end(); ++t2) {
        Track& track2 = *(*t2);
        RecVertex* sv = new RecVertex();
        _makeVertex(track1,track2,*sv);
        sv->setExtraInfo(vseed.extraInfo());
        overtices->insert(sv);
        m_selections.output()->push_back(sv);
        debug() << " created vertex " << endreq;
        if (m_debug) printInfo(" vertex ",*sv);
        // TODO - wait till RecVertex is done
        // if (m_transferExtraInfo)
        //   sv->setExtraInfo( vseed.extraInfo() );
      }
    }
  }

  return sc;
}



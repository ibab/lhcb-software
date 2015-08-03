// Include files 

// from LHCb
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// local
#include "FlagSignalChain.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlagSignalChain
//
// 2015-07-23 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FlagSignalChain )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlagSignalChain::FlagSignalChain( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IFlagSignalChain>(this);

}
//=============================================================================
// Destructor
//=============================================================================
FlagSignalChain::~FlagSignalChain() {} 

//==============================================================================
// Set from signal flag
//==============================================================================
void FlagSignalChain::setFromSignalFlag( const LHCb::MCParticle* mother ) {

  SmartRefVector<LHCb::MCVertex>::const_iterator iv;
  for ( iv = mother->endVertices().begin();
        iv != mother->endVertices().end(); iv++ ) {
    SmartRefVector<LHCb::MCParticle>::const_iterator idau;
    for ( idau = (*iv)->products().begin();
          idau != (*iv)->products().end(); idau++ ) {
      const LHCb::MCParticle* mcpc = *idau;
      LHCb::MCParticle* mcp = const_cast< LHCb::MCParticle* >( mcpc ) ;
      mcp->setFromSignal(true);
      setFromSignalFlag( *idau );
    }
  }
  
}
//==============================================================================

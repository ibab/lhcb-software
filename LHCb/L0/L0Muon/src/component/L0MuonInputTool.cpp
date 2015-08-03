// Include files 

#include "Event/L0MuonData.h"


// local
#include "L0MuonInputTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonInputTool
//
// 2010-02-25 : Julien Cogan
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( L0MuonInputTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonInputTool::L0MuonInputTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IL0MuonInputTool>(this);
  declareProperty( "L0Context" , m_l0Context = ""  );
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonInputTool::~L0MuonInputTool() {} 

//=============================================================================

StatusCode L0MuonInputTool::getTiles(std::vector<LHCb::MuonTileID> & tiles, std::string rootInTes)
{
  StatusCode sc=StatusCode::SUCCESS;

  tiles.clear();

  sc = setProperty( "RootInTES", rootInTes );
  if( sc.isFailure() ) return Error( "Unable to set RootInTES property",StatusCode::FAILURE,50);

  // Data container on TES
  std::string location = LHCb::L0MuonDataLocation::Default + m_l0Context;
  LHCb::L0MuonDatas* pdatas = getIfExists<LHCb::L0MuonDatas>( location );
  if ( NULL == pdatas ) {
    return Warning("L0MuonDatasnot found at "+rootInTes+"/.../"+location,StatusCode::FAILURE,50);
  }
  
  // Loop over L0Muon datas
  for (LHCb::L0MuonDatas::const_iterator itdata = pdatas->begin() ; itdata!=pdatas->end() ; ++itdata){
    std::vector<LHCb::MuonTileID> ols = (*itdata)->ols();
    tiles.insert(tiles.end(),ols.begin(),ols.end());
  }

  sc = setProperty( "RootInTES", "" );
  if( sc.isFailure() ) return Error( "Unable to reset RootInTES property",StatusCode::FAILURE,50);

  return sc;
}

StatusCode L0MuonInputTool::getPads(std::vector<LHCb::MuonTileID> & pads, std::string rootInTes)
{
  StatusCode sc=StatusCode::SUCCESS;

  pads.clear();

  sc = setProperty( "RootInTES", rootInTes );
  if( sc.isFailure() ) return Error( "Unable to set RootInTES property",StatusCode::FAILURE,50);

  // Data container on TES
  std::string location = LHCb::L0MuonDataLocation::Default + m_l0Context;
  LHCb::L0MuonDatas* pdatas = getIfExists<LHCb::L0MuonDatas>( location );
  if ( NULL == pdatas ) {
    return Warning("L0MuonDatasnot found at "+rootInTes+"/.../"+location,StatusCode::FAILURE,50);
  }

  // Loop over L0Muon datas
  for (LHCb::L0MuonDatas::const_iterator itdata = pdatas->begin() ; itdata!=pdatas->end() ; ++itdata){
    std::vector<LHCb::MuonTileID> strips = (*itdata)->ols();
    std::vector<LHCb::MuonTileID>::iterator i1;
    std::vector<LHCb::MuonTileID>::iterator i2;
    for (i1=strips.begin(); i1<strips.end(); ++i1){
      if (i1->station()==0) {
        pads.push_back(*i1);// M1 contains pads already
      } else if ((i1->region()==0) & (i1->station()==3 || i1->station()==4)){
        pads.push_back(*i1);// M4 and M5 contains pads already in R1
      } else {
        for (i2=i1+1; i2<strips.end(); ++i2){
          LHCb::MuonTileID pad = (*i1).intercept(*i2);
          if (pad.isValid()){
            pads.push_back(pad);
          }
        }
      }
    }
  }

  sc = setProperty( "RootInTES", "" );
  if( sc.isFailure() ) return Error( "Unable to reset RootInTES property",StatusCode::FAILURE,50);

  return sc;
}

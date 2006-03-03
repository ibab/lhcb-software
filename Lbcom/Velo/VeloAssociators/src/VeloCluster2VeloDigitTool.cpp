// $Id: VeloCluster2VeloDigitTool.cpp,v 1.1 2006-03-03 12:33:03 szumlat Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "VeloCluster2VeloDigitTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloCluster2VeloDigitTool
//
// 2006-03-01 : Tomasz Szumlak
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VeloCluster2VeloDigitTool );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloCluster2VeloDigitTool::VeloCluster2VeloDigitTool( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IVeloCluster2VeloDigitTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
VeloCluster2VeloDigitTool::~VeloCluster2VeloDigitTool() {} 

//=============================================================================
SmartRefVector<LHCb::VeloDigit> getDigits(
                                LHCb::VeloCluster* cluster,
                                LHCb::VeloDigits* digitCont)
{
  // table for associated digits
  SmartRefVector<LHCb::VeloDigit> digitTable;
  // vector with channels for given cluster's strips 
  std::vector<LHCb::VeloChannelID> channels=cluster->channels();
  // first determine how many digits is related with given cluster
  int strips=static_cast<unsigned int>(cluster->size());
  // find related VeloDigit object for each strip
  LHCb::VeloChannelID channel;
  for(int i=0; i<strips; i++){
    channel=channels[i];
    LHCb::VeloDigit* digit=digitCont->object(channel);
    digitTable.push_back(digit);
  }
  //
  if(digitTable.size()==0){
    std::cerr<< " ==> No valid associations for the cluster! " <<std::endl;
    // create dummy object with strip == 0
    LHCb::VeloDigit fooDigit( LHCb::VeloChannelID(0) );
    digitTable.push_back(&fooDigit);
    return ( digitTable );
  }else{
    return ( digitTable );
  } 
}
//

// $Id: HltMatchOffTrack2OnCalo.cpp,v 1.2 2007-07-31 17:18:14 gguerrer Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltMatchOffTrack2OnCalo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltMatchOffTrack2OnCalo
//
// 2007-07-30 : Gabriel Guerrer
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltMatchOffTrack2OnCalo );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMatchOffTrack2OnCalo::HltMatchOffTrack2OnCalo( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITrackMatch>(this);

}


//=============================================================================
// Destructor
//=============================================================================
HltMatchOffTrack2OnCalo::~HltMatchOffTrack2OnCalo() {} 

//=============================================================================
// initialize
//=============================================================================
StatusCode HltMatchOffTrack2OnCalo::initialize()
{
  m_tooltrack2calo = tool<ITrack2Calo>("Track2Calo"); //initialize m_fgfdg =
	if (m_tooltrack2calo)
		return StatusCode::SUCCESS;
	else
		error() << "Unable to get tool Track2Calo" << endreq;
	return StatusCode::FAILURE;
}
//=============================================================================
// match
//=============================================================================

StatusCode HltMatchOffTrack2OnCalo::match(const LHCb::Track& track0, //calo tracks
                   						  					const LHCb::Track& track1, 
                   						 						double& quality)
{

	bool hascaloid = false;
	CaloCellID caloid;
	std::vector<LHCbID> caloids = track0.lhcbIDs();

	
 	//for (std::vector<LHCbID>::iterator it = caloids.begin(); it != caloids.end(); ++it)	{
	//	if ( (*it).isCalo() )	{
	//		caloid = (*it).caloID();
	//		hascaloid = true;
	//		}
	//	}
	
	if (caloids.size()>0) {
		std::vector<LHCbID>::iterator it = caloids.begin(); 
		if ( (*it).isCalo() )	{
				caloid = (*it).caloID();
				hascaloid = true;
				}	
		}				
		
	if ( hascaloid == false )	{
		error() << "Input calotrack has no CaloCellId" << endreq;
		return StatusCode::FAILURE;
		}
	 

	
	
	m_tooltrack2calo->match(&track1, DeCalorimeterLocation::Hcal);
	
	CaloCellID hptoffcentercell = m_tooltrack2calo->caloCellID();
		
	if ( !( hptoffcentercell.row() == 0 && hptoffcentercell.col() == 0 ) )	{

		std::vector<CaloCellID> hptoff3x3cells = HltUtils::get3x3CellIDs( hptoffcentercell );					
		std::vector<CaloCellID>	hpton2x2cells = HltUtils::get2x2CellIDs( caloid );

		if ( HltUtils::matchCellIDs( hpton2x2cells, hptoff3x3cells ) )	{						
			quality = double( track1.key() );
			return StatusCode::SUCCESS;
			}	
	}	
		
	quality = -1. ;	
	return StatusCode::SUCCESS;
}
//=============================================================================

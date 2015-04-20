
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorWithPanelFlip.cpp
 *
 *  Implementation file for tool : Rich::Rec::PixelCreatorWithPanelFlip
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/11/2006
 */
//---------------------------------------------------------------------------------

// local
#include "RichPixelCreatorWithPanelFlip.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//---------------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PixelCreatorWithPanelFlip )

// Standard constructor
PixelCreatorWithPanelFlip::
PixelCreatorWithPanelFlip( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : PixelCreatorFromRawBuffer ( type, name, parent  ),
    m_flipRICH                ( Rich::NRiches, true )
{
  declareProperty ( "FlipRICHes", m_flipRICH );
}

StatusCode PixelCreatorWithPanelFlip::initialize()
{
  // base class initilize
  const StatusCode sc = PixelCreatorFromRawBuffer::initialize();
  if ( sc.isFailure() ) { return sc; }

  info() << "Flipping RICH hits for RICHes(1/2) : " << m_flipRICH << endmsg;

  return sc;
}

LHCb::RichSmartID 
PixelCreatorWithPanelFlip::flip( const LHCb::RichSmartID & id ) const
{
  // make a temp copy of the ID
  LHCb::RichSmartID flipID(id);

  // Flip the appropriate panels
  if      ( Rich::Rich1 == id.rich() && m_flipRICH[Rich::Rich1] )
  {
    flipID.setPanel( Rich::top  == id.panel() ? Rich::bottom : Rich::top  );     
  }
  else if ( Rich::Rich2 == id.rich() && m_flipRICH[Rich::Rich2] )
  {
    flipID.setPanel( Rich::left == id.panel() ? Rich::right  : Rich::left );     
  }

  // return the flipped ID
  return flipID;
}

LHCb::RichRecPixel *
PixelCreatorWithPanelFlip::buildPixel( const Rich::HPDPixelCluster& cluster ) const
{
  // Make new container for flipped RichSmartIDs
  HPDPixelCluster::SmartIDVector flipIDs;
  flipIDs.reserve( cluster.smartIDs().size() );
  
  // Loop over each RichSmartID in this cluster, flip it and save in the new list
  for ( const auto& ID : cluster.smartIDs() )
  { 
    flipIDs.push_back( flip(ID) ); 
  }
  
  // create a new cluster
  const HPDPixelCluster flipCluster(flipIDs);
  
  // reconstruct with the new cluster
  return PixelCreatorFromRawBuffer::buildPixel(flipCluster);
}

LHCb::RichRecPixel *
PixelCreatorWithPanelFlip::buildPixel( const LHCb::RichSmartID & id ) const
{
  // reconstruct with the flipped ID
  return PixelCreatorFromRawBuffer::buildPixel( flip(id) );
}


#include "RichBase.h"
#include "RichPixel.h"
#include "RichPixelReadout.h"
#include "RichRegistry.h"

void RichBase::cleanUp() 
{
  // Delete the pixel map
  // CRJ : Not needed whilst using single properties
  //for ( activemap::iterator i = m_activePixels.begin();
  //      i != m_activePixels.end(); ++i ) {
  //  RichPixelProperties *& ptr = (*i).second;
  //  if ( ptr ) { delete ptr; ptr = NULL; }
  //}
  m_activePixels.clear();
}

void RichBase::upDate( const std::vector<LHCb::RichSmartID> & pixelList ) {

  // CleanUp current map
  cleanUp();
  
  // Initialise new pixel map
  for ( std::vector<LHCb::RichSmartID>::const_iterator intIndex = pixelList.begin();
        intIndex != pixelList.end();
        ++intIndex ) {
    // CRJ : Until pixels have different properties, use the same for each
    //RichPixelProperties* prop = new RichPixelProperties( );
    //m_activePixels.insert( activemap::value_type( *intIndex, prop ) );
    m_activePixels.insert( activemap::value_type( *intIndex, &m_pixProps ) );
  }

  RichRegistry::RegisterMe(this);
  CloseState();

}

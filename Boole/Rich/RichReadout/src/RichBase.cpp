
#include "RichBase.h"
#include "RichPixel.h"
#include "RichPixelReadout.h"
#include "RichRegistry.h"

RichBase::~RichBase() { cleanUp(); }

void RichBase::cleanUp() 
{
  // Delete the pixel map
  for ( activemap::iterator i = m_activePixels.begin();
        i != m_activePixels.end(); ++i ) {
    RichPixelProperties *& ptr = (*i).second;
    if ( ptr ) { delete ptr; ptr = NULL; }
  }
  m_activePixels.clear();
}

void RichBase::upDate( std::vector<RichSmartID> & pixelList ) {

  // CleanUp current map
  cleanUp();
  
  // Initialise new pixel map
  for ( std::vector<RichSmartID>::iterator intIndex = pixelList.begin();
        intIndex != pixelList.end();
        ++intIndex ) {
    // NB - Stripping sub-pixel infortamtion
    RichPixelProperties* prop = new RichPixelProperties( (*intIndex).pixelID() );
    m_activePixels.insert( activemap::value_type( *intIndex, prop ) );
  }

  RichRegistry::RegisterMe(this);
  CloseState();

}


//=============================================================================================
/** @file RichDAQHeaderPD_V4.cpp
 *
 *  Implementation file for RICH DAQ class : RichDAQHeaderV4::RichDAQHeaderPD
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   23/04/2007
 */
//=============================================================================================

// local
#include "RichKernel/RichDAQHeaderPD_V4.h"
#include "GaudiKernel/GaudiException.h"

namespace Rich
{
  namespace DAQ
  {
    namespace RichDAQHeaderV4
    {

      //RichDAQHeaderPD::~RichDAQHeaderPD() {}

      // Read correct number of data words from given stream
      // Note, after this call data pointer is incremented to the next word after the header
      void RichDAQHeaderPD::readFromDataStream( const LongType *& data )
      {
        if ( data )
        {
          // Read the first word (which gives us enough info to know the format)
          headerWords()[0] = *data;
          // If extended mode, read the two L0 words (requires last line)
          if ( UNLIKELY( extendedFormat() ) )
          {
            // Reset header to have 3 words
            headerWords() = HeaderPDBase::HeaderWords(nHeaderWordsExtended,0);
            headerWords()[0] = *(data++);
            headerWords()[1] = *(data++);
            headerWords()[2] = *(data++);
          }
          else
          {
            // Just increment the pointer
            ++data;
          }
        }
        else
        {
          throw GaudiException( "Attempt to read from NULL data stream",
                                "*RichDAQHeaderPD*", StatusCode::FAILURE );
        }
      }

      void RichDAQHeaderPD::makeExtended()
      {
        if ( nHeaderWords() != nHeaderWordsExtended )
        {
          // Save the main (first) word
          const auto tmpH = headerWords()[0];
          // reset the backend vector to have size three
          headerWords() = HeaderPDBase::HeaderWords(nHeaderWordsExtended,0);
          // reset the first word
          headerWords()[0] = tmpH;
        }
      }

    }
  }
}

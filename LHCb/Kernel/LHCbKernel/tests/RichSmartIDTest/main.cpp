
#include <iostream>
#include "Kernel/RichSmartID.h"
#include "Kernel/LHCbID.h"

using namespace std;

int main ( int /*argc*/, char** /*argv*/ )
{

  bool OK = true;

  for ( unsigned int irich = 0; irich <= LHCb::RichSmartID::HPD::MaxRich; ++irich )
  {
    const Rich::DetectorType rich = (Rich::DetectorType)irich;
    for ( unsigned int ipanel = 0; ipanel <= LHCb::RichSmartID::HPD::MaxPanel; ++ipanel )
    {
      const Rich::Side panel = (Rich::Side)ipanel;
      for ( unsigned int pdrow = 0; pdrow <= LHCb::RichSmartID::HPD::MaxPDNumInCol; ++pdrow )
      {
        for ( unsigned int pdcol = 0; pdcol <= LHCb::RichSmartID::HPD::MaxPDCol; ++pdcol )
        {
          for ( unsigned int pixrow = 0; pixrow <= LHCb::RichSmartID::HPD::MaxPixelRow; ++pixrow )
          {
            for ( unsigned int pixcol = 0; pixcol <= LHCb::RichSmartID::HPD::MaxPixelCol; ++pixcol )
            {
              for ( unsigned int subpix = 0; subpix <= LHCb::RichSmartID::HPD::MaxPixelSubRow; ++subpix )
              {

                // Make a test smart ID
                const LHCb::RichSmartID id( rich,panel,
                                            pdrow,pdcol,
                                            pixrow,pixcol,subpix,
                                            LHCb::RichSmartID::HPDID );

                // Check values
                if ( rich   != id.rich()       ||
                     panel  != id.panel()      ||
                     pdrow  != id.pdNumInCol() ||
                     pdcol  != id.pdCol()      ||
                     pixrow != id.pixelRow()   ||
                     pixcol != id.pixelCol()   ||
                     subpix != id.pixelSubRow() )
                {
                  cerr << "Problem with RichSmartID " << id << endl;
                  OK = false;
                }

                // Test conversion to and from an LHCbID
                const LHCb::LHCbID lID( id );
                const LHCb::RichSmartID nid( lID.richID() );
                if ( nid.key() != id.key() )
                {
                  cerr << "Problem with RichSmartID <-> LHCbID conversion" << endl;
                  cerr << "  Original  ID : " << id  << endl;
                  cerr << "  Converted ID : " << nid << endl;
                  OK = false;
                }

              }
            }
          }
        }
      }
    }
  }

  for ( unsigned int irich = 0; irich <= LHCb::RichSmartID::MaPMT::MaxRich; ++irich )
  {
    const Rich::DetectorType rich = (Rich::DetectorType)irich;
    for ( unsigned int ipanel = 0; ipanel <= LHCb::RichSmartID::MaPMT::MaxPanel; ++ipanel )
    {
      const Rich::Side panel = (Rich::Side)ipanel;
      for ( unsigned int pdrow = 0; pdrow <= LHCb::RichSmartID::MaPMT::MaxPDNumInCol; ++pdrow )
      {
        for ( unsigned int pdcol = 0; pdcol <= LHCb::RichSmartID::MaPMT::MaxPDCol; ++pdcol )
        {
          for ( unsigned int pixrow = 0; pixrow <= LHCb::RichSmartID::MaPMT::MaxPixelRow; ++pixrow )
          {
            for ( unsigned int pixcol = 0; pixcol <= LHCb::RichSmartID::MaPMT::MaxPixelCol; ++pixcol )
            {

              // Make a test smart ID
              const LHCb::RichSmartID id( rich,panel,
                                          pdrow,pdcol,
                                          pixrow,pixcol,
                                          LHCb::RichSmartID::MaPMTID );

              // Check values
              if ( rich   != id.rich()       ||
                   panel  != id.panel()      ||
                   pdrow  != id.pdNumInCol() ||
                   pdcol  != id.pdCol()      ||
                   pixrow != id.pixelRow()   ||
                   pixcol != id.pixelCol()    )
              {
                cerr << "Problem with RichSmartID " << id << endl;
                OK = false;
              }

              // Test conversion to and from an LHCbID
              const LHCb::LHCbID lID( id );
              const LHCb::RichSmartID nid( lID.richID() );
              if ( nid.key() != id.key() )
              {
                cerr << "Problem with RichSmartID <-> LHCbID conversion" << endl;
                cerr << "  Original  ID : " << id  << endl;
                cerr << "  Converted ID : " << nid << endl;
                OK = false;
              }

            }
          }
        }
      }
    }
  }

  if ( !OK )
  {
    cerr << "RichSmartID tests FAILED" << endl;
  }
  else
  {
    cout << "RichSmartID tests SUCCESSFUL" << endl;
  }

  return 0;
}

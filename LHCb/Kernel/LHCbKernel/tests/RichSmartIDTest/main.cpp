
// Include files
#include <cmath>
#include <vector>
#include <iostream>
#include <iomanip>
#include <float.h>

#include "Kernel/RichSmartID.h"

using namespace std;

//--- Example main program
int main ( int /*argc*/, char** /*argv*/ )
{

  bool OK = true;

  for ( unsigned int rich=0; rich<2; ++rich )
  {
    for ( unsigned int panel=0; panel<2; ++panel )
    {
      for ( unsigned int pdrow=0; pdrow<LHCb::RichSmartID::HPD::MaxPDNumInCol; ++pdrow )
      {
        for ( unsigned int pdcol=0; pdcol<LHCb::RichSmartID::HPD::MaxPDCol; ++pdcol )
        {
          for ( unsigned int pixrow=0; pixrow<LHCb::RichSmartID::HPD::MaxPixelRow; ++pixrow )
          {
            for ( unsigned int pixcol=0; pixcol<LHCb::RichSmartID::HPD::MaxPixelCol; ++pixcol )
            {

              // Make a test smart ID
              LHCb::RichSmartID id((Rich::DetectorType)rich,(Rich::Side)panel,
                                   pdrow,pdcol,pixrow,pixcol,LHCb::RichSmartID::HPDID);

              // Check values
              if ( (Rich::DetectorType)rich != id.rich()       ||
                   (Rich::Side)panel        != id.panel()      ||
                   pdrow                    != id.pdNumInCol() ||
                   pdcol                    != id.pdCol()      ||
                   pixrow                   != id.pixelRow()   ||
                   pixcol                   != id.pixelCol()    )
              {
                cout << "Problem with RichSmartID " << id << endl;
                OK = false;
              }

            }
          }
        }
      }
    }
  }

  for ( unsigned int rich=0; rich<2; ++rich )
  {
    for ( unsigned int panel=0; panel<2; ++panel )
    {
      for ( unsigned int pdrow=0; pdrow<LHCb::RichSmartID::MaPMT::MaxPDNumInCol; ++pdrow )
      {
        for ( unsigned int pdcol=0; pdcol<LHCb::RichSmartID::MaPMT::MaxPDCol; ++pdcol )
        {
          for ( unsigned int pixrow=0; pixrow<LHCb::RichSmartID::MaPMT::MaxPixelRow; ++pixrow )
          {
            for ( unsigned int pixcol=0; pixcol<LHCb::RichSmartID::MaPMT::MaxPixelCol; ++pixcol )
            {

              // Make a test smart ID
              LHCb::RichSmartID id((Rich::DetectorType)rich,(Rich::Side)panel,
                                   pdrow,pdcol,pixrow,pixcol,LHCb::RichSmartID::MaPMTID);

              // Check values
              if ( (Rich::DetectorType)rich != id.rich()       ||
                   (Rich::Side)panel        != id.panel()      ||
                   pdrow                    != id.pdNumInCol() ||
                   pdcol                    != id.pdCol()      ||
                   pixrow                   != id.pixelRow()   ||
                   pixcol                   != id.pixelCol()    )
              {
                cout << "Problem with RichSmartID " << id << endl;
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
    cout << "RichSmartID tests SUCCESSFULL" << endl;
  }

  return 0;
}

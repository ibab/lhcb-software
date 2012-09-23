#ifndef UTDETECTORPLOT_H 
#define UTDETECTORPLOT_H 1

/** @class UTDetectorPlot UTDetectorPlot.h Kernel/UTDetectorPlot.h
 *
 *  class to plot detector view (ie displays the detector with
 *  one bin per sector) for the IT.
 *
 *  @author Johan Luisier
 *  @author M Needham
 *  @date   2009-07-22
 */

#include "Kernel/STHisto2DProperties.h"
#include <string>

class DeUTSector;

namespace ST
{
  class UTDetectorPlot: public ST::Histo2DProperties {

public:

    struct Bins{
      int  xBin;
      int beginBinY;
      int endBinY;
    };

    UTDetectorPlot(const std::string& name, const std::string& title,
                   const unsigned int xBinsPerSector = 1u);

    ~UTDetectorPlot();
    typedef Bins Bins;
    Bins toBins(const DeUTSector* sector) const;
         
  };
}

#endif // STDETECTORPLOT_H 

#ifndef TTDETECTORPLOT_H 
#define TTDETECTORPLOT_H 1

/** @class TTDetectorPlot TTDetectorPlot.h Kernel/TTDetectorPlot.h
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

class DeTTSector;

namespace ST
{
  class TTDetectorPlot: public ST::Histo2DProperties {

public:

    struct Bins{
      int  xBin;
      int beginBinY;
      int endBinY;
    };

    TTDetectorPlot(const std::string& name, const std::string& title,
                   const unsigned int xBinsPerSector = 1u);

    ~TTDetectorPlot();
    typedef Bins Bins;
    Bins toBins(const DeTTSector* sector) const;
         
  };
};

#endif // STDETECTORPLOT_H 

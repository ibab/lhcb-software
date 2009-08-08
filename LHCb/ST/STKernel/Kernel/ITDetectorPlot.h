#ifndef ITDETECTORPLOT_H 
#define ITDETECTORPLOT_H 1

/** @class ITDetectorPlot ITDetectorPlot.h Kernel/ITDetectorPlot.h
 *
 *  class to plot detector view (ie displays the detector with
 *  one bin per sector) for the IT.
 *
 *  @author Johan Luisier
 *  @author M Needham
 *  @date   2009-07-22
 */

#include "Kernel/STHisto2DProperties.h"
#include "Kernel/STChannelID.h"
#include <string>


namespace ST
{
  class ITDetectorPlot: public ST::Histo2DProperties {

public:

    typedef struct {
      int xBin;
      int yBin;
    } Bins;

    ITDetectorPlot(const std::string& name, const std::string& title);

    ~ITDetectorPlot();

    Bins toBins(const LHCb::STChannelID& chan) const;
         
  };
};

#endif // STDETECTORPLOT_H 

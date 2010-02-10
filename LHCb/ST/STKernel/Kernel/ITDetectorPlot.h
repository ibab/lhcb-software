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
 *  Modified by Mark Tobin so that the number of bins per sector can be
 *  given as an input where 3 (12) bins displays per Beetle (Port)
 */

#include "Kernel/STHisto2DProperties.h"
#include "Kernel/STChannelID.h"
#include <string>


namespace ST
{
  class ITDetectorPlot: public ST::Histo2DProperties {

public:

    typedef struct {
      double xBin;
      int yBin;
    } Bins;

    ITDetectorPlot(const std::string& name, const std::string& title,
                   const unsigned int xBinsPerSector = 1u);

    ~ITDetectorPlot();

    Bins toBins(const LHCb::STChannelID& chan) const;
         
  private:
    bool m_plotByPort;
    bool m_plotByBeetle;
    int m_xBinsPerSector;

  };
};

#endif // STDETECTORPLOT_H 

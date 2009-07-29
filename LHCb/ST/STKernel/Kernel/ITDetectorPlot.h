#ifndef STDETECTORPLOT_H 
#define STDETECTORPLOT_H 1

/** @namespace STDetectorPlot STDetectorPlot.h Kernel/STDetectorPlot.h
 *
 *  Namespace to plot detector view (ie displays the detector with
 *  one bin per sector).
 *
 *  @author Johan Luisier
 *  @date   2009-07-22
 */

#include "Kernel/STChannelID.h"
#include <string>

namespace ST
{
  class ITDetectorHistoProperties
    {
    public:
      ITDetectorHistoProperties(const std::string& detType);
      ~ITDetectorHistoProperties();
      bool setChannelID(const LHCb::STChannelID& chanID);
      const double& xBin() const;
      const double& yBin() const;
      const double& minBinX() const;
      const double& minBinY() const;
      const double& maxBinX() const;
      const double& maxBinY() const;
      const unsigned int& nBinX() const;
      const unsigned int& nBinY() const;
      std::string histoTitle() const;
    private:
      double m_xMin, m_xMax, m_yMin, m_yMax, m_xBin, m_yBin;
      unsigned int m_nBinX, m_nBinY;
      std::string m_detType;
      LHCb::STChannelID m_channelID;
    };

};

#endif // STDETECTORPLOT_H 

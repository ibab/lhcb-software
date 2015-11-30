#ifndef RichChannelParamsPMT_h
#define RichChannelParamsPMT_h 1

#include "RichPixelReadout.h"

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      class RichPixel;

      /// General pixel properties
      class RichChannelParamsPMT
      {

      public:

        RichChannelParamsPMT()
        { }

        ~RichChannelParamsPMT()
        {
        }

	// GET
        double pixelMeanGain() 
	{
	  return m_pixelMeanGain;
	}

        double pixelSigmaGain() 
	{
	  return m_pixelSigmaGain;
	}

        double pixelSigmaNoise() 
	{
	  return m_pixelSigmaNoise;
	}

        double channelThreshold() 
	{
	  return m_channelThreshold;
	}

	// SET
        void setPixelMeanGain(double val) 
	{
	  m_pixelMeanGain = val;
	}

        void setPixelSigmaGain(double val) 
	{
	  m_pixelSigmaGain = val;
	}
        void setPixelSigmaNoise(double val) 
	{
	  m_pixelSigmaNoise = val;
	}

        void setChannelThreshold(double val) 
	{
	  m_channelThreshold = val;
	}

      private:

	// Parameters
        double m_pixelMeanGain;
        double m_pixelSigmaGain;
        double m_pixelSigmaNoise;
        double m_channelThreshold;


      };

    }
  }
}

#endif

#ifndef COMPONENT_RICHCHANNELPROPERTIESPMT_H 
#define COMPONENT_RICHCHANNELPROPERTIESPMT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Interfaces                                                                              
//#include "RichKernel/IRichSmartIDTool.h"

// Kernel                                                                                  
#include "Kernel/RichSmartID.h"
//#include "Kernel/RichDetectorType.h"

#include <map>
#include "RichChannelParamsPMT.h"

static const InterfaceID IID_RichChannelPropertiesPMT ( "RichChannelPropertiesPMT", 1, 0 );

      
      /** @class RichChannelPropertiesPMT RichChannelPropertiesPMT.h component/RichChannelPropertiesPMT.h
       *  
       * 
       *  @author Mariusz Witek
       *  @date   2015-08-19
       */
      class RichChannelPropertiesPMT : public GaudiTool {
      public: 
      
        // Return the interface ID
        static const InterfaceID& interfaceID() { return IID_RichChannelPropertiesPMT; }
      
        /// Standard constructor
        RichChannelPropertiesPMT( const std::string& type, 
                                  const std::string& name,
                                  const IInterface* parent);
      
        virtual ~RichChannelPropertiesPMT( ); ///< Destructor
      
        StatusCode  ReadChannelParameters(const LHCb::RichSmartID::Vector & pixels);

        double pixelMeanGain(const LHCb::RichSmartID);
        double pixelSigmaGain(const LHCb::RichSmartID);
        double pixelSigmaNoise(const LHCb::RichSmartID);
        double channelThreshold(const LHCb::RichSmartID);
      
      protected:
      
      private:
      
        void generateParameters(const LHCb::RichSmartID::Vector & pixels);
	void setParametersFromDB(const LHCb::RichSmartID::Vector & pixels);
        // for the moment the same parameters for each pixel (later introduce variations)
        // unit of all parameters in 10**6 electrons
        double m_meanGainGen;
        double m_sigmaGainGen;
        double m_sigmaNoiseGen;
        double m_channelThresholdGen;
      
        bool  m_setParametersFromDB;
      
        // write class with enums eMeanGain, eSigmaGain, eSigmaNoise, eThreshold,...
	std::map<LHCb::RichSmartID, Rich::MC::Digi::RichChannelParamsPMT> m_tabChannelParemeters;
      
};

#endif // COMPONENT_RICHCHANNELPROPERTIESPMT_H

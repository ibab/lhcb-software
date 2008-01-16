// $Id: ConversionFunctions.h,v 1.1 2008-01-16 12:31:46 wouter Exp $
#ifndef CONVERSIONFUNCTIONS_H 
#define CONVERSIONFUNCTIONS_H 1

// Include files

/** @class OTRawBankConverter OTRawBankConverter.h
 *  
 *
 *  @author Wouter HULSBERGEN
 *  @date   2007-11-23
 */

namespace OTDAQ
{
  namespace DC06 {
    
    inline unsigned int channelToStraw(unsigned int otis, unsigned int channel)
    {
      return otis/2 ? 32 * (6-otis) - channel : 32 * otis + channel + 1 ;
    }

    inline void strawToChannel(unsigned int straw, unsigned int& otis, unsigned int& channel)
    {
      otis   = (straw-1)/32 ;
      channel = (straw-1)%32 ;
      if(otis>2) {
	otis    = 5-otis ;
	channel = 31-channel ;
      }
    }
  }

  namespace V3 {
    
    inline unsigned int  channelToStraw(unsigned int otis, unsigned int channel, unsigned int layer)
    {
      unsigned int tmpstraw = DC06::channelToStraw(otis,channel) ;
      return layer%2 ? tmpstraw : 129 - tmpstraw ;
    }
    
    inline void strawToChannel(unsigned int straw, unsigned int layer,
			       unsigned int& otis, unsigned int& channel)
    {
      unsigned int tmpstraw =  layer%2 ? straw : 129 - straw ;
      DC06::strawToChannel(tmpstraw,otis,channel) ;
    }
  }
}

#endif

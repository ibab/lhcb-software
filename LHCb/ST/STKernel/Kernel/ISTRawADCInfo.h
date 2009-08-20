// $Id: ISTRawADCInfo.h,v 1.1 2009-08-20 07:58:46 mneedham Exp $
// ============================================================================
#ifndef STKERNEL_ISTRAWADCINFO_H 
#define STKERNEL_ISTRAWADCINFO 1
// Include files
// STD & STL 
#include <string>
#include <vector>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// forward declaration
namespace LHCb{
  class  STChannelID;    
};
static const InterfaceID IID_ISTRawADCInfo ( "ISTRawADCInfo" , 1 , 0 );

/** @class ISTRawADCInfo ISTRawADCInfo.h
 *  
 *  An abstract interface for linking channels to RAW info 
 *
 *
 *  @author M Needham
 *  @date   08/08/2009
 */

class ISTRawADCInfo :
  virtual public IAlgTool {
public:
  
  /** static interface identification
   *  @see IInterface
   *  @see IID_ISTRawADCInfo
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_ISTRawADCInfo; }

  /** link to RAW adc info 
  * @param LHCb::STChannelID chan  
  * @param adc value
  * @return StatusCode   
  */
  virtual StatusCode link(const LHCb::STChannelID& chan, unsigned int& value) const =0;

  /** link to RAW adc info for a list of channels
  * @param std::vector<LHCb::STChannelID> chan  
  * @param adc value
  * @return StatusCode   
  */
  virtual StatusCode link(const std::vector<LHCb::STChannelID>& chans, 
                  std::vector<unsigned int>& values) const =0;

  /** link to RAW adc info for a list of channels in a window
  * @param std::vector<LHCb::STChannelID> chan  
  * @param adc value
  * @return StatusCode   
  */
  virtual StatusCode link(const LHCb::STChannelID& chan, 
                  const unsigned int window,
                  std::vector<unsigned int>& values) const = 0;

};
  
// ============================================================================
#endif // ST_ISTRawADCInfo
// ============================================================================

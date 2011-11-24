// $Id: $
#ifndef RICHDET_RICHDETCONFIGTYPE_H
#define RICHDET_RICHDETCONFIGTYPE_H 1

#include <string>

namespace Rich
{

  /** @enum Rich::RichPhDetConfigType
   *
   *  RICH photon detector type
   *
   *  @author Sajan Easo
   *  @date   17/11/2011
   */
  enum RichPhDetConfigType
    {
      InvalidPDConfig = -1, ///< Invalid PD config
      HPDConfig       = 0,  ///< HPD photodetectors
      PMTConfig       = 1   ///< PMT photodetectors
    };

  /** Text conversion for RichPhDetConfigType enumeration
   *
   *  @param conf RICH photon detector type
   *  @return RICH photon detector type as a string
   */
  std::string text( const Rich::RichPhDetConfigType conf );

}

#endif // RICHDET_RICHDETCONFIGTYPE_H

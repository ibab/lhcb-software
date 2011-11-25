
//-----------------------------------------------------------------------------
/** @file RichDAQVersions.h
 *
 *  Contains simple typedefs for the types to use for each LHCb L1 version
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   06/09/2006
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHDAQVERSIONS_H
#define RICHDAQ_RICHDAQVERSIONS_H 1

// From RichKernel
#include "RichKernel/RichDAQHeaderPD.h"
#include "RichKernel/RichDAQNullFooter.h"
#include "RichKernel/RichDAQParityFooter.h"
#include "RichKernel/RichDAQParityFooterTB2006.h"
#include "RichKernel/RichDAQL1IngressHeader.h"

// data blocks
#include "RichNonZeroSuppData.h"
#include "RichNonZeroSuppALICEData.h"
#include "RichZeroSuppData.h"
#include "RichZeroSuppALICEData.h"
#include "RichFlatFormatSmartIDData.h"

namespace Rich
{
  namespace DAQ
  {

    /// Data types for BankVersion::LHCb1
    namespace RichDAQ_LHCb1
    {
      /// Tag class
      class LHCb1 {};
      /// Header version
      typedef RichDAQHeaderV1::RichDAQHeaderPD                                  Header;
      /// Footer version
      typedef NullFooter                                                        Footer;
      /// Non zero suppressed LHCb mode data
      typedef RichNonZeroSuppDataV1::RichNonZeroSuppData<LHCb1,Header,Footer>   NonZeroSuppLHCb;
      /// Zero suppressed LHCb mode data
      typedef RichZeroSuppDataV1::RichZeroSuppData<LHCb1,Header,Footer>         ZeroSuppLHCb;
    }

    /// Data types for BankVersion::LHCb2
    namespace RichDAQ_LHCb2
    {
      /// Tag class
      class LHCb2 {};
      /// Header version
      typedef RichDAQHeaderV1::RichDAQHeaderPD                                  Header;
      /// Footer version
      typedef NullFooter                                                        Footer;
      /// Non zero suppressed LHCb mode data
      typedef RichNonZeroSuppDataV1::RichNonZeroSuppData<LHCb2,Header,Footer>   NonZeroSuppLHCb;
      /// Zero suppressed LHCb mode data
      typedef RichZeroSuppDataV2::RichZeroSuppData<LHCb2,Header,Footer>         ZeroSuppLHCb;
    }

    /// Data types for BankVersion::LHCb3
    namespace RichDAQ_LHCb3
    {
      /// Tag class
      class LHCb3 {};
      /// Header version
      typedef RichDAQHeaderV2::RichDAQHeaderPD                                          Header;
      /// Footer version
      typedef NullFooter                                                                Footer;
      /// Non zero suppressed Alice mode data
      typedef RichNonZeroSuppALICEDataV1::RichNonZeroSuppALICEData<LHCb3,Header,Footer> NonZeroSuppAlice;
      /// Non zero suppressed LHCb mode data
      typedef RichNonZeroSuppDataV2::RichNonZeroSuppData<LHCb3,Header,Footer>           NonZeroSuppLHCb;
      /// Zero suppressed LHCb mode data
      typedef RichZeroSuppDataV3::RichZeroSuppData<LHCb3,Header,Footer>                 ZeroSuppLHCb;
    }

    /// Data types for BankVersion::LHCb4
    namespace RichDAQ_LHCb4
    {
      /// Tag class
      class LHCb4 {};
      /// Header version
      typedef RichDAQHeaderV3::RichDAQHeaderPD                                          Header;
      /// Footer version
      typedef ParityFooterTB2006                                                        Footer;
      /// Non zero suppressed Alice mode data
      typedef RichNonZeroSuppALICEDataV1::RichNonZeroSuppALICEData<LHCb4,Header,Footer> NonZeroSuppAlice;
      /// Non zero suppressed LHCb mode data
      typedef RichNonZeroSuppDataV2::RichNonZeroSuppData<LHCb4,Header,Footer>           NonZeroSuppLHCb;
      /// Zero suppressed LHCb mode data
      typedef RichZeroSuppDataV3::RichZeroSuppData<LHCb4,Header,Footer>                 ZeroSuppLHCb;
    }

    /// Data types for BankVersion::LHCb5
    namespace RichDAQ_LHCb5
    {
      /// Tag class
      class LHCb5 {};
      /// Header version
      typedef RichDAQHeaderV4::RichDAQHeaderPD                                          Header;
      /// Footer version
      typedef ParityFooter                                                              Footer;
      /// Non zero suppressed Alice mode data
      typedef RichNonZeroSuppALICEDataV2::RichNonZeroSuppALICEData<LHCb5,Header,Footer> NonZeroSuppAlice;
      /// Non zero suppressed LHCb mode data
      typedef RichNonZeroSuppDataV3::RichNonZeroSuppData<LHCb5,Header,Footer>           NonZeroSuppLHCb;
      /// Zero suppressed LHCb mode data
      typedef RichZeroSuppDataV4::RichZeroSuppData<LHCb5,Header,Footer>                 ZeroSuppLHCb;
      /// Zero suppressed Alice mode data
      typedef RichZeroSuppALICEDataV1::RichZeroSuppALICEData<LHCb5,Header,Footer>       ZeroSuppAlice;
    }

    /// Data types for BankVersion::FlatList
    namespace RichDAQ_FlatList
    {
      /// Tag class
      class FlatList {};
      /// Header version
      typedef RichDAQHeaderV4::RichDAQHeaderPD                                          Header;
      /// Footer version
      typedef ParityFooter                                                              Footer;
      /// Data Format
      typedef RichFlatFormatSmartIDData<FlatList,Header,Footer>                         Data;
    }

  }
}

#endif // RICHDAQ_RICHDAQVERSIONS_H

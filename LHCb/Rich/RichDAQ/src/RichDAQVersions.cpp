
//-----------------------------------------------------------------------------
/** @file RichDAQVersions.cpp
 *
 *  Instantiates specific versions of templated types for Rich DAQ software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

#include "RichDAQVersions.h"

#include "RichHPDDataBank.icpp"

// ALICE Mode formats

#include "RichNonZeroSuppALICEData_V1.icpp"
#include "RichNonZeroSuppALICEData_V2.icpp"

#include "RichZeroSuppALICEData_V1.icpp"

// LHCb mode format

#include "RichNonZeroSuppData_V1.icpp"
#include "RichNonZeroSuppData_V2.icpp"
#include "RichNonZeroSuppData_V3.icpp"

#include "RichZeroSuppData_V1.icpp"
#include "RichZeroSuppData_V2.icpp"
#include "RichZeroSuppData_V3.icpp"
#include "RichZeroSuppData_V4.icpp"

// Flat List format

#include "RichFlatFormatSmartIDData.icpp"

namespace Rich {
  namespace DAQ {
    
// LHCb1 types
template class HPDDataBankImp<RichDAQ_LHCb1::LHCb1,RichDAQ_LHCb1::Header,RichDAQ_LHCb1::Footer> ;
template class RichNonZeroSuppDataV1::RichNonZeroSuppData<RichDAQ_LHCb1::LHCb1,RichDAQ_LHCb1::Header,RichDAQ_LHCb1::Footer>;
template class RichZeroSuppDataV1::RichZeroSuppData<RichDAQ_LHCb1::LHCb1,RichDAQ_LHCb1::Header,RichDAQ_LHCb1::Footer>;

// LHCb2 types
template class HPDDataBankImp<RichDAQ_LHCb2::LHCb2,RichDAQ_LHCb2::Header,RichDAQ_LHCb2::Footer> ;
template class RichNonZeroSuppDataV1::RichNonZeroSuppData<RichDAQ_LHCb2::LHCb2,RichDAQ_LHCb2::Header,RichDAQ_LHCb2::Footer>;
template class RichZeroSuppDataV2::RichZeroSuppData<RichDAQ_LHCb2::LHCb2,RichDAQ_LHCb2::Header,RichDAQ_LHCb2::Footer>;

// LHCb3 types
template class HPDDataBankImp<RichDAQ_LHCb3::LHCb3,RichDAQ_LHCb3::Header,RichDAQ_LHCb3::Footer> ;
template class RichNonZeroSuppALICEDataV1::RichNonZeroSuppALICEData<RichDAQ_LHCb3::LHCb3,RichDAQ_LHCb3::Header,RichDAQ_LHCb3::Footer>;
template class RichNonZeroSuppDataV2::RichNonZeroSuppData<RichDAQ_LHCb3::LHCb3,RichDAQ_LHCb3::Header,RichDAQ_LHCb3::Footer>;
template class RichZeroSuppDataV3::RichZeroSuppData<RichDAQ_LHCb3::LHCb3,RichDAQ_LHCb3::Header,RichDAQ_LHCb3::Footer>;

// LHCb4 types
template class HPDDataBankImp<RichDAQ_LHCb4::LHCb4,RichDAQ_LHCb4::Header,RichDAQ_LHCb4::Footer> ;
template class RichNonZeroSuppALICEDataV1::RichNonZeroSuppALICEData<RichDAQ_LHCb4::LHCb4,RichDAQ_LHCb4::Header,RichDAQ_LHCb4::Footer>;
template class RichNonZeroSuppDataV2::RichNonZeroSuppData<RichDAQ_LHCb4::LHCb4,RichDAQ_LHCb4::Header,RichDAQ_LHCb4::Footer>;
template class RichZeroSuppDataV3::RichZeroSuppData<RichDAQ_LHCb4::LHCb4,RichDAQ_LHCb4::Header,RichDAQ_LHCb4::Footer>;

// LHCb5 types
template class HPDDataBankImp<RichDAQ_LHCb5::LHCb5,RichDAQ_LHCb5::Header,RichDAQ_LHCb5::Footer> ;
template class RichNonZeroSuppALICEDataV2::RichNonZeroSuppALICEData<RichDAQ_LHCb5::LHCb5,RichDAQ_LHCb5::Header,RichDAQ_LHCb5::Footer>;
template class RichNonZeroSuppDataV3::RichNonZeroSuppData<RichDAQ_LHCb5::LHCb5,RichDAQ_LHCb5::Header,RichDAQ_LHCb5::Footer>;
template class RichZeroSuppALICEDataV1::RichZeroSuppALICEData<RichDAQ_LHCb5::LHCb5,RichDAQ_LHCb5::Header,RichDAQ_LHCb5::Footer>;
template class RichZeroSuppDataV4::RichZeroSuppData<RichDAQ_LHCb5::LHCb5,RichDAQ_LHCb5::Header,RichDAQ_LHCb5::Footer>;

// FlatList formats
template class HPDDataBankImp<RichDAQ_FlatList::FlatList,RichDAQ_FlatList::Header,RichDAQ_FlatList::Footer> ;
template class RichFlatFormatSmartIDData<RichDAQ_FlatList::FlatList,RichDAQ_FlatList::Header,RichDAQ_FlatList::Footer>;

  } // namespace DAQ
} // namespace Rich

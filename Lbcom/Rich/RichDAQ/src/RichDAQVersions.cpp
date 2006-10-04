
//-----------------------------------------------------------------------------
/** @file RichDAQVersions.cpp
 *
 *  Instantiates specific versions of templated types
 *
 *  $Id: RichDAQVersions.cpp,v 1.4 2006-10-04 16:21:46 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-17
 */
//-----------------------------------------------------------------------------

#include "RichDAQVersions.h"

#include "RichHPDDataBank.icpp"

#include "RichNonZeroSuppALICEData_V1.icpp"

#include "RichNonZeroSuppData_V1.icpp"
#include "RichNonZeroSuppData_V2.icpp"

#include "RichZeroSuppData_V1.icpp"
#include "RichZeroSuppData_V2.icpp"
#include "RichZeroSuppData_V3.icpp"

// LHCb1 types
template class RichHPDDataBankImp<RichDAQ_LHCb1::LHCb1,RichDAQ_LHCb1::Header,RichDAQ_LHCb1::Footer> ;
//template class RichDAQ_LHCb1::NonZeroSuppLHCb ;
//template class RichDAQ_LHCb1::ZeroSuppLHCb ;
template class RichNonZeroSuppDataV1::RichNonZeroSuppData<RichDAQ_LHCb1::LHCb1,RichDAQ_LHCb1::Header,RichDAQ_LHCb1::Footer>;
template class RichZeroSuppDataV1::RichZeroSuppData<RichDAQ_LHCb1::LHCb1,RichDAQ_LHCb1::Header,RichDAQ_LHCb1::Footer>;

// LHCb2 types
template class RichHPDDataBankImp<RichDAQ_LHCb2::LHCb2,RichDAQ_LHCb2::Header,RichDAQ_LHCb2::Footer> ;
//template class RichDAQ_LHCb2::NonZeroSuppLHCb ;
//template class RichDAQ_LHCb2::ZeroSuppLHCb ;
template class RichNonZeroSuppDataV1::RichNonZeroSuppData<RichDAQ_LHCb2::LHCb2,RichDAQ_LHCb2::Header,RichDAQ_LHCb2::Footer>;
template class RichZeroSuppDataV2::RichZeroSuppData<RichDAQ_LHCb2::LHCb2,RichDAQ_LHCb2::Header,RichDAQ_LHCb2::Footer>;

// LHCb3 types
template class RichHPDDataBankImp<RichDAQ_LHCb3::LHCb3,RichDAQ_LHCb3::Header,RichDAQ_LHCb3::Footer> ;
//template class RichDAQ_LHCb3::NonZeroSuppAlice ;
//template class RichDAQ_LHCb3::NonZeroSuppLHCb ;
//template class RichDAQ_LHCb3::ZeroSuppLHCb ;
template class RichNonZeroSuppALICEDataV1::RichNonZeroSuppALICEData<RichDAQ_LHCb3::LHCb3,RichDAQ_LHCb3::Header,RichDAQ_LHCb3::Footer>;
template class RichNonZeroSuppDataV2::RichNonZeroSuppData<RichDAQ_LHCb3::LHCb3,RichDAQ_LHCb3::Header,RichDAQ_LHCb3::Footer>;
template class RichZeroSuppDataV3::RichZeroSuppData<RichDAQ_LHCb3::LHCb3,RichDAQ_LHCb3::Header,RichDAQ_LHCb3::Footer>;

// LHCb4 types
template class RichHPDDataBankImp<RichDAQ_LHCb4::LHCb4,RichDAQ_LHCb4::Header,RichDAQ_LHCb4::Footer> ;
//template class RichDAQ_LHCb4::NonZeroSuppAlice ;
//template class RichDAQ_LHCb4::NonZeroSuppLHCb ;
//template class RichDAQ_LHCb4::ZeroSuppLHCb ;
template class RichNonZeroSuppALICEDataV1::RichNonZeroSuppALICEData<RichDAQ_LHCb4::LHCb4,RichDAQ_LHCb4::Header,RichDAQ_LHCb4::Footer>;
template class RichNonZeroSuppDataV2::RichNonZeroSuppData<RichDAQ_LHCb4::LHCb4,RichDAQ_LHCb4::Header,RichDAQ_LHCb4::Footer>;
template class RichZeroSuppDataV3::RichZeroSuppData<RichDAQ_LHCb4::LHCb4,RichDAQ_LHCb4::Header,RichDAQ_LHCb4::Footer>;

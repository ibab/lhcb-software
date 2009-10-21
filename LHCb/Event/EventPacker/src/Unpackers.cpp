// $Id: Unpackers.cpp,v 1.1 2009-10-21 16:41:15 jonrob Exp $

#include "UnpackerBaseAlg.icpp"

#include "Event/PackedMCRichHit.h"
#include "Event/PackedMCRichOpticalPhoton.h"
#include "Event/PackedMCRichSegment.h"
#include "Event/PackedMCRichTrack.h"
#include "Event/PackedMCCaloHit.h"
#include "Event/PackedMCHit.h"
#include "Event/PackedMCRichDigitSummary.h"

template class DataPacking::Unpack<LHCb::MCRichHitPacker>;
typedef DataPacking::Unpack<LHCb::MCRichHitPacker> A;
DECLARE_ALGORITHM_FACTORY( A );

template class DataPacking::Unpack<LHCb::MCRichOpticalPhotonPacker>;
typedef DataPacking::Unpack<LHCb::MCRichOpticalPhotonPacker> B;
DECLARE_ALGORITHM_FACTORY( B );

template class DataPacking::Unpack<LHCb::MCRichSegmentPacker>;
typedef DataPacking::Unpack<LHCb::MCRichSegmentPacker> C;
DECLARE_ALGORITHM_FACTORY( C );

template class DataPacking::Unpack<LHCb::MCRichTrackPacker>;
typedef DataPacking::Unpack<LHCb::MCRichTrackPacker> D;
DECLARE_ALGORITHM_FACTORY( D );

template class DataPacking::Unpack<LHCb::MCPrsHitPacker>;
typedef DataPacking::Unpack<LHCb::MCPrsHitPacker> Ea;
DECLARE_ALGORITHM_FACTORY( Ea );

template class DataPacking::Unpack<LHCb::MCSpdHitPacker>;
typedef DataPacking::Unpack<LHCb::MCSpdHitPacker> Eb;
DECLARE_ALGORITHM_FACTORY( Eb );

template class DataPacking::Unpack<LHCb::MCEcalHitPacker>;
typedef DataPacking::Unpack<LHCb::MCEcalHitPacker> Ec;
DECLARE_ALGORITHM_FACTORY( Ec );

template class DataPacking::Unpack<LHCb::MCHcalHitPacker>;
typedef DataPacking::Unpack<LHCb::MCHcalHitPacker> Ed;
DECLARE_ALGORITHM_FACTORY( Ed );

template class DataPacking::Unpack<LHCb::MCVeloHitPacker>;
typedef DataPacking::Unpack<LHCb::MCVeloHitPacker> F;
DECLARE_ALGORITHM_FACTORY( F );

template class DataPacking::Unpack<LHCb::MCPuVeloHitPacker>;
typedef DataPacking::Unpack<LHCb::MCPuVeloHitPacker> G;
DECLARE_ALGORITHM_FACTORY( G );

template class DataPacking::Unpack<LHCb::MCTTHitPacker>;
typedef DataPacking::Unpack<LHCb::MCTTHitPacker> H;
DECLARE_ALGORITHM_FACTORY( H );

template class DataPacking::Unpack<LHCb::MCITHitPacker>;
typedef DataPacking::Unpack<LHCb::MCITHitPacker> I;
DECLARE_ALGORITHM_FACTORY( I );

template class DataPacking::Unpack<LHCb::MCOTHitPacker>;
typedef DataPacking::Unpack<LHCb::MCOTHitPacker> J;
DECLARE_ALGORITHM_FACTORY( J );

template class DataPacking::Unpack<LHCb::MCMuonHitPacker>;
typedef DataPacking::Unpack<LHCb::MCMuonHitPacker> K;
DECLARE_ALGORITHM_FACTORY( K );

template class DataPacking::Unpack<LHCb::MCRichDigitSummaryPacker>;
typedef DataPacking::Unpack<LHCb::MCRichDigitSummaryPacker> L;
DECLARE_ALGORITHM_FACTORY( L );

// $Id: Packers.cpp,v 1.4 2009-11-04 17:13:18 jonrob Exp $

#include "PackerBaseAlg.icpp"

#include "Event/PackedMCRichHit.h"
#include "Event/PackedMCRichOpticalPhoton.h"
#include "Event/PackedMCRichSegment.h"
#include "Event/PackedMCRichTrack.h"
#include "Event/PackedMCCaloHit.h"
#include "Event/PackedMCHit.h"
#include "Event/PackedMCRichDigitSummary.h"
#include "Event/PackedMuonPID.h"
#include "Event/PackedRichPID.h"

template class DataPacking::Pack<LHCb::MCRichHitPacker>;
typedef DataPacking::Pack<LHCb::MCRichHitPacker> A;
DECLARE_ALGORITHM_FACTORY( A );

template class DataPacking::Pack<LHCb::MCRichOpticalPhotonPacker>;
typedef DataPacking::Pack<LHCb::MCRichOpticalPhotonPacker> B;
DECLARE_ALGORITHM_FACTORY( B );

template class DataPacking::Pack<LHCb::MCRichSegmentPacker>;
typedef DataPacking::Pack<LHCb::MCRichSegmentPacker> C;
DECLARE_ALGORITHM_FACTORY( C );

template class DataPacking::Pack<LHCb::MCRichTrackPacker>;
typedef DataPacking::Pack<LHCb::MCRichTrackPacker> D;
DECLARE_ALGORITHM_FACTORY( D );

template class DataPacking::Pack<LHCb::MCPrsHitPacker>;
typedef DataPacking::Pack<LHCb::MCPrsHitPacker> Ea;
DECLARE_ALGORITHM_FACTORY( Ea );

template class DataPacking::Pack<LHCb::MCSpdHitPacker>;
typedef DataPacking::Pack<LHCb::MCSpdHitPacker> Eb;
DECLARE_ALGORITHM_FACTORY( Eb );

template class DataPacking::Pack<LHCb::MCEcalHitPacker>;
typedef DataPacking::Pack<LHCb::MCEcalHitPacker> Ec;
DECLARE_ALGORITHM_FACTORY( Ec );

template class DataPacking::Pack<LHCb::MCHcalHitPacker>;
typedef DataPacking::Pack<LHCb::MCHcalHitPacker> Ed;
DECLARE_ALGORITHM_FACTORY( Ed );

template class DataPacking::Pack<LHCb::MCVeloHitPacker>;
typedef DataPacking::Pack<LHCb::MCVeloHitPacker> F;
DECLARE_ALGORITHM_FACTORY( F );

template class DataPacking::Pack<LHCb::MCVeloPixHitPacker>;
typedef DataPacking::Pack<LHCb::MCVeloPixHitPacker> Fp;
DECLARE_ALGORITHM_FACTORY( Fp );

template class DataPacking::Pack<LHCb::MCPuVetoHitPacker>;
typedef DataPacking::Pack<LHCb::MCPuVetoHitPacker> G;
DECLARE_ALGORITHM_FACTORY( G );

template class DataPacking::Pack<LHCb::MCTTHitPacker>;
typedef DataPacking::Pack<LHCb::MCTTHitPacker> H;
DECLARE_ALGORITHM_FACTORY( H );

template class DataPacking::Pack<LHCb::MCITHitPacker>;
typedef DataPacking::Pack<LHCb::MCITHitPacker> I;
DECLARE_ALGORITHM_FACTORY( I );

template class DataPacking::Pack<LHCb::MCOTHitPacker>;
typedef DataPacking::Pack<LHCb::MCOTHitPacker> J;
DECLARE_ALGORITHM_FACTORY( J );

template class DataPacking::Pack<LHCb::MCMuonHitPacker>;
typedef DataPacking::Pack<LHCb::MCMuonHitPacker> K;
DECLARE_ALGORITHM_FACTORY( K );

template class DataPacking::Pack<LHCb::MCRichDigitSummaryPacker>;
typedef DataPacking::Pack<LHCb::MCRichDigitSummaryPacker> L;
DECLARE_ALGORITHM_FACTORY( L );

template class DataPacking::Pack<LHCb::RichPIDPacker>;
typedef DataPacking::Pack<LHCb::RichPIDPacker> M;
DECLARE_ALGORITHM_FACTORY( M );

template class DataPacking::Pack<LHCb::MuonPIDPacker>;
typedef DataPacking::Pack<LHCb::MuonPIDPacker> N;
DECLARE_ALGORITHM_FACTORY( N );

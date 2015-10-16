// $Id: PackedEventDict.h,v 1.6 2010-05-18 09:04:35 jonrob Exp $
#ifndef DICT_PACKEDEVENTDICT_H 
#define DICT_PACKEDEVENTDICT_H 1

// Include files for LCG dictionary
#include "Event/PackedMCParticle.h"
#include "Event/PackedMCVertex.h"
#include "Event/PackedTrack.h"
#include "Event/PackedCaloHypo.h"
#include "Event/PackedProtoParticle.h"
#include "Event/PackedRecVertex.h"
#include "Event/PackedTwoProngVertex.h"
#include "Event/PackedMCHit.h"
#include "Event/PackedMCCaloHit.h"
#include "Event/PackedMCRichHit.h"
#include "Event/PackedMCRichOpticalPhoton.h"
#include "Event/PackedMCRichSegment.h"
#include "Event/PackedMCRichTrack.h"
#include "Event/PackedMCRichDigitSummary.h"
#include "Event/PackedRichPID.h"
#include "Event/PackedMuonPID.h"
#include "Event/PackedParticle.h"
#include "Event/PackedVertex.h"
#include "Event/PackedWeightsVector.h"
#include "Event/PackedDecReport.h"
#include "Event/PackedRelations.h"
#include "Event/PackedParticle2Ints.h"
#include "Event/PackedCluster.h"
#include "Event/PackedCaloCluster.h"
#include "Event/PackedFlavourTag.h"
#include "Event/PackedPartToRelatedInfoRelation.h"
#include "Event/GenericBuffer.h"

namespace 
{
  struct _Instantiations 
  {
    LHCb::PackedRelatedInfoRelations::PackedRelatedInfo i1;
    LHCb::PackedRelatedInfoRelations::InfoVector        i2;
  };
}

#endif // DICT_PACKEDEVENTDICT_H

// $Id: selection.h,v 1.6 2006-11-03 15:23:36 cattanem Exp $
// ====================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $ 
// ====================================================================
// Incldue files
// ====================================================================
// STD & STL
// ====================================================================
#include <string>
#include <vector>
// ====================================================================
// Relations
// ====================================================================
#include "Relations/RelationsDict.h"
// ====================================================================
// Provided include files: 
// ====================================================================
#include "Event/MCParticle.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloHypo.h"
// ====================================================================
// Other include files: put your own includes here:
// ====================================================================
//#include ...
//#include ...
//#include ...
// ====================================================================

/** @file 
  *       
  * Helper file for building Reflex dictionaries for Relations 
   * @author ibelyaev 
  * @date  2006-06-11 22:17:54.560281 
  *       
  */      

namespace Dict
{
  struct __Instantiations 
  {
GaudiDict::Relation1DDict<LHCb::Track,float>	_1 ;
GaudiDict::RelationWeighted1DDict<LHCb::CaloCluster,LHCb::MCParticle,float>	_2 ;
GaudiDict::RelationWeighted1DDict<LHCb::CaloDigit,LHCb::MCParticle,float>	_3 ;
GaudiDict::RelationWeighted1DDict<LHCb::ProtoParticle,LHCb::MCParticle,double>	_4 ;
GaudiDict::RelationWeighted2DDict<LHCb::CaloCluster,LHCb::Track,float>	_5 ;
GaudiDict::RelationWeighted2DDict<LHCb::CaloHypo,LHCb::Track,float>	_6 ;
GaudiDict::RelationWeighted2DDict<LHCb::Track,LHCb::CaloCluster,float>	_7 ;
GaudiDict::RelationWeighted2DDict<LHCb::Track,LHCb::CaloHypo,float>	_8 ;
  };
};

// This is a fix for the LCG_48 release cycle because of the lack of
// implicitly generated enums for dictionaries in the Event model
// Please remove this with the any release > LCG_48
 
#include "GaudiKernel/SmartDataPtr.h"
#include "Event/LinkReference.h"
#include "Event/LinksByKey.h"
#include "Event/HltEnums.h" //
#include "Event/HltNames.h"
#include "Event/HltSelectionSummary.h" //
#include "Event/HltSummary.h" //
#include "Event/L0CaloAdc.h"
#include "Event/IL0Candidate.h"
#include "Event/L0DUBase.h" //
#include "Event/L0MuonBase.h"
#include "Event/L0CaloCandidate.h"
#include "Event/L0DUCounter.h"
#include "Event/L0DUChannel.h"
#include "Event/L0DUElementaryCondition.h"
#include "Event/L0DUElementaryData.h"
#include "Event/L0DUPattern.h"
#include "Event/L0DUReport.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0ProcessorData.h"
#include "Event/L0PrsSpdHit.h"
#include "Event/ODIN.h" //
#include "Event/BankWriter.h"
#include "Event/ByteStream.h"
#include "Event/RawBank.h" //
#include "Event/RawEvent.h"
#include "Event/SmartBank.h"
#include "Event/PackedMCParticle.h"
#include "Event/PackedMCVertex.h"
#include "Event/FilterCriterionResult.h"
#include "Event/FlavourTag.h"
#include "Event/Particle.h"
#include "Event/Tagger.h"
#include "Event/Vertex.h"
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "Event/HepMCEvent.h" //
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"
#include "Event/CellID.h"
#include "Event/CaloClusterEntry.h"
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"
#include "Event/MuonCoord.h"
#include "Event/MuonPID.h"
#include "Event/ProcStatus.h"
#include "Event/ProtoParticle.h"
#include "Event/RecHeader.h"
#include "Event/RecVertex.h"
#include "Event/RichPID.h"
#include "Event/RichSummaryPhoton.h"
#include "Event/RichSummaryRadSegment.h"
#include "Event/RichSummaryTrack.h"
#include "Event/STSummary.h"
#include "Event/SelResult.h"
#include "Event/VertexBase.h"
#include "Event/Measurement.h"
#include "Event/StateParameters.h"
#include "Event/TrackFunctor.h"
#include "Event/TrackParameters.h"
#include "Event/TrackUnitsConverters.h"
#include "Event/TrackUse.h"
#include "Event/Node.h"
#include "Event/State.h"
#include "Event/Track.h"
#include "Event/MCCaloDigit.h"
#include "Event/MCTrackInfo.h"
#include "Event/MCTrackInfoBits.h"
#include "Event/MCTruth.h"
#include "Event/MCCaloHit.h"
#include "Event/MCEffParticle.h"
#include "Event/MCEffTree.h"
#include "Event/MCExtendedHit.h"
#include "Event/MCHeader.h"
#include "Event/MCHit.h"
#include "Event/MCMuonDigit.h"
#include "Event/MCMuonDigitInfo.h"
#include "Event/MCMuonHitHistory.h"
#include "Event/MuonBXFlag.h"
#include "Event/MuonOriginFlag.h"
#include "Event/PackMCMuonDigitInfo.h"
#include "Event/PackMCMuonDigitHistory.h"
#include "Event/MCOTDeposit.h"
#include "Event/MCOTTime.h"
#include "Event/MCParticle.h"
#include "Event/MCProperty.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichDigitHistoryCode.h"
#include "Event/MCRichDigitHit.h"
#include "Event/MCRichDigitSummary.h"
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichTrack.h"
#include "Event/MCSTDeposit.h"
#include "Event/MCSTDigit.h"
#include "Event/MCVeloFE.h"
#include "Event/MCVertex.h"
#include "Event/CaloAdc.h"
#include "Event/CaloDigit.h"
#include "Event/CaloDigitStatus.h"
#include "Event/MuonDigit.h"
#include "Event/PackMuonDigit.h"
#include "Event/OTTime.h"
#include "Event/RichDigit.h"
#include "Event/STCluster.h"
#include "Event/STDigit.h"
#include "Event/STLiteCluster.h"
#include "Event/VeloCluster.h"
#include "Event/VeloDigit.h"
#include "Event/VeloLiteCluster.h"
#include "Event/IntLink.h"
#include "Event/ProcessHeader.h"
#include "VeloEvent/VeloFullFPGADigit.h"

// End of fix for the LCG_48 release cycle

// ====================================================================
// The END ============================================================
// ====================================================================

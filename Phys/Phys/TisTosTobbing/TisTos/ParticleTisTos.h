// $Id: ParticleTisTos.h,v 1.1 2010-07-21 21:22:16 tskwarni Exp $
#ifndef PARTICLETISTOS_H
#define PARTICLETISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTisTos.h"            // Interface

#include "TisTos/TisTos.h"

#include "CaloInterfaces/ITrack2Calo.h"

#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/RecVertex.h"
#include "Event/Vertex.h"
#include "Event/Track.h"
#include "Event/HltObjectSummary.h"

/** @class ParticleTisTos ParticleTisTos.h
 *
 *  @author Tomasz Skwarnicki
 *  @date   2010-07-08
 *
 *  Default implementation of IParticleTisTos tool
 */
class ParticleTisTos :  public TisTos,
                        virtual public IParticleTisTos {
public:

  /// Standard constructor
  ParticleTisTos( const std::string& type,
                  const std::string& name,
                  const IInterface* parent);

  virtual ~ParticleTisTos( ); ///< Destructor


  virtual StatusCode         initialize();

  // ------------  various ways to define Signal (off-line input) -------------------------

  /// Particle input; for composite particles loop over daughters will be executed (true if Signal changed)
  bool addToSignal( const LHCb::Particle & particle );

  /// Proto-particle input
  bool addToSignal( const LHCb::ProtoParticle & protoParticle );

  /// Track input
  bool addToSignal( const LHCb::Track & track );

  /// Hits input
  bool addToSignal( const std::vector<LHCb::LHCbID> & hits )  { return addHitsToSignal(hits); }


  // -------------------------------------------------
  // ------------ outputs
  // -------------------------------------------------

  /// completely classify the Trigger object with respect to the previouly defined Signal
  unsigned int tisTos(const LHCb::Particle & particle)  ;
  unsigned int tisTos(const LHCb::RecVertex & recVertex )  ;
  unsigned int tisTos(const LHCb::Vertex & vertex )  ;
  unsigned int tisTos(const LHCb::Track & track ) ;
  unsigned int tisTos(const std::vector<LHCb::LHCbID> & hits ){  return tisTosSortedHits( sortedHits(hits) );  }
  unsigned int tisTos(const LHCb::HltObjectSummary & hos );


  /// check for TOS  - may be faster than using tisTos()
  bool tos(const LHCb::Particle & particle) ;
  bool tos(const LHCb::RecVertex & recVertex) ;
  bool tos(const LHCb::Vertex & vertex) ;
  bool tos(const LHCb::Track & track ) ;
  bool tos(const std::vector<LHCb::LHCbID> & hits ){    return tosSortedHits( sortedHits(hits) );  }
  bool tos(const LHCb::HltObjectSummary & hos );

  /// check for TIS  - may be faster than using tisTos()
  bool tis(const LHCb::Particle & particle) ;
  bool tis(const LHCb::RecVertex & recVertex) ;
  bool tis(const LHCb::Vertex & vertex) ;
  bool tis(const LHCb::Track & track ) ;
  bool tis(const std::vector<LHCb::LHCbID> & hits ){    return tisSortedHits( sortedHits(hits) );  }
  bool tis(const LHCb::HltObjectSummary & hos );

  /// check for TUS  (TPS or TOS) - may be faster than using tisTos()
  bool tus(const LHCb::Particle & particle) ;
  bool tus(const LHCb::RecVertex & recVertex) ;
  bool tus(const LHCb::Vertex & vertex) ;
  bool tus(const LHCb::Track & track ) ;
  bool tus(const std::vector<LHCb::LHCbID> & hits ){    return tusSortedHits( sortedHits(hits) );  }
  bool tus(const LHCb::HltObjectSummary & hos );

  /// analysis string
  std::string analysisReport(const LHCb::Particle & particle) ;
  std::string analysisReport(const LHCb::RecVertex & recVertex) ;
  std::string analysisReport(const LHCb::Vertex & vertex) ;
  std::string analysisReport(const LHCb::Track & track ) ;
  std::string analysisReport(const std::vector<LHCb::LHCbID> & hits ){ return analysisReportSortedHits( sortedHits(hits) );  }
  std::string analysisReport(const LHCb::HltObjectSummary & hos );

  // --------------------- control calls -------------------------

  void setProjectTracksToCalo(bool onOff){ m_projectTracksToCalo=onOff;  }
  void setCaloClustForCharged(bool onOff){ m_caloClustForCharged=onOff;  }
  void setCaloClustForNeutral(bool onOff){ m_caloClustForNeutral=onOff;  }
  void setCompositeTPSviaPartialTOSonly(bool onOff){ m_compositeTPSviaPartialTOSonly=onOff;  }
  void setFullAnalysisReport(bool onOff){ m_fullAnalysisReport=onOff;  }

  bool getProjectTracksToCalo()const { return m_projectTracksToCalo;  }
  bool getCaloClustForCharged()const { return m_caloClustForCharged;  }
  bool getCaloClustForNeutral()const { return m_caloClustForNeutral;  }
  bool getCompositeTPSviaPartialTOSonly()const { return m_compositeTPSviaPartialTOSonly;  }
  bool getFullAnalysisReport()const { return m_fullAnalysisReport;  }

  // --------------------- utilities ------------------------

  /// collect hits from protoparticle
  std::vector<LHCb::LHCbID> protoParticleHits(const LHCb::ProtoParticle & pp, bool extend=false );
  /// get calo Hits by projecting into eCal and Hcal (3x3 cells)
  std::vector<LHCb::LHCbID> projectTrack(const LHCb::Track& track );

protected:

  /// Tool for projecting track into Hcal and Ecal
  ITrack2Calo*  m_track2calo;
  /// Hcal detector geometry
  DeCalorimeter* m_hcalDeCal;
  /// Ecal detector geometry
  DeCalorimeter* m_ecalDeCal;

  /// true if signal Tracks are projected to Ecal and Hcal to claim calo cells
  bool m_projectTracksToCalo;
  /// true if signal ProtoParticle built on Track should collect hits from linked CaloCluster
  bool m_caloClustForCharged;
  /// true if signal ProtoParticle built on CaloCluster should collect hits from linked CaloClusters
  bool m_caloClustForNeutral;
  /// true if composite Trigger object should acquire TPS status only if at least one non-composite sub-object is TOS
  bool m_compositeTPSviaPartialTOSonly;

  /// true if full analysis report should be given in analysisReport(), instead of following tisTos() loop breaking
  bool m_fullAnalysisReport;

  /// to offset analysisReport printout
  unsigned int m_reportDepth;

protected:

  std::string offset()
  {
    std::string rep;
    for(unsigned int i=0;i<m_reportDepth;++i){ rep += "   "; }
    return rep;
  }

};
#endif // PARTICLETISTOS_H

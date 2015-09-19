// $Id: TisTosParticleTagger.h,v 1.1 2010-07-23 20:51:49 tskwarni Exp $
#ifndef TISTOSPARTICLETAGGER_H
#define TISTOSPARTICLETAGGER_H 1

// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DaVinciAlgorithm.h"

#include "Kernel/ITriggerTisTos.h"

/** @class TisTosParticleTagger TisTosParticleTagger.h
 *
 *  @author Tomasz Skwarnicki
 *  @date   2010-07-22
 *
 *  This algorithm provides tagging of Particles with TisTos information
 */
class TisTosParticleTagger : public DaVinciAlgorithm
{

public:

  /// Standard constructor
  TisTosParticleTagger( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TisTosParticleTagger( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

   template <class PARTICLES, class VERTICES, class CLONER>
   StatusCode _save () const;

   virtual StatusCode _saveInTES ();

   void cloneP2PVRelation(const LHCb::Particle* particle ,
                          const LHCb::Particle* clone) const;

   virtual void writeEmptyTESContainers();

private:

  template <class CONTAINER>
  void writeEmptyContainer() {
    auto container = new CONTAINER{};
    put(container, particleOutputLocation());
  }

  /// enumerate trigger stages
  enum TriggerStages {defaultTriggerStage=0,HLT1=defaultTriggerStage,HLT2,L0,NTriggerStages};

  /// checks integrity of the particle (skip if fails it)
  bool particleOK( const LHCb::Particle* po );

  /// tag particle with calculated TisTos condition (returns false if failed)
  static bool storeTag(LHCb::Particle* po, int index, bool passedCondition);

  /// pairs of <"InputTrigger%TisTosFunction",InfoID> ID=0 no tagging; (use Id>10000 but not too large)
  std::map<std::string,int> m_tistosSpecs ;

  /// Clone filtered particles
  bool m_cloneFilteredParticles;

  /// flag if the algorithm should pass on any input or just on at least one satisfied TisTos
  bool m_passOnAll;
  /// flag to disallow using regex in the "InputTrigger" (can make the Tagger faster)
  bool m_noRegex;


  /// Pass Trigger Stage on First Satisfied Spec Flag (affects execuation only if tag-keys are not zero)
  bool m_SatisfiedOnFirstSpec;
  /// Pass Trigger Stage on First Satisfied Spec (set always to true if all tag-keys are zero)
  bool m_fast;


  /// name of the TriggerTisTos tool if want to overwrite default (""=default)
  std::string m_TriggerTisTosName;

  /// location of input HltDecReports to be checked for existance ("None"=no check, ""=default)
  std::string m_decReportsLocation;
  /// location of input HltSe;Reports to be checked for existance ("None"=no check, ""=default)
  std::string m_selReportsLocation;

  /// location of HltDecReports to be checked for existance to determine if Hlt report is split ("None"=no check, ""=default)
  std::string m_split_decReportsLocation;

  /// flags to check existance of reports
  bool m_checkDecReport;
  bool m_checkSelReport;
  bool m_split_checkDecReport;
  /// and actual locations to check
  std::string m_decReportLoc;
  std::string m_selReportLoc;
  std::string m_decReportLoc1;
  std::string m_selReportLoc1;
  std::string m_decReportLoc2;
  std::string m_selReportLoc2;


  /// TisTos tool
  ITriggerTisTos* m_tistostool;
  ITriggerTisTos* m_tistostoolL0;
  ITriggerTisTos* m_tistostoolHlt1;
  ITriggerTisTos* m_tistostoolHlt2;
  ITriggerTisTos* m_tistostoolStage[NTriggerStages];

  /// pairs of < "InputTrigger", Info Id > (Id=0 do not tag Particles) (derived from m_tistosSpecs)
  std::map<std::string,int> m_tusSpecs[NTriggerStages];
  std::map<std::string,int> m_tosSpecs[NTriggerStages];
  std::map<std::string,int> m_tisSpecs[NTriggerStages];
  bool m_NoSpecs[NTriggerStages];


  /// TisTos tool configuration (see ParticleTisTos & TisTos tools)
  bool m_projectTracksToCalo;
  bool m_caloClustForCharged;
  bool m_caloClustForNeutral;
  bool m_compositeTPSviaPartialTOSonly;
  bool m_noHitTypes;
  std::map<int,double>  m_TOSFrac;
  std::map<int,double>  m_TISFrac;


};
#endif // TISTOSPARTICLETAGGER_H

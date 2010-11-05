// $Id: TisTosParticleTagger.h,v 1.1 2010-07-23 20:51:49 tskwarni Exp $
#ifndef TISTOSPARTICLETAGGER_H 
#define TISTOSPARTICLETAGGER_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "Kernel/ITriggerTisTos.h"

/** @class TisTosParticleTagger TisTosParticleTagger.h
 *  
 *
 *  @author Tomasz Skwarnicki 
 *  @date   2010-07-22
 *
 *  This algorithm provides tagging of Particles with TisTos information
 *
 */
class TisTosParticleTagger : public DVAlgorithm {
public: 
  /// Standard constructor
  TisTosParticleTagger( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TisTosParticleTagger( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  /// checks integrity of the particle (skip if fails it)
  bool particleOK( const LHCb::Particle* po );

  /// tag particle with calculated TisTos condition (returns false if failed)
  static bool storeTag(LHCb::Particle* po, int index, bool passedCondition);
  
  
  /// pairs of <"InputTrigger%TisTosFunction",InfoID> ID=0 no tagging; (use Id>10000 but not too large)
  std::map<std::string,int> m_tistosSpecs ; 

  /// flag if the algorithm should pass on any input or just on at least one satisfied TisTis
  bool m_passOnAll;  
  /// flag to disallow using regex in the "InputTrigger" (can make the Tagger faster)
  bool m_noRegex;  

  /// name of the TriggerTisTos tool if want to overwrite default (""=default)
  std::string m_TriggerTisTosName;

  /// location of input HltDecReports to be chcked for existance ("None"=no check, ""=default)
  std::string m_decReportsLocation;
  /// location of input HltSe;Reports to be chcked for existance ("None"=no check, ""=default)
  std::string m_selReportsLocation;
  
  /// flags to check existance of reports
  bool m_checkDecReport;
  bool m_checkSelReport;
  /// and actual locations to check
  std::string m_decReportLoc;
  std::string m_selReportLoc;


  /// TisTos tool
  ITriggerTisTos* m_tistostool;

  /// pairs of < "InputTrigger", Info Id > (Id=0 do not tag Particles) (derived from m_tistosSpecs)
  std::map<std::string,int> m_tusSpecs ; 
  std::map<std::string,int> m_tosSpecs ; 
  std::map<std::string,int> m_tisSpecs ; 

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

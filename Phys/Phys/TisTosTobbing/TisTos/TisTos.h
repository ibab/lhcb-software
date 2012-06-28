// $Id: TisTos.h,v 1.1 2010-07-21 21:22:17 tskwarni Exp $
#ifndef TISTOS_H
#define TISTOS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ITisTos.h"            // Interface

/** @class TisTos TisTos.h
 *
 *  @author Tomasz Skwarnicki
 *  @date   2010-07-07
 *
 *  Default implementation of ITisTos tool, classifying Trigger hit list with respect to Signal hit list
 *  @sa  ITisTos docs for more info.
 *  This interface also defines some inlined shortcuts for typical calls.
 */
class TisTos : public GaudiTool,
               virtual public ITisTos {
public:


  /// hits are split into @c HitType categories for matching in each category
  enum HitType {kVelo=0,kAllHits=kVelo,kOTIT,kTT,kMuon,kEcal,kHcal,nHitTypes,kNotUsed=nHitTypes};

  /// search condition input parameter in analyze method (analyze quits once the condition is determined)
  enum SearchCond {kSearchNone=0, kSearchForAll, kSearchForTOS, kSearchForTUS, kSearchForTIS};

  /// Standard constructor
  TisTos( const std::string& type,
          const std::string& name,
          const IInterface* parent);

  virtual ~TisTos( ); ///< Destructor


  virtual StatusCode         initialize();

  // ----------------- signal input ----------------------

  /// erase signal definition (returns true if erased non-empty signal)
  bool setSignal();

  /// add hits to signal definition (true if the Signal definition changed)
  bool addHitsToSignal(const std::vector<LHCb::LHCbID> & hits );

  /// add sorted hits to signal definition (true if the Signal definition changed)
  bool addSortedHitsToSignal(const std::vector<LHCb::LHCbID> & hits );

  /// retrieve signal definition
  std::vector<LHCb::LHCbID> signal() const;

  // ---------------- classification methods ----------------------

  /// completely classify the Trigger hit sequence with respect to the Signal hit sequence
  unsigned int tisTosSortedHits(const std::vector<LHCb::LHCbID> & triggerHits) const;

  /// check for TOS - may be faster than using tisTos()
  bool tosSortedHits(const std::vector<LHCb::LHCbID> & triggerHits) const;

  /// check for TIS - may be faster than using tisTos()
  bool tisSortedHits(const std::vector<LHCb::LHCbID> & triggerHits) const;

  /// check for TUS (TPS or TOS) - may be faster than using tisTos()
  bool tusSortedHits(const std::vector<LHCb::LHCbID> & triggerHits) const;

  // extras for diagnostics -------------

  /// hit analysis: number of trigger hits and fraction found in the signal for each hit type
  unsigned int analyzeSortedHits(const std::vector<LHCb::LHCbID> & triggerHits,
                                 std::vector<unsigned int> & nTrigger ,
                                 std::vector<double> & fractionInSignal ) const;

  /// analysis report
  std::string analysisReportSortedHits(const std::vector<LHCb::LHCbID> & triggerHits) const;


  // Control calls ------------------------------

  /// set using hit types on or off (returns true if call resulted in a change of the value of this switch)
  bool setNoHitTypes(bool onOff);


  // following calls return false is hitType was illegal

  /// set minimal fraction of trigger hits to be found on signal for TOS - see @c HitType for hit types (<=0 don't use this type)
  bool setTOSFrac(unsigned int hitType,double tosFrac);

  /// set maximal fraction of trigger hits to be found on signal for TIS - (should be below TOSFrac except when TOSFrac<=0)
  bool setTISFrac(unsigned int hitType,double tisFrac);

  /// set minimal fractions of trigger hits to be found on signal for TOS - see @c HitType for hit types (<=0 don't use this type)
  bool setTOSFrac(std::vector<double> tosFrac);

  /// set maximal fractions of trigger hits to be found on signal for TIS - (should be below TOSFrac except when TOSFrac<=0)
  bool setTISFrac(std::vector<double> tisFrac);

  // access to values used --------------------------

  /// actual number of hit types used
  unsigned int numberOfHitTypes() const  { return m_nHitTypes;  }

  /// Names if the hit types
  std::string hitTypeName(unsigned int hitType) const
  {
    switch(hitType)
    {
    case kVelo: if( m_noHitTypes ){return "All";} else {return "Velo";};
    case kOTIT: return "OTIT";
    case kTT:   return "TT";
    case kMuon: return "Mu";
    case kEcal: return "Ecal";
    case kHcal: return "Hcal";
    default: return "NotUsed";
    };
  }

  /// get minimal fraction of trigger hits to be found on signal for TOS - see @c HitType for hit types
  double getTOSFrac(unsigned int hitType) const;

  /// get maximal fraction of trigger hits to be found on signal for TIS
  double getTISFrac(unsigned int hitType) const;

  /// get minimal fractions of trigger hits to be found on signal for TOS
  std::vector<double> getTOSFrac() const;

  /// get maximal fraction of trigger hits to be found on signal for TIS
  std::vector<double> getTISFrac() const;



  // ------------------- utilities ------------------


  /// sort hits
  std::vector<LHCb::LHCbID> sortedHits(const std::vector<LHCb::LHCbID> & hitlist) const
  {
    std::vector<LHCb::LHCbID> copied;
    copied.insert(copied.end(),hitlist.begin(),hitlist.end());
    // sort hits
    std::sort( copied.begin(), copied.end() ) ;
    // eliminate duplicates
    copied.erase( std::unique( copied.begin(), copied.end() ), copied.end() ) ;
    return copied;
  }



  /// classify hit into @c HitType
  static unsigned int hitMatchType(const LHCb::LHCbID & id);

  /// analyze trigger hits with respect to previously defined signal, return searched condition
  unsigned int analyze(const std::vector<LHCb::LHCbID> & triggerSortedHits,
                       unsigned int searchCond,
                       unsigned int* nHitsAll, unsigned int* nHitsMatched, double* overlap) const;


protected:

  /// maximum fraction of matching hits allowed for TIS (<=)
  double m_TISFrac[nHitTypes];
  /// minimum fraction of matching hits required for TOS (>=)
  double m_TOSFrac[nHitTypes];

  /// actual number of hitTypes used
  unsigned int m_nHitTypes;

  /// true if hits are not split into separate categories
  bool m_noHitTypes;

private:

  //  -------------------------- data members --------------------

  /// Signal stored as classified hits
  std::vector<LHCb::LHCbID> m_Signal;

};
#endif // TISTOS_H

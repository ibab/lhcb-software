// $Id: ITisTos.h,v 1.2 2010-07-22 14:58:26 tskwarni Exp $
#ifndef KERNEL_ITISTOS_H 
#define KERNEL_ITISTOS_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Kernel/LHCbID.h"

/** @class ITisTos ITisTos.h Kernel/ITisTos.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2010-07-07
 *
 *  @par 
 *     Lowest level TIS/TOS tool to classify LHCbID hit sequence on Trigger object
 *     with respect to Signal also passed as LHCbID hit sequence.
 *  @par
 *     This tool can be used to evaluate overlap between any two LHCbID hit sequences.
 *     Notice that the roles of "Trigger" and "Signal" hit sequnces may be different in the tool, thus
 *     reversing their roles can change the results.
 *     E.g. the tool may normalize overlap fraction to the hits encountered on the "Trigger" sequence.
 */
class GAUDI_API ITisTos : virtual public IAlgTool {
public: 

  DeclareInterfaceID ( ITisTos, 1 , 0 ) ;

  /** @par Meaning of TisTosTob result:
   *       TPS    = Trigger Partially on Signal
   *       TOS    = Trigger On Signal 
   *          TUS =   TPS || TOS   (Trigger Used Signal)   
   *       TIS    = Trigger Independent of Signal
   *
   *       Decision = Trigger line decision (not used in Trigger object classification)
   *
   *       when encoded in an unsigned int TPS,TOS,TIS,Decision are indicated by subsequent bits 
   */

  /// values of TisTosTob result (info encoded in bits)
  enum {kUnknown=0,kTPS=1,kTOS=2,kTUS=3,kTIS=4,kTOSandTIS=6,kDecision=8};

  /// to help encode/decode TisTosTob result in/from unsigned int
  class TisTosTob
  {
  public:

    TisTosTob():m_tistostob(kUnknown){}
    TisTosTob(unsigned int value):m_tistostob(value){}
    TisTosTob(bool tps, bool tos, bool tis){ set_value(tps,tos,tis); }
    TisTosTob(bool tps, bool tos, bool tis, bool dec){ set_value(tps,tos,tis,dec); }

    virtual ~TisTosTob(){};
  
    /// check for TIS
    bool tis() const {return ((m_tistostob&kTIS)!=0);}
    /// check for TOS
    bool tos() const {return ((m_tistostob&kTOS)!=0);}
    /// check for TPS
    bool tps() const {return ((m_tistostob&kTPS)!=0);}
    /// check for TUS i.e. if Signal was used (TPS||TOS)
    bool tus() const {return ((m_tistostob&kTUS)!=0);}
    /// check for decision (used only for trigger selections)
    bool decision() const {return ((m_tistostob&kDecision)!=0);}
    /// get entire classification
    unsigned int value() const {return m_tistostob;}

    /// set TIS (on top of previous value)
    void set_tis(bool tis=true){if(tis){m_tistostob|=kTIS;}else{m_tistostob&=(~kTIS);};}
    /// set TOS (on top of previous value)
    void set_tos(bool tos=true){if(tos){m_tistostob|=kTOS;}else{m_tistostob&=(~kTOS);};}
    /// set TPS (on top of previous value)
    void set_tps(bool tps=true){if(tps){m_tistostob|=kTPS;}else{m_tistostob&=(~kTPS);};}
    /// set decision (on top of previous value)
    void set_decision(bool dec=true){if(dec){m_tistostob|=kDecision;}else{m_tistostob&=(~kDecision);};}

    /// set TisTosTob value (erases previous value)
    void set_value(unsigned int value){m_tistostob=value;}
    /// set default value i.e. unknown (erase previous value)
    void set_value(){m_tistostob=kUnknown;}
    /// set TisTosTob value (erases previous value)
    void set_value(bool tps, bool tos, bool tis){ 
         set_value();
         if(tps)set_tps();
         if(tos)set_tos();
         if(tis)set_tis();
    }
    void set_value(bool tps, bool tos, bool tis,bool dec){ 
         set_value();
         if(tps)set_tps();
         if(tos)set_tos();
         if(tis)set_tis();
         if(dec)set_decision();
    }
    

  private:

    unsigned int m_tistostob;

  };

  /// erase signal definition (returns true if erased non-empty signal)
  virtual bool setSignal() = 0;

  /// add to signal definition (returns true if the signal changed after the operation)
  virtual bool addHitsToSignal(const std::vector<LHCb::LHCbID> & hits) = 0;

  /// add sorted hits to signal definition (returns true if the signal changed after the operation)
  virtual bool addSortedHitsToSignal(const std::vector<LHCb::LHCbID> & hits) = 0;

  /// set signal to given hits
  bool setSignal(const std::vector<LHCb::LHCbID> & hits){ 
    bool changed=setSignal(); 
    if(addHitsToSignal(hits))changed=true;
    return changed;
  }
  
  /// set signal to given sorted hits
  bool setSortedSignal(const std::vector<LHCb::LHCbID> & hits){ 
    bool changed=setSignal(); 
    if(addSortedHitsToSignal(hits))changed=true;
    return changed;
  }
  
  
  /// retrieve signal definition
  virtual std::vector<LHCb::LHCbID> signal() const = 0;
  
  /// completely classify the Trigger hit sequence with respect to the Signal hit sequence
  virtual unsigned int tisTosSortedHits(const std::vector<LHCb::LHCbID> & triggerHits) const =0;
  
  /// completely classify the Trigger hit sequence with respect to the Signal, return helper class
  TisTosTob  tisTosTobSortedHits(const std::vector<LHCb::LHCbID> & triggerHits) const 
  { return TisTosTob(tisTosSortedHits(triggerHits));}
  
  /// check for TOS  - may be faster than using tisTosSortedHits()
  virtual bool tosSortedHits(const std::vector<LHCb::LHCbID> & triggerHits) const =0;
  
  /// check for TIS - may be faster than using tisTosSortedHits()
  virtual bool tisSortedHits(const std::vector<LHCb::LHCbID> & triggerHits) const =0;
  
  /// check for TUS (TPS or TOS) - may be faster than using tisTosSortedHits()
  virtual bool tusSortedHits(const std::vector<LHCb::LHCbID> & triggerHits) const =0;

  // extras for diagnostics -------------

  /// hit analysis: number of trigger hits and fraction found in the signal for each hit type  
  virtual unsigned int analyzeSortedHits(const std::vector<LHCb::LHCbID> & triggerHits,
                           std::vector<unsigned int> & nTrigger ,
                           std::vector<double> & fractionInSignal ) const =0;
  
  /// analysis report 
  virtual std::string analysisReportSortedHits(const std::vector<LHCb::LHCbID> & triggerSortedHits) const =0;  

  // Control calls ------------------------------

  /// set using hit types on or off (returns true if call resulted in a change of the value of this switch)
  virtual bool setNoHitTypes(bool onOff) =0;
 
 /// set minimal fraction of trigger hits to be found on signal for TOS - hit types implementation dependent (<=0 don't use)
  virtual bool setTOSFrac(unsigned int hitType,double tosFrac) =0;
  
  /// set maximal fraction of trigger hits to be found on signal for TIS - (should be below TOSFrac except when TOSFrac<=0)
  virtual bool setTISFrac(unsigned int hitType,double tisFrac) =0;

  /// set minimal fractions of trigger hits to be found on signal for TOS - (<=0 don't use this type neither in TOS nor TIS)
  virtual bool setTOSFrac(std::vector<double> tosFrac) =0;
  
  /// set maximal fractions of trigger hits to be found on signal for TIS - (should be below TOSFrac except when TOSFrac<=0)
  virtual bool setTISFrac(std::vector<double> tisFrac) =0;
  
  // access to internal values used --------------------------

  /// Number of hit types used 
  virtual unsigned int numberOfHitTypes() const =0;
  
  /// Names if the hit types
  virtual std::string hitTypeName(unsigned int hitType) const =0;

  /// get minimal fraction of trigger hits to be found on signal for TOS - returns 0.0 if illegal hitType
  virtual double getTOSFrac(unsigned int hitType) const =0;
  
  /// get maximal fraction of trigger hits to be found on signal for TIS 
  virtual double getTISFrac(unsigned int hitType) const =0;

  /// get minimal fractions of trigger hits to be found on signal for TOS 
  virtual std::vector<double> getTOSFrac() const =0;
  
  /// get maximal fraction of trigger hits to be found on signal for TIS 
  virtual std::vector<double> getTISFrac() const =0;

  /// utility to sort hits (can be used by user to convert unsorted hits in calls to various methods)
  virtual std::vector<LHCb::LHCbID> sortedHits(const std::vector<LHCb::LHCbID> & hits) const =0;  

protected:

private:

};


#endif // KERNEL_ITISTOS_H

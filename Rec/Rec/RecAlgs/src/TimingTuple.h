// $Id: $
#ifndef TIMINGTUPLE_H 
#define TIMINGTUPLE_H 1

#include <algorithm>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/Memory.h"
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiAlg/ISequencerTimerTool.h"

#include "OTDAQ/IOTRawBankDecoder.h"
#include "L0Interfaces/IL0DUFromRawTool.h"
#include "Kernel/LHCbID.h"
#include "LoKi/select.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/ODIN.h"
#include "Event/STCluster.h"
#include "Event/VeloCluster.h"
#include "Event/Track.h"

//for velo count 
#include "boost/bind.hpp"

class ISequencerTimerTool ;

/** @class TimingTuple TimingTuple.h
 *  
 *  Fill a Tuple with timing, memory and some event variables
 *
 *  @author Patrick Koppenburg
 *  @date   2010-08-18
 */
class TimingTuple : public GaudiTupleAlg {
public: 
  /// Standard constructor
  TimingTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TimingTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
  unsigned int nVelo(const LHCb::Tracks* tracks); ///< extract and count velo tracks

private:
  ISequencerTimerTool* m_timerTool ; ///< timer tool
  int m_timer ; ///< timer index
  const IOTRawBankDecoder* m_rawBankDecoder;
  std::string m_fromRawTool;
  IL0DUFromRawTool* m_l0BankDecoder;
  unsigned int m_evtCounter ;
  Rich::DAQ::IRawBufferToSmartIDsTool* m_richTool ;
  
  void fillTuple(Tuple& tuple, std::string var, int number){ 
    if (msgLevel(MSG::VERBOSE)) verbose() << "Filling " << var << " with " << number << endmsg ;
    tuple->column( var , number);
  }
  
  void fillTuple(Tuple& tuple, std::string var, unsigned int number){ 
    if (msgLevel(MSG::VERBOSE)) verbose() << "Filling " << var << " with " << number << endmsg ;
    tuple->column( var , number);
  }
  
  void fillTuple(Tuple& tuple, std::string var, double number){ 
    if (msgLevel(MSG::VERBOSE)) verbose() << "Filling " << var << " with " << number << endmsg ;
    tuple->column( var , number);
  }
  

  template <class TYPE>
  bool safeExist  ( const std::string& location ) 
  {
    DataObject* obj = 0 ;
    StatusCode sc = evtSvc()->findObject( location , obj ) ;
    return sc.isSuccess() && 0 != obj && 0 != dynamic_cast<TYPE*>( obj ) ;
  }

  /// generic templated method to extract the number of entries in a given location.
  /// usage int n = number<LHCb::Particles>('/Event/Phys/MyParts/Particles')
  template<class CLASS> 
  inline int number( const std::string& location)
  {
    return ( safeExist<CLASS>(location) ? (get<CLASS>(location))->size() : -1 ) ;
  }

  // velo count from Matthew David Needham
  inline bool inCloneContainer(const std::vector<LHCb::Track*>& keepCont, 
                      const std::vector<LHCb::LHCbID>& vids) {

    if (msgLevel(MSG::VERBOSE)) verbose() << "inClone " << vids.size() << endmsg ;

    bool keep = true;
    std::vector<LHCb::Track*>::const_iterator iter = keepCont.begin();
    for (;iter != keepCont.end(); ++iter ){
      const std::vector<LHCb::LHCbID>& vids2 = (*iter)->lhcbIDs(); 
      std::vector<LHCb::LHCbID> veloHits2; veloHits2.reserve(vids.size());
      LoKi::select(vids2.begin(), vids2.end(), std::back_inserter(veloHits2),boost::bind(&LHCb::LHCbID::isVelo,_1));
      if (std::equal(vids.begin(), vids.end(), veloHits2.begin())){
        keep = false;
        break;
      }
    }
    return keep;
  }  

};
#endif // TIMINGTUPLE_H

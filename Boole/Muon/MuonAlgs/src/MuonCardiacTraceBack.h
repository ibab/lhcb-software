// $Id: MuonCardiacTraceBack.h,v 1.1 2007-02-27 08:53:10 asatta Exp $
#ifndef MUONCARDIACTRACEBACK_H 
#define MUONCARDIACTRACEBACK_H 1
#include <vector>
#include "MuonPhysicalChannelOutput.h"
// I#include <vector>nclude files

/** @class MuonCardiacTraceBack MuonCardiacTraceBack.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-12-11
 */
class MuonCardiacTraceBack {
public: 
  /// Standard constructor
  MuonCardiacTraceBack( ); 
  MuonCardiacTraceBack(MuonHitTraceBack* value );
  virtual ~MuonCardiacTraceBack( ){}; ///< Destructor
  void setDeadTime()
  { m_trackeBack.second=false;
  }
   bool getDeadTime()
  { return !(m_trackeBack.second);
  }
  MuonHitTraceBack*   getHitTrace()
  { return m_trackeBack.first;
  }
  void setHitDialogDeadtime(){
    (m_trackeBack.first)-> getMCMuonHistory(). setKilledByDialogDeadtimeHit(1);
    
  }  
    
protected:

private:
  std::pair<MuonHitTraceBack*,bool> m_trackeBack;
  
};


#endif // MUONCARDIACTRACEBACK_H

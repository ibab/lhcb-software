// $Id: RODimLineService.h,v 1.1 2009-04-03 09:16:40 ocallot Exp $
#ifndef RODIMLINESERVICE_H 
#define RODIMLINESERVICE_H 1

// Include files
#include "GaudiKernel/MsgStream.h"   // Get default definitions from Gaudi
#include "dim/dis.hxx"
#include "RONodeCounter.h"

/** @class DimNodeService DimNodeService.h
 *  Holds a Dim service for one line of the Farm Monitoring
 *
 *  @author Olivier Callot
 *  @date   2007-12-21
 */
class RODimLineService {
public: 
  /// Standard constructor
  RODimLineService( std::string part, int nb ); 

  virtual ~RODimLineService( ); ///< Destructor

  void update( RONodeCounter& counter );

  std::string& part() { return m_part; }
  
protected:

private:
  std::string m_part;
  DimService* m_service;
  struct dimBuffer {
    int number;
    char name[12];
    int mep;
    int evt;
    int con;
    int snd;
    int acc;
    int tsk;
    int mepRate;
    int evtRate;
    int conRate;
    int sndRate;
    int accRate;
  };  
  dimBuffer m_dimBuffer;
};
#endif // RODIMLINESERVICE_H

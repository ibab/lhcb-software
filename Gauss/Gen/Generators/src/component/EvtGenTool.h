// $Id: EvtGenTool.h,v 1.9 2009-03-16 16:53:26 robbep Exp $
#ifndef GENERATORS_EVTGENTOOL_H 
#define GENERATORS_EVTGENTOOL_H 1

// Avoid contamination with WINDOWS symbols
#ifdef WIN32
#define NOATOM
#define NOGDI
#define NOGDICAPMASKS
#define NOMETAFILE
#define NOMINMAX
#define NOMSG
#define NOOPENFILE
#define NORASTEROPS
#define NOSCROLL
#define NOSOUND
#define NOSYSMETRICS
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOCRYPT
#define NOMCX
#endif

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
//interface
#include "IEvtGenTool.h"

// from EvtGen
#include "EvtGen/EvtGen.hh"

/** @class EvtGenTool EvtGenTool.h "EvtGenTool.h"
 *  
 *  Tool to interface to EvtGen generator. 
 *
 *  @author Alex Shires, Patrick Robbe
 *  @date   2011-10-28
 */
class EvtGenTool : public GaudiTool, virtual public IEvtGenTool {
public:
  /// Standard constructor
  EvtGenTool( const std::string& type, const std::string& name,
              const IInterface* parent);

  virtual ~EvtGenTool( ) ; ///< Destructor

  /** Initialize method.
   *  */
  virtual StatusCode initialize() ;

  /// Finalize method
  virtual StatusCode finalize() ;
  
  /// Get the pointer to the EvtGen instance
  EvtGen* getEvtGen( ) { return m_gen ; }
  
  /// Get the pointer to the EvtGen instance
  void setEvtGen( EvtGen * evtgenptr ) {  m_gen = evtgenptr ; m_init = true ; }

  /// return the initalised state of EvtGen
  bool isInit() { return m_init ; }

private:
 
    EvtGen * m_gen ; ///< EvtGen engine

    bool m_init ; ///< Initialisation flag


};


#endif // GENERATORS_EVTGENTOOL_H

// $Id: EvtTypeSvc.h,v 1.3 2009-03-09 13:04:16 rlambert Exp $
#ifndef EVTTYPESVC_H 
#define EVTTYPESVC_H 1

// Include files
#include "GaudiKernel/Service.h"
// from Gaudi
#include "Kernel/IEvtTypeSvc.h"
#include "EvtTypeInfo.h"

#include <string>
#include <cstdlib>
#include <iostream>

#include <set>
#include <vector>

// Forward declarations
template <class TYPE> class SvcFactory;
class IToolSvc; //forward declaration
//class IMCDecayFinder; //forward declaration


/** @class EvtTypeSvc EvtTypeSvc.h
 *
 *  Service that provide information for (MC)EVTTYPE code.
 *
 *  @author Gloria CORTI
 *  @date   2004-04-30
 *
 *  Edited R. Lambert  2009-03-04
 *  Added implimentation of allTypes(void), returning a std::set of all known types
 *
 */
class EvtTypeSvc : public Service
                 , virtual public IEvtTypeSvc 
{
public: 

  /// Query the interfaces.
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvInterface );

  /// Initialize the service.
  virtual StatusCode initialize();
  
  /// Finalize the service.
  virtual StatusCode finalize();

  /** Given the integer code of an Event type provide its Nick Name
   *  if it does not exist returns an empty string
   *  @see IEvtTypeSvc
   */
  virtual std::string nickName( const int evtCode );

  /** Given an EventType provide its ASCII decay descriptor
   *  @see IEvtTypeSvc
   */
  virtual std::string decayDescriptor( const int evtCode );

  /** Check if an event type corresponding to the integer code is known
   *  @see IEvtTypeSvc
   */
  virtual bool typeExists( const int evtCode );

  /** return a set of all known event types
   *  @see IEvtTypeSvc
   */
  virtual LHCb::EventTypeSet allTypes( void );

protected:

  /** Standard Constructor.
   *  @param  name   String with service name
   *  @param  svc    Pointer to service locator interface
   */
  EvtTypeSvc( const std::string& name, ISvcLocator* svc );
  
  /// Destructor.
  virtual ~EvtTypeSvc();
 
  // Function to compare evtcode of an evttype
  class EvtCodeEqual {
  public: 
    EvtCodeEqual( int code = 10000000 ) : m_code( code ) { }
    
    inline bool operator() ( const EvtTypeInfo* type ) {
      return ( type->evtCode() == m_code );
    }

  private:
    int m_code;
   
  };
  
protected:
  
private:

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<EvtTypeSvc>;

  /// Parse the input table containing all known event types and
  /// theirs' nicknames and ascii descriptor
  StatusCode parseFile( const std::string input );
  
  /// Name of file with input table
  const std::string& inputFile() {
    return m_inputFile;
  }

  /// Typedefs
  typedef std::vector<EvtTypeInfo*> EvtTypeInfos;
  
  // Data
  std::string  m_inputFile;    ///< Name of input file with necessary info
  EvtTypeInfos m_evtTypeInfos; ///< List of objects containing all EvtType info

};
#endif // EVTTYPESVC_H

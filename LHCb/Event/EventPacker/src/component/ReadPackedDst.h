// $Id: ReadPackedDst.h,v 1.4 2009-12-04 16:40:19 jonrob Exp $
#ifndef READPACKEDDST_H
#define READPACKEDDST_H 1

// Include files
// from Gaudi
#include "GaudiKernel/SmartIF.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/IGenericTool.h"
#include "GaudiKernel/ToolHandle.h"
#include "GaudiKernel/LinkManager.h"

/** @class ReadPackedDst ReadPackedDst.h
 *  This is the unpacker of DAQEVent containing the DST data
 *
 *  @author Olivier Callot
 *  @date   2009-01-19
 */
class ReadPackedDst : public GaudiAlgorithm
{

public:

  /// Standard constructor
  ReadPackedDst( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~ReadPackedDst( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize();      ///< Algorithm finalization
  virtual StatusCode execute   ();    ///< Algorithm execution

protected:
  //== extract a string from the buffer
  std::string stringFromData() {
    unsigned int len = *m_data++;
    m_size--;
    if ( 0 == len ) return "";
    len = len/4+1;
    std::string value( (char*) m_data );
    m_data += len;
    m_size -= len;
    return value;
  }

  //== extract an int from the buffer
  int nextInt() { m_size--; return *m_data++; }

  //== Extract the linkMgr information and set the version
  void processLinks( DataObject* obj, unsigned char version ) {
    DataObject* dummy = 0;
    unsigned int nbLinks = *m_data++;
    m_size--;
    for ( unsigned int kk=0; nbLinks > kk; ++kk ) {
      std::string name = stringFromData();
      obj->linkMgr()->addLink( name, dummy );
    }
    obj->setVersion( version );
  }

  //== Fill a vector of objects CLASS from the buffer and the blobs.
  template <class CLASS> void getFromBlob( std::vector<CLASS>& vect,
                                           const std::vector<LHCb::RawBank*>& blobs );

private:
  std::string   m_inputLocation;
  std::string   m_postfix;
  unsigned int* m_data = nullptr;
  int           m_size{0};
  ToolHandle<IGenericTool> m_odinDecoder;
  SmartIF<IAddressCreator> m_addrCreator;
  SmartIF<IDataManagerSvc> m_evtMgr;
};
#endif // READPACKEDDST_H

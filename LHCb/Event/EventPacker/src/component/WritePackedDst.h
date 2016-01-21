// $Id: WritePackedDst.h,v 1.2 2009-12-04 16:40:20 jonrob Exp $
#ifndef WRITEPACKEDDST_H
#define WRITEPACKEDDST_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "PackedBank.h"

/** @class WritePackedDst WritePackedDst.h
 *  Write the packed DST as a MDF file
 *
 *  @author Olivier Callot
 *  @date   2008-12-01
 */
class WritePackedDst : public GaudiAlgorithm
{

public:

  /// Standard constructor
  WritePackedDst( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~WritePackedDst( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  
protected:
 
  void storeInBlob( PackedBank& pBnk, 
                    const void* data, 
                    unsigned int nb, 
                    unsigned int bSize );

private:
  
  std::vector<std::string> m_containers;
  LHCb::RawEvent* m_dst = nullptr;
  unsigned int m_blobNumber{0};
  unsigned int m_bankNb{0};

};

#endif // WRITEPACKEDDST_H

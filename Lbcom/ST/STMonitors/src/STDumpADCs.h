// $Id: STDumpADCs.h,v 1.1 2010-03-10 14:41:23 mtobin Exp $
#ifndef STDUMPADCS_H 
#define STDUMPADCS_H 1

// Include files
// from Gaudi
#include "Kernel/STTupleAlgBase.h"

/** @class STDumpADCs STDumpADCs.h
 *  
 *
 *  @author Mark TOBIN
 *  @date   2006-06-12
 */
namespace ST
{
  class STDumpADCs : public TupleAlgBase {
  public: 
    /// Standard constructor
    STDumpADCs( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~STDumpADCs( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization


  protected:

  private:
    std::string m_dataLocation;
    std::vector<double> m_channelNumbers;
  };
} // ST namespace
#endif // STDUMPADCS_H

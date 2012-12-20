#ifndef STDUMPADCS_H 
#define STDUMPADCS_H 1

// Include files
// from Gaudi
#include "Kernel/STTupleAlgBase.h"

/** @class STDumpADCs STDumpADCs.h
 *  
 *  Dumps the ADC values for each TELL1 into a tuple
 *  ADC values can be accessed/scanned event by event using:
 * 
 *  TTree* adcs=(TTree*)gDirectory->Get("AlgName/ADCs");
 *  adcs->Draw("TELL34:channels","","",1,1)
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

  protected:

  private:
    std::string m_dataLocation;
    std::vector<double> m_channelNumbers;
  };
} // ST namespace
#endif // STDUMPADCS_H

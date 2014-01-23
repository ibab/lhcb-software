// $Id: HltLumiFillRawBuffer.h,v 1.1 2008-07-31 21:34:47 panmanj Exp $
#ifndef HLTLUMIFILLRAWBUFFER_H
#define HLTLUMIFILLRAWBUFFER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/RawEvent.h"

/** @class HltLumiFillRawBuffer HltLumiFillRawBuffer.h
 *  Fills the Raw Buffer banks for the LumiSummary
 *
 *  @author Jaap Panman
 *  @date   2004-07-22
 */
class HltLumiFillRawBuffer : public GaudiAlgorithm
{
  public:
    /// Standard constructor
    HltLumiFillRawBuffer( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~HltLumiFillRawBuffer(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution
    virtual StatusCode finalize();   ///< Algorithm finalization

  protected:
    void fillDataBankShort();

  private:
    std::string m_inputBank;

    int m_LastKey;
    // Statistics
    double m_totDataSize;
    int m_nbEvents;
    std::vector<unsigned int> m_bank;
};
#endif // HLTLUMIFILLRAWBUFFER_H

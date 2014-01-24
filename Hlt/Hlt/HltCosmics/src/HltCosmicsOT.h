// $Id: HltCosmicsOT.h,v 1.2 2009-12-23 15:07:41 graven Exp $
#ifndef INCLUDE_HLTCOSMICSOT_H
#define INCLUDE_HLTCOSMICSOT_H 1

#include <string>

#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "OTDAQ/IOTRawBankDecoder.h"

/** @class HltCosmicsOT HltCosmicsOT.h
 *
 * trigger on OT Activity (idea based on a suggestion by Dirk Wiedner)
 *
 * starting from a minimum number of hits in OT, we look for neighbouring
 * hits inside a module. if we find any, we accept the event and set a flag
 * that the event passed this algorithm (using the setFilterPassed(true) method,
 * otherwise, we use setFilterPassed(false))
 *
 * @author Manuel Tobias Schiller <schiller@physi.uni-heidelberg.de>
 * @date   2009-07-16
 *
 * 2009-09-08 : adapted by Stephan Nies <snies@cern.ch>
 *                   and Jesko Merkel <jmerkel@cern.ch>
 *
 */

class HltCosmicsOT : public HltAlgorithm
{
  public:
    /// Standard Constructor
    HltCosmicsOT( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~HltCosmicsOT(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm event execution

  private:
    Hlt::SelectionContainer0 m_selections;

    /// decoder so that we can ask quickly if there were any hits
    IOTRawBankDecoder* m_otdecoder;
    /// name of raw bank decoder to use
    std::string m_otdecodername;
    /// minimum number of hits in OT before we start decoding
    unsigned m_minhits;
    /// tolerance of straw sum test
    unsigned m_tolstraws;
};
#endif // INCLUDE_HLTCOSMICSOT_H

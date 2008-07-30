// $Id: HltRawBankFilter.h,v 1.2 2008-07-30 13:37:32 graven Exp $
#ifndef HLTRAWBANKFILTER_H 
#define HLTRAWBANKFILTER_H 1

// Include files
// from Gaudi

#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltSelectionContainer.h"
#include "GaudiKernel/Property.h"
#include "Event/RawBank.h"

/** @class HltRawBankFilter HltRawBankFilter.h
 *  
 *  functionality:
 *      give pass if the RawBank is present
 *
 *  Options:
 *      RawBankNames: list of names of the RawBank required to be present
 *                      if no names are listed, all events are accepted
 *
 *  @author Gerhard Raven
 *  @date   2008-07-01
 */
class HltRawBankFilter : public HltAlgorithm {
public: 

  /// Standard constructor
  HltRawBankFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltRawBankFilter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  class BankTypeList {
  public:
      typedef StringArrayProperty property_type;
      typedef std::vector<LHCb::RawBank::BankType>::const_iterator iter_type;
      BankTypeList() { m_property.declareUpdateHandler( &BankTypeList::updateHandler, this ); }
      BankTypeList(const BankTypeList& rhs) 
        : m_property( rhs.m_property ) , m_types( rhs.m_types )
      { m_property.declareUpdateHandler( &BankTypeList::updateHandler, this ); }
      iter_type begin() const { return m_types.begin(); }
      iter_type end() const { return m_types.end(); }
      property_type& property() { return m_property; }
  private:
      void updateHandler(Property& prop);
      property_type                        m_property;
      std::vector<LHCb::RawBank::BankType> m_types;
  };

  std::string         m_rawEventLocation;
  BankTypeList        m_rawBanks;
  Hlt::SelectionContainer0 m_selection;
};
#endif

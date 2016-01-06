#ifndef EVTTYPEINFO_H 
#define EVTTYPEINFO_H 1

// Include files

/** @class EvtTypeInfo EvtTypeInfo.h
 *  
 *  Private class holding all the info relative to an MC Event type
 *
 *  @author Gloria CORTI
 *  @date   2005-07-30
 */
class EvtTypeInfo final {
public: 

  /// Standard constructor
  EvtTypeInfo( ) = default;

  /* Constructor with arguments
   * @param evtCode   integer describing eventtype
   * @param nickName  short description of decay
   * @param decayDesc ASCII detailed description of decay,
   *                  usable by (MC)DecayFinder
   */
  EvtTypeInfo( int evtCode, std::string nickName, std::string decayDesc)
    : m_evtCode(evtCode)
    , m_nickName(std::move(nickName))
    , m_decayDescriptor(std::move(decayDesc))
  {
  };

  /// Accessor to event type integer code
  int evtCode() const 
  { 
    return m_evtCode; 
  }

  /// Accessor 
  const std::string& nickName() const 
  { 
    return m_nickName; 
  }

  /// Accessor 
  const std::string& decayDescriptor() const
  {
    return m_decayDescriptor;
  }

  /// Setter methods
  void setEvtCode( int value ) 
  {
    m_evtCode = value;
  }
  
  /// Setter  
  void setNickName( const std::string& value )
  {
    m_nickName = value;
  }
  
  /// Setter 
  void setDecayDescriptor( std::string value )
  {
    m_decayDescriptor = value;
  }

private:

  int         m_evtCode = 0;           ///< integer code describing event type
  std::string m_nickName;          ///< nick name 
  std::string m_decayDescriptor;   ///< decay descriptor

};

#endif // EVTTYPEINFO_H

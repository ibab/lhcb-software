#ifndef _DeTTHalfModule_H_
#define _DeTTHalfModule_H_

#include <vector>
#include <string>

#include "Kernel/STChannelID.h"
#include "STDet/DeSTBaseElement.h"

class DeTTSector;

/** @class DeTTHalfModule DeTTHalfModule.h "STDet/DeTTHalfModule.h"
 *
 *  Class representing a TT Half module (7 sensors ganged together)
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 * \li <a href="http://doc.cern.ch//archive/electronic/cern/others/LHB/internal/lhcb-2006-034.pdf"><b>LHCb note on STDet</b></a>
   \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
 **
 */

static const CLID CLID_DeTTHalfModule = 9110;

class DeTTHalfModule : public DeSTBaseElement  {

public:

  /** parent type */
  typedef STDetTraits<DeTTHalfModule>::parent parent_type;

  /** child type */
  typedef STDetTraits<DeTTHalfModule>::child child_type;

  /** children */
  typedef std::vector<child_type*> Children;

  /** Constructor */
  DeTTHalfModule ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeTTHalfModule();

  /**
  * Retrieves reference to class identifier
  * @return the class identifier for this class
  */
  static const CLID& classID(){return CLID_DeTTHalfModule;}

  /**
  * another reference to class identifier
  * @return the class identifier for this class
  */
  const CLID& clID () const;

  /** initialization method
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

  /** region where module is located
  * @return m_region
  */
  unsigned int detRegion() const;

  /// Workaround to prevent hidden base class function
  inline const std::type_info& type(const std::string &name) const
                      {return ParamValidDataObject::type(name);}
  /** indicate the module type (KLM, LM)
  * @return m_type
  */
  std::string type() const;

  /** indicate the module position (T, B)
  * @return m_position
  */
  std::string position() const;

  /** first readout sector on module
  * @return m_firstSector
  */
  unsigned int firstSector() const;

  /** last readout sector on module
  * @return m_firstSector
  */
  unsigned int lastSector() const;

  /** test whether contains channel
  * @param  aChannel test channel
  * @return bool
  */
  virtual bool contains(const LHCb::STChannelID aChannel) const;

  /** print to stream */
  std::ostream& printOut( std::ostream& os ) const;

  /** print to stream */
  MsgStream& printOut( MsgStream& os) const;

  /**  locate sector based on a channel id
  @return  sector */
  DeTTSector* findSector(const LHCb::STChannelID aChannel);

  /** locate sector based on a point
  @return sector */
  DeTTSector* findSector(const Gaudi::XYZPoint& point) ;

  /** children */
  const DeTTHalfModule::Children& sectors() const;

  /** column number */
  unsigned int column() const;

  /** production id */
  unsigned int prodID() const;

  /**
  * fraction active channels
  * @return bool fraction active
  */
  double fractionActive() const;

  /** version */
  std::string versionString() const;

protected:

  StatusCode updateProdIDCondition();

private:

  unsigned int m_detRegion;
  unsigned int m_firstSector;
  unsigned int m_column;
  std::string m_type;
  std::string m_position;
  parent_type* m_parent;
  Children m_sectors;
  unsigned int m_prodID;
  std::string m_versionString;
  std::string m_prodIDString;
};

inline const DeTTHalfModule::Children& DeTTHalfModule::sectors() const{
  return m_sectors;
}

inline unsigned int DeTTHalfModule::detRegion() const {
  return m_detRegion;
}

inline std::string DeTTHalfModule::type() const {
  return m_type;
}

inline std::string DeTTHalfModule::position() const {
  return m_position;
}

inline unsigned int DeTTHalfModule::lastSector() const {
  return firstSector()+m_sectors.size()-1u;
}

inline unsigned int DeTTHalfModule::firstSector() const {
  return m_firstSector;
}

inline unsigned int DeTTHalfModule::column() const{
  return m_column;
}

inline unsigned int DeTTHalfModule::prodID() const{
  return m_prodID;
}

inline std::string DeTTHalfModule::versionString() const
{
  return m_versionString;
}


/** ouput operator for class DeTTHalfModule
 *  @see DeHalfModule
 *  @see MsgStream
 *  @param os      reference to STL output stream
 *  @param aModule reference to DeTTHalfModule object
 */
inline std::ostream& operator<<( std::ostream& os , const DeTTHalfModule* aModule )
{ return aModule->printOut( os ); }


/** ouput operator for class DeTTHalfModule
 *  @see DeTTHalfModule
 *  @see MsgStream
 *  @param os      reference to MsgStream output stream
 *  @param aModule reference to DeTTHalfModule object
 */
inline MsgStream& operator<<( MsgStream& os , const DeTTHalfModule* aModule )
{ return aModule->printOut( os ); }


#endif // _DeTTHalfModule_H

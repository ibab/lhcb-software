// $Id: DeSTStation.h,v 1.5 2009-03-14 09:18:07 mneedham Exp $
#ifndef _DeSTStation_H_
#define _DeSTStation_H_

// STL
#include <string>
#include <vector>

#include "Kernel/STChannelID.h"

#include "STDet/DeSTBaseElement.h"

/// GaudiKernel
#include "GaudiKernel/MsgStream.h"

class DeSTLayer;

/** @class DeSTStation DeSTStation.h "STDet/DeSTStation.h"
 *
 *  Base class for TT and IT Station Detector elements
 *
 *  @author Matthew Needham Matthew.Needham@cern.ch
 *
 * <b> Additional information: </b>
 * \li <a href="http://doc.cern.ch//archive/electronic/cern/others/LHB/internal/lhcb-2006-034.pdf"><b>LHCb note on STDet</b></a>
   \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
 *
 */

class DeSTStation : public DeSTBaseElement {

public:

  /** Constructor */
  DeSTStation ( const std::string& name = "" ) ;

  /** Destructor */
  virtual ~DeSTStation();

  /** initialization method
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

  /** station identifier
  *  @return identifier
  */
  unsigned int id() const;

  /** print to stream */
  std::ostream& printOut( std::ostream& os ) const;

  /** print to stream */
  MsgStream& printOut( MsgStream& os) const;

  /** check contains channel
  *  @param  aChannel channel
  *  @return bool
  */
  virtual bool contains(const LHCb::STChannelID aChannel) const;

  /**
  * Nickname for the station
  **/
  const std::string& nickname() const ;

 protected:

 std::string m_nickname;

 private:

  unsigned int m_id;

};

inline unsigned int DeSTStation::id() const{
  return m_id;
}

inline bool DeSTStation::contains(const LHCb::STChannelID aChannel) const{
  return elementID().station() == aChannel.station();
}

inline const std::string& DeSTStation::nickname() const{
  return m_nickname;
}

/** ouput operator for class DeSTStation
 *  @see DeSTStation
 *  @see MsgStream
 *  @param os       reference to STL output stream
 *  @param aStation reference to DeSTStation object
 */
inline std::ostream& operator<<( std::ostream& os , const DeSTStation* aStation )
{ return aStation->printOut( os ); }


/** ouput operator for class DeSTStation
 *  @see DeSTStation
 *  @see MsgStream
 *  @param os       reference to MsgStream output stream
 *  @param aStation reference to DeSTStation object
 */
inline MsgStream& operator<<( MsgStream& os , const DeSTStation* aStation )
{ return aStation->printOut( os ); }


#endif // _DeSTStation_H

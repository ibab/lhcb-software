// $Id: DeSTStation.h,v 1.1 2005-12-19 15:18:24 mneedham Exp $
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
  *  @param channel
  *  @return bool
  */
  bool contains(const LHCb::STChannelID aChannel) const;

 private:

  unsigned int m_id;
};

inline unsigned int DeSTStation::id() const{
  return m_id;
}

inline bool DeSTStation::contains(const LHCb::STChannelID aChannel) const{
  return elementID().station() == aChannel.station();
}

/** ouput operator for class DeSTStation
 *  @see DeSTStation
 *  @see MsgStream
 *  @param os reference to STL output stream
 *  @param de reference to DeSTStation object
 */
inline std::ostream& operator<<( std::ostream& os , const DeSTStation* aStation )
{ return aStation->printOut( os ); }


/** ouput operator for class DeSTStation
 *  @see DeSTStation
 *  @see MsgStream
 *  @param os reference to MsgStream output stream
 *  @param de reference to DeSTStation object
 */
inline MsgStream& operator<<( MsgStream& os , const DeSTStation* aStation )
{ return aStation->printOut( os ); }

#endif // _DeSTStation_H





// $Id: DeSTLayer.h,v 1.11 2008-07-14 07:38:36 mneedham Exp $
#ifndef _DeSTLayer_H_
#define _DeSTLayer_H_

#include <string>

#include "Kernel/STChannelID.h"
#include "STDet/DeSTBaseElement.h"
#include "GaudiKernel/Plane3DTypes.h"

/** @class DeSTLayer DeSTLayer.h "STDet/DeSTLayer.h"
 *
 *  Base class for TT and IT Layer Detector elements
 *
 * <b> Additional information: </b>
 * \li <a href="http://doc.cern.ch//archive/electronic/cern/others/LHB/internal/lhcb-2006-034.pdf"><b>LHCb note on STDet</b></a>
   \li  <a href="http://ckm.physik.unizh.ch/software"><b>ST Software page</b></a><p>
 *  @author Matthew Needham Matthew.Needham@cern.ch
 */

class DeSTSector;

class DeSTLayer : public DeSTBaseElement  {

public:

  typedef std::vector<DeSTSector*> Sectors;

  /** Constructor */
  DeSTLayer ( const std::string& name = "" );

  /** Destructor */
  virtual ~DeSTLayer();

  /** initialization method
  * @return Status of initialisation
  */
  virtual StatusCode initialize();

  /** layer identifier
  *  @return identifier
  */
  unsigned int id() const;

  /** stereo angle
  *  @return identifier
  */
  double angle() const;

  /** cosine stereo angle
  *  @return identifier
  */
  double cosAngle() const;

  /** sine stereo angle
  *  @return identifier
  */
  double sinAngle() const;


  /** print to stream */
  std::ostream& printOut( std::ostream& os ) const;

  /** print to stream */
  MsgStream& printOut( MsgStream& os) const;

  /** check contains channel
  *  @param  aChannel channel
  *  @return bool
  */
  virtual bool contains(const LHCb::STChannelID aChannel) const = 0;

  /** flat vector of sectors
  * @return vector of sectors
  */
  const Sectors& sectors() const;

  /** plane corresponding to the sector
  * @return the plane
  */
  Gaudi::Plane3D plane() const;

  /**
  * Nickname for the layer
  **/
  const std::string& nickname() const;



protected:

  Sectors m_sectors;
  std::string m_nickname;

private:

  StatusCode cachePlane();

  Gaudi::Plane3D m_plane;

  unsigned int m_id;
  double m_angle;
  double m_sinAngle;
  double m_cosAngle;

};

inline unsigned int DeSTLayer::id() const{
  return m_id;
}

inline double DeSTLayer::angle() const{
  return m_angle;
}

inline double DeSTLayer::cosAngle() const{
  return m_cosAngle;
}

inline double DeSTLayer::sinAngle() const{
  return m_sinAngle;
}

inline const DeSTLayer::Sectors& DeSTLayer::sectors() const{
  return m_sectors;
}

inline Gaudi::Plane3D DeSTLayer::plane() const {
  return m_plane;
}

/** ouput operator for class DeSTLayer
 *  @see DeSTLayer
 *  @see MsgStream
 *  @param os     reference to STL output stream
 *  @param aLayer reference to DeSTLayer object
 */
inline std::ostream& operator<<( std::ostream& os , const DeSTLayer* aLayer )
{ return aLayer->printOut( os ); }


/** ouput operator for class DeSTLayer
 *  @see DeSTLayer
 *  @see MsgStream
 *  @param os     reference to MsgStream output stream
 *  @param aLayer reference to DeSTLayer object
 */
inline MsgStream& operator<<( MsgStream& os , const DeSTLayer* aLayer )
{ return aLayer->printOut( os ); }

inline const std::string& DeSTLayer::nickname() const{
  return m_nickname;
}

#endif // _DeSTLayer_H









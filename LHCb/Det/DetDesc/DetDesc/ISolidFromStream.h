#ifndef    DETDESC_ISOLIDFROMSTREAM_H
#define    DETDESC_ISOLIDFROMSTREAM_H 1 


#include <functional> 

class StreamBuffer;
class ISolid; 
class LVolume; 
///
/// small helper class to implement "virtual constructor" for Solids 
///

class ISolidFromStream: public std::unary_function<StreamBuffer&,ISolid*>
{
  ///
  friend class LVolume;
  ///
 public:
  ///
  ISolid* operator()( StreamBuffer& s )  const;
  ///  
}; 


#endif  // DETDESC_ISOLIDFROMSTREAM_H

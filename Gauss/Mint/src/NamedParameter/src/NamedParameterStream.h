#ifndef NAMEDPARAMETERSTREAM_HH
#define NAMEDPARAMETERSTREAM_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

/* 
   The NamedParameterStream is essentially a stringstream, but
   creating my own class allows specialisations on how
   certain data types are read in by NamedParameter
   (previously attempted with template
   specialisation, of NamedParameter, but compiler won't have it)
*/

#include <string>
#include <sstream>

namespace MINT{
class NamedParameterStream : public std::stringstream{
 public:
  explicit NamedParameterStream( openmode which = ios_base::out|ios_base::in )
    : std::stringstream(which){}
  explicit NamedParameterStream( const std::string & str,
				 openmode which = ios_base::out|ios_base::in )
    : std::stringstream(str, which){}

  //  NamedParameterStream(const NamedParameterStream& other)
  //    : std::stringstream(other){}
};

NamedParameterStream& operator>>(NamedParameterStream& eam
				 , std::string& ing
				 );

}// namespace MINT
#endif
//

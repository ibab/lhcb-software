// $Id: RichAlgorithmType.h,v 1.1 2002-06-21 15:33:02 jonesc Exp $
#ifndef RICHKERNEL_RICHALGORITHMTYPE_H
#define RICHKERNEL_RICHALGORITHMTYPE_H 1

// Include files
#include <string>
#include <vector>

// from gaudi
#include "GaudiKernel/MsgStream.h"

/** @enumeration RichAlgorithmType RichAlgorithmType.h RichKernel/RichAlgorithmType.h
 *
 *  Enumeration for particle ID algorithm types
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-06-19
 */

namespace Rich {

  /// Enumeration for particle ID algorithm types
  enum AlgorithmType {
    SummaryAlgorithm = 0,
    GlobalAlgorithm,
    LocalAlgorithm
  };

  /// Text conversion for Rich::AlgorithmType enumeration
  std::string text( const Rich::AlgorithmType& algorithm ) ;

  /// Std Vector typedef
  typedef std::vector<AlgorithmType> AlgorithmTypeVector;

}

/// Implement textual ostream << method for Rich::AlgorithmType enumeration
inline ostream& operator << ( ostream& s,
                              const Rich::AlgorithmType& algorithm ) {
  s << Rich::text( algorithm );
  return s;
}

/// Implement textual MsgStream << method for Rich::AlgorithmType enumeration
inline MsgStream& operator << ( MsgStream& s,
                                const Rich::AlgorithmType& algorithm ) {
  s << Rich::text( algorithm );
  return s;
}

#endif // RICHKERNEL_RICHALGORITHMTYPE_H

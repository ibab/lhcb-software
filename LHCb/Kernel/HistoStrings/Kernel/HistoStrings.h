// $Id: HistoStrings.h,v 1.1.1.1 2009-10-22 09:17:02 ibelyaev Exp $
// ============================================================================
#ifndef KERNEL_HISTOSTRINGS_H 
#define KERNEL_HISTOSTRINGS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <string>
#include <iosfwd>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
// ============================================================================
// forward declarations 
// ============================================================================
// Forward declarations
// ============================================================================
// AIDA
// ============================================================================
namespace AIDA 
{ 
  class IHistogram1D ; 
  class IHistogram2D ; 
  class IHistogram3D ; 
  class IProfile1D   ; 
  class IProfile2D   ; 
}
// ============================================================================
// ROOT 
// ============================================================================
class TH1D ;                                                            // ROOT 
class TH2D ;                                                            // ROOT 
class TH3D ;                                                            // ROOT 
class TProfile   ;                                                      // ROOT 
class TProfile2D ;                                                      // ROOT 
// ============================================================================
namespace Gaudi 
{
  // ==========================================================================
  namespace Utils
  {
    // ========================================================================
    namespace Histos 
    {
      // ======================================================================
      /** @class HistoStrings
       *  Helepr class to produce "good" Reflex dictionries 
       *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
       *  @date 2009-09-26
       */
      class GAUDI_API HistoStrings 
      {
        // ====================================================================
      public:
        // ====================================================================
        /** convert the histogram into the string 
         *  @param histo  (INPUT)  the histogram to be streamed 
         *  @param asXML  (INPUT)  use XML-format 
         *  @return the string representation of the histogram 
         *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
         *  @date 2009-09-26
         */
        static std::string toString 
        ( const TH1D&               histo         , 
          const bool                asXML = false ) ;
        // ====================================================================
        /** convert the histogram into the string 
         *  @param histo  (INPUT)  the histogram to be streamed 
         *  @param asXML  (INPUT)  use XML-format 
         *  @return the string representation of the histogram 
         *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
         *  @date 2009-09-26
         */
        static std::string toString 
        ( const TH2D&               histo         , 
          const bool                asXML = false ) ;
        // ====================================================================
        /** convert the histogram into the string 
         *  @param histo  (INPUT)  the histogram to be streamed 
         *  @param asXML  (INPUT)  use XML-format 
         *  @return the string representation of the histogram 
         *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
         *  @date 2009-09-26
         */
        static std::string toString 
        ( const AIDA::IHistogram1D& histo         , 
          const bool                asXML = false ) ;
        // ====================================================================
        /** convert the histogram into the string 
         *  @param histo  (INPUT)  the histogram to be streamed 
         *  @param asXML  (INPUT)  use XML-format 
         *  @return the string representation of the histogram 
         *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
         *  @date 2009-09-26
         */
        static std::string toString 
        ( const AIDA::IHistogram2D& histo         , 
          const bool                asXML = false ) ;
        // ====================================================================
        /** parse the histogram from string
         *  @param result (OUTPUT)  the parsed histogram 
         *  @param input  (INPUT)   the input XML string 
         *  @return status code 
         */
        static StatusCode fromString
        ( TH1D& result , const std::string& input ) ;
        // ====================================================================
        /** parse the histogram from string
         *  @param result (OUTPUT)  the parsed histogram 
         *  @param input  (INPUT)   the input XML string 
         *  @return status code 
         */
        static StatusCode fromString
        ( AIDA::IHistogram1D& result , const std::string& input ) ;
        // ====================================================================
        /** parse the histogram from string
         *  @param result (OUTPUT)  the parsed histogram 
         *  @param input  (INPUT)   the input XML string 
         *  @return status code 
         */
        static StatusCode fromString
        ( TH2D& result , const std::string& input ) ;
        // ====================================================================
        /** parse the histogram from string
         *  @param result (OUTPUT)  the parsed histogram 
         *  @param input  (INPUT)   the input XML string 
         *  @return status code 
         */
        static StatusCode fromString
        ( AIDA::IHistogram2D& result , const std::string& input ) ;
        // ====================================================================
        static std::string toXml ( const TH1D&               histo ) ;
        static std::string toXml ( const TH2D&               histo ) ;
        static std::string toXml ( const TH3D&               histo ) ;
        static std::string toXml ( const TProfile&           histo ) ;
        static std::string toXml ( const TProfile2D&         histo ) ;
        // ====================================================================
        static std::string toXml ( const AIDA::IHistogram1D& histo ) ;
        static std::string toXml ( const AIDA::IHistogram2D& histo ) ;
        static std::string toXml ( const AIDA::IHistogram3D& histo ) ;
        static std::string toXml ( const AIDA::IProfile1D&   histo ) ;
        static std::string toXml ( const AIDA::IProfile2D&   histo ) ;
        // ====================================================================
        static StatusCode fromXml 
        ( TH1D&               result , const std::string& input ) ;
        static StatusCode fromXml
        ( TH2D&               result , const std::string& input ) ;
        static StatusCode fromXml 
        ( TH3D&               result , const std::string& input ) ;
        static StatusCode fromXml
        ( TProfile&           result , const std::string& input ) ;
        static StatusCode fromXml
        ( TProfile2D&         result , const std::string& input ) ;
        // ====================================================================
        static StatusCode fromXml 
        ( AIDA::IHistogram1D& result , const std::string& input ) ;
        static StatusCode fromXml 
        ( AIDA::IHistogram2D& result , const std::string& input ) ;
        static StatusCode fromXml 
        ( AIDA::IHistogram3D& result , const std::string& input ) ;
        static StatusCode fromXml 
        ( AIDA::IProfile1D&   result , const std::string& input ) ;
        static StatusCode fromXml 
        ( AIDA::IProfile2D&   result , const std::string& input ) ;
        // ====================================================================
      };  
      // ======================================================================
    } //                                  end of namespace Gaudi::Utils::Histos 
    // ========================================================================
  } //                                            end of namespace Gaudi::Utils
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_HISTOSTRINGS_H
// ============================================================================

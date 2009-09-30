// $Id: CaloCellIDAsProperty.h,v 1.1 2009-09-30 13:18:43 ibelyaev Exp $
// ============================================================================
#ifndef CALOUTILS_CALOCELLIDASPROPERTY_H 
#define CALOUTILS_CALOCELLIDASPROPERTY_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <iosfwd>
#include <string>
#include <vector>
#include <map>
// ============================================================================
// LHCbKernel
// ============================================================================
namespace LHCb { class CaloCellID ; }
// ============================================================================
/** @file 
 *  Streamer& Parsing function to allos CaloCellID & related classes to be 
 *  used as properteis for Gaudi components 
 *  @see class LHCb::CaloCellID
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-09-29
 */ 
// ============================================================================
//                                                                    Streamers 
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    /** put CellID into the output stream 
     *  @see LHCb::CaloCellID
     *  @see LHCb::CaloCellID::fillStream 
     *  @param object (INPUT)   object to be streamed 
     *  @param stream (OUTPUT) the stream to be updated 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-29
     */
    GAUDI_API
    std::ostream& toStream 
    ( LHCb::CaloCellID& object , 
      std::ostream&     stream ) ;
    // ========================================================================
    /** convert cellID into string 
     *  @param object (INPUT)   object to be streamed 
     *  @param stream (OUTPUT) the stream to be updated 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-29
     */
    GAUDI_API
    std::string toString ( const LHCb::CaloCellID& object ) ;
    // ========================================================================
  } //                                            end of namespace Gaudi::Utils
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
//                                                                      Parsers 
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Parsers 
  {
    // ========================================================================
    /** parse cellID from the string 
     *  @param result (OUPUT) the parsed cellID 
     *  @param input  (INPUT) the input string
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-29
     */
    GAUDI_API
    StatusCode parse 
    ( LHCb::CaloCellID&  result , 
      const std::string& input  ) ;    
    // ========================================================================
    /** parse the vector of cellIDs from the string 
     *  @param result (OUPUT) the parsed vector of cellIDs 
     *  @param input  (INPUT) the input string
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-29
     */
    GAUDI_API
    StatusCode parse 
    ( std::vector<LHCb::CaloCellID>& result , 
      const std::string&             input  ) ;
    // ========================================================================    
    /** parse the map of  { cellID : double } from the string 
     *  @param result (OUPUT) the parsed map { cellID : double } 
     *  @param input  (INPUT) the input string
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-29
     */
    GAUDI_API
    StatusCode parse 
    ( std::map<LHCb::CaloCellID,double>& result , 
      const std::string&                 input  ) ;    
    // ========================================================================
    /** parse the map of  { cellID : vector<double> } from the string 
     *  @param result (OUPUT) the parsed map { cellID : vetcor<double> } 
     *  @param input  (INPUT) the input string
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-29
     */
    GAUDI_API
    StatusCode parse 
    ( std::map<LHCb::CaloCellID,std::vector<double> >& result , 
      const std::string&                               input  ) ;    
    // ========================================================================
  } //                                          end of namespace Gaudi::Parsers 
  // ==========================================================================
} //                                                     end of namespace Gaudi
// ============================================================================
//                                                      Another name for parser
// ============================================================================
namespace CaloCellCode 
{
  // ==========================================================================
  class GAUDI_API Cell2String
  {
  public:
    // ========================================================================
    /** parse cellID from the string 
     *  @param result (OUPUT) the parsed cellID 
     *  @param input  (INPUT) the input string
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-29
     */
    static StatusCode cellFromString 
    ( LHCb::CaloCellID&  result , 
      const std::string& input  ) ;    
    // ========================================================================
    /** convert cellID into string 
     * @param  cell (INPUT) cell to be converted 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-09-29
     */
    static std::string cellToString ( const LHCb::CaloCellID& cell ) ;
    // ========================================================================
  } ;
  // ==========================================================================
} //                                              end of namespace CaloCellCode
// ============================================================================
// The END 
// ============================================================================
#endif // CALOCELLIDASPROPERTY_H
// ============================================================================

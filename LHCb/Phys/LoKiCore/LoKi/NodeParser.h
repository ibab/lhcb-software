// $Id$
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $ , version $Reviison:$
// ============================================================================
#ifndef DECAYS_NODEPARSER_H 
#define DECAYS_NODEPARSER_H 1
// ============================================================================
// Include files
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  namespace Parsers 
  {
    // ========================================================================
    /** parse the decay node
     *  @param node      (OUTPUT) the decay node 
     *  @param input     (INPUT)  the input string to be parsed 
     *  @param symbols   (INPUT)  the list of known predefiend symbols 
     *  @param particles (INPUT)  the list of particle 
     *  @param stream    the sterem to report the errors
     */
    StatusCode parse 
    ( Decays::Node&                   node      , 
      const std::string&              input     ,
      const std::vector<std::string>& symbols   ,
      const std::vector<std::string>& particles , 
      std::ostream&                   stream    ) ;
    // ========================================================================
  } // end of namespace Decays::Parsers 
  // ==========================================================================
} // end of namespace Decays
// ============================================================================
// The END 
// ============================================================================
#endif // DECAYS_NODEPARSER_H
// ============================================================================

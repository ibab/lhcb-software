// $Id$
// ============================================================================
#ifndef DECAYS_TREEPARSER_H 
#define DECAYS_TREEPARSER_H 1
// ============================================================================
// Include files
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  namespace Parsers 
  {
    // ========================================================================
    /** parse the decay tree into the helper structue
     *  @param tree   (OUTPUT) the representation of the decay tree 
     *  @param input  the input string to be parsed 
     *  @param symbols  the list of prededined symbols 
     *  @param paricles the list of particles  
     *  @param stream   the stream to report the errors 
     */
    StatusCode parse 
    ( Decays::Parsers::Tree&          tree      , 
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
#endif // DECAYS_TREEPARSER_H
// ============================================================================

// $Id: H1.h,v 1.1.1.1 2009-10-22 09:17:02 ibelyaev Exp $
// ============================================================================
#ifndef HISTOSTRINGS_H1_H 
#define HISTOSTRINGS_H1_H 1
// ============================================================================
// Include files
// ============================================================================
namespace 
{
  // ==========================================================================
  /// the actual type to represent the bin content 
  typedef std::pair<double,double> Bin ;
  /// vector of bins 
  typedef std::vector<Bin>         Bins ;
  // ==========================================================================
  /** @struct Edges 
   *  helper structure to represent the edges 
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-10-21
   */
  struct Edges 
  {
    //
    void setHighEdge ( const double               value ) { high  = value ; }
    void setLowEdge  ( const double               value ) { low   = value ; }
    void setNBins    ( const unsigned int         value ) { nbins = value ; }
    void setEdges    ( const std::vector<double>& value ) { edges = value ; }
    //
    Edges& operator -= ( const double value ) 
    { setLowEdge  ( value ) ; return *this ; }
    Edges& operator += ( const double value ) 
    { setHighEdge ( value ) ; return *this ; }
    Edges& operator *= ( const std::vector<double>& value ) 
    { setEdges    ( value ) ; return *this ; }
    Edges& operator /= ( const unsigned int value  ) 
    { setNBins    ( value ) ; return *this ; }
    //
    bool ok () const 
    {
      return 
        edges.empty() ?  low < high && 0 < nbins : 2 < edges.size() ;
    }
    unsigned int nBins() const 
    {
      return 
        edges.empty() ? nbins : edges.size() - 1 ;
    } 
    //
    double       low   ;
    double       high  ;
    unsigned int nbins ;
    std::vector<double> edges ;
  };  
  // ==========================================================================
  /** @struct H1
   *  the trivial representation of the 1D-histogram
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-10-21
   */
  struct H1 
  {
    //
    void setName  ( const std::string& value ) { m_name   = value ; }
    void setTitle ( const std::string& value ) { m_title  = value ; }
    void setEdges ( const Edges&       value ) { m_edges  = value ; }
    void setBins  ( const Bins&        value ) { m_bins   = value ; }
    //
    H1& operator*=( const std::string& value ) { setName  ( value ) ; return *this ; }
    H1& operator/=( const std::string& value ) { setTitle ( value ) ; return *this ; }
    H1& operator&=( const Edges&       value ) { setEdges ( value ) ; return *this ; }
    H1& operator+=( const Bins&        value ) { setBins  ( value ) ; return *this ; }
    //
    bool ok  () const 
    {
      if (  m_bins.empty() ) { return false ; }
      if ( !m_edges.ok  () ) { return false ; }
      if (  m_bins.size() != m_edges.nBins()  + 2 ) { return false ; }
      return true ;
    }  
    //
    std::string m_name  ;
    std::string m_title ;
    Edges       m_edges ;
    Bins        m_bins  ;
    
  } ;
  // ==========================================================================
  /** @struct H2
   *  the trivial representation of the 2D-histogram
   *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date 2009-10-21
   */
  struct H2
  {
    //
    void setName   ( const std::string& value ) { m_name   = value ; }
    void setTitle  ( const std::string& value ) { m_title  = value ; }
    void setXEdges ( const Edges&       value ) { m_xedges = value ; }
    void setYEdges ( const Edges&       value ) { m_yedges = value ; }
    void setBins   ( const Bins&        value ) { m_bins   = value ; }
    //
    H2& operator*=( const std::string& value ) { setName   ( value ) ; return *this ; }
    H2& operator/=( const std::string& value ) { setTitle  ( value ) ; return *this ; }
    H2& operator&=( const Edges&       value ) { setXEdges ( value ) ; return *this ; }
    H2& operator|=( const Edges&       value ) { setYEdges ( value ) ; return *this ; }
    H2& operator+=( const Bins&        value ) { setBins   ( value ) ; return *this ; }
    //
    bool ok  () const 
    {
      if (  m_bins.empty () ) { return false ; }
      if ( !m_xedges.ok  () ) { return false ; }
      if ( !m_yedges.ok  () ) { return false ; }
      if (  m_bins.size() != 
            ( m_xedges.nBins()  + 2 ) * 
            ( m_yedges.nBins()  + 2 ) ) { return false ; }
      return true ;
    }  
    //    
    //
    std::string m_name   ;
    std::string m_title  ;
    Edges       m_xedges ;
    Edges       m_yedges ;
    Bins        m_bins   ;
  } ;
  // ==========================================================================
} // end of anonymous namespace 
// ============================================================================
// The END 
// ============================================================================
#endif // HISTO_STRINGS_H1_H
// ============================================================================

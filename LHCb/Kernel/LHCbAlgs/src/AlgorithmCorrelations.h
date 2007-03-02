// $Id: AlgorithmCorrelations.h,v 1.1 2007-03-02 08:49:26 cattanem Exp $
#ifndef ALGORITHMCORRELATIONS_H 
#define ALGORITHMCORRELATIONS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IAlgorithmCorrelations.h"            // Interface

/** @class AlgorithmCorrelations AlgorithmCorrelations.h
 *  
 *  Tool to print a table of correlations of Bernoullian variables. See
 *  Interface doxygen.
 *  Options for this implementation:
 *    If one wants to declare the algorithm directly to the tool (ignoring
 *    the corresponding methods):
 *      Algorithms = { "alg1", "alg2"... }
 *      AlgorithmsRow, optional, for non-squere matrices.
 *    Other options:
 *      OnlyNonZero = true ; ///< skips algorithms with eff=0 in table
 *      Decimals = -1 ; ///< defines precision. -1 means automatic precision.
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-04-19
 */
class AlgorithmCorrelations : public GaudiTool, virtual public IAlgorithmCorrelations {
  class AlgoMatrix ;
  class AlgoResult ;
public: 
  /// Standard constructor
  AlgorithmCorrelations( const std::string& type, 
                         const std::string& name,
                         const IInterface* parent);

  virtual ~AlgorithmCorrelations( ); ///< Destructor
  StatusCode initialize() ;
  StatusCode algorithms(const std::vector<std::string>&)  ;
  StatusCode algorithmsRow(const std::vector<std::string>&);
  StatusCode printTable(void) ;
  StatusCode printList(void) ;
  ///< Fill results one by one for each algorithm
  StatusCode fillResult(const std::string& algo,const bool& result) ;
  ///< Actually tell the tool that we reached the end of the event (only for one-by-one filling)
  StatusCode endEvent() ;

protected:
  StatusCode reset(void); // reset everything
  bool isEffective(const std::string& ); ///< Algo did something
  int happyAlgorithms(void); ///< Number of algos that did something
  int getDecimals(void); ///< Number of significant decimals
  StatusCode addResults(void); // add all results
  StatusCode resetAlgoResult(std::vector<AlgoResult>&);
  StatusCode fillResult(const std::string&,const bool&,std::vector<AlgoResult>&) ;
  
private:
  std::vector<std::string> m_algorithmsRow ; ///< Algorithms to check
  std::vector<std::string> m_algorithmsColumn ; ///< Algorithms to against (= Row by default)
  std::vector<AlgoMatrix> m_SelResultMatrices ; ///< Pairs of correlations
  std::vector<AlgoResult> m_rowResults; ///< results of algorithms in this event
  std::vector<AlgoResult> m_columnResults; ///< results of algorithms in this event
  
  int m_longestName ; ///< Longest algorithm name
  bool m_minimize ; ///< Use mimimal table width
  int m_decimals ; ///< Number of decimals 

  // container of results for one algorithm
  class AlgoResult{
  public:
    AlgoResult(){m_algo = "Undefined";reset(); return;};
    AlgoResult(std::string& algo){m_algo = algo;reset(); return;};
    ~AlgoResult(){};
   
    std::string algo()const{return m_algo;};
    bool result()const{return m_result;};
    StatusCode setResult(const bool& b){
      if (m_updated) return StatusCode::FAILURE ;
      m_result = b;
      return StatusCode::SUCCESS;};
    void setAlgo(const std::string& a){m_algo = a;return;};
    void reset(){m_result = false ; m_updated = false ; return ;} ///< Reset for next event

  private:
    std::string m_algo; ///< Algo name
    bool m_result; ///< Result in this event
    bool m_updated; ///< Already in this event?
  };
  
  // container of SelResults matrix for two algorithms
  class AlgoMatrix{
  public:
    /// Standard constructor
    AlgoMatrix();
    
    /// Useful constructor
    AlgoMatrix( const std::string, const std::string );
    
    /// Standard destructor
    ~AlgoMatrix(){};

    /// Add SelResult
    void addResult(const bool&, const bool&);

    /// Return full statistics
    int getStatistics() const{return m_alg2passed;};
  
    /// Return full statistics
    double getFraction() const{
      if ( m_alg2passed>0) return double(m_bothpassed)/m_alg2passed;
      else return -1. ; };
    double getPercent() const{return 100.*this->getFraction();};

    /// Return algorithm names
    void getAlgorithms( std::string&, std::string& ) const;

  private:

    std::string m_algorithm1 ; ///< Algorithm in row
    std::string m_algorithm2 ; ///< Algorithm in column
    /// statistics
    int m_bothpassed ; ///< both passed
    int m_alg2passed ;  ///< alg 2 passed
  };

};
#endif // ALGORITHMCORRELATIONS_H

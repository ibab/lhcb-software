// $Id: AlgorithmCorrelations.h,v 1.5 2009-04-15 09:48:14 pkoppenb Exp $
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
 *  Options for this implementation, ff one wants to declare the algorithm directly to the tool (ignoring
 *    the corresponding methods):
 *  @code
 *      Algorithms = { "alg1", "alg2"... }
 *      AlgorithmsRow, optional, for non-square matrices. Defines headers for columns
 *  @endcode
 *    Other options:
 *  @code
 *      OnlyNonZero = true ; // skips algorithms with eff=0 in table
 *      Decimals = -1 ; // defines precision. -1 means automatic precision.
 *      UseNumbers = true ;  // labels columns by numbers
 *  @endcode
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-04-19
 */
typedef std::vector<std::string> strings;
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
  StatusCode algorithms(const strings&)  ;
  StatusCode algorithmsRow(const strings&);
  StatusCode printTable(void) ;
  StatusCode printList(void) ;
  ///< Fill results one by one for each algorithm
  StatusCode fillResult(const std::string& algo,const bool& result) ;
  ///< Actually tell the tool that we reached the end of the event (only for one-by-one filling)
  StatusCode endEvent() ;

private:
  StatusCode reset(void);                     ///< reset everything
  bool isEffective(const std::string& ) const ; ///< Algo did something
  double algoRate(const std::string& ) const ; ///< Algo did something
  int happyAlgorithms(void) const ;           ///< Number of algos that did something
  unsigned int getDecimals(void) const ;      ///< Number of significant decimals
  StatusCode addResults(void);                ///< add all results
  StatusCode resetAlgoResult(std::vector<AlgoResult>&); ///< reset algo Results
  StatusCode fillResult(const std::string&,const bool&,std::vector<AlgoResult>&) ; ///< fill results
  StatusCode testAlgos(const strings&) const ;
  
private:
  strings m_conditionAlgorithms ;    ///< Algorithms to check against
  strings m_algorithmsToTest ; ///< Algorithms to check
  std::vector<AlgoMatrix> m_AlgoMatrices ;     ///< Pairs of correlations
  std::vector<AlgoResult> m_conditionResults ; ///< results of algorithms in this event
  std::vector<AlgoResult> m_testResults;       ///< results of algorithms in this event
  
  unsigned int m_longestName ; ///< Longest algorithm name
  bool m_minimize ; ///< Use mimimal table width
  int m_decimals ; ///< Number of decimals 
  bool m_square ; ///< it is a squere matrix
  bool m_useNumbers ; ///< use numbers as column labels
  unsigned int m_nEvents ; ///< number of events

  // container of results for one algorithm
  class AlgoResult{
  public:
    AlgoResult(){m_algo = "UNDEFINED";reset(); return;};
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
  
  // container of Algos matrix for two algorithms
  class AlgoMatrix{
  public:
    /// Standard constructor
    AlgoMatrix();
    
    /// Useful constructor
    AlgoMatrix( const std::string, const std::string );
    
    /// Standard destructor
    ~AlgoMatrix(){};

    /// Add Algo
    void addConditionalResult(const bool&, const bool&);

    /// Return full statistics
    int getConditionalStatistics() const{return m_bothpassed;};
    int getFullStatistics() const{return m_alg2passed;};
  
    /// Return full statistics
    double getConditionalFraction() const{
      if ( m_alg2passed>0) return double(m_bothpassed)/m_alg2passed;
      else return -1. ; };
    double getConditionalPercent() const{return 100.*getConditionalFraction();};

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

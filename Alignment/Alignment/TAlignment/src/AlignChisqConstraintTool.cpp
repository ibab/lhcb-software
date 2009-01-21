#include "IAlignConstraintTool.h"
#include "IGetElementsToBeAligned.h"
#include "GaudiAlg/GaudiTool.h"
#include "SolvKernel/AlMat.h"
#include "GaudiKernel/ToolFactory.h"
#include "LHCbMath/LHCbMath.h"
#include "boost/tokenizer.hpp"
#include <string>
#include <boost/lexical_cast.hpp>

/* helper class */

namespace Al
{
  
  class AlignChisqConstraintTool : public GaudiTool,
				   virtual public IAlignConstraintTool
  {
  public:
    typedef IGetElementsToBeAligned::Elements Elements ;

    AlignChisqConstraintTool(const std::string& type,
			     const std::string& name,
			     const IInterface* parent) ;
    ~AlignChisqConstraintTool() ;
    StatusCode initialize() ;

    size_t addConstraints(const Elements& elements,
			  const Al::Equations& equations,
			  AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const ;
    
    void printConstraints( const AlVec& parameters, const AlSymMat& covariance,
			   std::ostream& logmessage) const ;
  private:
    struct ConstraintDefinition
    {
      ConstraintDefinition( std::string apat, size_t adof, double anerror )
	: pattern( apat), dof(adof), error(anerror) {}
      std::string pattern ;
      size_t dof ;
      double error ;
    } ;
    typedef std::vector<ConstraintDefinition> ConstraintContainer ;
  private:
    std::vector<std::string> m_constraintNames ;
    ConstraintContainer m_constraints ;
    const IGetElementsToBeAligned* m_elementtool ;
  } ;
  
  DECLARE_TOOL_FACTORY( AlignChisqConstraintTool );
  
  AlignChisqConstraintTool::AlignChisqConstraintTool(const std::string& type,
					   const std::string& name,
					   const IInterface* parent)
    : GaudiTool(type,name,parent)
  {
    // interfaces
    declareInterface<IAlignConstraintTool>(this);
    declareProperty("Constraints", m_constraintNames ) ;
  }

  AlignChisqConstraintTool::~AlignChisqConstraintTool()
  {
  }


  static std::string removechars(const std::string& s,
				 const std::string& chars)
  {
    std::string rc ;
    for( std::string::const_iterator it = s.begin() ;
	 it != s.end(); ++it)
      if( std::find(chars.begin(), chars.end(), *it)==chars.end() )
	rc.push_back(*it);
    return rc ;
  }

  static std::vector<std::string> tokenize(const std::string& s,
					   const char* separator)
  {
    typedef boost::char_separator<char> Separator;
    typedef boost::tokenizer<Separator> Tokenizer;
    Tokenizer split(s, Separator(separator));
    std::vector<std::string> rc ;
    rc.insert(rc.end(), split.begin(), split.end() ) ;
    return rc ;
  }
  
  StatusCode AlignChisqConstraintTool::initialize()
  {
    // check that constraints exist
    StatusCode sc = GaudiTool::initialize() ;
    
    m_elementtool = tool<IGetElementsToBeAligned>("GetElementsToBeAligned");
    info() << "Number of constraints = " << m_constraintNames.size() << endreq ;

    if( sc.isSuccess() ) {
      
      for( std::vector<std::string>::const_iterator ic = m_constraintNames.begin() ;
	   ic != m_constraintNames.end(); ++ic ) {
	info() << "Constraint: " << *ic << endreq ;
	// tokenize
	std::vector<std::string> tokens = tokenize(*ic,":") ;
	if( tokens.size() != 3 ) {
	  error() << "constraint has wrong number of tokens: " << *ic << endreq ;
	} else {
	  std::string pattern = removechars(tokens[0]," ,") ;
	  std::string dofstr = removechars(tokens[1]," ,") ;
	  std::string errstr = removechars(tokens[2]," ,") ;
	  static const std::vector<std::string> dofnames = 
	    boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz") ;
	  size_t dof = std::distance( dofnames.begin(), std::find(dofnames.begin(),dofnames.end(),dofstr)) ;
	  if( dof < dofnames.size() ) {
	    double err = boost::lexical_cast<double>(errstr) ;
	    m_constraints.push_back( ConstraintDefinition( pattern, dof, err ) );
	    // report the number of matching constraints:
	    IGetElementsToBeAligned::Elements elements ;
	    m_elementtool->findElements( pattern, elements ) ;
	    info() << "Constraint for " << dofnames[dof] << " with error " << err << " for "
		   << elements.size() << " elements matching pattern \""
		   << pattern << "\"" << endreq ;
	  } else {
	    error() << "Unknown dof: " << dof << endreq ;
	    sc = StatusCode::FAILURE ;
	  }
	}
      }
    }
    return sc ;
  }
  
  size_t AlignChisqConstraintTool::addConstraints(const Elements& /*inputelements*/,
						  const Al::Equations& /*equations*/,
						  AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2) const
  {
    size_t numconstraints(0) ;
    for( ConstraintContainer::const_iterator ic = m_constraints.begin() ;
	 ic != m_constraints.end(); ++ic ) {
      IGetElementsToBeAligned::Elements elements ;
      m_elementtool->findElements( ic->pattern, elements ) ;
      for( IGetElementsToBeAligned::Elements::const_iterator ielem = elements.begin() ;
	   ielem != elements.end(); ++ielem) {
	int ipar = (*ielem)->activeParIndex( ic->dof ) ;
	if( ipar>=0 ) {
	  // get the current difference with nominal. this is what we
	  // constrain.
	  AlParameters currentdelta = (*ielem)->currentActiveDelta() ;
	  double weight = 1/( ic->error * ic->error ) ;
	  double residual = -currentdelta.parameters()(ic->dof) ;
	  halfDChi2DAlpha(ipar) += weight * residual; 
	  halfD2Chi2DAlpha2.fast(ipar,ipar) += weight ;
	  ++numconstraints ;
	}
      }
    }
    info() << "Added " << numconstraints << " chisquare constraints." << endreq ;
    return numconstraints ;
  }  
  
  void AlignChisqConstraintTool::printConstraints(const AlVec& parameters, const AlSymMat& /*covariance*/,
						  std::ostream& logmessage) const
  {
    size_t numconstraints(0) ;
    double chisquare(0) ;
    for( ConstraintContainer::const_iterator ic = m_constraints.begin() ;
	 ic != m_constraints.end(); ++ic ) {
      IGetElementsToBeAligned::Elements elements ;
      m_elementtool->findElements( ic->pattern, elements ) ;
      for( IGetElementsToBeAligned::Elements::const_iterator ielem = elements.begin() ;
	   ielem != elements.end(); ++ielem) {
	int ipar = (*ielem)->activeParIndex( ic->dof ) ;
	if( ipar>=0 ) {
	  // get the current difference with nominal. this is what we
	  // constrain.
	  AlParameters currentdelta = (*ielem)->currentActiveDelta() ;
	  double weight = 1/( ic->error * ic->error ) ;
	  double residual = -parameters(ipar)-currentdelta.parameters()(ic->dof) ; // sign????
	  chisquare += residual*residual*weight ;
	  ++numconstraints ; 
	}
      }
    }
    logmessage << "Total chisquare / dof from survey constraints: "
	       << chisquare << " / " << numconstraints << std::endl ;
  }
}

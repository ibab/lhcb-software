#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/AlignmentCondition.h"

#include "IAlignChisqConstraintTool.h"
#include "IGetElementsToBeAligned.h"

#include <string>
#include <fstream>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>

namespace Al
{
  struct ConfiguredSurveyData
  {
    ConfiguredSurveyData() {}
    std::string name ;
    Gaudi::Vector6 par ;
    Gaudi::Vector6 err ;
  } ;

  struct XmlSurveyData
  {
    XmlSurveyData() {
      pivot[0] = pivot[1] = pivot[2] = 0 ;
    }
    std::string name ;
    Gaudi::Vector6 par ;
    double pivot[3] ;
  } ;

  class AlignChisqConstraintTool : public GaudiTool,
                                   virtual public IAlignChisqConstraintTool
  {
  public:
    AlignChisqConstraintTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent) ;
    ~AlignChisqConstraintTool() ;
    StatusCode initialize() ;
    LHCb::ChiSquare addConstraints(const Elements& elements,
                                   AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2,
                                   std::ostream& stream) const ;
    LHCb::ChiSquare addConstraints(const Elements& inputelements,
                                   Al::Equations& equations,
                                   std::ostream& logmessage) const ;
    LHCb::ChiSquare chiSquare( const AlignmentElement& element, bool activeonly ) const ;
    const AlParameters* surveyParameters(const AlignmentElement& element) const ;

  private:
    StatusCode parseXmlFile( const std::string& filename) ;
    StatusCode parseXmlUncertainties( const std::string& patterm ) ;
    StatusCode parseElement( const std::string& element) ;
    const Gaudi::Vector6* findXmlUncertainty(const std::string& name) const ;
  private:
    std::vector< std::string > m_constraintNames ;
    std::vector< std::string > m_xmlFiles ;
    std::vector< std::string > m_xmlUncertainties ;
    typedef std::map<std::string, XmlSurveyData> XmlData ;
    XmlData m_xmldata ;
    typedef std::vector<ConfiguredSurveyData> ConstraintData ;
    ConstraintData m_configuredSurvey ;
    typedef std::pair<std::string, Gaudi::Vector6> NamedXmlUncertainty ;
    std::vector<NamedXmlUncertainty> m_xmlUncertaintyMap ;
    typedef std::map<const AlignmentElement*, AlParameters> SurveyParameters ;
    mutable SurveyParameters m_cachedSurvey ;
  } ;
  
  DECLARE_TOOL_FACTORY( AlignChisqConstraintTool );
  
  AlignChisqConstraintTool::~AlignChisqConstraintTool() {}

  AlignChisqConstraintTool::AlignChisqConstraintTool(const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent)
    : GaudiTool(type,name,parent)
  {
    // interfaces
    declareInterface<IAlignChisqConstraintTool>(this);
    declareProperty("Constraints", m_constraintNames) ;
    declareProperty("XmlFiles", m_xmlFiles) ;
    declareProperty("XmlUncertainties", m_xmlUncertainties) ;
  }
  
  StatusCode
  AlignChisqConstraintTool::initialize()
  {
    StatusCode sc = GaudiTool::initialize() ;
    for( std::vector<std::string>::const_iterator ifile = m_constraintNames.begin() ;
         ifile != m_constraintNames.end() && sc.isSuccess() ; ++ifile ) 
      sc = parseElement( *ifile ) ;
    
    for( std::vector<std::string>::const_iterator ifile = m_xmlFiles.begin() ;
         ifile != m_xmlFiles.end() && sc.isSuccess() ; ++ifile ) 
      sc = parseXmlFile( *ifile ) ;
    
    for( std::vector<std::string>::const_iterator ipattern = m_xmlUncertainties.begin() ;
         ipattern != m_xmlUncertainties.end() && sc.isSuccess(); ++ipattern ) {
      sc = parseXmlUncertainties(*ipattern) ;
      if( !sc.isSuccess() ) 
        error() << "Cannot parse XML uncertainty pattern \'" << *ipattern << "\'" << endreq ;
    }
    
    info() << "Number of entries in xml data: " << m_xmldata.size() << endreq ;
    info() << "Number of entries specified by job-option: " << m_configuredSurvey.size() << endreq ;
    
    return sc ;
  }

  namespace {
    // collection of static functions used in below

    std::string assembleConditionLine( std::ifstream& file ) {
      // creates a single line for a condition that may span several lines
      bool conditionopen = false ;
      std::string rc ;
      while ( file && (conditionopen || rc.empty() ) ) {
        char line[2048] ;
        file.getline( line, 2048 ) ;
        std::string linestr(line) ;
        size_t ipos(0),jpos(0) ;
        try {
          if( (ipos = linestr.find( "<condition", 0)) != std::string::npos ) {
            conditionopen = true ;
          } else {
            ipos = 0 ;
          }
          if( (jpos = linestr.find( "/condition>",0)) != std::string::npos ) {
            conditionopen = false ;
            rc += linestr.substr( ipos, jpos - ipos + 11 ) ;
          } 
          else if( conditionopen )
            rc += linestr.substr( ipos, linestr.size()-ipos) ;
        } 
        catch ( std::exception e) {
          std::cout << "problem parsing line: " << linestr << std::endl ;
          throw e ;
        }
      }
      return rc ;
    }
    
    std::string removechars(const std::string& s,
                            const std::string& chars)
    {
      std::string rc ;
      for( std::string::const_iterator it = s.begin() ;
           it != s.end(); ++it)
        if( std::find(chars.begin(), chars.end(), *it)==chars.end() )
          rc.push_back(*it);
      return rc ;
    }

    std::string replacestring(const std::string& s,
                              const std::string& in,
                              const std::string& out)
    {
      std::string::size_type pos = 0;
      std::string rc = s ;
      while (pos != std::string::npos){
        pos = rc.find(in,pos);
        if (pos != std::string::npos) rc.replace(pos,in.size(),out);
      }
      return rc ;
    }

    std::vector<std::string> tokenize(const std::string& s,
                                      const char* separator)
    {
      typedef boost::char_separator<char> Separator;
      typedef boost::tokenizer<Separator> Tokenizer;
      Tokenizer split(s, Separator(separator));
      std::vector<std::string> rc ;
      rc.insert(rc.end(), split.begin(), split.end() ) ;
      return rc ;
    }
    
    double extractdouble( const std::string& str ) {
      std::string input = removechars(str," \t") ;
      double rc = 1 ;
      size_t ipos ;
      if( (ipos = input.find("*mrad")) != std::string::npos) {
        rc = 0.001 ;
        input.erase(ipos,5) ;
      } else if ( (ipos = input.find("*mm")) != std::string::npos) {
        input.erase(ipos,3) ;
      }
      try {
        rc *= boost::lexical_cast<double>(input) ;
      } catch ( std::exception e) {
        std::cout << "error parsing input string: \'" << str << "\'" << std::endl ;
        throw e ;
      }
      return rc ;
    }

    inline bool match( const std::string& name,
                       const std::string& pattern )
    {
      boost::regex ex ;
      bool rc = false ;
      try {
        ex.assign( pattern, boost::regex_constants::icase ) ;
        rc = boost::regex_match(name,ex) ;
      } catch (...) {
        std::cout << "Problem with pattern: " << pattern << std::endl ;
      }
      return rc ;
    }
  }
    
  StatusCode AlignChisqConstraintTool::parseXmlFile( const std::string& filename ) 
  {
    // I tried to use the standard xml parser, but single xml files
    // are not self-contaiend, so that will never work. Terefore, I
    // have written soemthing very stupid that should work for xml
    // files with alignment conditions.
    std::ifstream ifs( filename.c_str() ) ;
    if( !ifs.is_open() ) {
      error() << "Cannot open file: " << filename << endreq ;
      return StatusCode::FAILURE ;
    }

    while( ifs ) {
      std::string linestr = assembleConditionLine(ifs) ;
      if( !linestr.empty()) {
        size_t ipos(0),jpos(0) ;
        ipos = linestr.find( "name",0) ;
        ipos = linestr.find_first_of("\"", ipos +1 ) ;
        jpos = linestr.find_first_of("\"", ipos +1 ) ;
        std::string conditionname = linestr.substr( ipos+1, jpos-ipos-1) ;
	
        ipos = linestr.find( "dPosXYZ",jpos+1 ) ;
        ipos = linestr.find_first_of( ">",ipos ) ;
        jpos = linestr.find_first_of( "<", ipos+1) ;
        std::vector<std::string> numbers = tokenize(linestr.substr(ipos+1,jpos-ipos-1)," ") ;
        assert(numbers.size()==3) ;
        std::vector<double> pospars(3,0) ;
        for( size_t i=0; i<3; ++i) pospars[i] = extractdouble(numbers[i]) ;
	
        //std::cout << "position string: "
        //<< std::endl << numbers << std::endl ;
        //sscanf( numbers.c_str(),"%g %g %g", &dx,&dy,&dz) ;
	
        ipos = linestr.find( "dRotXYZ",jpos+1 ) ;
        ipos = linestr.find_first_of( ">",ipos ) ;
        jpos = linestr.find_first_of( "<", ipos+1) ;
        numbers = tokenize(linestr.substr(ipos+1,jpos-ipos-1)," ") ;
        assert(numbers.size()==3) ;
        std::vector<double> rotpars(3,0) ;
        for( size_t i=0; i<3; ++i) rotpars[i] = extractdouble(numbers[i]) ;
	
        ipos = linestr.find( "pivotXYZ",jpos+1 ) ;
	std::vector<double> pivotpars(3,0) ;
        if( ipos != std::string::npos ) {
          ipos = linestr.find_first_of( ">",ipos ) ;
          jpos = linestr.find_first_of( "<", ipos+1) ;
          numbers = tokenize(linestr.substr(ipos+1,jpos-ipos-1)," ") ;
          assert(numbers.size()==3) ;
          for( size_t i=0; i<3; ++i) pivotpars[i] = extractdouble(numbers[i]);
        }
        // now put the result in the dictionary
	XmlSurveyData& entry = m_xmldata[conditionname] ;
        entry.name = conditionname ;
	for(int i=0;i<3; ++i) {
	  entry.par[i]   = pospars[i] ;
	  entry.par[i+3] = rotpars[i] ;
	  entry.pivot[i] = pivotpars[i] ;
	}
      }
    }
    return StatusCode::SUCCESS ;
  }
  
  StatusCode AlignChisqConstraintTool::parseElement( const std::string& element ) 
  {
    // there are two types of definitions, namely those that look like
    //   " pattern : valTx valTy valTz valRx valRy valRz : errTx errTy errTz errRx errRy errRz" --> 6 values and 6 errors
    // and the old type
    //   " pattern : Tx : val +/- err " or even " pattern : Tx : err "
    StatusCode sc = StatusCode::SUCCESS ;
    debug() << "Parsing constraint: " << element << endreq ;
    std::vector<std::string> tokens = tokenize(element,":") ;
    if( tokens.size() != 3 ) {
      error() << "constraint has wrong number of tokens: " << element << endreq ;
      sc = StatusCode::FAILURE ;
    } else {
      std::string name = removechars(tokens[0]," ") ;
      m_configuredSurvey.push_back( ConfiguredSurveyData() ) ;
      ConfiguredSurveyData& entry = m_configuredSurvey.back() ;
      entry.name = name ;

      // first see if definition is of the old type
      const std::vector<std::string> dofnames = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz") ;
      std::string dofstr = removechars(tokens[1]," ,") ;
      size_t dof = std::distance( dofnames.begin(), std::find(dofnames.begin(),dofnames.end(),dofstr)) ;
      if( dof < dofnames.size() ) {
	error() << "constraint value has wrong number of tokens: " << element << endreq ;
	sc = StatusCode::FAILURE ;
      } else {
        std::vector<std::string> numbers = tokenize(tokens[1]," ") ;
        if( numbers.size() != 6 ) {
          error() << "constraint has wrong number of deltas: " << tokens[1] << endreq ;
          sc = StatusCode::FAILURE ;
        } else {
          for(size_t i=0; i<6; ++i)  entry.par[i] = boost::lexical_cast<double>(numbers[i]) ;
        }
	
        numbers = tokenize(tokens[2]," ") ;
        if( numbers.size() != 6 ) {
          error() << "constraint has wrong number of errors: " << tokens[2] << endreq ;
          sc = StatusCode::FAILURE ;
        } else {
          for(size_t i=0; i<6; ++i)  entry.err[i] = boost::lexical_cast<double>(numbers[i]) ;
        }
      }
    }
    return sc ;
  }
 
  StatusCode
  AlignChisqConstraintTool::parseXmlUncertainties(const std::string& ipattern)
  {
    // now still parse the patterns with XML uncertainties
    StatusCode sc = StatusCode::SUCCESS ;
    std::vector<std::string> tokens = tokenize(ipattern,":") ;
    if( tokens.size() != 2 ) {
      error() << "xml uncertainty pattern has wrong number of tokens: " << ipattern << endreq ;
      sc = StatusCode::FAILURE ;
    }
    std::string pattern = removechars(tokens[0]," ") ;
    std::vector<std::string> valuetokens = tokenize(tokens[1]," ") ;
    if( valuetokens.size() != 6 ) {
      error() << "xml uncertainty pattern has wrong number of errors: " << tokens[1] << endreq ;
      sc = StatusCode::FAILURE ;
    }
    Gaudi::Vector6 errors ;
    for(int i=0; i<6; ++i) errors[i] = boost::lexical_cast<double>(valuetokens[i]) ;
    m_xmlUncertaintyMap.push_back(std::make_pair(pattern,errors)) ;
    return sc ;
  }

  const Gaudi::Vector6* AlignChisqConstraintTool::findXmlUncertainty(const std::string& name) const 
  {
    // always take the last one:
    const Gaudi::Vector6* rc=0 ;
    const NamedXmlUncertainty* foundentry(0) ;
    BOOST_FOREACH( const NamedXmlUncertainty& entry, m_xmlUncertaintyMap ) {
      if( match( name, entry.first) ) foundentry = &entry ;
    }
    if(foundentry) {
      rc = &(foundentry->second) ;
      debug() << "Matched " << name << " to "
	      << foundentry->first << " " << foundentry->second << endreq ;
    }
    return rc ;
  }

  const AlParameters* AlignChisqConstraintTool::surveyParameters(const AlignmentElement& element) const
  {    
    const AlParameters* rc(0) ;
    SurveyParameters::const_iterator it = m_cachedSurvey.find( &element ) ;
    if( it != m_cachedSurvey.end() ) {
      rc = &(it->second) ;
    } else {

      AlParameters newsurvey ;

      // did we add information for this alignable explicitely? note
      // that we take the last one that matches.
      bool found = false ;
      for( ConstraintData::const_iterator it = m_configuredSurvey.begin() ;
	   it != m_configuredSurvey.end() ; ++it )
	// match the name of the alignable, or, if there is only one element, match the name of the condition
	if( element.name() == it->name || 
	    match(element.name(),it->name ) ) {
	  const ConfiguredSurveyData& survey = *it ;
	  AlParameters::TransformParameters parameters = survey.par ;
	  AlParameters::TransformCovariance covmatrix ;
	  for(int i=0; i<6; ++i) covmatrix(i,i) = survey.err[i] * survey.err[i] ;
	  newsurvey = AlParameters( parameters, covmatrix ) ;
	  found = true ;
	}
    
      if(!found) {
	// OK. Here is the new strategy which should also work for
	// groups of elements. Make an average of the xml found for all
	// daughters. Assign the errors based on the xml-error
	// patterns. Let's first look for that pattern.
	
	// for the error pattern we have the following rule:
	// - first we match the name of the alignable
	// - if there is only a single element, we then try to match the name of the condition
	// - if there is a single element, we then try to match the name of the element
	// extract the condition name, but only if this alignable has only one detector element
	
	const IGeometryInfo* geom(0) ;
	const AlignmentCondition* cond(0) ;
	const Gaudi::Vector6* errors = findXmlUncertainty(element.name()) ;
	if(errors==0 && element.detelements().size()==1 ) {
	  if((geom=element.detelements().front()->geometry()) &&
	     (cond=geom->alignmentCondition() ) ) {
	    // need to remove a leading slash
	    std::string condname = cond->name().substr( 1, cond->name().size()-1 ) ;
	    errors = findXmlUncertainty( condname ) ;
	  }
	  
	  if (!errors) 
	    errors = findXmlUncertainty(element.detelements().front()->name()) ;
	}

	if(!errors) {
	  error() << "Cannot find XML uncertainties for alignable: "
		  << element.name() << endreq ;
	} else {
	  AlParameters::TransformCovariance localcovmatrix ;
	  for(int i=0; i<6; ++i) 
	    localcovmatrix(i,i) = (*errors)[i] * (*errors)[i]  ;
	  
	  AlParameters::TransformParameters sumparameters ;
	  AlParameters::TransformCovariance sumcovmatrix ;
	  
	  size_t numfound(0) ;
	  
	  // now loop over all daughter elements
	  BOOST_FOREACH( const DetectorElement* detelem,element.detelements() ) {
	    // find this element in the XML catalogue
	    std::string condname ;
	    if((geom=detelem->geometry()) &&
	       (cond=geom->alignmentCondition() ) ) 
	      // need to remove a leading slash
	      condname = cond->name().substr( 1, cond->name().size()-1 ) ;
	    
	    XmlData::const_iterator it = m_xmldata.find( condname ) ;
	    if( it == m_xmldata.end() ) {
	      error() << "Cannot find condition \"" << cond->name() << "\" for alignable "
		      << element.name() << " in survey dictionary." << endreq ;
	    } else {
	      const XmlSurveyData& survey = it->second ;
	      
	      Gaudi::Transform3D nominalFrame = 
		AlignmentElement::toGlobalMatrixMinusDelta( *detelem ) ;
	      AlParameters surveypars( survey.par, localcovmatrix ) ;
	      const ROOT::Math::Translation3D pivot(survey.pivot[0],survey.pivot[1],survey.pivot[2]) ;
	      Gaudi::Transform3D surveyFrame = nominalFrame * pivot ;
	      // get the transform from survey to alignment and transform the survey
	      Gaudi::Transform3D fromSurveyToAlignment = element.alignmentFrame().Inverse() * surveyFrame ;
	      AlParameters surveyParameters = surveypars.transformTo( fromSurveyToAlignment ) ;
	      
	      sumparameters += surveyParameters.transformParameters() ;
	      sumcovmatrix  += surveyParameters.transformCovariance() ;
	      ++numfound ;
	    }
	  }
	  
	  if(numfound == element.detelements().size()) {
	    // now solve a peculiar problem: if we share xml parameters
	    // with the daughters (because there is no xml for this
	    // alignable) set the survey for these parameters to
	    // 0. otherwise, it cannot converge.
	    std::vector<int> shareddofs = element.redundantDofs() ;
	    BOOST_FOREACH(int idof, shareddofs) {
	      sumparameters(idof) = 0 ;
	      for(int jdof=0; jdof<6; ++jdof)
		sumcovmatrix(idof,jdof) = 0 ;
	      sumcovmatrix(idof,idof) = numfound * localcovmatrix(idof,idof) ;
	    }
	  
	    newsurvey = AlParameters( sumparameters/numfound, Gaudi::SymMatrix6x6(sumcovmatrix/numfound) ) ;
	    found = true ;
	    
	  }
	}
      }
      if(!found) {
	error() << "Haven't been able to construct survey for " << element.name() << endreq ;
      } else {
	m_cachedSurvey[&element] = newsurvey ;
	rc = &(m_cachedSurvey[&element]) ;
      }
    }

    return rc ;
  }


  LHCb::ChiSquare AlignChisqConstraintTool::addConstraints(const IGetElementsToBeAligned::Elements& inputelements,
                                                           AlVec& halfDChi2DAlpha, AlSymMat& halfD2Chi2DAlpha2,
                                                           std::ostream& logmessage) const
  {
    size_t totalnumconstraints(0) ;
    double totalchisq(0) ;
    for( IGetElementsToBeAligned::Elements::const_iterator ielem = inputelements.begin() ;
         ielem != inputelements.end(); ++ielem) {
      const AlParameters * psurvey = surveyParameters(**ielem) ;
      if( psurvey ) {
	const AlParameters& surveypars = *psurvey ;
	//AlParameters currentdelta = (*ielem)->currentLocalDelta() ;
	AlParameters currentdelta = (*ielem)->currentDelta() ;
	AlParameters::TransformParameters residual = surveypars.transformParameters() -  currentdelta.transformParameters() ;
	AlParameters::TransformCovariance weight = surveypars.transformCovariance() ;
	weight.Invert() ;
	// FIX ME: take only active dofs when evaluating survey chisq
	totalchisq +=  ROOT::Math::Similarity( residual, weight ) ;
	totalnumconstraints += 6 ;
	Gaudi::Vector6      dChi2DAlpha = weight * residual ;
	Gaudi::SymMatrix6x6 d2Chi2DAlpha2 = weight ;
	for(int idof=0 ; idof<6; ++idof ) {
          int ipar = (*ielem)->activeParIndex( idof ) ;
	  if( ipar>=0) {
	    halfDChi2DAlpha(ipar) += dChi2DAlpha( idof ) ;
	    for(int jdof=0; jdof<=idof; ++jdof) {
	      int jpar = (*ielem)->activeParIndex( jdof ) ;
	      if( jpar>=0) 
		halfD2Chi2DAlpha2.fast(ipar,jpar) += d2Chi2DAlpha2(idof,jdof) ;
	    }
	  }
	}
      }
    }
    logmessage << "Total chisquare of survey constraints: " << totalchisq << " / " << totalnumconstraints << std::endl ;
    return LHCb::ChiSquare(totalchisq,totalnumconstraints) ;
  } 


  LHCb::ChiSquare AlignChisqConstraintTool::addConstraints(const IGetElementsToBeAligned::Elements& inputelements,
                                                           Al::Equations& equations,
                                                           std::ostream& logmessage) const
  {
    size_t totalnumconstraints(0) ;
    double totalchisq(0) ;
    for( IGetElementsToBeAligned::Elements::const_iterator ielem = inputelements.begin() ;
         ielem != inputelements.end(); ++ielem) {
      const AlParameters * psurvey = surveyParameters(**ielem) ;
      if( psurvey ) {
	const AlParameters& surveypars = *psurvey ;
      	ElementData& elemdata = equations.element((*ielem)->index()) ;
	AlParameters currentdelta = (*ielem)->currentDelta() ;
	AlParameters::TransformParameters residual = surveypars.transformParameters() -  currentdelta.transformParameters() ;
	AlParameters::TransformCovariance weight = surveypars.transformCovariance() ;
	weight.InvertChol() ;
	totalchisq +=  ROOT::Math::Similarity( residual, weight ) ;
	elemdata.m_dChi2DAlpha   += weight * residual ;
	elemdata.m_d2Chi2DAlpha2 += weight ;
	totalnumconstraints += 6 ;
      }//end if ielem..isActive
    }
    logmessage << "Total chisquare of survey constraints: " << totalchisq << " / " << totalnumconstraints << std::endl ;
    return LHCb::ChiSquare(totalchisq,totalnumconstraints) ;
  }   

  LHCb::ChiSquare AlignChisqConstraintTool::chiSquare(const AlignmentElement& element,
                                                      bool activeonly ) const
  {
    size_t totalnumconstraints(0) ;
    double totalchisq(0) ;
    const AlParameters * psurvey = surveyParameters(element) ;
    if( psurvey ) {
      const AlParameters& surveypars = *psurvey ;
      AlParameters currentdelta = element.currentDelta() ;
      AlParameters::TransformParameters residual = surveypars.transformParameters() -  currentdelta.transformParameters() ;
      AlParameters::TransformCovariance weight = surveypars.transformCovariance() ;
      totalnumconstraints += 6 ;
      if( activeonly ) {
	// this is extremely lazy: just set errors on inactive parameters 'big'
	const double largeerrors[6] = {1000,1000,1000,1,1,1} ;
	for(int idof=0; idof<6; ++idof)
	  if( ! element.dofMask().isActive( idof ) ) {
	    weight(idof,idof) += largeerrors[idof] * largeerrors[idof] ;
	    --totalnumconstraints ;
	  }
      }
      weight.InvertChol() ;
      totalchisq +=  ROOT::Math::Similarity( residual, weight ) ;
    }
    return LHCb::ChiSquare(totalchisq,totalnumconstraints) ;
  }

}

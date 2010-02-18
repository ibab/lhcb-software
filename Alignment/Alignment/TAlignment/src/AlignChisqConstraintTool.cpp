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


namespace Al
{

  /* helper class */
  namespace {
    struct SurveyDictionaryEntry
    {
      SurveyDictionaryEntry() {
        err[0] = err[1] = err[2] = err[3] = err[4] = err[5] = -1 ;
        par[0] = par[1] = par[2] = par[3] = par[4] = par[5] = 0 ;
      }
      std::string name ;
      double par[6] ;
      double err[6] ;
    } ;
  }
  
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
    SurveyData surveyData( const AlignmentElement& element ) const {
      const SurveyData* s = findSurveyData(element) ;
      return s ? *s : SurveyData() ;
    }
  private:
    StatusCode parseXmlFile( const std::string& filename) ;
    StatusCode parseXmlUncertainties( const std::string& patterm ) ;
    StatusCode parseElement( const std::string& element) ;
    const SurveyData* findSurveyData( const AlignmentElement& element ) const ;
    
  private:
    typedef std::map<std::string, SurveyData> XmlData ;
    XmlData m_xmldata ;
    typedef std::map<std::string, SurveyData> ConstraintData ;
    ConstraintData m_constraints ;
    std::vector< std::string > m_constraintNames ;
    std::vector< std::string > m_xmlUncertainties ;
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
    declareProperty("XmlUncertainties", m_xmlUncertainties) ;
  }
  
  StatusCode
  AlignChisqConstraintTool::initialize()
  {
    StatusCode sc = GaudiTool::initialize() ;
    for( std::vector<std::string>::const_iterator ifile = m_constraintNames.begin() ;
         ifile != m_constraintNames.end() && sc.isSuccess() ; ++ifile ) 
      if ( ifile->find(".xml") != std::string::npos ) 
        sc = parseXmlFile( *ifile ) ;
      else 
        sc = parseElement( *ifile ) ;

    for( std::vector<std::string>::const_iterator ipattern = m_xmlUncertainties.begin() ;
         ipattern != m_xmlUncertainties.end() && sc.isSuccess(); ++ipattern ) {
      sc = parseXmlUncertainties(*ipattern) ;
      if( !sc.isSuccess() ) 
        error() << "Cannot parse XML uncertainty pattern \'" << *ipattern << "\'" << endreq ;
    }
    
    info() << "Number of entries in xml data: " << m_xmldata.size() << endreq ;
    info() << "Number of entries specified by job-option: " << m_constraints.size() << endreq ;
    
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
        std::vector<double> pospars ;
        for( size_t i=0; i<3; ++i) pospars.push_back( extractdouble(numbers[i]) ) ;
	
        //std::cout << "position string: "
        //<< std::endl << numbers << std::endl ;
        //sscanf( numbers.c_str(),"%g %g %g", &dx,&dy,&dz) ;
	
        ipos = linestr.find( "dRotXYZ",jpos+1 ) ;
        ipos = linestr.find_first_of( ">",ipos ) ;
        jpos = linestr.find_first_of( "<", ipos+1) ;
        numbers = tokenize(linestr.substr(ipos+1,jpos-ipos-1)," ") ;
        assert(numbers.size()==3) ;
        std::vector<double> rotpars ;
        for( size_t i=0; i<3; ++i) rotpars.push_back( extractdouble(numbers[i]) ) ;
	
        ipos = linestr.find( "pivotXYZ",jpos+1 ) ;
        if( ipos != std::string::npos ) {
          ipos = linestr.find_first_of( ">",ipos ) ;
          jpos = linestr.find_first_of( "<", ipos+1) ;
          numbers = tokenize(linestr.substr(ipos+1,jpos-ipos-1)," ") ;
          assert(numbers.size()==3) ;
          std::vector<double> pivotpars ;
          for( size_t i=0; i<3; ++i) pivotpars.push_back( extractdouble(numbers[i]) ) ;
          if( std::abs(pivotpars[0]) > 1e-9 || std::abs(pivotpars[1]) > 1e-9 || std::abs(pivotpars[2]) > 1e-9) {
            // shit, need to correct everything for pivot point translation
            Gaudi::Transform3D T = DetDesc::localToGlobalTransformation( pospars,rotpars,pivotpars ) ;
            DetDesc::getZYXTransformParameters(T,pospars,rotpars) ;
          }
        }
        // now put the result in the dictionary
        SurveyData entry ;
        entry.name    = conditionname ;
        entry.par[0] = pospars[0] ;
        entry.par[1] = pospars[1] ;
        entry.par[2] = pospars[2] ;
        entry.par[3] = rotpars[0] ;
        entry.par[4] = rotpars[1] ;
        entry.par[5] = rotpars[2] ;
        m_xmldata[entry.name] = entry ;
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
      SurveyData& entry = m_constraints[name] ;
      entry.name = name ;

      // first see if definition is of the old type
      const std::vector<std::string> dofnames = boost::assign::list_of("Tx")("Ty")("Tz")("Rx")("Ry")("Rz") ;
      std::string dofstr = removechars(tokens[1]," ,") ;
      size_t dof = std::distance( dofnames.begin(), std::find(dofnames.begin(),dofnames.end(),dofstr)) ;
      if( dof < dofnames.size() ) {
        // just repace +/- with a space, because that is what we'll use to tokenize 
        // (we cannot tokenize with +/-!)
        std::string errstr = replacestring(tokens[2], "+/-", " ") ;
        std::vector<std::string> valuetokens = tokenize(errstr," ") ;
        // if only single token, then this is just the error
        if( valuetokens.size() == 1 )
          entry.err[dof] = boost::lexical_cast<double>(valuetokens[0]) ;
        else if( valuetokens.size() == 2 ) {
          entry.par[dof] = boost::lexical_cast<double>(valuetokens[0]) ;
          entry.err[dof] = boost::lexical_cast<double>(valuetokens[1]) ;
          //std::cout << "dof, val, err: " << dof << " " <<  entry.par[dof] << " " << entry.err[dof] << std::endl ;
        } else {
          error() << "constraint value has wrong number of tokens: " << element << endreq ;
          sc = StatusCode::FAILURE ;
        }
      } else {

        // this must be the new type, then
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
    double errors[6] ;
    for(int i=0; i<6; ++i) errors[i] = boost::lexical_cast<double>(valuetokens[0]) ;
    // now find all matching elements
    size_t nummatches(0) ;
    for( ConstraintData::iterator it = m_xmldata.begin() ; it != m_xmldata.end(); ++it ) 
      // match the name of the alignable, or, if there is only one element, match the name of the condition
      if( match(it->first,pattern) ) {
        ++nummatches ;
        for(int i=0; i<6; ++i) it->second.err[i] = errors[i] ;
      }
    if( nummatches==0 ) warning() << "Found no matches for xml uncertainty pattern: \'" << pattern << "\'" << endreq ;
    return sc ;
  }
  
  const AlignChisqConstraintTool::SurveyData* AlignChisqConstraintTool::findSurveyData( const AlignmentElement& element ) const 
  {
    const SurveyData* rc(0) ;
    std::string elementname = element.name() ;
    // extract the condition name, but only if this alignable has only one detector element
    std::string condname ;
    const IGeometryInfo* geom(0) ;
    const AlignmentCondition* cond(0) ;
    if( element.detelements().size()==1 &&
        (geom=element.detelements().front()->geometry()) &&
        (cond=geom->alignmentCondition() ) )
      // need to remove a leading slash
      condname = cond->name().substr( 1, cond->name().size()-1 ) ;
    
    // did we add information for this alignable explicitely?
    for( ConstraintData::const_iterator it = m_constraints.begin() ;
         it != m_constraints.end() && !rc; ++it ) 
      // match the name of the alignable, or, if there is only one element, match the name of the condition
      if( match(elementname,it->first ) || 
          (condname.size()>0 && match(condname, it->first) ) )
        rc = &(it->second) ;

    // if there is only one detelement, we look in the survey database
    if( rc==0 && condname.size()>0 ) {
      XmlData::const_iterator it = m_xmldata.find( condname ) ;
      if( it != m_xmldata.end() ) {
        rc = &(it->second) ;
      } else {
        debug() << "Cannot find condition \"" << cond->name() << "\" for alignable "
                << element.name() << " in survey dictionary." << endreq ;
      }
    }
    
    if( rc==0 ) {
      warning() << "No survey data for alignable: " << element.name() << " " << condname << endreq ;
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
      const SurveyData* survey = findSurveyData( **ielem ) ;
      if( survey ) {
        AlParameters currentdelta = (*ielem)->currentLocalDelta() ;
        for(int idof=0 ; idof<6; ++idof ) {
          int ipar = (*ielem)->activeParIndex( idof ) ;
          if( ipar>=0 ) 
          {
            if( survey->err[idof] > 0 ) 
            {
              double weight = 1/( survey->err[idof] * survey->err[idof] ) ;
              double residual = survey->par[idof] - currentdelta.parameters()(idof) ;
              halfDChi2DAlpha(ipar) += weight * residual; 
              halfD2Chi2DAlpha2.fast(ipar,ipar) += weight ;
              ++ totalnumconstraints;
              totalchisq += weight * residual * residual ;
              info() << "adding survey constraint: "
                     << (*ielem)->name() << " "
                     << currentdelta.parName(idof) << " "
                     << survey->par[idof] << " " << survey->err[idof] << endreq ;
            } 
            else 
            {
              warning() << "No survey constraint for dof: "
                        << (*ielem)->name() << " " << currentdelta.parName(idof) << endreq ;
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
         ielem != inputelements.end(); ++ielem) 
    {
      const SurveyData* survey = findSurveyData( **ielem ) ;
      ElementData& elemdata = equations.element((*ielem)->index()) ;
      if( survey ) 
      {
        AlParameters currentdelta = (*ielem)->currentLocalDelta() ;
        for(int idof=0 ; idof<6; ++idof ) 
        {
          if( (*ielem)->dofMask().isActive(idof) )
          {
            if( survey->err[idof] > 0 ) 
            {
              double weight = 1/( survey->err[idof] * survey->err[idof] ) ;
              double residual = survey->par[idof] - currentdelta.parameters()(idof) ;
              // FIXME: the sign is odd here simlpy because we have from the start reversed the sign of dChi2DAlpha
              elemdata.m_dChi2DAlpha(idof)        += weight * residual ; 
              elemdata.m_d2Chi2DAlpha2(idof,idof) += weight ;
              ++ totalnumconstraints;
              totalchisq += weight * residual * residual ;
              debug() << "adding survey constraint: "
                      << (*ielem)->name() << " "
                      << currentdelta.parName(idof) << " "
                      << survey->par[idof] << " " << survey->err[idof] << endreq ;
            } 
            else 
            {
              warning() << "No survey constraint for dof: "
                        << (*ielem)->name() << " " << currentdelta.parName(idof) << endreq ;
            }
          }//end if ielem..isActive
          
        }
      }
    }
    logmessage << "Total chisquare of survey constraints: " << totalchisq << " / " << totalnumconstraints << std::endl ;
    return LHCb::ChiSquare(totalchisq,totalnumconstraints) ;
  }   

  LHCb::ChiSquare AlignChisqConstraintTool::chiSquare(const AlignmentElement& element,
                                                      bool activeonly ) const
  {
    size_t totalnumconstraints(0) ;
    double totalchisq(0) ;
    const SurveyData* survey = findSurveyData( element ) ;
    if( survey ) 
    {
      AlParameters currentdelta = element.currentLocalDelta() ;
      for(int idof=0 ; idof<6; ++idof ) 
        if( (!activeonly || element.dofMask().isActive( idof ) ) &&
            survey->err[idof] > 0 ) 
        {
          double weight = 1/( survey->err[idof] * survey->err[idof] ) ;
          double residual = survey->par[idof] - currentdelta.parameters()(idof) ;
          ++totalnumconstraints;
          totalchisq += weight * residual * residual ;
        }
    }
    return LHCb::ChiSquare(totalchisq,totalnumconstraints) ;
  }
}

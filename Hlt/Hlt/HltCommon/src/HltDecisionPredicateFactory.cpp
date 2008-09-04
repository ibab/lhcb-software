#include "HltDecisionPredicateFactory.h"
#include "GaudiKernel/ToolFactory.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/HltDecReports.h"
#include "Event/HltDecReport.h"
#include <boost/spirit.hpp>
#include <boost/spirit/actor.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/ref.hpp>
#include <memory>

using namespace std;
using namespace boost;
using namespace boost::spirit;
using namespace boost::lambda;

namespace {
    // define the elements in the parse tree...

    ostream& operator<<(ostream& o, const HltDecisionPredicateFactory::decisionPredicate& e) { return e.print(o); }

    class eval_present: public HltDecisionPredicateFactory::decisionPredicate {
    public:
        eval_present(const std::string& decision) : m_decision( decision ) {}
        bool operator()(const LHCb::HltDecReports& in) const  { return in.decReport(m_decision)!=0; }
        ostream& print(ostream& os) const { return os << "present(" << m_decision <<")"; }
    private:
        const std::string m_decision;
    };

    class eval_passed  : public HltDecisionPredicateFactory::decisionPredicate {
    public:
        eval_passed(const std::string& decision) : m_decision( decision ) {}
        bool operator()(const LHCb::HltDecReports& in) const  { const LHCb::HltDecReport *r = in.decReport(m_decision); return r!=0 && r->decision(); }
        ostream& print(ostream& os) const { return os << "passed("<<m_decision<<")"; }
    private:
        const std::string m_decision;
    };

    template <typename OP>
    class eval_binary : public HltDecisionPredicateFactory::decisionPredicate {
    public:
        eval_binary(HltDecisionPredicateFactory::decisionPredicate* lhs,
                    HltDecisionPredicateFactory::decisionPredicate* rhs, const char * label) 
                    : m_lhs(lhs),m_rhs(rhs),m_label(label) {}
        ~eval_binary() { }
        bool operator()(const LHCb::HltDecReports& in)  const  { return OP()( (*m_lhs)(in),(*m_rhs)(in)); }
        ostream& print(ostream& os) const { return os << m_label << "[" << *m_lhs << "," << *m_rhs << "]" ; }
    private:
        auto_ptr<HltDecisionPredicateFactory::decisionPredicate> m_lhs,m_rhs;
        const char *m_label;
    };

    template <typename OP>
    class eval_unary : public HltDecisionPredicateFactory::decisionPredicate {
    public:
        eval_unary(HltDecisionPredicateFactory::decisionPredicate* rhs, const char *label) 
            : m_rhs(rhs), m_label(label) {}
        ~eval_unary() { }
        bool operator()(const LHCb::HltDecReports& in)  const  { return OP()((*m_rhs)(in)); }
        ostream& print(ostream& os) const { return os << m_label << "[" << *m_rhs << "]" ; }
    private:
        auto_ptr<HltDecisionPredicateFactory::decisionPredicate> m_rhs;
        const char *m_label;
    };

    struct eval_and : eval_binary< std::logical_and<bool> > {
        eval_and(HltDecisionPredicateFactory::decisionPredicate* lhs,
                 HltDecisionPredicateFactory::decisionPredicate* rhs) 
                    : eval_binary< std::logical_and<bool> >( lhs,rhs,"AND") {}
    };
    struct eval_or : eval_binary< std::logical_or<bool> > {
        eval_or(HltDecisionPredicateFactory::decisionPredicate* lhs,
                 HltDecisionPredicateFactory::decisionPredicate* rhs) 
                    : eval_binary< std::logical_or<bool> >( lhs,rhs,"OR") {}
    };
    struct eval_not : eval_unary< std::logical_not<bool> > {
        eval_not(HltDecisionPredicateFactory::decisionPredicate* rhs)
                    : eval_unary< std::logical_not<bool> >( rhs,"NOT") {}
    };


    class combiner_factory {
    public:
        typedef IHltDecisionPredicateFactory::decisionPredicate eval;
        static eval* create(const std::string& input) {
            combiner_factory p(input); return p.valid() ? p.pop() : (eval*)0 ;
        }
        ~combiner_factory() { assert(m_stack.empty()); }
    private:
        combiner_factory(const std::string& input) {

            // TODO simplify using closures/phoenix
            // TODO see http://www.boost.org/doc/libs/1_34_1/libs/spirit/doc/closures.html

            // define our grammar, 
            // and associate parser actions
            rule<phrase_scanner_t> decision, term, unary, expression;

            term       = (+alnum_p)                         [ bind(&combiner_factory::op_passed,this,_1,_2)] 
                       | '(' >> expression >> ')'
                       ;
            unary      = term 
                       | ( '!' >> term )                    [ bind(&combiner_factory::op_not,this,_1,_2) ]
                       ;
#if 0
            lhs        = 

            bin_op     = '=='
                       | '<'
                       | '>'
                       ;
            rhs        = (+int_p) 
                       | lhs
                       ;

            condition  = lhs >> bin_op >> rhs;
#endif
            expression = unary >> *( ( '|' >> unary )       [ bind(&combiner_factory::op_or ,this,_1,_2) ]
                                   | ( '&' >> unary )       [ bind(&combiner_factory::op_and,this,_1,_2) ]
                                   );

            parse_info<> info = parse( input.c_str(), expression, space_p  );

            m_valid = info.full;
            if (!m_valid) {
                cerr << "Parsing failed at:" << info.stop << endl;
            }
        }

        bool valid() const { return m_valid;}

        combiner_factory& op_passed(const char* begin, const char *end) {
           return push(new eval_passed(string(begin,end)));
        }
        combiner_factory& op_or(const char*,const char*) {
           eval* lhs = pop(); eval* rhs = pop(); 
           return push( new eval_or(lhs,rhs) );
        }
        combiner_factory& op_and(const char*,const char*) {
           eval* lhs = pop(); eval* rhs = pop(); 
           return push( new eval_and(lhs,rhs) );
        }
        combiner_factory& op_not(const char*,const char*) {
           return push( new eval_not( pop() ) );
        }

        eval* pop() { assert(!m_stack.empty()); eval* t = m_stack.back(); m_stack.pop_back(); return t; }
        combiner_factory& push(eval* t) { m_stack.push_back(t); return *this;}

        std::vector<eval*> m_stack;
        bool m_valid;
    };
}


DECLARE_TOOL_FACTORY( HltDecisionPredicateFactory );

HltDecisionPredicateFactory::HltDecisionPredicateFactory( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent)
    : GaudiTool(type,name,parent)                                                                
{ 
    declareInterface<IHltDecisionPredicateFactory>(this);
}

HltDecisionPredicateFactory::~HltDecisionPredicateFactory()
{ }

IHltDecisionPredicateFactory::decisionPredicate* 
HltDecisionPredicateFactory::create(const std::string& specification) const {
   return combiner_factory::create(specification); 
}

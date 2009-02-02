#include "EvtGenBase/EvtMathematicaFn.hh"
#include "EvtGenModels/EvtBToVllBenchMarkPhysicsModels.hh"
#include "EvtGenModels/EvtBToVllConstants.hh"

#include <sstream>

const std::string qcd::GenericModel::modelName = "Generic_Model";
const std::string qcd::GenericModel::modelCommand = "wilsonCoefficients";
qcd::WCPtr qcd::GenericModel::getWilsonCoefficientsMW() const{
	qcd::SMPhysicsModel* model = new qcd::SMPhysicsModel;
	qcd::WCPtr C = model->getWilsonCoefficientsMW();
	
	//nasty and unreliable command parser
	std::ostringstream valueString;
	unsigned int coefficientNumber = 0;
	unsigned int coefficientCount = 0;
	bool doMultiply = false;//'*' or '='
	
	/**
	 * The following parser allows for the setting or modification
	 * of arbitrary wilson coefficients. The syntax is as follows:
	 * 
	 * wilsonCoefficients(C1=0.16:C2=0.05:C10*0.95:)
	 * 
	 * So the individual values can either be set directly, or the SM
	 * values modified by a factor. In each case, the statement must
	 * be ended with a ':' to be stored. The parser is probably
	 * not super reliable, so be careful! 
	 */
	for (std::string::const_iterator it = command.begin(); it != command.end(); ++it) {
			const char c = *it;

			switch (c) {
			case ':': //new command so reset
				{
					WilsonType value;
					if(valueString.str() != ""){
						std::istringstream in(valueString.str());
						in >> value;
						//do we multiply or replace
						const WilsonType set = doMultiply ? value*(*C)(coefficientNumber) : value;
						(*C)(coefficientNumber) = set;
						std::cout << "C(" << coefficientNumber << ") = " << set << std::endl;
					}
					valueString.str("");
					coefficientCount--;//count committed WCs
					doMultiply = false;//default is to replace
				}
				break;
			case 'c'://start of a Wilson coefficient index
			case 'C':
				coefficientCount++;//start of a new WC;
				break;
			case '*'://note fall-through here
				doMultiply = true;
			case '=':
				{
					const std::string index = valueString.str();
					if(index == ""){
						report(ERROR, "EvtGen")
						<< "EVTBTOKSTARLLDURHAM07_MODEL: Wilson coefficient without number found. Check your decay file." << std::endl;
						::abort();
					}
					std::istringstream(index) >> coefficientNumber;
					if( (coefficientNumber < 1) || (coefficientNumber > C->getOperatorBasis()) ){
						report(ERROR, "EvtGen")
						<< "EVTBTOKSTARLLDURHAM07_MODEL: Wilson coefficient index outside of allowed range. Check your decay file." << std::endl;
						::abort();
					}
					valueString.str("");
					break;
				}
			default:
				valueString << c;
			};
		}
	
	if(coefficientCount){
		report(ERROR, "EvtGen")
		<< "EVTBTOKSTARLLDURHAM07_MODEL: Wilson coefficient not saved. Each must end with a ':'. Check your decay file." << std::endl;
		::abort();
	}
	
	delete model;
	return C;
}

qcd::WCPtr qcd::TestPhysicsModel::getWilsonCoefficientsMW() const{
		
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(constants::MW,12,DIMENSION8);
	(*C)(1) = 0.1608606666316409;
	(*C)(2) = 1.0018961461369336;
	(*C)(3) = -0.0003015998220831912;
	(*C)(4) = -0.003469390761680933;
	(*C)(5) = 0.00003468699101875856;
	(*C)(6) = 0.0000650381081601723;
	(*C)(7) = -0.2215000427422243;
	(*C)(8) = -0.11944650868493106;
	(*C)(11) = 0;
	(*C)(12) = 0;
	return qcd::WCPtr(C);
}

qcd::WCPtr qcd::SMPhysicsModel::getWilsonCoefficientsMW() const{
		
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(constants::MW,12,DIMENSION10);
	(*C)(1) = 0.16085744685445263;
	(*C)(2) = 1.0018961455766344;
	(*C)(3) = -0.0003030009873295852;
	(*C)(4) = -0.003386738264675202;
	(*C)(5) = 0.000034931503595122554;
	(*C)(6) = 0.0000654965692408548;
	(*C)(7) = -0.0021047340747641936;
	(*C)(8) = -0.001141068926574835;
	(*C)(9) = 0.018836431075389393;
	(*C)(10) = -0.039757638588932206;
	(*C)(11) = 0;
	(*C)(12) = 0;
	return qcd::WCPtr(C);
}

qcd::WCPtr qcd::NegC7PhysicsModel::getWilsonCoefficientsMW() const{
	qcd::SMPhysicsModel* model = new qcd::SMPhysicsModel;
	qcd::WCPtr C = model->getWilsonCoefficientsMW();
	(*C)(7) = - (*C)(7);//set C7 to be -ve of what it is in SM
	delete model;
	return C;
}

qcd::WCPtr qcd::LHTPhysicsModel::getWilsonCoefficientsMW() const{
		
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(constants::MW,12,DIMENSION10);
	(*C)(1) = 0.16085744685445263;
	(*C)(2) = 1.0018961455766344;
	(*C)(3) = -0.0003030009873295852;
	(*C)(4) = -0.003386738264675202;
	(*C)(5) = 0.000034931503595122554;
	(*C)(6) = 0.0000654965692408548;
	(*C)(7) = -0.002141452927712699;
	(*C)(8) = -0.001141068926574835;
	(*C)(9) = EvtComplex(0.019566198421429443,0.00005986921935614682);
	(*C)(10) = EvtComplex(-0.04433815341403649,-0.0003831882825155768);
	(*C)(11) = 0;
	(*C)(12) = 0;	
	return qcd::WCPtr(C);
}

qcd::WCPtr qcd::UEDPhysicsModel::getWilsonCoefficientsMW() const{
		
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(constants::MW,12,DIMENSION10);
	(*C)(1) = 0.16085744685445263;
	(*C)(2) = 1.0018961455766344;
	(*C)(3) = -0.0003030009873295852;
	(*C)(4) = -0.003386738264675202;
	(*C)(5) = 0.000034931503595122554;
	(*C)(6) = 0.0000654965692408548;
	(*C)(7) = -0.0014711630647824129;
	(*C)(8) = -0.0009418367338016073;
	(*C)(9) = 0.019308302237859537;
	(*C)(10) = -0.04565602811980898;
	(*C)(11) = 0;
	(*C)(12) = 0;
	return qcd::WCPtr(C);
}

qcd::WCPtr qcd::MSSMLowTanBeta::getWilsonCoefficientsMW() const{
		
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(constants::MW,12,DIMENSION10);
	(*C)(1) = 0.16085744685445263;
	(*C)(2) = 1.0018961455766344;
	(*C)(3) = -0.0003022425584746714;
	(*C)(4) = EvtComplex(-0.0033977706457551057,0.000013717957679417306);
	(*C)(5) = 0.00003484553070117339;
	(*C)(6) = 0.00006533536983759025;
	(*C)(7) = EvtComplex(-0.0025226749637721844,0.000033769411681613795);
	(*C)(8) = EvtComplex(-0.0019935042686775204,-0.000024440802644736547);
	(*C)(9) = EvtComplex(0.01787255182164677,-0.0012391010518830363);
	(*C)(10) = EvtComplex(-0.03070263568801261,0.0022375675294246606);
	(*C)(11) = 0;
	(*C)(12) = 0;
	
	return qcd::WCPtr(C);
}

qcd::WCPtr qcd::MSSMHighTanBeta::getWilsonCoefficientsMW() const{
		
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(constants::MW,12,DIMENSION10);
	(*C)(1) = 0.16085744685445263;
	(*C)(2) = 1.0018961455766344;
	(*C)(3) = -0.0003029143959734802;
	(*C)(4) = -0.0033877885597099542;
	(*C)(5) = 0.000034921732019190476;
	(*C)(6) = 0.00006547824753598215;
	(*C)(7) = EvtComplex(-0.0032387696956034666,0.000575241635577403);
	(*C)(8) = EvtComplex(-0.0020744923298895053,0.0004799806502176044);
	(*C)(9) = EvtComplex(0.01770661057437654,-0.00008150984281768796);
	(*C)(10) = EvtComplex(-0.04915478900395872,0.005902752109883426);
	(*C)(11) = EvtComplex(-0.011800772703488295,-0.011653798933536113);
	(*C)(12) = EvtComplex(-0.16731362914874073,-0.011038040936514561);
	
	return qcd::WCPtr(C);
}


#include "EvtGenBase/EvtReport.hh"

#include "EvtGenBase/EvtMathematicaFn.hh"
#include "EvtGenModels/EvtBToVllBenchMarkPhysicsModels.hh"
#include "EvtGenModels/EvtBToVllConstants.hh"

#include <sstream>

qcd::WCPtr qcd::LeftHandedPhysicsModel::getRightWilsonCoefficientsMW() const{
	//make same scale etc as left handed versions
	qcd::WCPtr C = getLeftWilsonCoefficientsMW();
	qcd::WilsonCoefficients<WilsonType>* CR = new qcd::WilsonCoefficients<WilsonType>(C->getScale(),
				C->getOperatorBasis(),C->getDimension());
	//set all elements to 0.0
	for(unsigned int i = 1; i <= C->getDimension(); i++){
		(*CR)(i) = 0.0;
	}
	return qcd::WCPtr(CR);
}

const std::string qcd::GenericModel::modelCommand = "wilsonCoefficients";
qcd::WCPtr qcd::GenericModel::getLeftWilsonCoefficientsMW() const{
	//start with the SM
	qcd::SMPhysicsModel* model = new qcd::SMPhysicsModel;
	qcd::WCPtr C = model->getLeftWilsonCoefficientsMW();
	qcd::WCPtr CR = model->getRightWilsonCoefficientsMW();
	delete model;
	parseCommand(C.get(),CR.get());
	return C;
}
qcd::WCPtr qcd::GenericModel::getRightWilsonCoefficientsMW() const{
	//start with the SM
	qcd::SMPhysicsModel* model = new qcd::SMPhysicsModel;
	qcd::WCPtr C = model->getLeftWilsonCoefficientsMW();
	qcd::WCPtr CR = model->getRightWilsonCoefficientsMW();
	delete model;
	parseCommand(C.get(),CR.get());
	return CR;
}
void qcd::GenericModel::parseCommand(const WilsonCoefficients<WilsonType>* C,
		const WilsonCoefficients<WilsonType>* CR) const{
	
	//nasty and unreliable command parser
	std::ostringstream valueString;
	unsigned int coefficientNumber = 0;
	unsigned int coefficientCount = 0;
	bool doMultiply = false;//'*' or '='
	bool isRight = false;
	
	/**
	 * The following parser allows for the setting or modification
	 * of arbitrary wilson coefficients. The syntax is as follows:
	 * 
	 * wilsonCoefficients(C1=0.16:C2=0.05:R10*0.95:)
	 * 
	 * So the individual values can either be set directly, or the SM
	 * values modified by a factor. In each case, the statement must
	 * be ended with a ':' to be stored. The parser is probably
	 * not super reliable, so be careful! 
	 * 
	 * C means left handed, R means right handed (C' in other notation)
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
						//choose C or C' here
						if(!isRight){
							//do we multiply or replace
							const WilsonType set = doMultiply ? value*(*C)(coefficientNumber) : value;
							(*C)(coefficientNumber) = set;
							report(INFO, "EvtGen") << "C(" << coefficientNumber << ") = " << set << std::endl;
						}else{
							//do we multiply or replace
							const WilsonType set = doMultiply ? value*(*CR)(coefficientNumber) : value;
							(*CR)(coefficientNumber) = set;
							report(INFO, "EvtGen") << "R(" << coefficientNumber << ") = " << set << std::endl;
						}
					}
					valueString.str("");
					coefficientCount--;//count committed WCs
					doMultiply = false;//default is to replace
					isRight = false;//default is left-handed
				}
				break;
			case 'r'://start of a Wilson coefficient index
			case 'R':
				isRight = true;//note fall-through here
			case 'c':
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
					assert(C->getOperatorBasis() == CR->getOperatorBasis());//implicit assumption here
					if( (coefficientNumber < 1) || (coefficientNumber > C->getOperatorBasis()) || (coefficientNumber > CR->getOperatorBasis())){
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
}

qcd::WCPtr qcd::SMPhysicsModel::getLeftWilsonCoefficientsMW() const{
		
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

qcd::WCPtr qcd::NegC7PhysicsModel::getLeftWilsonCoefficientsMW() const{
	qcd::SMPhysicsModel* model = new qcd::SMPhysicsModel;
	qcd::WCPtr C = model->getLeftWilsonCoefficientsMW();
	(*C)(7) = - (*C)(7);//set C7 to be -ve of what it is in SM
	delete model;
	return C;
}

qcd::WCPtr qcd::LHTPhysicsModel::getLeftWilsonCoefficientsMW() const{
		
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

qcd::WCPtr qcd::UEDPhysicsModel::getLeftWilsonCoefficientsMW() const{
		
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

qcd::WCPtr qcd::MSSMLowTanBeta::getLeftWilsonCoefficientsMW() const{
		
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

qcd::WCPtr qcd::MSSMHighTanBeta::getLeftWilsonCoefficientsMW() const{
		
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


qcd::WCPtr qcd::FBMSSMPhysicsModel::getLeftWilsonCoefficientsMW() const{
	
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(constants::MW,12,DIMENSION10);
	(*C)(1) = 0.16027730228555678;
	(*C)(2) = 1.001883694120693;
	(*C)(3) = -0.0003010297434531897;
	(*C)(4) = -0.0033763414871926236;
	(*C)(5) = 0.00003470534399433969;
	(*C)(6) = 0.00006507251998938692;
	(*C)(7) = EvtComplex(0.002911053507490618,0.0069589066220656715);
	(*C)(8) = EvtComplex(-0.000058506886215297705,0.0021567506338936756);
	(*C)(9) = 0.019126464524867607;
	(*C)(10) = -0.039328048091928594;
	(*C)(11) = EvtComplex(-0.04446677247779823,-0.056326205784609125);
	(*C)(12) = EvtComplex(0.042672335465123945,0.05439030834907253);
	return qcd::WCPtr(C);
}
qcd::WCPtr qcd::FBMSSMPhysicsModel::getRightWilsonCoefficientsMW() const{
	
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(constants::MW,12,DIMENSION10);
	(*C)(1) = 0.0;
	(*C)(2) = 0.0;
	(*C)(3) = 0.0;
	(*C)(4) = 0.0;
	(*C)(5) = 0.0;
	(*C)(6) = 0.0;
	(*C)(7) = EvtComplex(0.010608359855623632,-0.01272196868892201);
	(*C)(8) = EvtComplex(0.002402061155750922,-0.004007472550665057);
	(*C)(9) = EvtComplex(0.002381990935713308,-0.000052802454496512474);
	(*C)(10) = 0.003902447228122236;
	(*C)(11) = 0.0;
	(*C)(12) = 0.0;
	return qcd::WCPtr(C);
	
}

qcd::WCPtr qcd::GMSSMPhysicsModel::getLeftWilsonCoefficientsMW() const{
	
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(constants::MW,12,DIMENSION10);
	(*C)(1) = 0.16027730228555678;
	(*C)(2) = 1.001883694120693;
	(*C)(3) = -0.0003010297434531897;
	(*C)(4) = -0.0033763414871926236;
	(*C)(5) = 0.00003470534399433969;
	(*C)(6) = 0.00006507251998938692;
	(*C)(7) = EvtComplex(-0.0004803778839307768,0.000016247731584620343);
	(*C)(8) = EvtComplex(0.0002768848840513995,0.00006166700793876063);
	(*C)(9) = 0.018878325003090557;
	(*C)(10) = -0.0410450721295377;
	(*C)(11) = EvtComplex(0.0002987273062585297,0.0007724666520045042);
	(*C)(12) = EvtComplex(0.0007818674483701221,0.0006982595368483323);
	return qcd::WCPtr(C);
}
qcd::WCPtr qcd::GMSSMPhysicsModel::getRightWilsonCoefficientsMW() const{
	
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(constants::MW,12,DIMENSION10);
	(*C)(1) = 0.0;
	(*C)(2) = 0.0;
	(*C)(3) = 0.0;
	(*C)(4) = 0.0;
	(*C)(5) = 0.0;
	(*C)(6) = 0.0;
	(*C)(7) = EvtComplex(0.1744518871605414,0.1744955440720616);
	(*C)(8) = EvtComplex(0.4984731347470766,0.5050215413429553);
	(*C)(9) = EvtComplex(0.018171005138395074,0.018494808888938386);
	(*C)(10) = EvtComplex(0.0033620098506148494,0.003352657913120917);
	(*C)(11) = 0.0;
	(*C)(12) = 0.0;
	return qcd::WCPtr(C);
	
}



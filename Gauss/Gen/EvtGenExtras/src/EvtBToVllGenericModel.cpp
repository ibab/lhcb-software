#include "EvtGenBase/EvtReport.hh"

#include "EvtGenBase/EvtMathematicaFn.hh"
#include "EvtGenModels/EvtBToVllBenchMarkPhysicsModels.hh"
#include "EvtGenModels/EvtBToVllGenericModel.hh"
#include "EvtGenModels/EvtBToVllConstants.hh"

#include <functional>
#include <map>
#include <sstream>

const std::string qcd::GenericModel::modelCommand = "wilsonCoefficients";
qcd::WCPtr qcd::GenericModel::getLeftNewPhysicsDeltasMW() const{
	//start with an empty list
	qcd::WCPtr C = getZeroWCs();
	qcd::WCPtr CR = getZeroWCs();
	parseCommand(C.get(),CR.get());
	return C;
}

qcd::WCPtr qcd::GenericModel::getRightWilsonCoefficientsMW() const{
	//start with an empty list
	qcd::WCPtr C = getZeroWCs();
	qcd::WCPtr CR = getZeroWCs();
	parseCommand(C.get(),CR.get());
	return CR;
}
void qcd::GenericModel::parseCommand(const WilsonCoefficients<WilsonType>* C,
		const WilsonCoefficients<WilsonType>* CR) const{

	//start with the SM
	qcd::SMPhysicsModel* model = new qcd::SMPhysicsModel;
	qcd::WCPtr C_SM = model->getLeftWilsonCoefficientsMW();
	qcd::WCPtr CR_SM = model->getRightWilsonCoefficientsMW();
	delete model;

	//nasty and unreliable command parser
	std::ostringstream valueString;
	unsigned int coefficientNumber = 0;
	unsigned int coefficientCount = 0;
	bool isRight = false;
	char op = '=';
	
	/**
	 * The following parser allows for the setting or modification
	 * of arbitrary wilson coefficients. The syntax is as follows:
	 * 
	 * wilsonCoefficients(L1=0.16:L2=0.05:R10*=0.95:)
	 * 
	 * So the individual values can either be set directly, or the SM
	 * values modified by a factor. In each case, the statement must
	 * be ended with a ':' to be stored. The parser is probably
	 * not super reliable, so be careful! 
	 * 
	 * L means left handed (C in other notation)
	 * R means right handed (C' in other notation)
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
							const WilsonType smValue = (*C_SM)(coefficientNumber);
							//do we apply an operator?
							WilsonType set;
							if(op == '='){
								set = value;
							}else{
								switch (op) {
								case '+':
									set = smValue + value;
									break;
								case '-':
									set = smValue - value;
									break;
								case '/':
									set = smValue/value;
									break;
								case '*':
									set = smValue*value;
									break;
								};
							}
							(*C)(coefficientNumber) = set - smValue;
							report(INFO, "EvtGen") << "L(" << coefficientNumber << ") = " << (*C)(coefficientNumber) << std::endl;
						}else{
							const WilsonType smValue = (*CR_SM)(coefficientNumber);
							//do we apply an operator?
							WilsonType set;
							if(op == '='){
								set = value;
							}else{
								switch (op) {
								case '+':
									set = smValue + value;
									break;
								case '-':
									set = smValue - value;
									break;
								case '/':
									set = smValue/value;
									break;
								case '*':
									set = smValue*value;
									break;
								};
							}
							(*CR)(coefficientNumber) = set - smValue;
							report(INFO, "EvtGen") << "R(" << coefficientNumber << ") = " << (*CR)(coefficientNumber) << std::endl;
						}
					}
					valueString.str("");
					coefficientCount--;//count committed WCs
					isRight = false;//default is left-handed
					op = '=';
				}
				break;
			case 'r'://start of a Wilson coefficient index
			case 'R':
				isRight = true;//note fall-through here
			case 'l':
			case 'L':
			case 'c':
			case 'C':
				coefficientCount++;//start of a new WC;
				break;
			case '+'://operators
			case '-':
			case '/':
			case '*':
				{
					//operators are *=, +=, %= etc.
					std::string::const_iterator n = it + 1;
					if( (n != command.end()) && (*n == '=')){
						op = *it;
					}else{
						if( (*it == '+') || (*it == '-')){
							valueString << c;
						}else{
							report(ERROR, "EvtGen")
							<< "BTOKSTARLLDURHAM07_MODEL: Unsupported operator. Use +=, -=, *=, /=." << std::endl;
							::abort();
						}
					}
					break;
				}
			case '=':
				{
					const std::string index = valueString.str();
					if(index == ""){
						report(ERROR, "EvtGen")
						<< "BTOKSTARLLDURHAM07_MODEL: Wilson coefficient without number found. Check your decay file." << std::endl;
						::abort();
					}
					std::istringstream(index) >> coefficientNumber;
					assert(C->getOperatorBasis() == CR->getOperatorBasis());//implicit assumption here
					if( (coefficientNumber < 1) || (coefficientNumber > C->getOperatorBasis()) || (coefficientNumber > CR->getOperatorBasis())){
						report(ERROR, "EvtGen")
						<< "BTOKSTARLLDURHAM07_MODEL: Wilson coefficient index outside of allowed range. Check your decay file." << std::endl;
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
		<< "BTOKSTARLLDURHAM07_MODEL: Wilson coefficient not saved. Each must end with a ':'. Check your decay file." << std::endl;
		::abort();
	}
}

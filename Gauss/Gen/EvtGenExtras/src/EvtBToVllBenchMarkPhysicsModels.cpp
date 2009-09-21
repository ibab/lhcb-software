#include "EvtGenBase/EvtReport.hh"

#include "EvtGenBase/EvtMathematicaFn.hh"
#include "EvtGenModels/EvtBToVllBenchMarkPhysicsModels.hh"
#include "EvtGenModels/EvtBToVllConstants.hh"

#include <functional>
#include <map>
#include <sstream>

/** utility function to return an array of zeros with the correct settings. */
qcd::WCPtr qcd::SMPhysicsModel::getZeroWCs() const{
	//make same scale etc as left handed versions
	qcd::WCPtr C = getLeftWilsonCoefficientsMW();
	qcd::WilsonCoefficients<WilsonType>* Z = new qcd::WilsonCoefficients<WilsonType>(C->getScale(),
				C->getOperatorBasis(),C->getDimension());
	//set all elements to 0.0
	for(unsigned int i = 1; i <= C->getDimension(); i++){
		(*Z)(i) = 0.0;
	}
	return qcd::WCPtr(Z);
}

qcd::WCPtr qcd::SMPhysicsModel::getLeftWilsonCoefficientsMW() const{
	
	qcd::WilsonCoefficients<WilsonType>* C = new qcd::WilsonCoefficients<WilsonType>(qcd::MU_MW,12,DIMENSION10);	
	(*C)(1) = 0.16027730228555678;
	(*C)(2) = 1.001883694120693;
	(*C)(3) = -0.0003010297434531897;
	(*C)(4) = -0.0033763414871926236;
	(*C)(5) = 0.00003470534399433969;
	(*C)(6) = 0.00006507251998938692;
	(*C)(7) = -0.0020965376427322536;
	(*C)(8) = -0.0011364429105011212;
	(*C)(9) = 0.01877215804087482;
	(*C)(10) = -0.03961638355851951;
	(*C)(11) = 0.0;
	(*C)(12) = 0.0;
	return qcd::WCPtr(C);
}

qcd::WCPtr qcd::LHTPhysicsModel::getLeftNewPhysicsDeltasMW() const{
		
	qcd::WCPtr C = getZeroWCs();
	(*C)(7) = EvtComplex(-0.00379132,-0.0008);
	(*C)(9) = EvtComplex(0.07535043478260867,0.00618165739130435);
	(*C)(10) = EvtComplex(-0.4729504347826087,-0.03956521739130435);
	return C;
}

qcd::WCPtr qcd::UEDPhysicsModel::getLeftNewPhysicsDeltasMW() const{
		
	qcd::WCPtr C = getZeroWCs();
	(*C)(7) = 0.008556503668868952;
	(*C)(8) = 0.03294389846399369;
	(*C)(9) = 0.009507910468826578;
	(*C)(10) = -0.1188488808603323;
	return C;
}

qcd::WCPtr qcd::FBMSSMPhysicsModel::getLeftNewPhysicsDeltasMW() const{
		
	qcd::WCPtr C = getZeroWCs();
	(*C)(7) = EvtComplex(0.5187532692183312,0.7208966251609806);
	(*C)(8) = EvtComplex(0.11166703107972265,0.22342507777841597);
	(*C)(9) = 0.036703804556474026;
	(*C)(10) = 0.029869644193890477;
	(*C)(11) = EvtComplex(-0.04446677247779823,-0.056326205784609125);
	(*C)(12) = EvtComplex(0.042672335465123945,0.05439030834907253);
	return C;
}

qcd::WCPtr qcd::FBMSSMPhysicsModel::getRightWilsonCoefficientsMW() const{

	qcd::WCPtr C = getZeroWCs();
	(*C)(7) = EvtComplex(0.010608359855623632,-0.01272196868892201);
	(*C)(8) = EvtComplex(0.002402061155750922,-0.004007472550665057);
	(*C)(9) = EvtComplex(0.002381990935713308,-0.000052802454496512474);
	(*C)(10) = 0.003902447228122236;
	return C;
	
}

qcd::WCPtr qcd::GMSSMPhysicsModel::getLeftNewPhysicsDeltasMW() const{
		
	qcd::WCPtr C = getZeroWCs();
	(*C)(7) = EvtComplex(0.1674234443880391,0.0016831573553141637);
	(*C)(8) = EvtComplex(0.14641139659906605,0.006388293495111137);
	(*C)(9) = EvtComplex(0.010998193957975154,-0.00014630964227521778);
	(*C)(10) = EvtComplex(-0.14800267127990357,-0.000013538357564939124);
	(*C)(11) = EvtComplex(0.0002987273062585297,0.0007724666520045042);
	(*C)(12) = EvtComplex(0.0007818674483701221,0.0006982595368483323);
	return C;
}

qcd::WCPtr qcd::GMSSMPhysicsModel::getRightWilsonCoefficientsMW() const{

	qcd::WCPtr C = getZeroWCs();
	(*C)(7) = EvtComplex(0.1744518871605414,0.1744955440720616);
	(*C)(8) = EvtComplex(0.4984731347470766,0.5050215413429553);
	(*C)(9) = EvtComplex(0.018171005138395074,0.018494808888938386);
	(*C)(10) = EvtComplex(0.0033620098506148494,0.003352657913120917);
	return C;
	
}



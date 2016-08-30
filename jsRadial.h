#pragma once
#include "common.h"

class JSRadial : public MPxNode
{
public:
	JSRadial();
	virtual ~JSRadial();
	static void* creator();
	static MStatus initialize();

	virtual MStatus compute(const MPlug &plug, MDataBlock &data);
	
	double RBF(const double &r, const double &height, const double &sigma, const short &ftype);

	static const char* kName;
	static MTypeId kId;

	// Debug and test attrs, maybe delete later
	static MObject aFunctionType;
	static MObject aHeight;
	static MObject aSigma;
	static MObject aRotationMultiplier;
		
	// Node attrs
	static MObject aPose;
	static MObject aTargets;
	static MObject aOutScale;
	static MObject aOutInterpolate;

private:
	MDoubleArray norms_;
	std::vector<Eigen::VectorXd> vecs_;
};
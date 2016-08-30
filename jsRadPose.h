#pragma once
#include "common.h"

class JSRadPose : public MPxNode
{
public:
	JSRadPose();
	virtual ~JSRadPose();
	static void* creator();
	static MStatus initialize();

	virtual MStatus compute(const MPlug &plug, MDataBlock &data);

	double RBF(double r, double height, double sigma, short ftype);

	static const char* kName;
	static MTypeId kId;

	// Node attrs
	// RBF solver attrs
	static MObject aFunctionType;
	static MObject aHeight;
	static MObject aGlobalSigma;
	static MObject aUseGlobalSigma;

	// Multiplier attrs for adjusting relative weights of translate/rotate/scale/rgba
	static MObject aTranslateMultiplier;
	static MObject aRotateMultiplier;
	static MObject aScaleMultiplier;
	static MObject aRGBAMultiplier;

	// These two sets allow user to determine which attributes are inputs and outputs
	static MObject aIncludeTranslateX;
	static MObject aIncludeTranslateY;
	static MObject aIncludeTranslateZ;
	static MObject aIncludeRotate;
	static MObject aIncludeScaleX;
	static MObject aIncludeScaleY;
	static MObject aIncludeScaleZ;
	static MObject aIncludeR;
	static MObject aIncludeG;
	static MObject aIncludeB;
	static MObject aIncludeA;
	static MObject aInputIncludes;		// parent attribute

	// Input pose connections
	static MObject aPoseMatrix;
	static MObject aPoseColor;
	static MObject aPoseTransparency;

	// Input target connections
	static MObject aTargetMatrix;
	static MObject aTargetColor;
	static MObject aTargetTransparency;
	static MObject aTargetSigma;
	static MObject aTarget;				// parent attribute

	// Output connections
	static MObject aOutputTranslateX;
	static MObject aOutputTranslateY;
	static MObject aOutputTranslateZ;
	static MObject aOutputRotateX;
	static MObject aOutputRotateY;
	static MObject aOutputRotateZ;
	static MObject aOutputScaleX;
	static MObject aOutputScaleY;
	static MObject aOutputScaleZ;
	static MObject aOutputColor;
	static MObject aOutputTransparency;
	static MObject aOutputInterpolate;	// This output is per-target
	static MObject aOutputSigma;		// This output is per-target


private:
	// Member variables
	unsigned int maxDim_;
	unsigned int targetCount_;

	// RBF values
	short ftype_;
	double height_;
	double globalSigma;
	bool usesGlobalSigma_;

	// Multipliers
	double trsMult_;
	double rotMult_;
	double sclMult_;
	double colMult_;
	
	// Eigenlib vectors
	Eigen::VectorXd includeVec_;
	Eigen::VectorXd multVec_;
	Eigen::VectorXd poseVec_;
	std::vector<Eigen::VectorXd> targetVecs_;
	Eigen::VectorXd bVec_;
	Eigen::VectorXd weightVec_;
	
	MDoubleArray sigmas_;
	MDoubleArray rbfValues_;

};
#include "common.h"
#include "jsRadial.h"

const char* JSRadial::kName = "jsRadial";
MTypeId JSRadial::kId(0x00000003);

MObject JSRadial::aFunctionType;
MObject JSRadial::aHeight;
MObject JSRadial::aSigma;
MObject JSRadial::aRotationMultiplier;

MObject JSRadial::aPose;
MObject JSRadial::aTargets;
MObject JSRadial::aOutScale;
MObject JSRadial::aOutInterpolate;

JSRadial::JSRadial() {}
JSRadial::~JSRadial() {}
void* JSRadial::creator() { return new JSRadial; }

MStatus JSRadial::initialize()
{
	MStatus status;
	MFnEnumAttribute fnEnum;
	MFnNumericAttribute fnNumeric;
	MFnTypedAttribute fnTyped;

	// Debug and test attrs
	aFunctionType = fnEnum.create("functionType", "functionType");
	fnEnum.addField("Gaussian", 0);
	fnEnum.addField("Multiquadratic", 1);
	fnEnum.addField("Inverse Multiquadratic", 2);
	fnEnum.setKeyable(true);
	fnEnum.setStorable(true);
	addAttribute(aFunctionType);

	aHeight = fnNumeric.create("height", "height", MFnNumericData::kDouble, 1.0);
	fnNumeric.setKeyable(true);
	fnNumeric.setStorable(true);
	fnNumeric.setMin(0.00001);
	addAttribute(aHeight);

	aSigma = fnNumeric.create("sigma", "sigma", MFnNumericData::kDouble, 1.0);
	fnNumeric.setKeyable(true);
	fnNumeric.setStorable(true);
	fnNumeric.setMin(0.00001);
	addAttribute(aSigma);

	aRotationMultiplier = fnNumeric.create("rotationMultiplier", "rotationMultiplier", MFnNumericData::kDouble, 1.0);
	fnNumeric.setKeyable(true);
	fnNumeric.setSoftMax(true);
	addAttribute(aRotationMultiplier);

	// Node attrs
	aPose = fnTyped.create("pose", "pose", MFnData::kDoubleArray);
	fnTyped.setKeyable(true);
	fnTyped.setStorable(true);
	addAttribute(aPose);

	aTargets = fnTyped.create("targets", "targets", MFnData::kDoubleArray);
	fnTyped.setKeyable(true);
	fnTyped.setStorable(true);
	fnTyped.setArray(true);
	addAttribute(aTargets);

	// Output attrs
	aOutScale = fnNumeric.create("outScale", "outScale", MFnNumericData::kDouble, 0.1);
	fnNumeric.setKeyable(false);
	fnNumeric.setStorable(false);
	fnNumeric.setArray(true);
	addAttribute(aOutScale);

	aOutInterpolate = fnNumeric.create("outInterpolate", "outInterpolate", MFnNumericData::kDouble, 0.0);
	fnNumeric.setKeyable(false);
	fnNumeric.setStorable(false);
	addAttribute(aOutInterpolate);

	// Attribute affects
	attributeAffects(aFunctionType, aOutScale);
	attributeAffects(aHeight, aOutScale);
	attributeAffects(aSigma, aOutScale);
	attributeAffects(aRotationMultiplier, aOutScale);
	attributeAffects(aPose, aOutScale);
	attributeAffects(aTargets, aOutScale);

	attributeAffects(aFunctionType, aOutInterpolate);
	attributeAffects(aHeight, aOutInterpolate);
	attributeAffects(aSigma, aOutInterpolate);
	attributeAffects(aRotationMultiplier, aOutInterpolate);
	attributeAffects(aPose, aOutInterpolate);
	attributeAffects(aTargets, aOutInterpolate);
		
	return MS::kSuccess;
}

MStatus JSRadial::compute(const MPlug &plug, MDataBlock &data)
{
	MStatus status;
	if (plug != aOutScale && plug != aOutInterpolate) { return MS::kInvalidParameter; }
	
	// Get output handles
	MArrayDataHandle hArrayOutScale = data.outputArrayValue(aOutScale);
	MDataHandle hOutInterpolate = data.outputValue(aOutInterpolate);

	// Get input values
	short ftype = data.inputValue(aFunctionType).asShort();
	double height = data.inputValue(aHeight).asDouble();
	double sigma = data.inputValue(aSigma).asDouble();
	double rotMult = data.inputValue(aRotationMultiplier).asDouble();
	
	MObject oPose = data.inputValue(aPose).data();
	MFnDoubleArrayData fnPose(oPose);
	MDoubleArray pose = fnPose.array();
	short dim = pose.length();
	double poseNorm = 0.0;

	pose[3] *= rotMult;
	pose[4] *= rotMult;
	pose[5] *= rotMult;
		
	Eigen::VectorXd poseVec(7);
	poseVec << pose[0], pose[1], pose[2], pose[3], pose[4], pose[5], pose[6];
	poseNorm = poseVec.norm();
	

	MArrayDataHandle hArrayTargets = data.outputArrayValue(aTargets);
	unsigned int count = hArrayTargets.elementCount();

	// Build eigenlib matrix
	Eigen::MatrixXd mtx(count, count);

	// Initialize weights and solutions eigenlib vectors
	norms_.setLength(count);
	vecs_.resize(count);
	Eigen::VectorXd wvec(count);
	Eigen::VectorXd rvec(count);
	rvec << height, height, height;
	cout << "rvec == " << endl << rvec << endl;

	for (unsigned int i = 0; i < count; ++i)
	{
		// Get handles
		hArrayTargets.jumpToElement(i);
		MObject oTarget = hArrayTargets.inputValue().data();
		MFnDoubleArrayData fnTarget(oTarget);
		MDoubleArray target = fnTarget.array();

		// Apply rotMult to "normalize" rotation to translation
		target[3] *= rotMult;	target[4] *= rotMult;	target[5] *= rotMult;

		// Initialize eigenlib vector to scale target spheres
		Eigen::VectorXd vec(dim);
		vec << target[0], target[1], target[2], target[3], target[4], target[5], target[6];
		double norm = vec.norm();
		norms_[i] = norm;
		vecs_[i] = vec;
		Eigen::VectorXd distVec = poseVec - vec;
		double dist = fabs(distVec.norm());
		double rbf_scale = RBF(dist, height, sigma, ftype);

		// Populate eigenlib matrix
		for (unsigned int j = 0; j < count; ++j)
		{
			// Get norm of every target and find rbf of inorm - jnorm
			hArrayTargets.jumpToElement(j);
			MObject jTarget = hArrayTargets.inputValue().data();
			MFnDoubleArrayData fnJTarget(jTarget);
			MDoubleArray jArray = fnJTarget.array();

			// Apply rotMult to "normalize" rotation to translation
			jArray[3] *= rotMult;	jArray[4] *= rotMult;	jArray[5] *= rotMult;

			Eigen::VectorXd jvec(dim);
			jvec << jArray[0], jArray[1], jArray[2], jArray[3], jArray[4], jArray[5], jArray[6];
			Eigen::VectorXd diffVec = vec - jvec;
			
			double diffNorm = fabs(diffVec.norm());
			double val = RBF(diffNorm, height, sigma, ftype);
			mtx(i, j) = val;
		}

		hArrayOutScale.jumpToElement(i);
		MDataHandle hOutScale = hArrayOutScale.outputValue();
		hOutScale.setDouble(rbf_scale);

	}

	// Solve linear system with colPivHouseholderQr()
	wvec = mtx.colPivHouseholderQr().solve(rvec);

	cout << "wvec = " << endl << wvec << endl << endl;


	// Sum weighted RBFs
	double sum = 0.0;
	for (unsigned int i = 0; i < count; ++i)
	{
		Eigen::VectorXd diffVec = poseVec - vecs_[i];
		double rbf = RBF(fabs(diffVec.norm()), height, sigma, ftype);
		rbf *= wvec[i];
		sum += rbf;
	}

	hOutInterpolate.setDouble(sum);

	data.setClean(aOutScale);
	data.setClean(aOutInterpolate);


	return MS::kSuccess;
}

double JSRadial::RBF(const double &r, const double &height, const double &sigma, const short &ftype)
{
	double out;
	if (ftype == 0)
	{
		// Gaussian
		out = height * exp (-(r*r / 2*sigma*sigma));
	}
	else if (ftype == 1)
	{
		// Multiquadratic
		out = height * pow((r*r + sigma*sigma), 0.5);
	}
	else if (ftype == 2)
	{
		// Inverse multiquadratic
		out = height * pow((r*r + sigma*sigma), -0.5);
	}
	else
	{
		return MS::kInvalidParameter;
	}

	return out;
}
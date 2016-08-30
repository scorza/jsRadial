#include "common.h"
#include "jsPose.h"

const char* JSPose::kName = "jsPose";
MTypeId JSPose::kId(0x00000004);

MObject JSPose::aInMatrix;
MObject JSPose::aOutArray;

JSPose::JSPose() {}
JSPose::~JSPose() {}
void* JSPose::creator() { return new JSPose; }

MStatus JSPose::initialize()
{
	MStatus status;

	MFnMatrixAttribute fnMatrix;
	MFnNumericAttribute fnNumeric;
	MFnTypedAttribute fnTyped;

	aInMatrix = fnMatrix.create("inMatrix", "inMatrix");
	fnMatrix.setKeyable(true);
	fnMatrix.setStorable(true);
	fnMatrix.setArray(true);
	addAttribute(aInMatrix);

	aOutArray = fnTyped.create("outArray", "outArray", MFnData::kDoubleArray);
	fnTyped.setKeyable(false);
	fnTyped.setStorable(false);
	fnTyped.setArray(true);
	addAttribute(aOutArray);

	attributeAffects(aInMatrix, aOutArray);

	return MS::kSuccess;
}

MStatus JSPose::compute(const MPlug &plug, MDataBlock &data)
{
	MStatus status;
	if (plug != aOutArray) { return MS::kInvalidParameter; }

	// Get output array handle
	MArrayDataHandle hArrayOut = data.outputArrayValue(aOutArray);

	MArrayDataHandle hArrayMatrix = data.inputArrayValue(aInMatrix);
	unsigned int count = hArrayMatrix.elementCount();
	for (unsigned int i = 0; i < count; ++i)
	{
		MDoubleArray out;
		out.setLength(7);

		// Get output handle
		hArrayOut.jumpToElement(i);
		MDataHandle hOut = hArrayOut.outputValue();

		// Get input matrix handle
		hArrayMatrix.jumpToElement(i);
		MMatrix mtx = hArrayMatrix.inputValue().asMatrix();
		MTransformationMatrix tmtx(mtx);

		MVector trs = tmtx.translation(MSpace::kTransform); /////// Doesn't this only take kTransform?
		MQuaternion quat = tmtx.rotation();
		MVector axis;
		double angle;
		quat.getAxisAngle(axis, angle);

		// Set out array values (xyz, xyz, angle)
		out[0] = trs[0]; out[1] = trs[1]; out[2] = trs[2];
		out[3] = axis.x; out[4] = axis.y; out[5] = axis.z; 	out[6] = angle;

		MFnDoubleArrayData fnData;
		MObject oData = fnData.create(out, &status);
		CHECK_MSTATUS_AND_RETURN_IT(status);

		hOut.set(oData);
	}

	data.setClean(aOutArray);

	return MS::kSuccess;
}
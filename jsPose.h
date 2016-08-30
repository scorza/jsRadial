#pragma once
#include "common.h"

class JSPose : public MPxNode
{
public:
	JSPose();
	virtual ~JSPose();
	static void* creator();
	static MStatus initialize();

	virtual MStatus compute(const MPlug &plug, MDataBlock &data);

	static const char* kName;
	static MTypeId kId;

	// Node attrs
	static MObject aInMatrix;
	static MObject aOutArray;
};
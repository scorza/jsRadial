#include "common.h"
#include "jsRadial.h"
#include "jsPose.h"
#include "jsRadPose.h"
#include <maya/MFnPlugin.h>

MStatus initializePlugin(MObject mObj)
{
	MStatus status;
	MFnPlugin fnPlugin(mObj, "James Sumner III", "1.0", "Any");
	status = fnPlugin.registerNode(JSRadial::kName, JSRadial::kId, JSRadial::creator, JSRadial::initialize, MPxNode::kDependNode);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.registerNode(JSPose::kName, JSPose::kId, JSPose::creator, JSPose::initialize, MPxNode::kDependNode);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.registerNode(JSRadPose::kName, JSRadPose::kId, JSRadPose::creator, JSRadPose::initialize, MPxNode::kDependNode);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}

MStatus uninitializePlugin(MObject mObj)
{
	MStatus status;
	MFnPlugin fnPlugin(mObj);

	status = fnPlugin.deregisterNode(JSRadial::kId);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.deregisterNode(JSPose::kId);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	status = fnPlugin.deregisterNode(JSRadPose::kId);
	CHECK_MSTATUS_AND_RETURN_IT(status);

	return MS::kSuccess;
}
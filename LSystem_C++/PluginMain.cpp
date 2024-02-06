#include <maya/MPxCommand.h>
#include <maya/MFnPlugin.h>
#include <maya/MIOStream.h>
#include <maya/MString.h>
#include <maya/MArgList.h>
#include <maya/MGlobal.h>
#include <maya/MSimple.h>
#include <maya/MDoubleArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MDGModifier.h>
#include <maya/MPlugArray.h>
#include <maya/MVector.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStringArray.h>
#include <list>

#include "LSystemCmd.h"
#include "LSystemNode.h"

MStatus initializePlugin(MObject obj) {
    MGlobal::displayInfo("test");
    MStatus status = MStatus::kSuccess;
    MFnPlugin plugin(obj, "MyPlugin", "1.0", "Any");

    // Register Command
    status = plugin.registerCommand("LSystemCmd", LSystemCmd::creator, LSystemCmd::newSyntax);
    if (!status) {
        status.perror("registerCommand");
        return status;
    }

    // Register Node
    status = plugin.registerNode("LSystemNode", LSystemNode::id, LSystemNode::creator,
        LSystemNode::initialize);
    if (!status) {
        status.perror("registerNode");
        plugin.deregisterCommand("LSystemCmd");
        return status;
    }

    // Load MEL script for GUI
    MString pluginPath = plugin.loadPath();
    pluginPath.substitute("\\", "/");
    MString melScriptPath = pluginPath + "/LSystemGUI.mel";
    MGlobal::executeCommand("source \"" + melScriptPath + "\"");
    MGlobal::executeCommand("createLSystemMenu");
    return status;
}

MStatus uninitializePlugin(MObject obj) {
    MStatus status = MStatus::kSuccess;
    MFnPlugin plugin(obj);

    // Deregister Node
    status = plugin.deregisterNode(LSystemNode::id);
    if (!status) {
        status.perror("deregisterNode");
        return status;
    }

    //// Deregister Command
    status = plugin.deregisterCommand("LSystemCmd");
    if (!status) {
        status.perror("deregisterCommand");
        return status;
    }

    // Remove the custom menu from Maya
    MGlobal::executeCommand("if (`menu -exists LSystemMenu`) deleteUI -menu LSystemMenu;");

    return status;
}

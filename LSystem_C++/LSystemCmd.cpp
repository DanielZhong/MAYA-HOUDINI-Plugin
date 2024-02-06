#include "LSystemCmd.h"
#include "LSystem.h"

#include <maya/MGlobal.h>
#include <maya/MFnNurbsCurve.h>
#include <maya/MPointArray.h>
#include <maya/MDagModifier.h>
#include <maya/MFnCircleSweepManip.h> 
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <list>
LSystemCmd::LSystemCmd() : MPxCommand()
{
}

LSystemCmd::~LSystemCmd() 
{
}

MSyntax LSystemCmd::newSyntax() {
    MSyntax syntax;
    syntax.addFlag("-ss", "-stepSize", MSyntax::kDouble);
    syntax.addFlag("-a", "-angle", MSyntax::kDouble);
    syntax.addFlag("-g", "-grammar", MSyntax::kString);
    syntax.addFlag("-i", "-iterations", MSyntax::kLong);
    return syntax;
}

MStatus LSystemCmd::doIt(const MArgList& args)
{
    MStatus status;

    MArgDatabase argData(syntax(), args);

    // Parse the command line arguments
    if (argData.isFlagSet("-ss")) {
        argData.getFlagArgument("-ss", 0, defaultStepSize);
    }

    if (argData.isFlagSet("-a")) {
        argData.getFlagArgument("-a", 0, defaultAngle);
    }

    if (argData.isFlagSet("-g")) {
        argData.getFlagArgument("-g", 0, grammar);
        MGlobal::displayInfo("Grammar after command: " + grammar);
    }

    if (argData.isFlagSet("-i")) {
        argData.getFlagArgument("-i", 0, iterations);
    }

    MGlobal::displayInfo(MString("Default Step Size: ") + defaultStepSize);
    MGlobal::displayInfo(MString("Default Angle: ") + defaultAngle);
    MGlobal::displayInfo(MString("Iterations: ") + iterations);
    MGlobal::displayInfo(MString("Grammar: ") + grammar);

    LSystem lSystem;
    lSystem.loadProgramFromString(grammar.asChar());
    
    lSystem.setDefaultAngle(defaultAngle);
    lSystem.setDefaultStep(defaultStepSize);

    std::vector<LSystem::Branch> branches;
    lSystem.process(iterations, branches);

    for (auto& branch : branches) {
        // custom shape: curve
        MPoint startPoint(branch.first[0], branch.first[2], branch.first[1]);
        MPoint endPoint(branch.second[0], branch.second[2], branch.second[1]);

        MPointArray curvePoints;
        curvePoints.append(startPoint);
        curvePoints.append(endPoint);

        MDoubleArray knotSequences;
        knotSequences.append(0.0);
        knotSequences.append(1.0);

        MDagModifier dagModifier;
        MObject curveTransformObj = dagModifier.createNode("nurbsCurve");
        dagModifier.doIt();

        MFnNurbsCurve curveFn;
        curveFn.setObject(curveTransformObj);
        curveFn.create(curvePoints, knotSequences, 1, MFnNurbsCurve::kOpen, false, false, MObject::kNullObj, &status);

        if (!status) {
            status.perror("Failed to create curve for branch");
            return status;
        }
    }

    std::cout.flush();
    

    return MStatus::kSuccess;
}
#include "LSystem.h"    
#include <fstream>
#include <stack>

#pragma warning(disable : 4244)
#pragma warning(disable : 4290)
#include "matrix.h"

#define Rad2Deg 57.295779513082320876798154814105
#define Deg2Rad 0.017453292519943295769236907684886

LSystem::LSystem() : mDfltAngle(22.5), mDfltStep(1.0)
{
}

void LSystem::setDefaultAngle(float degrees)
{
    mDfltAngle = degrees;
}

void LSystem::setDefaultStep(float distance)
{
    mDfltStep = distance;
}

float LSystem::getDefaultAngle() const
{
    return mDfltAngle;
}

float LSystem::getDefaultStep() const
{
    return mDfltStep;
}

const std::string& LSystem::getGrammarString() const
{
    return mGrammar;
}

void LSystem::reset()
{
    current = "";
    iterations.clear();
    productions.clear();
}

const std::string& LSystem::getIteration(unsigned int n)
{
    if (n >= iterations.size())
    {
        for (unsigned int i = iterations.size(); i <= n; i++)
        {
            current = iterate(current);
            iterations.push_back(current);
        }        
    }
    return iterations[n];
}

void LSystem::loadProgram(const std::string& fileName)
{
    reset();

    std::string line;
    std::ifstream file(fileName.c_str());
    if (file.is_open())
    {
        while (file.good())
        {
            getline(file,line);
            addProduction(line);
        }
    }
    // for each line in p, add production
    file.close();
}

void LSystem::loadProgramFromString(const std::string& program)
{
    reset(); 
    mGrammar = program;

    size_t index = 0;
    while (index < program.size())
    {
        size_t nextIndex = program.find("\n", index);
        std::string line = program.substr(index, nextIndex);
        addProduction(line);
        if (nextIndex == std::string::npos) break;
        index = nextIndex+1;
    }
}

void LSystem::addProduction(std::string line)
{
    size_t index;

    // 1. Strip whitespace
    while ((index = line.find(" ")) != std::string::npos)
    {
        line.replace(index, 1, ""); 
    }

    if (line.size() == 0) return;

    // 2. Split productions
    index = line.find("->");
    if (index != std::string::npos)
    {
        std::string symFrom = line.substr(0, index);
        std::string symTo = line.substr(index+2);
        productions[symFrom] = symTo;
    }
    else  // assume its the start sym
    {
        current = line;
    }
}

std::string LSystem::iterate(const std::string& input)
{
    std::string output = "";
    for (unsigned int i = 0; i < input.size(); i++)
    {
        std::string sym = input.substr(i,1);
        std::string next = productions.count(sym) > 0? productions[sym] : sym; 
        output = output + next;
    }
    return output;
    //for each sym in current state, replace the sym
}


LSystem::Turtle::Turtle() :
    pos(0,0,0),
    up(0,0,1),
    forward(1,0,0),
    left(0,1,0)
{
}

LSystem::Turtle::Turtle(const LSystem::Turtle& t)
{
    pos = t.pos;
    up = t.up;
    forward = t.forward;
    left = t.left;
}

LSystem::Turtle& LSystem::Turtle::operator=(const LSystem::Turtle& t)
{
    if (&t == this) return *this;

    pos = t.pos;
    up = t.up;
    forward = t.forward;
    left = t.left;
    return *this;
}

void LSystem::Turtle::moveForward(float length)
{
    pos = pos + length * forward;
}

void LSystem::Turtle::applyUpRot(float degrees)
{
    math::RotationMatrix<float> mat(2,Deg2Rad*degrees); // Z axis
    math::RotationMatrix<float> world2local(forward, left, up); 
    up =  world2local * mat * vec3(0,0,1);
    left = world2local * mat * vec3(0,1,0);
    forward = world2local * mat * vec3(1,0,0);
}

void LSystem::Turtle::applyLeftRot(float degrees)
{
    math::RotationMatrix<float> mat(1,Deg2Rad*degrees); // Y axis
    math::RotationMatrix<float> world2local(forward, left, up); 
    up =  world2local * mat * vec3(0,0,1);
    left = world2local * mat * vec3(0,1,0);
    forward = world2local * mat * vec3(1,0,0);
}

void LSystem::Turtle::applyForwardRot(float degrees)
{
    math::RotationMatrix<float> mat(0,Deg2Rad*degrees); // X axis
    math::RotationMatrix<float> world2local(forward, left, up); 
    up =  world2local * mat * vec3(0,0,1);
    left = world2local * mat * vec3(0,1,0);
    forward = world2local * mat * vec3(1,0,0);
}

void LSystem::process(unsigned int n, 
    std::vector<Branch>& branches)
{
    std::vector<Geometry> models;
    process(n,branches,models);
}

// TODO:: Finish this function.
//        This is the function that will be called from Python to generate the branches and flowers.
//        Notice the argument types. Since we are interfacing with Python, we must simplify the data
//          types that we are using because Python cannot easily handle the "vec" class that is usually 
//          used in the Branch. Instead, we will return slightly altered data packets:
//				flowers:  vector of vector of floats: [posx, posy, poz]
//              branches: vector of vector of floats: [startx, starty, startz, endx, endy, endz]
//
//         You will need to repackage the branches and flowers so that they can be passed to Python
//           in the format described above. Notice that in the LSystem.i file, we have defined
//           a std::vector<float> as a VecFloat in Python and a std::vector<std::vector<float> > as
//           a VectorPyBranch in Python.
void LSystem::processPy(unsigned int n,
		std::vector<std::vector<float> >& branches, std::vector<std::vector<float> >& flowers) {
		
	std::vector<Branch> preBranches;
    std::vector<Geometry> preFlowers;

	process(n, preBranches, preFlowers);

    std::vector<float> Branch;
    for (int i = 0; i < preBranches.size(); i++)
    {
        Branch.push_back(preBranches[i].first[0]);
        Branch.push_back(preBranches[i].first[1]);
        Branch.push_back(preBranches[i].first[2]);

        Branch.push_back(preBranches[i].second[0]);
        Branch.push_back(preBranches[i].second[1]);
        Branch.push_back(preBranches[i].second[2]);

        branches.push_back(Branch);
        Branch.clear();
    }

    std::vector<float> Flower;
    for (int i = 0; i < preFlowers.size(); i++)
    {
        Flower.push_back(preFlowers[i].first[0]);
        Flower.push_back(preFlowers[i].first[1]);
        Flower.push_back(preFlowers[i].first[2]);

        flowers.push_back(Flower);
        Flower.clear();
    }
}

// LOOK: This is where the L-System creates the branches and the flowers.
//        Branches are returns in the "branches" vector and flowers (or other symbols) are
//        returned in the "models" vector.
void LSystem::process(unsigned int n, 
    std::vector<Branch>& branches, 
    std::vector<Geometry>& models)
{
    Turtle turtle;
    std::stack<Turtle> stack;

    // Init so we're pointing up
    turtle.applyLeftRot(-90);

    std::string insn = getIteration(n);

    std::vector<int> depth;
    int curDepth = 0;

    for (unsigned int i = 0; i < insn.size(); i++)
    {
        std::string sym = insn.substr(i,1);
        if (sym == "F")
        {
            vec3 start = turtle.pos;
            turtle.moveForward(mDfltStep);
            branches.push_back(Branch(start,turtle.pos));
        }
        else if (sym == "f")
        {
            turtle.moveForward(mDfltStep);
        }
        else if (sym == "+")
        {
            turtle.applyUpRot(mDfltAngle);
        }
        else if (sym == "-")
        {
            turtle.applyUpRot(-mDfltAngle);
        }
        else if (sym == "&")
        {
            turtle.applyLeftRot(mDfltAngle);
        }
        else if (sym == "^")
        {
            turtle.applyLeftRot(-mDfltAngle);
        }
        else if (sym == "\\")
        {
            turtle.applyForwardRot(mDfltAngle);
        }
        else if (sym == "/")
        {
            turtle.applyForwardRot(-mDfltAngle);
        }
        else if (sym == "|")
        {
            turtle.applyUpRot(180);
        }
        else if (sym == "[")
        {
            stack.push(turtle);
        }
        else if (sym == "]")
        {
            turtle = stack.top();
            stack.pop();
        }
        else
        {
			// LOOK: When a different symbol (such as a *) is encountered, the turtle's position 
			//        along with the corresponding symbol is added to the models vector.
            models.push_back(Geometry(turtle.pos, sym));
        }
    }
}

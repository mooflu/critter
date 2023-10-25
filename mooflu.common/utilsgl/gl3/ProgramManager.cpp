#include "ProgramManager.hpp"

#include <memory>

#include "Trace.hpp"
#include "ResourceManager.hpp"

#include "gl3/Program.hpp"
#include "gl3/Shader.hpp"

using namespace std;

ProgramManager::ProgramManager() {}

ProgramManager::~ProgramManager() {}

void ProgramManager::reset() {
    LOG_INFO << "ProgramManager::reset\n";
    Program::release();
    while (!_programs.empty()) {
        auto progIt = _programs.begin();
        string progName = progIt->first;
        Program* prog = progIt->second;
        _programs.erase(progName);
        LOG_INFO << "Deleting shader prog '" << progName << "'\n";
        delete prog;
    }
}

Program* ProgramManager::createProgram(const string& name) {
    if (_programs.find(name) != _programs.end()) {
        LOG_WARNING << "Shader exists - use getProgram: " << name << "\n";
        return _programs[name];
    }

    const string vFileName = "shaders/" + name + ".vert.glsl";
    Shader* vs = new Shader(GL_VERTEX_SHADER, loadShaderSource(vFileName));
    vs->compile();

    const string fFileName = "shaders/" + name + ".frag.glsl";
    Shader* fs = new Shader(GL_FRAGMENT_SHADER, loadShaderSource(fFileName));
    fs->compile();

    Program* prog = new Program();

    prog->attach(vs);
    prog->attach(fs);

    prog->link();

    LOG_INFO << "Shader program created: " << name << "\n";

    _programs[name] = prog;

    return prog;
}

Program* ProgramManager::getProgram(const string& name) {
    auto program = _programs[name];
    if (!program) {
        LOG_ERROR << "Shader program not found: " << name << endl;
    }
    return program;
}

string ProgramManager::loadShaderSource(const string& shaderSrcFile) {
    if (!ResourceManagerS::instance()->hasResource(shaderSrcFile)) {
        LOG_ERROR << shaderSrcFile << " not found!" << endl;
        return "";
    }
    std::unique_ptr<ziStream> infilePtr(ResourceManagerS::instance()->getInputStream(shaderSrcFile));
    string result = infilePtr->readAll();

#if defined(EMSCRIPTEN)
    result = "#version 300 es\nprecision highp float;\nprecision highp int;\n" + result;
#else
    result = "#version 330\n" + result;
#endif

    return result;
}

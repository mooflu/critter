#pragma once

#include "Singleton.hpp"

#include <string>
#include <unordered_map>

class Program;

class ProgramManager {
    friend class Singleton<ProgramManager>;

public:
    Program* createProgram(const std::string& name);
    Program* getProgram(const std::string& name);

    void reset();

protected:
    static std::string loadShaderSource(const std::string& shaderSrcFile);

private:
    virtual ~ProgramManager();
    ProgramManager(void);
    ProgramManager(const ProgramManager&);
    ProgramManager& operator=(const ProgramManager&);

    std::unordered_map<std::string, Program*> _programs;
};

typedef Singleton<ProgramManager> ProgramManagerS;

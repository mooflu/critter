#pragma once
// Description:
//   Input subsystem.
//
// Copyright (C) 2007 Frank Becker
//
#include <string>

#include "Trace.hpp"
#include "hashMap.hpp"
#include "Singleton.hpp"
#include "Trigger.hpp"
#include "Keys.hpp"
#include "ConfigHandler.hpp"
#include "CallbackManager.hpp"
#include "InterceptorI.hpp"

#include <vmmlib/vector.hpp>
using namespace vmml;

class Callback;

#if defined(EMSCRIPTEN)
#define ESCAPE_KEY SDLK_BACKSPACE
#else
#define ESCAPE_KEY SDLK_ESCAPE
#endif

namespace HASH_NAMESPACE {
template <>
struct hash<Trigger> {
    //a simple hash function for Trigger
    int operator()(const Trigger& t) const {
        int hashval;

        if (t.type == eMotionTrigger) {
            hashval = t.type * 1000;
        } else {
            hashval = t.type * 1000 + t.data1;
        }

        return hashval;
    }
};
}  // namespace HASH_NAMESPACE

class Input : public ConfigHandler {
    friend class Singleton<Input>;

public:
    bool init(void);
    bool update(void);

    //used for loading/saving bindings
    virtual void handleLine(const std::string line);
    virtual void save(std::ostream& of);

    //Input takes ownership of callback
    void bindNextTrigger(const std::string& action) {
        _bindMode = true;
        _action = action;
        LOG_INFO << "bindNextTrigger -> " << _action << "\n";
    }

    bool waitingForBind(void) { return _bindMode; }

    void addCallback(Callback* cb);

    std::string getTriggerName(std::string& action);

    void enableInterceptor(InterceptorI* i) { _interceptor = i; }

    void disableInterceptor(void) { _interceptor = 0; }

    const vec2f& mousePos(void);
    void resetMousePosition();

private:
    virtual ~Input();
    Input(void);
    Input(const Input&);
    Input& operator=(const Input&);

    void bind(Trigger& t, Callback* action);
    bool tryGetTrigger(Trigger& trigger, bool& isDown);
    void updateMouseSettings(void);
    bool _bindMode;
    std::string _action;
    CallbackManager _callbackManager;
    Keys _keys;

    typedef hash_map<std::string, Trigger*, hash<std::string>, std::equal_to<std::string>> ATMap;
    ATMap _actionTriggerMap;

    hash_map<Trigger, Callback*, hash<Trigger>, std::equal_to<Trigger>> _callbackMap;

    //mouse position [0..1]
    vec2f _mousePos;
    vec2f _mouseDelta;

    //intercept raw input
    InterceptorI* _interceptor;
};

typedef Singleton<Input> InputS;

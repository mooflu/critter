#pragma once
// Description:
//   Different kinds of selectables.
//
// Copyright (C) 2010 Frank Becker
//
// This program is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation;  either version 2 of the License,  or (at your option) any  later
// version.
//
// This program is distributed in the hope that it will be useful,  but  WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details
//

#include <string>
#include <list>
#include <vector>

#include "Point.hpp"
#include "Trigger.hpp"
#include "GLBitmapFont.hpp"
#include "GLBitmapCollection.hpp"

#include "tinyxml.h"

class Selectable {
public:
    enum UserFeedback {
        eNoFeedback,
        eBeep,
        eTick,
    };

    Selectable(bool enabled, const BoundingBox& r, const std::string& info);
    virtual ~Selectable();

    virtual void input(const Trigger& /*trigger*/, const bool& /*isDown*/, const Point2Di& /*offset*/) {}

    virtual void activate(bool /*beQuiet*/ = false) {}

    virtual void deactivate(void) {}

    virtual void select(void) {}

    virtual void update(void) {}

    virtual void draw(const Point2Di& offset);

    const BoundingBox& getInputBox(void) { return _inputBox; }

    static void reset(void) { _active = 0; }

    void updateActive(UserFeedback feedback);

protected:
    static Selectable* _active;

    bool _enabled;
    BoundingBox _inputBox;
    BoundingBox _boundingBox;
    std::string _info;
    GLBitmapFont* _fontWhite;

private:
    Selectable(const Selectable&);
    Selectable& operator=(const Selectable&);
};

class EscapeSelectable : public Selectable {
public:
    EscapeSelectable(bool enabled, const BoundingBox& r, float size = 1.0);

    virtual void input(const Trigger& trigger, const bool& isDown, const Point2Di& offset);
    virtual void activate(bool beQuiet = false);
    virtual void select(void);
    virtual void draw(const Point2Di& offset);

protected:
    float _size;
    GLBitmapCollection* _icons;
    int _exitOn;
    int _exitOff;

private:
    EscapeSelectable(const EscapeSelectable&);
    EscapeSelectable& operator=(const EscapeSelectable&);
};

class LeaderBoardSelectable : public Selectable {
public:
    LeaderBoardSelectable(bool enabled, const BoundingBox& r, const std::string& text, const std::string& info);
    virtual ~LeaderBoardSelectable();

    virtual void input(const Trigger& trigger, const bool& /*isDown*/, const Point2Di& offset);
    virtual void activate(bool beQuiet = false);
    virtual void draw(const Point2Di& offset);

protected:
    std::string _text;
    GLBitmapFont* _fontShadow;
    float _size;

private:
    LeaderBoardSelectable(const LeaderBoardSelectable&);
    LeaderBoardSelectable& operator=(const LeaderBoardSelectable&);
};

class TextOnlySelectable : public Selectable {
public:
    TextOnlySelectable(bool enabled, const BoundingBox& r, const std::string& text, const std::string& info,
                       bool center = true, float size = 1.0, float red = 1.0, float green = 0.852, float blue = 0.0);

    virtual void input(const Trigger& trigger, const bool& /*isDown*/, const Point2Di& offset);
    virtual void activate(bool beQuiet = false);
    virtual void draw(const Point2Di& offset);

protected:
    std::string _text;
    GLBitmapFont* _fontShadow;
    GLBitmapCollection* _icons;
    float r;
    float g;
    float b;
    float _size;

private:
    TextOnlySelectable(const TextOnlySelectable&);
    TextOnlySelectable& operator=(const TextOnlySelectable&);
};

class ResolutionSelectable : public TextOnlySelectable {
    struct Resolution {
        Resolution(int w, int h) :
            width(w),
            height(h),
            text() {
            char buf[32];
            sprintf(buf, "%dx%d", w, h);
            text = buf;
        }

        Resolution(int w, int h, std::string t) :
            width(w),
            height(h),
            text(t) {}

        Resolution(const Resolution& r) :
            width(r.width),
            height(r.height),
            text(r.text) {}

        Resolution& operator=(const Resolution& r) {
            width = r.width;
            height = r.height;
            text = r.text;
            return *this;
        }

        int width;
        int height;
        std::string text;

        bool operator==(Resolution& r1) { return (r1.width == width) && (r1.height == height); }

        bool operator!=(Resolution& r1) { return !operator==(r1); }
    };

    static bool DescendingAreaOrder(const Resolution& res1, const Resolution& res2);

public:
    ResolutionSelectable(bool enabled, const BoundingBox& r, const std::string& text, const std::string& info);

    virtual void input(const Trigger& trigger, const bool& /*isDown*/, const Point2Di& offset);
    virtual void activate(bool beQuiet = false);
    virtual void select(void);
    virtual void draw(const Point2Di& offset);

protected:
    void addFullscreenResolutions(void);
    void applyResolution(void);
    void nextResolution(void);
    void prevResolution(void);

    GLBitmapFont* _fontShadow;
    BoundingBox _bRect;
    int _checkmark;
    int _checkmarkOff;
    std::vector<Resolution> _resolutionList;
    std::vector<Resolution>::iterator _activeResolution;

private:
    ResolutionSelectable(const ResolutionSelectable&);
    ResolutionSelectable& operator=(const ResolutionSelectable&);
};

class FloatSelectable : public TextOnlySelectable {
public:
    FloatSelectable(bool enabled, const BoundingBox& r, const std::string& text, const std::string& info,
                    const std::string& variable, const std::string& range, const std::string& sliderOffset);

    virtual void input(const Trigger& trigger, const bool& isDown, const Point2Di& offset);
    virtual void activate(bool beQuiet = false);
    virtual void draw(const Point2Di& offset);

protected:
    void incBy(float delta);

    BoundingBox _bRect;
    std::string _variable;
    float _max;
    float _min;
    float _startX;
    float _curVal;

    float _xPos;

    int _slider;
    int _doubleArrow;
    int _sliderOffset;

private:
    FloatSelectable(const FloatSelectable&);
    FloatSelectable& operator=(const FloatSelectable&);
};

class EnumSelectable : public TextOnlySelectable {
public:
    EnumSelectable(bool enabled, const BoundingBox& r, const std::string& text, const std::string& info,
                   const std::string& variable, const std::string& values);

    virtual void input(const Trigger& trigger, const bool& isDown, const Point2Di& offset);
    virtual void activate(bool beQuiet = false);
    virtual void draw(const Point2Di& offset);

protected:
    void nextEnum(void);
    void prevEnum(void);

    std::string _variable;
    float _xOff;
    std::list<std::string> _enumList;
    std::list<std::string>::iterator _activeEnum;

private:
    EnumSelectable(const EnumSelectable&);
    EnumSelectable& operator=(const EnumSelectable&);
};

class BoolSelectable : public TextOnlySelectable {
public:
    BoolSelectable(bool enabled, const BoundingBox& r, const std::string& text, const std::string& info,
                   const std::string& variable);

    virtual void input(const Trigger& trigger, const bool& isDown, const Point2Di& offset);
    virtual void activate(bool beQuiet = false);
    virtual void select(void);
    virtual void draw(const Point2Di& offset);

protected:
    void toggle(void);

    std::string _variable;
    float _xOff;
    int _checkmark;
    int _checkmarkOff;

private:
    BoolSelectable(const BoolSelectable&);
    BoolSelectable& operator=(const BoolSelectable&);
};

class TextSelectable : public TextOnlySelectable {
public:
    TextSelectable(bool enabled, const BoundingBox& r, const std::string& text, const std::string& info);

    virtual void input(const Trigger& trigger, const bool& isDown, const Point2Di& offset);
    virtual void activate(bool beQuiet = false);
    virtual void deactivate(void);
    virtual void update(void);
    virtual void draw(const Point2Di& offset);

protected:
    float _ds;
    float _prevSize;
    float _minSize;
    float _maxSize;

private:
    TextSelectable(const TextSelectable&);
    TextSelectable& operator=(const TextSelectable&);
};

class ActionSelectable : public TextSelectable {
public:
    ActionSelectable(bool enabled, const BoundingBox& r, const std::string& action, const std::string& text,
                     const std::string& info);

    virtual void select(void);

protected:
    std::string _action;

private:
    ActionSelectable(const ActionSelectable&);
    ActionSelectable& operator=(const ActionSelectable&);
};

class MenuSelectable : public TextSelectable {
public:
    MenuSelectable(TiXmlNode* node, bool enabled, const BoundingBox& r, const std::string& text,
                   const std::string& info);

    virtual void select(void);

protected:
    TiXmlNode* _node;

private:
    MenuSelectable(const MenuSelectable&);
    MenuSelectable& operator=(const MenuSelectable&);
};

class BindKeySelectable : public TextOnlySelectable {
public:
    BindKeySelectable(bool enabled, const BoundingBox& r, const std::string& text, const std::string& info,
                      const std::string& action, float size = 1.0);

    virtual void input(const Trigger& trigger, const bool& isDown, const Point2Di& offset);
    virtual void activate(bool beQuiet = false);
    virtual void select(void);
    virtual void draw(const Point2Di& offset);

protected:
    int _bindKeyOn;
    int _bindKeyOff;
    std::string _action;

private:
    BindKeySelectable(const BindKeySelectable&);
    BindKeySelectable& operator=(const BindKeySelectable&);
};

#pragma once
// Description:
//   Bezier curve template consisting of Bezier segments.
//
// Copyright (C) 2001-2009 Frank Becker
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

#include <list>

template <class POINT>
class BezierCurve {
public:
    struct Line {
        Line(const POINT& a, const POINT& b) :
            p1(a),
            p2(b) {}

        POINT p1;
        POINT p2;
    };

    //Contruct Bezier curve with a given number of points per Bezier segment
    BezierCurve(const int pps);
    //Cleanup
    ~BezierCurve();

    //Set/get control point at given index
    void SetControlPoint(int idx, const POINT& np);
    POINT GetControlPoint(int idx);

    POINT* ControlPoints() { return _controlPoints; };

    int GetNumControlPoints(void) { return (_numSegments * 3 + 1); }

    int GetNumSegments(void) { return _numSegments; }

    //Set/get points per segments
    void SetPointsPerSegment(const int pps) { _pointsPerSegment = pps; }

    int GetPointsPerSegment(void) { return _pointsPerSegment; }

    //Add a segment to Bezier curve
    void AddSegment(const POINT& p);
    void AddSegment(const POINT& p1, const POINT& p2, const POINT& p3);

    //Delete Bezier segments from-to.
    void DeleteSegments(int fidx, int tidx);

    //Move curve by vector p
    void MoveCurve(const POINT& p);
    //Scale curve by s
    void ScaleCurve(const float s);

    //Get point on curve at time 0<=t<=_numSegments
    void GetPos(float t, POINT& pos);

    //Get tangent for point on curve at time 0<=t<=_numSegments
    void GetTangent(float t, POINT& tangent);

    //Convert Bezier curve to an outline. Outline has to be deleted[] by client
    void ConvertToOutline(POINT*& outline, int& numPoints);
    //Convert partial Bezier curve to an outline.
    //Outline has to be deleted[] by client.
    void ConvertToOutline(int fidx, int tidx, POINT*& outline, int& numPoints);
    //Convert Bezier curve to an outline with perpendicular vectors.
    //Outline and pvecs have to be deleted[] by client.
    void ConvertToOutlineWithPVec(POINT*& outline, POINT*& pvecs, int& numPoints);

    //approximate segment length
    float arcLength(float error = 0.005) const;

    std::list<Line>& ArcLengthLines() { return _lines; }

    //Save Bezier curve to file.
    bool Save(const char* filename);
    //Load Bezier curve from file.
    bool Load(const char* filename);

private:
    BezierCurve(void);
    BezierCurve(const BezierCurve&);
    BezierCurve& operator=(const BezierCurve&);

    POINT* _controlPoints;
    int _numSegments;
    int _pointsPerSegment;

    //arcLength stuff
    void split(const POINT cPts[4], POINT left[4], POINT right[4]) const;
    float arcLength(const POINT cPts[4]) const;

    mutable float _error;
    mutable std::list<Line> _lines;
};

#include "BezierCurve.cpp"

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
#include <memory>

#include <stdio.h>
#include <math.h>

#include "Trace.hpp"
#include "Bezier.hpp"
#include "ResourceManager.hpp"

using namespace std;

//Contruct Bezier curve with a given number of points per Bezier segment
template <class POINT>
BezierCurve<POINT>::BezierCurve(const int pps) :
    _controlPoints(0),
    _numSegments(-1),
    _pointsPerSegment(pps) {
    XTRACE();
    //    LOG_INFO << _pointsPerSegment << " points per bezier segment" << endl;
}

//Cleanup
template <class POINT>
BezierCurve<POINT>::~BezierCurve() {
    XTRACE();
    if (_controlPoints) {
        delete[] _controlPoints;
    }
}

//Change control point at given index
template <class POINT>
void BezierCurve<POINT>::SetControlPoint(int idx, const POINT& np) {
    XTRACE();
    if ((idx >= 0) && (idx < (_numSegments * 3 + 1))) {
        _controlPoints[idx] = np;
    }
}

//Get control point info
template <class POINT>
POINT BezierCurve<POINT>::GetControlPoint(int idx) {
    XTRACE();
    if ((idx >= 0) && (idx < (_numSegments * 3 + 1))) {
        return (_controlPoints[idx]);
    }

    POINT p;
    return (p);
}

//Add a segment to Bezier curve
template <class POINT>
void BezierCurve<POINT>::AddSegment(const POINT& p) {
    XTRACE();
    if (_numSegments == -1) {
        _controlPoints = new POINT[1];
        _controlPoints[0] = p;
        _numSegments = 0;
    } else {
        _numSegments++;
        POINT* newControlPoints = new POINT[3 * _numSegments + 1];
        memcpy(newControlPoints, _controlPoints, sizeof(POINT) * (3 * (_numSegments - 1) + 1));

        delete[] _controlPoints;
        _controlPoints = newControlPoints;

        //first control point of this segment is the last of the bezier curve
        POINT p1 = _controlPoints[3 * _numSegments - 3];
        POINT dp = (p - p1) / 3.0;

        //place the middle 2 control points on line segment
        //(i.e. new bezier segment is/straight line).
        _controlPoints[3 * _numSegments - 2] = p1 + dp;
        _controlPoints[3 * _numSegments - 1] = p - dp;

        //last control point is the point that was passed in
        _controlPoints[3 * _numSegments] = p;
    }
}

//Add a segment to Bezier curve
template <class POINT>
void BezierCurve<POINT>::AddSegment(const POINT& p1, const POINT& p2, const POINT& p3) {
    XTRACE();
    if (_numSegments == -1) {
        _controlPoints = new POINT[1];
        _controlPoints[0] = p1;
        _numSegments = 0;
    }

    _numSegments++;
    POINT* newControlPoints = new POINT[3 * _numSegments + 1];
    memcpy(newControlPoints, _controlPoints, sizeof(POINT) * (3 * (_numSegments - 1) + 1));

    delete[] _controlPoints;
    _controlPoints = newControlPoints;

    _controlPoints[3 * _numSegments - 2] = p1;
    _controlPoints[3 * _numSegments - 1] = p2;
    _controlPoints[3 * _numSegments] = p3;
}

//Delete Bezier segments from-to.
template <class POINT>
void BezierCurve<POINT>::DeleteSegments(int fidx, int tidx) {
    XTRACE();
    if (fidx == -1) {
        fidx = 0;
    }

    fidx = fidx - (fidx % 3);

    tidx = tidx + (3 - (tidx % 3));

    if ((tidx > 3) && (tidx > (_numSegments * 3))) {
        tidx = _numSegments * 3;
    }

    if ((fidx < 0) || (fidx > (_numSegments * 3)) || (tidx < 0) || (tidx > (_numSegments * 3))) {
        return;
    }

    int nSegments = (tidx - fidx) / 3;

    _numSegments -= nSegments;

    POINT* newControlPoints = new POINT[3 * _numSegments + 1];

    int i;
    for (i = 0; i <= fidx; i++) {
        newControlPoints[i] = _controlPoints[i];
    }
    int j;
    for (j = 1; i < (_numSegments * 3 + 1); j++, i++) {
        newControlPoints[i] = _controlPoints[tidx + j];
    }

    delete[] _controlPoints;
    _controlPoints = newControlPoints;
}

//Move curve by vector p
template <class POINT>
void BezierCurve<POINT>::MoveCurve(const POINT& p) {
    XTRACE();
    for (int i = 0; i < (3 * _numSegments + 1); i++) {
        _controlPoints[i] = _controlPoints[i] + p;
    }
}

//Scale curve by s
template <class POINT>
void BezierCurve<POINT>::ScaleCurve(const float s) {
    XTRACE();
    for (int i = 0; i < (3 * _numSegments + 1); i++) {
        _controlPoints[i] = _controlPoints[i] * s;
    }
}

//Get point on curve at time 0<=t<=_numSegments
template <class POINT>
void BezierCurve<POINT>::GetPos(float t, POINT& pos) {
    XTRACE();
    if ((t < 0.0) || (t > (float)_numSegments)) {
        return;
    }

    float seg = floor(t);
    float dt = t - seg;
    Bezier<POINT> bez(&_controlPoints[(int)(seg * 3)]);
    bez.Pos(dt, pos);
}

//Get tangent for point on curve at time 0<=t<=_numSegments
template <class POINT>
void BezierCurve<POINT>::GetTangent(float t, POINT& tangent) {
    XTRACE();
    if ((t < 0.0) || (t > (float)_numSegments)) {
        return;
    }

    float seg = floor(t);
    float dt = t - seg;
    Bezier<POINT> bez(&_controlPoints[(int)(seg * 3)]);
    bez.TVec(dt, tangent);
}

//Convert Bezier curve to an outline. Outline has to be deleted[] by client
template <class POINT>
void BezierCurve<POINT>::ConvertToOutline(POINT*& outline, int& numPoints) {
    XTRACE();
    if (_numSegments <= 0) {
        outline = 0;
        numPoints = 0;
        return;
    }

    numPoints = _pointsPerSegment * _numSegments + 1;
    outline = new POINT[numPoints];

    float dt = 1.0 / _pointsPerSegment;

    int outlineIndex = 0;
    for (int i = 0; i < _numSegments; i++) {
        Bezier<POINT> bez(&_controlPoints[i * 3]);
        float t = 0;
        for (int tt = 0; tt < _pointsPerSegment; tt++) {
            bez.Pos(t, outline[outlineIndex]);
            outlineIndex++;
            t += dt;
        }
    }

    outline[outlineIndex] = _controlPoints[_numSegments * 3];
}

//Convert partial Bezier curve to an outline.
//Outline has to be deleted[] by client.
template <class POINT>
void BezierCurve<POINT>::ConvertToOutline(int fidx, int tidx, POINT*& outline, int& numPoints) {
    XTRACE();
    if (fidx == -1) {
        fidx = 0;
    }

    fidx = fidx - (fidx % 3);

    tidx = tidx + (3 - (tidx % 3));

    if ((tidx > 3) && (tidx > (_numSegments * 3))) {
        tidx = _numSegments * 3;
    }

    if ((fidx < 0) || (fidx > (_numSegments * 3)) || (tidx < 0) || (tidx > (_numSegments * 3))) {
        numPoints = 0;
        outline = 0;
        return;
    }

    int nSegments = (tidx - fidx) / 3;

    numPoints = _pointsPerSegment * nSegments + 1;
    outline = new POINT[numPoints];

    float dt = 1.0 / _pointsPerSegment;

    int outlineIndex = 0;
    for (int i = 0; i < nSegments; i++) {
        Bezier<POINT> bez(&_controlPoints[fidx + i * 3]);
        float t = 0;
        for (int tt = 0; tt < _pointsPerSegment; tt++) {
            bez.Pos(t, outline[outlineIndex]);
            outlineIndex++;
            t += dt;
        }
    }

    outline[outlineIndex] = _controlPoints[fidx + nSegments * 3];
}

//Convert Bezier curve to an outline with perpendicular vectors.
//Outline and pvecs have to be deleted[] by client.
template <class POINT>
void BezierCurve<POINT>::ConvertToOutlineWithPVec(POINT*& outline, POINT*& pvecs, int& numPoints) {
    XTRACE();

    if (_numSegments <= 0) {
        outline = 0;
        numPoints = 0;
        return;
    }

    numPoints = _pointsPerSegment * _numSegments + 1;
    outline = new POINT[numPoints];
    pvecs = new POINT[numPoints];

    float dt = 1.0 / _pointsPerSegment;

    int outlineIndex = 0;
    for (int i = 0; i < _numSegments; i++) {
        Bezier<POINT> bez(&_controlPoints[i * 3]);
        float t = 0;
        for (int tt = 0; tt < _pointsPerSegment; tt++) {
            bez.Pos(t, outline[outlineIndex]);
            bez.PVec(t, pvecs[outlineIndex]);
            outlineIndex++;
            t += dt;
        }
        if (i == (_numSegments - 1)) {
            bez.Pos(t, outline[outlineIndex]);
            bez.PVec(t, pvecs[outlineIndex]);
        }
    }
    //  outline[ outlineIndex] = _controlPoints[ _numSegments*3];
}

//Save Bezier curve to file.
template <class POINT>
bool BezierCurve<POINT>::Save(const char* filename) {
    XTRACE();
    FILE* fp;
    if ((fp = fopen(filename, "w")) != NULL) {
        int dim = POINT::dimension();
        fprintf(fp, "%d\n", _pointsPerSegment);
        fprintf(fp, "%d\n", _numSegments);
        for (int idx = 0; idx < (_numSegments * 3 + 1); idx++) {
            for (int d = 0; d < dim; d++) {
                fprintf(fp, "%f ", (_controlPoints[idx])[d]);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
    } else {
        LOG_ERROR << "Could not create " << filename << endl;
        return false;
    }
    return true;
}

//Load Bezier curve from file.
template <class POINT>
bool BezierCurve<POINT>::Load(const char* filename) {
    XTRACE();

    if (!ResourceManagerS::instance()->hasResource(string(filename))) {
        LOG_ERROR << "Curve [" << filename << "] not found." << endl;
        return false;
    }
    shared_ptr<ziStream> infilePtr(ResourceManagerS::instance()->getInputStream(filename));
    ziStream& infile = *infilePtr;

    string line;

    int oldPointsPerSegment = _pointsPerSegment;
    int oldNumSegments = _numSegments;
    getline(infile, line);
    if (sscanf(line.c_str(), "%d\n", &_pointsPerSegment) != 1) {
        LOG_ERROR << "Expected points per segment" << endl;
        return false;
    }

    getline(infile, line);
    if (sscanf(line.c_str(), "%d\n", &_numSegments) != 1) {
        _pointsPerSegment = oldPointsPerSegment;
        LOG_ERROR << "Expected number of segments" << endl;
        return false;
    }

    if (oldNumSegments != _numSegments) {
        delete[] _controlPoints;
        _controlPoints = new POINT[3 * _numSegments + 1];
    }

    for (int idx = 0; idx < (_numSegments * 3 + 1); idx++) {
        //	LOG_INFO << "Loading control point #" << idx << endl;
        //FIXME: only support 1d, 2d and 3d bezier curves...
        float val[3];
        getline(infile, line);
        int dim = sscanf(line.c_str(), "%f %f %f\n", &val[0], &val[1], &val[2]);

        if (dim != POINT::dimension()) {
            for (int i = dim; i < POINT::dimension(); i++) {
                //fill missing dimensions with 0
                val[i] = 0.0;
            }
#if 0
	    delete[] _controlPoints;
	    _controlPoints = 0;
	    _pointsPerSegment = oldPointsPerSegment;
	    _numSegments = 0;
	    LOG_ERROR << "Expected another "
		      << POINT::dimension() << "-tuple"
		      << "in [" << filename << "]" << endl;
	    return false;
#endif
        }
        for (int i = 0; i < dim; i++) {
            _controlPoints[idx].set(i, val[i]);
        }
    }

    return true;
}

template <class POINT>
float BezierCurve<POINT>::arcLength(float error) const {
    _error = error;
    _lines.clear();

    float arcLen = 0.0;
    if (_numSegments > 0) {
        for (int i = 0; i < _numSegments; i++) {
            arcLen += arcLength(&_controlPoints[i * 3]);
        }
    }

    return arcLen;
}

template <class POINT>
float BezierCurve<POINT>::arcLength(const POINT cPts[4]) const {
    float len = dist(cPts[1] - cPts[0]) + dist(cPts[2] - cPts[1]) + dist(cPts[3] - cPts[2]);

    float chord = dist(cPts[0] - cPts[3]);

    if ((len - chord) > _error) {
        POINT left[4];
        POINT right[4];

        split(cPts, left, right);
        float lenLeft = arcLength(left);
        float lenRight = arcLength(right);
        return lenLeft + lenRight;
    }

    _lines.push_back(Line(cPts[0], cPts[3]));
    return len;
}

template <class POINT>
void BezierCurve<POINT>::split(const POINT cPts[4], POINT left[4], POINT right[4]) const {
    POINT triMat[4][4];
    int dim = cPts[0].dimension();

    for (int j = 0; j < 4; j++) {
        triMat[0][j] = cPts[j];
    }

    for (int i = 1; i < 4; i++) {
        for (int j = 0; j < 4 - i; j++) {
            for (int d = 0; d < dim; d++) {
                triMat[i][j].set(d, 0.5 * triMat[i - 1][j][d] + 0.5 * triMat[i - 1][j + 1][d]);
            }
        }
    }

    for (int j = 0; j < 4; j++) {
        left[j] = triMat[j][0];
        right[j] = triMat[3 - j][j];
    }
}

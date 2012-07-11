#pragma once
// Description:
//   Packs 2-dim objects into a bin.
//
// Copyright (C) 2008 Frank Becker
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
#include "Point.hpp"

class DimensionObject
{
public:
    DimensionObject( int w, int h):_dim(w,h){}
    virtual ~DimensionObject(){}
    
    virtual const Dimension& dimension(void) const
    {
        return _dim;
    }
    
protected:
    Dimension _dim;
};

class RectanglePacker
{
protected:
    struct Offset
    {
        Offset(void):x(0),y(0){}
        Offset(int X, int Y):x(X), y(Y){}
        int x;
        int y;
    };
    
    struct Node
    {
        Node( void):isLeaf(true),object(0),left(0),right(0){}
        
        bool isLeaf;
        DimensionObject *object;
        
        Offset off;
        Dimension dim;
        
        Node *left;    //non-leaf
        Node *right;   //non-leaf
    };    
    
public:
    RectanglePacker( int width, int height);
    virtual ~RectanglePacker();
    
    bool Insert( DimensionObject *object);
    const Point2Di &LastOffset( void)
    {
        return _lastOffset;
    }
    
    int PercentUsed(void) const
    {
        return (_areaUsed*100)/_totalArea;
    }
    
protected:
    bool Insert( Node *atNode);
    void Cleanup( Node *atNode);
    
    Node *_root;
    DimensionObject *_object;
    Point2Di _lastOffset;
    
    int _totalArea;
    int _areaUsed;
};

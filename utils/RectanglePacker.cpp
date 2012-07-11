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
#include "RectanglePacker.hpp"

RectanglePacker::RectanglePacker( int width, int height):
    _areaUsed(0)
{
    _root = new Node();
    _root->dim = Dimension(width,height);
    _totalArea = _root->dim.Area();
}

RectanglePacker::~RectanglePacker()
{
    Cleanup(_root);
    delete _root;
}

void RectanglePacker::Cleanup( Node *atNode)
{
    if( !atNode) return;
    
    Cleanup( atNode->left);
    Cleanup( atNode->right);      
    
    delete atNode->left;
    delete atNode->right;

    //delete atNode->object; ??
}

bool RectanglePacker::Insert( DimensionObject *object)
{
    _object = object;
    return Insert( _root);
}

bool RectanglePacker::Insert( Node *atNode)
{
    if( !atNode) return false;
    
    if( atNode->isLeaf)
    {
        if( atNode->object) 
            return false;	//leaf is full
        
        if( atNode->dim < _object->dimension())
            return false;	//leaf is too small for object
        
        if( atNode->dim == _object->dimension())
        {
            //exact fit
            atNode->object = _object;
            _areaUsed += _object->dimension().Area();
            _lastOffset = Point2Di(atNode->off.x, atNode->off.y);
            return true;
        }
        else
        {
            //need to split
            Node *left = new Node();
            Node *right = new Node();
            
            atNode->isLeaf = false;
            atNode->left = left;
            atNode->right = right;
            
            int dw = atNode->dim.width - _object->dimension().width;
            int dh = atNode->dim.height - _object->dimension().height;
            
            if( dw > dh)
            {
                //split the horizontal |
                right->dim = Dimension(_object->dimension().width, atNode->dim.height);
                right->off = atNode->off;
                left->dim = Dimension(dw, atNode->dim.height);
                left->off = Offset( atNode->off.x+right->dim.width, atNode->off.y);
                return Insert( right );
            }
            else
            {
                //split the vertical -
                left->dim = Dimension(atNode->dim.width, _object->dimension().height);
                left->off = atNode->off;
                right->dim = Dimension(atNode->dim.width, dh);
                right->off = Offset( atNode->off.x, atNode->off.y+left->dim.height);
                return Insert( left);
            }
        }
    }
    else
    {
        if( ! Insert( atNode->left))
            return Insert( atNode->right);
        
        return true;
    }
    
    //failed to insert
    return false;
}


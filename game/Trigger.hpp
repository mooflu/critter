#pragma once
// Description:
//   Structure for input triggers.
//
// Copyright (C) 2001 Frank Becker
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

enum TriggerTypeEnum
{
    eUnknownTrigger,
    eKeyTrigger,
    eButtonTrigger,
    eMotionTrigger
};

struct Trigger
{
    TriggerTypeEnum type;
    int data1;
    int data2;
    int data3;

//just for mouse smoothing for now...
    float fData1;
    float fData2;

    Trigger():type(eUnknownTrigger),data1(0),data2(0),data3(0),fData1(.0f),fData2(.0f)
    {
    }

    bool operator==(const Trigger &t) const
    {
	if( (type==eMotionTrigger) && (type==t.type))
	{
	    return true;
	}
	//ignore modifiers (data2) and unicode (data3)
	return( (type==t.type) && 
		(data1==t.data1));
    }
};

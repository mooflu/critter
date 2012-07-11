// Description:
//   Keyboard/Trigger helpers.
//
// Copyright (C) 2007 Frank Becker
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
#include "Keys.hpp"

#include <sstream>

#include "Trace.hpp"

using namespace std;

bool Keys::convertStringToTrigger( string & keyname, Trigger & trigger)
{
//    XTRACE();
    if( keyname == "MOTION")
    {
        trigger.type = eMotionTrigger;
        trigger.data1= 0;
        trigger.data2= 0;
        trigger.data3= 0;
        return true;
    }

    if( keyname.substr(0,11) == "MOUSEBUTTON")
    {
        trigger.type = eButtonTrigger;
        if( keyname.length() == 12)
        {
	    trigger.data1= keyname[11] - '0'; 
        }
        else
        {
	    trigger.data1= 1; //default to button 1
        }
//        trigger.data1= atoi( keyname.substr(11,1).c_str());
        trigger.data2= 0;
        trigger.data3= 0;
        return true;
    }

    trigger.type = eKeyTrigger;
    unsigned int i;
    for( i=0; i<SDLK_LAST; i++)
    {
        if( _symmap[i] == keyname) break;
    }

    if( i == SDLK_LAST) return false;

    trigger.data1=i; 
    trigger.data2= 0;
    trigger.data3= 0;

    return true;
}

string Keys::convertTriggerToString( const Trigger & trigger)
{
//    XTRACE();
    string triggerName;

    switch( trigger.type)
    {
        case eKeyTrigger:
            triggerName = _symmap[ trigger.data1];
            break;
        case eButtonTrigger:
            {
                ostringstream ost;
                ost << "MOUSEBUTTON" << trigger.data1;
		triggerName = ost.str();
            }
            break;
        case eMotionTrigger:
            triggerName = "MOTION";
            break;
        case eUnknownTrigger:
        default:
            triggerName = "UNKNOWN_";
            break;
    }

    return triggerName;
}

Keys::Keys( void)
{
    XTRACE();

    for( int i=0; i<SDLK_LAST; i++)
    {
        _symmap[i] == "KEY";
    }

    _symmap[ SDLK_UNKNOWN] = "UNKNOWN"; 
    _symmap[ SDLK_BACKSPACE] = "BACKSPACE"; 
    _symmap[ SDLK_TAB] = "TAB"; 
    _symmap[ SDLK_CLEAR] = "CLEAR"; 
    _symmap[ SDLK_RETURN] = "RETURN"; 
    _symmap[ SDLK_PAUSE] = "PAUSE"; 
    _symmap[ SDLK_ESCAPE] = "ESCAPE"; 
    _symmap[ SDLK_SPACE] = "SPACE"; 
    _symmap[ SDLK_EXCLAIM] = "EXCLAIM"; 
    _symmap[ SDLK_QUOTEDBL] = "QUOTEDBL"; 
    _symmap[ SDLK_HASH] = "HASH"; 
    _symmap[ SDLK_DOLLAR] = "DOLLAR"; 
    _symmap[ SDLK_AMPERSAND] = "AMPERSAND"; 
    _symmap[ SDLK_QUOTE] = "QUOTE"; 
    _symmap[ SDLK_LEFTPAREN] = "LEFTPAREN"; 
    _symmap[ SDLK_RIGHTPAREN] = "RIGHTPAREN"; 
    _symmap[ SDLK_ASTERISK] = "ASTERISK"; 
    _symmap[ SDLK_PLUS] = "PLUS"; 
    _symmap[ SDLK_COMMA] = "COMMA"; 
    _symmap[ SDLK_MINUS] = "MINUS"; 
    _symmap[ SDLK_PERIOD] = "PERIOD"; 
    _symmap[ SDLK_SLASH] = "SLASH"; 
    _symmap[ SDLK_0] = "0"; 
    _symmap[ SDLK_1] = "1"; 
    _symmap[ SDLK_2] = "2"; 
    _symmap[ SDLK_3] = "3"; 
    _symmap[ SDLK_4] = "4"; 
    _symmap[ SDLK_5] = "5"; 
    _symmap[ SDLK_6] = "6"; 
    _symmap[ SDLK_7] = "7"; 
    _symmap[ SDLK_8] = "8"; 
    _symmap[ SDLK_9] = "9"; 
    _symmap[ SDLK_COLON] = "COLON"; 
    _symmap[ SDLK_SEMICOLON] = "SEMICOLON"; 
    _symmap[ SDLK_LESS] = "LESS"; 
    _symmap[ SDLK_EQUALS] = "EQUALS"; 
    _symmap[ SDLK_GREATER] = "GREATER"; 
    _symmap[ SDLK_QUESTION] = "QUESTION"; 
    _symmap[ SDLK_AT] = "AT"; 
    _symmap[ SDLK_LEFTBRACKET] = "LEFTBRACKET"; 
    _symmap[ SDLK_BACKSLASH] = "BACKSLASH"; 
    _symmap[ SDLK_RIGHTBRACKET] = "RIGHTBRACKET"; 
    _symmap[ SDLK_CARET] = "CARET"; 
    _symmap[ SDLK_UNDERSCORE] = "UNDERSCORE"; 
    _symmap[ SDLK_BACKQUOTE] = "BACKQUOTE"; 
    _symmap[ SDLK_a] = "A"; 
    _symmap[ SDLK_b] = "B"; 
    _symmap[ SDLK_c] = "C"; 
    _symmap[ SDLK_d] = "D"; 
    _symmap[ SDLK_e] = "E"; 
    _symmap[ SDLK_f] = "F"; 
    _symmap[ SDLK_g] = "G"; 
    _symmap[ SDLK_h] = "H"; 
    _symmap[ SDLK_i] = "I"; 
    _symmap[ SDLK_j] = "J"; 
    _symmap[ SDLK_k] = "K"; 
    _symmap[ SDLK_l] = "L"; 
    _symmap[ SDLK_m] = "M"; 
    _symmap[ SDLK_n] = "M"; 
    _symmap[ SDLK_o] = "O"; 
    _symmap[ SDLK_p] = "P"; 
    _symmap[ SDLK_q] = "Q"; 
    _symmap[ SDLK_r] = "R"; 
    _symmap[ SDLK_s] = "S"; 
    _symmap[ SDLK_t] = "T"; 
    _symmap[ SDLK_u] = "U"; 
    _symmap[ SDLK_v] = "V"; 
    _symmap[ SDLK_w] = "W"; 
    _symmap[ SDLK_x] = "X"; 
    _symmap[ SDLK_y] = "Y"; 
    _symmap[ SDLK_z] = "Z"; 
    _symmap[ SDLK_DELETE] = "DELETE"; 
    _symmap[ SDLK_WORLD_0] = "WORLD_0"; 
    _symmap[ SDLK_WORLD_1] = "WORLD_1"; 
    _symmap[ SDLK_WORLD_2] = "WORLD_2"; 
    _symmap[ SDLK_WORLD_3] = "WORLD_3"; 
    _symmap[ SDLK_WORLD_4] = "WORLD_4"; 
    _symmap[ SDLK_WORLD_5] = "WORLD_5"; 
    _symmap[ SDLK_WORLD_6] = "WORLD_6"; 
    _symmap[ SDLK_WORLD_7] = "WORLD_7"; 
    _symmap[ SDLK_WORLD_8] = "WORLD_8"; 
    _symmap[ SDLK_WORLD_9] = "WORLD_9"; 
    _symmap[ SDLK_WORLD_10] = "WORLD_10"; 
    _symmap[ SDLK_WORLD_11] = "WORLD_11"; 
    _symmap[ SDLK_WORLD_12] = "WORLD_12"; 
    _symmap[ SDLK_WORLD_13] = "WORLD_13"; 
    _symmap[ SDLK_WORLD_14] = "WORLD_14"; 
    _symmap[ SDLK_WORLD_15] = "WORLD_15"; 
    _symmap[ SDLK_WORLD_16] = "WORLD_16"; 
    _symmap[ SDLK_WORLD_17] = "WORLD_17"; 
    _symmap[ SDLK_WORLD_18] = "WORLD_18"; 
    _symmap[ SDLK_WORLD_19] = "WORLD_19"; 
    _symmap[ SDLK_WORLD_20] = "WORLD_20"; 
    _symmap[ SDLK_WORLD_21] = "WORLD_21"; 
    _symmap[ SDLK_WORLD_22] = "WORLD_22"; 
    _symmap[ SDLK_WORLD_23] = "WORLD_23"; 
    _symmap[ SDLK_WORLD_24] = "WORLD_24"; 
    _symmap[ SDLK_WORLD_25] = "WORLD_25"; 
    _symmap[ SDLK_WORLD_26] = "WORLD_26"; 
    _symmap[ SDLK_WORLD_27] = "WORLD_27"; 
    _symmap[ SDLK_WORLD_28] = "WORLD_28"; 
    _symmap[ SDLK_WORLD_29] = "WORLD_29"; 
    _symmap[ SDLK_WORLD_30] = "WORLD_30"; 
    _symmap[ SDLK_WORLD_31] = "WORLD_31"; 
    _symmap[ SDLK_WORLD_32] = "WORLD_32"; 
    _symmap[ SDLK_WORLD_33] = "WORLD_33"; 
    _symmap[ SDLK_WORLD_34] = "WORLD_34"; 
    _symmap[ SDLK_WORLD_35] = "WORLD_35"; 
    _symmap[ SDLK_WORLD_36] = "WORLD_36"; 
    _symmap[ SDLK_WORLD_37] = "WORLD_37"; 
    _symmap[ SDLK_WORLD_38] = "WORLD_38"; 
    _symmap[ SDLK_WORLD_39] = "WORLD_39"; 
    _symmap[ SDLK_WORLD_40] = "WORLD_40"; 
    _symmap[ SDLK_WORLD_41] = "WORLD_41"; 
    _symmap[ SDLK_WORLD_42] = "WORLD_42"; 
    _symmap[ SDLK_WORLD_43] = "WORLD_43"; 
    _symmap[ SDLK_WORLD_44] = "WORLD_44"; 
    _symmap[ SDLK_WORLD_45] = "WORLD_45"; 
    _symmap[ SDLK_WORLD_46] = "WORLD_46"; 
    _symmap[ SDLK_WORLD_47] = "WORLD_47"; 
    _symmap[ SDLK_WORLD_48] = "WORLD_48"; 
    _symmap[ SDLK_WORLD_49] = "WORLD_49"; 
    _symmap[ SDLK_WORLD_50] = "WORLD_50"; 
    _symmap[ SDLK_WORLD_51] = "WORLD_51"; 
    _symmap[ SDLK_WORLD_52] = "WORLD_52"; 
    _symmap[ SDLK_WORLD_53] = "WORLD_53"; 
    _symmap[ SDLK_WORLD_54] = "WORLD_54"; 
    _symmap[ SDLK_WORLD_55] = "WORLD_55"; 
    _symmap[ SDLK_WORLD_56] = "WORLD_56"; 
    _symmap[ SDLK_WORLD_57] = "WORLD_57"; 
    _symmap[ SDLK_WORLD_58] = "WORLD_58"; 
    _symmap[ SDLK_WORLD_59] = "WORLD_59"; 
    _symmap[ SDLK_WORLD_60] = "WORLD_60"; 
    _symmap[ SDLK_WORLD_61] = "WORLD_61"; 
    _symmap[ SDLK_WORLD_62] = "WORLD_62"; 
    _symmap[ SDLK_WORLD_63] = "WORLD_63"; 
    _symmap[ SDLK_WORLD_64] = "WORLD_64"; 
    _symmap[ SDLK_WORLD_65] = "WORLD_65"; 
    _symmap[ SDLK_WORLD_66] = "WORLD_66"; 
    _symmap[ SDLK_WORLD_67] = "WORLD_67"; 
    _symmap[ SDLK_WORLD_68] = "WORLD_68"; 
    _symmap[ SDLK_WORLD_69] = "WORLD_69"; 
    _symmap[ SDLK_WORLD_70] = "WORLD_70"; 
    _symmap[ SDLK_WORLD_71] = "WORLD_71"; 
    _symmap[ SDLK_WORLD_72] = "WORLD_72"; 
    _symmap[ SDLK_WORLD_73] = "WORLD_73"; 
    _symmap[ SDLK_WORLD_74] = "WORLD_74"; 
    _symmap[ SDLK_WORLD_75] = "WORLD_75"; 
    _symmap[ SDLK_WORLD_76] = "WORLD_76"; 
    _symmap[ SDLK_WORLD_77] = "WORLD_77"; 
    _symmap[ SDLK_WORLD_78] = "WORLD_78"; 
    _symmap[ SDLK_WORLD_79] = "WORLD_79"; 
    _symmap[ SDLK_WORLD_80] = "WORLD_80"; 
    _symmap[ SDLK_WORLD_81] = "WORLD_81"; 
    _symmap[ SDLK_WORLD_82] = "WORLD_82"; 
    _symmap[ SDLK_WORLD_83] = "WORLD_83"; 
    _symmap[ SDLK_WORLD_84] = "WORLD_84"; 
    _symmap[ SDLK_WORLD_85] = "WORLD_85"; 
    _symmap[ SDLK_WORLD_86] = "WORLD_86"; 
    _symmap[ SDLK_WORLD_87] = "WORLD_87"; 
    _symmap[ SDLK_WORLD_88] = "WORLD_88"; 
    _symmap[ SDLK_WORLD_89] = "WORLD_89"; 
    _symmap[ SDLK_WORLD_90] = "WORLD_90"; 
    _symmap[ SDLK_WORLD_91] = "WORLD_91"; 
    _symmap[ SDLK_WORLD_92] = "WORLD_92"; 
    _symmap[ SDLK_WORLD_93] = "WORLD_93"; 
    _symmap[ SDLK_WORLD_94] = "WORLD_94"; 
    _symmap[ SDLK_WORLD_95] = "WORLD_95"; 
    _symmap[ SDLK_KP0] = "KP0"; 
    _symmap[ SDLK_KP1] = "KP1"; 
    _symmap[ SDLK_KP2] = "KP2"; 
    _symmap[ SDLK_KP3] = "KP3"; 
    _symmap[ SDLK_KP4] = "KP4"; 
    _symmap[ SDLK_KP5] = "KP5"; 
    _symmap[ SDLK_KP6] = "KP6"; 
    _symmap[ SDLK_KP7] = "KP7"; 
    _symmap[ SDLK_KP8] = "KP8"; 
    _symmap[ SDLK_KP9] = "KP9"; 
    _symmap[ SDLK_KP_PERIOD] = "KP_PERIOD"; 
    _symmap[ SDLK_KP_DIVIDE] = "KP_DIVIDE"; 
    _symmap[ SDLK_KP_MULTIPLY] = "KP_MULTIPLY"; 
    _symmap[ SDLK_KP_MINUS] = "KP_MINUS"; 
    _symmap[ SDLK_KP_PLUS] = "KP_PLUS"; 
    _symmap[ SDLK_KP_ENTER] = "KP_ENTER"; 
    _symmap[ SDLK_KP_EQUALS] = "KP_EQUALS"; 
    _symmap[ SDLK_UP] = "UP"; 
    _symmap[ SDLK_DOWN] = "DOWN"; 
    _symmap[ SDLK_RIGHT] = "RIGHT"; 
    _symmap[ SDLK_LEFT] = "LEFT"; 
    _symmap[ SDLK_INSERT] = "INSERT"; 
    _symmap[ SDLK_HOME] = "HOME"; 
    _symmap[ SDLK_END] = "END"; 
    _symmap[ SDLK_PAGEUP] = "PAGEUP"; 
    _symmap[ SDLK_PAGEDOWN] = "PAGEDOWN"; 
    _symmap[ SDLK_F1] = "F1"; 
    _symmap[ SDLK_F2] = "F2"; 
    _symmap[ SDLK_F3] = "F3"; 
    _symmap[ SDLK_F4] = "F4"; 
    _symmap[ SDLK_F5] = "F5"; 
    _symmap[ SDLK_F6] = "F6"; 
    _symmap[ SDLK_F7] = "F7"; 
    _symmap[ SDLK_F8] = "F8"; 
    _symmap[ SDLK_F9] = "F9"; 
    _symmap[ SDLK_F10] = "F10"; 
    _symmap[ SDLK_F11] = "F11"; 
    _symmap[ SDLK_F12] = "F12"; 
    _symmap[ SDLK_F13] = "F13"; 
    _symmap[ SDLK_F14] = "F14"; 
    _symmap[ SDLK_F15] = "F15"; 
    _symmap[ SDLK_NUMLOCK] = "NUMLOCK"; 
    _symmap[ SDLK_CAPSLOCK] = "CAPSLOCK"; 
    _symmap[ SDLK_SCROLLOCK] = "SCROLLOCK"; 
    _symmap[ SDLK_RSHIFT] = "RSHIFT"; 
    _symmap[ SDLK_LSHIFT] = "LSHIFT"; 
    _symmap[ SDLK_RCTRL] = "RCTRL"; 
    _symmap[ SDLK_LCTRL] = "LCTRL"; 
    _symmap[ SDLK_RALT] = "RALT"; 
    _symmap[ SDLK_LALT] = "LALT"; 
    _symmap[ SDLK_RMETA] = "RMETA"; 
    _symmap[ SDLK_LMETA] = "LMETA"; 
    _symmap[ SDLK_LSUPER] = "LSUPER"; 
    _symmap[ SDLK_RSUPER] = "RSUPER"; 
    _symmap[ SDLK_MODE] = "MODE"; 
#ifndef WIN32
//FIXME...
    _symmap[ SDLK_COMPOSE] = "COMPOSE"; 
#endif
    _symmap[ SDLK_HELP] = "HELP"; 
    _symmap[ SDLK_PRINT] = "PRINT"; 
    _symmap[ SDLK_SYSREQ] = "SYSREQ"; 
    _symmap[ SDLK_BREAK] = "BREAK"; 
    _symmap[ SDLK_MENU] = "MENU"; 
    _symmap[ SDLK_POWER] = "POWER"; 
    _symmap[ SDLK_EURO] = "EURO"; 
}

Keys::~Keys()
{
    XTRACE();
}

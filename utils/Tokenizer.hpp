#pragma once
// Description:
//   Helper to tokenize a string.
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

#include <string>
#include <vector>

#include <iostream>
#include <algorithm>
#include <iterator>
#include <sstream>

class Tokenizer
{
public:
    Tokenizer( std::string line, const char *whitespace = " \t\n\r"):
        _line(line),
        _whitespace(whitespace),
        _pos(0),
        _tokenCount(0),
	_withQuotes(true)
    {
    }

    Tokenizer( std::string line, bool withQuotes, const char *whitespace = " \t\n\r"):
        _line(line),
        _whitespace(whitespace),
        _pos(0),
        _tokenCount(0),
	_withQuotes(withQuotes)
    {
    }

    std::string operator()( void){ return next();}

    void setWhitespace( const std::string &whitespace)
    {
        _whitespace = whitespace;
    }

    std::string next( void)
    {
        std::string retVal = "";
	std::string::size_type start = _line.find_first_not_of( _whitespace, _pos);
	std::string::size_type adj = 0;
        if( start != std::string::npos)
        {
	    if( _withQuotes && (_line[ start] == '"'))
	    {
		start++;
		_pos = _line.find_first_of( "\"", start);
		_pos++;
		adj = 1;
	    }
	    else
	    {
		_pos = _line.find_first_of( _whitespace, start);
	    }

            if( _pos == std::string::npos)
            {
                retVal = _line.substr( start);
            }
            else
            {
                retVal = _line.substr( start, _pos-start-adj);
            }
            _tokenCount++;
        }

        return retVal;
    }

    int tokensReturned( void){ return _tokenCount;}

    std::string::size_type getPos( void){ return _pos;}
    void setPos( std::string::size_type pos){ _pos = pos;}

    //doesn't really belong here...
    
    static inline std::string replaceAll( const std::string& s, const std::string& f, const std::string& r ) 
    {
        if ( s.empty() || f.empty() || f == r || f.size() > s.size() || s.find(f) == std::string::npos ) 
        {
            return s;
        }
        std::ostringstream build_it;
        typedef std::string::const_iterator iter;
        iter i( s.begin() );
        const iter::difference_type f_size( distance( f.begin(), f.end() ) );
        for ( iter pos; ( pos = std::search( i , s.end(), f.begin(), f.end() ) ) != s.end(); ) 
        {
            std::copy( i, pos, std::ostreambuf_iterator<char>( build_it ) );
            std::copy( r.begin(), r.end(), std::ostreambuf_iterator<char>( build_it ) );
            std::advance( pos, f_size);
            i = pos;
        }
        std::copy( i, s.end(), std::ostreambuf_iterator<char>( build_it ) );
        return build_it.str();
    }
    
    static std::vector<std::string> csvToVector( const std::string &line, char delimiter=',', char enclosure='"')
    {
        std::vector<std::string> result;
        result.push_back("");
        
        int idx = 0;
        bool quoted = false;
        for( unsigned int i=0; i<line.length(); i++)
        {
            char ch = line[i];
            if( ch == enclosure)
            {
                quoted = !quoted;
            }
            
            if( ch == delimiter && !quoted)
            {
                result.push_back("");
                idx++;
            }
            else
            {
                result[idx] += ch;
            }
        }
        
        std::string escapedEnclosure = enclosure + std::string("") + enclosure;
        for( unsigned int k=0; k<result.size(); k++)
        {
            if( result[k] != "" && result[k][0] == enclosure)
            {
                result[k] = result[k].substr( 1, result[k].length()-2);
                result[k] = replaceAll( result[k], escapedEnclosure, std::string("")+enclosure);
            }
        }
        
        return result;
    }
    
private:
    std::string _line;
    std::string _whitespace;
    std::string::size_type _pos;

    int _tokenCount;
    bool _withQuotes;
};

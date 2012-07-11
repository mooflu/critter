#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifndef WIN32
#include <unistd.h>
#include <dirent.h>
#else
#define PATH_MAX 512
#define S_ISDIR(f) (f&_S_IFDIR)
#define S_ISREG(f) (f&_S_IFREG)
#endif

#include <list>
#include <fstream>

#include <zlib.h>

#include <Trace.hpp>
#include <Tokenizer.hpp>
#include <ResourceManager.hpp>
#include <RectanglePacker.hpp>
#include <WalkDirectory.hpp>
#include <PNG.hpp>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

using namespace std;

const int MAX_LEVEL = 5;
const char RPATH_SEPERATOR = '/';
//OS dependent seperator
const char PATH_SEPERATOR = '/';

bool operator<( DirectoryEntry &de1, DirectoryEntry &de2)
{
    return( de1.resourceName < de2.resourceName);
}

class PNGDimensionObject: public DimensionObject
{
public:
    PNGDimensionObject( const string &aFileName): DimensionObject(0,0)
    {
	fileName = aFileName;
	img = IMG_Load( fileName.c_str());
	SDL_SetAlpha(img,0,0);

	_dim.width = img->w;
	_dim.height = img->h;
    }

    SDL_Surface *img;
    string fileName;
};

class PNGPacker: public RectanglePacker
{
public:
    PNGPacker(const string &fileName, int size):RectanglePacker(size,size),_fileName(fileName)
    {
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;
#endif

	img = SDL_CreateRGBSurface( SDL_SWSURFACE, _root->dim.width, _root->dim.height, 32, 
	    rmask, gmask, bmask, amask);
    }

    void Save( void)
    {
	Combine();

	PNG png;
	png.Save( img, _fileName, true);
    }

protected:
    void Combine( void)
    {
	Combine( _root);
    }
    void Combine( Node *atNode)
    {
	if( !atNode) return;

	if( ! atNode->isLeaf)
	{
	    Combine( atNode->left);
	    Combine( atNode->right);        
	}
	else if( atNode->object)
	{
	    PNGDimensionObject *png = (PNGDimensionObject*)atNode->object;
	    SDL_Rect dstRect;
	    LOG_INFO << "Blitting "<<png->fileName <<" to " << atNode->off.x << "," << atNode->off.y << "\n";
	    dstRect.x = atNode->off.x;
	    dstRect.y = atNode->off.y;
	    if( SDL_BlitSurface( png->img, 0, img, &dstRect) != 0)
	    {
		LOG_ERROR << "Blit failed\n";
	    }
	}
    }
    SDL_Surface *img;
    string _fileName;
};

class Packer
{
public:
    Packer( const string &packfileDescription, const string &packfile, bool packPNG, int size):
        _level( 0),
        _infile( packfileDescription.c_str()),
        _outfile( packfile.c_str(), ios::out | ios::trunc | ios::binary),
	_packPNG(packPNG),
	_pngPacker(0)
    {
	if( _packPNG)
	{
	    _pngPacker = new PNGPacker(packfile, size);
	}
    }

    bool Process( void);

private:
    void addFile( 
	const string &fileName, 
	const string &relativePath,
	const string &fName);

    bool addPNG( 
	const string &fileName, 
	const string &relativePath,
	const string &fName);

    void addDirectory( 
	const string &dirName,
	const string &resourceName, 
	const string &relativePath);

    void addEntry( 
	const string &fileName,
	const string &resourceName,
	const string &relativePath,
	const string &dName);

    int _level;
    ifstream _infile;
    ofstream _outfile;

    list<DirectoryEntry> dirEntryList;

    bool _packPNG;
    PNGPacker *_pngPacker;
};

bool Packer::Process( void)
{
    string line;

    if( !_infile.is_open())
    {
	return false;
    }

    while( !getline( _infile, line).eof())
    {
        if( line[0] == '#') continue;
        Tokenizer  t( line);
        string fileName = t.next();
        string resourceName = t.next();
        if( fileName == "") continue;

	char fullPath[PATH_MAX];
#ifdef WIN32
	if( _fullpath(fullPath, fileName.c_str(), PATH_MAX) == 0)
#else
	if( realpath(fileName.c_str(), fullPath) == 0)
#endif
        {
	    LOG_WARNING << "Problem getting full path to file: " 
                        << fileName << endl;
	    LOG_WARNING << "Skipping..." << endl;
            continue;
        }

        if( resourceName != "")
        {
            fileName = resourceName;
        }
        string fullFileName = fullPath;
        string relativePath = "";
        addEntry( fullFileName, resourceName, relativePath, fileName);
    }
    _infile.close();

    if( _packPNG)
    {
	_outfile.close();
	_pngPacker->Save();
    }
    else
    {
	LOG_INFO << "Starting sort of " << dirEntryList.size() << " elements" << endl;
	dirEntryList.sort();

	Uint32 dirStart = _outfile.tellp();
	_outfile << (Uint32) dirStart;

	list<DirectoryEntry>::iterator i;
	for( i=dirEntryList.begin(); i!=dirEntryList.end(); i++)
	{
	    const string &rName = (*i).resourceName;
	    Uint32 offset = (*i).offset;
	    Uint32 origSize = (*i).origSize;
	    Uint32 compSize = (*i).compSize;
	    LOG_INFO << "Writing: " << rName << " [" << offset << "]" << endl;

	    _outfile.write( rName.c_str(), rName.length()+1);
	    _outfile << (Uint32) offset;
	    _outfile << (Uint32) origSize;
	    _outfile << (Uint32) compSize;
	}

	Uint32 dirLen = (Uint32)_outfile.tellp() - dirStart;

	_outfile << (Uint32) dirLen;
	_outfile << (Uint32) RESOURCE_MAGIC;

	_outfile.close();
    }

    return true;
}

bool Packer::addPNG( 
	const string &fileName, 
	const string &relativePath,
	const string &fName)
{
    string rName = relativePath + PATH_SEPERATOR + fName;

    if( relativePath == "")
    {
	rName = fName;
    }

    for( unsigned int i=0; i<rName.length(); i++)
    {
	if( rName[i] == PATH_SEPERATOR)
	{
	    rName[i] = RPATH_SEPERATOR;
	}
    }

    PNGDimensionObject *png = new PNGDimensionObject( fileName);

    if( ! _pngPacker->Insert( png))
    {
	LOG_ERROR << "Atlas out of space for: " << fileName << "\n";
	return false;
    }
    LOG_INFO << "Atlas at " << _pngPacker->PercentUsed() << "% (" << png->dimension().width << "x" << png->dimension().height << ")\n";

    return true;
}

void Packer::addFile( 
    const string &fileName, 
    const string &relativePath,
    const string &fName)
{
    string rName = relativePath + PATH_SEPERATOR + fName;

    if( relativePath == "")
    {
        rName = fName;
    }

    for( unsigned int i=0; i<rName.length(); i++)
    {
        if( rName[i] == PATH_SEPERATOR)
        {
            rName[i] = RPATH_SEPERATOR;
        }
    }

    ifstream theFile( fileName.c_str(), ios::in | ios::binary);
    ziStream inFile( theFile);
    if( !inFile.isOK())
    {
	LOG_WARNING << "Unable to open " << fileName << endl;
        return;
    }

    DirectoryEntry de;
    de.resourceName = rName;
    de.offset = _outfile.tellp();

    {
	zoStream outFile( _outfile/*, eLZMACompression*/);
	if( !outFile.isOK())
	{
	    LOG_ERROR << "Unable to open packfile!" << endl;
	    return;
	}

	LOG_INFO << "Processing [" << fileName << "] ";
	char inBuf[8192];

	int inCount = 0;
	inFile.read( inBuf, 8192);
	while( inFile.gcount() > 0)
	{ 
	    inCount+=inFile.gcount();
	    outFile.write( inBuf, inFile.gcount());
	    inFile.read( inBuf, 8192);
	}

	de.origSize = inCount;
    }
    _outfile.flush();
    de.compSize = (Uint32)_outfile.tellp()-de.offset;

    int percent = de.origSize ? (100*de.compSize)/de.origSize : 0;

    LOG_INFO << " OK (" << de.origSize << "/" 
             << de.compSize << ") " << percent << "%" << endl;

    dirEntryList.insert( dirEntryList.end(), de);
}

void Packer::addDirectory( 
    const string &dirName,
    const string &resourceName, 
    const string &relativePath)
{
    WalkDirectory dir( dirName );

    DirEntry dirEntry;
    while( dir.getNext(dirEntry) )
    {
        string fileName = dirEntry.name;
        if( (fileName == ".") || (fileName == "..")) continue;

        string fullName = dirName +PATH_SEPERATOR+ fileName;
        addEntry( fullName, resourceName, relativePath, fileName);
    }
}

void Packer::addEntry( 
    const string &fileName,
    const string &resourceName,
    const string &relativePath,
    const string &dName)
{
    if( _level > MAX_LEVEL)
    {
	LOG_WARNING << "Max directory depth (" 
                    << MAX_LEVEL << ") exceeded." << endl;
	LOG_WARNING << "Skipping: " << fileName << endl;
        return;
    }

    struct stat statInfo;
    if( stat( fileName.c_str(), &statInfo) == -1)
    {
	LOG_WARNING << "2Problem accessing: " << fileName << endl;
	LOG_WARNING << "Skipping..." << endl;
	return;
    }

    //skip .svn directories
    if( dName == ".svn")
    {
	LOG_WARNING << "Skipping Subversion directory." << endl;
	return;
    }

    //skip CVS directories
    if( dName == "CVS")
    {
	LOG_WARNING << "Skipping CVS directory." << endl;
	return;
    }

    if( S_ISDIR( statInfo.st_mode))
    {
        string newRelativePath = relativePath;
        if( _level > 0)
        {
            newRelativePath += PATH_SEPERATOR+ dName;
        }
        else
        {
            newRelativePath += dName;
        }
        _level++;
	addDirectory( fileName, resourceName, newRelativePath);
        _level--;
    }
    else if( S_ISREG( statInfo.st_mode))
    {
	if( _packPNG)
	{
	    addPNG( fileName, relativePath, dName);
	}
	else
	{
	    addFile( fileName, relativePath, dName);
	}
    }
    else
    {
	LOG_WARNING << "Unsupported file type: " << fileName << endl;
	LOG_WARNING << "Skipping..." << endl;
	return;
    }
}

int main( int argc, char *argv[])
{
    if( argc < 3)
    {
	LOG_ERROR << "Packer description_file pack_file [-png]" << endl;
        return -1;
    }

    string description_file( argv[1]);
    string pack_file( argv[2]);
    bool packPNGAtlas = false;
    int size = 1024;
    if( argc == 4)
    {
	packPNGAtlas = true;
	size = atoi( &argv[3][1]);
	if( size < 128) size = 1024;
    }
    Packer p( description_file, pack_file, packPNGAtlas, size);
    if( ! p.Process())
    {
	LOG_ERROR << "Failed to process\n";
	return -1;
    }

//-----

    if( !packPNGAtlas)
    {
	ResourceManager &rm = *ResourceManagerS::instance();
	if( rm.addResourcePack( string( argv[2])))
	{
	    LOG_INFO << "Verified packfile OK." << endl;
	}
    }
#if 0
    rm.dump();
{
    string resName("models/Bonus2.model");
    if( rm.selectResource( resName))
    {
	ziStream &infile = rm.getInputStream();
	string line;
	while( !getline( infile, line).eof())
        {
            cout << line << endl;
        }
    }
    else
    {
        cout << resName << " not found" << endl;
    }
}
{
    string resName("models/P1.model");
    if( rm.selectResource( resName))
    {
	ziStream &infile = rm.getInputStream();
	string line;
	while( !getline( infile, line).eof())
        {
            cout << line << endl;
        }
    }
    else
    {
        cout << resName << " not found" << endl;
    }
}
{
    string resName("bitmaps/menuShadow.font");
    if( rm.selectResource( resName))
    {
	ziStream &infile = rm.getInputStream();
	char buffer[100000];
	infile.read( buffer, 100000);
	streamsize numBytes = infile.gcount();
        ofstream outfile( "test.png", ios::out | ios::trunc | ios::binary);
	outfile.write( buffer, numBytes); 
    }
    else
    {
        cout << resName << " not found" << endl;
    }
}
#endif

    return 0;
}

#include "zStreamBuffer.hpp"
#include "Trace.hpp"

#include <physfs.h>

zoStreamBuffer::zoStreamBuffer(const std::string& fileName) :
    _physFile(),
    _isOK(_init(fileName)) {}

zoStreamBuffer::~zoStreamBuffer() {
    sync();
    PHYSFS_close(_physFile);
}

bool zoStreamBuffer::_init(const std::string& fileName) {
    _physFile = PHYSFS_openWrite(fileName.c_str());
    if (!_physFile) {
        LOG_ERROR << "File not found:" << fileName << "\n";
        LOG_ERROR << "PhysFS:" << PHYSFS_getLastErrorCode() << "\n";
        return false;
    }

    setp(_buf, _buf + kWriteBufferSize);
    return true;
}

int zoStreamBuffer::overflow(int i) {
    char c = (char)i;

    if (pbase() == pptr()) {
        return 0;
    }

    size_t size = pptr() - pbase();
    PHYSFS_sint64 bytesWritten = PHYSFS_writeBytes(_physFile, pbase(), size);
    if (bytesWritten <= 0) {
        return traits_type::eof();
    }

    if (c != traits_type::eof()) {
        PHYSFS_sint64 bytesWritten = PHYSFS_writeBytes(_physFile, &c, 1);
        if (bytesWritten <= 0) {
            return traits_type::eof();
        }
    }

    setp(_buf, _buf + bytesWritten);
    return 0;
}

int zoStreamBuffer::sync(void) {
    return overflow(traits_type::eof());
}

//--------------------------------------------------------------------------

ziStreamBuffer::ziStreamBuffer(const std::string& fileName) :
    _physFile(),
    _isOK(_init(fileName)) {}

ziStreamBuffer::~ziStreamBuffer() {
    PHYSFS_close(_physFile);
}

bool ziStreamBuffer::_init(const std::string& fileName) {
    _physFile = PHYSFS_openRead(fileName.c_str());
    if (!_physFile) {
        LOG_ERROR << "Unable to open file:" << fileName << "\n";
        LOG_ERROR << "PhysFS:" << PHYSFS_getLastErrorCode() << "\n";
        return false;
    }
    return true;
}

int ziStreamBuffer::fileSize(void) {
    return (int)PHYSFS_fileLength(_physFile);
}

ziStreamBuffer::int_type ziStreamBuffer::underflow(void) {
    if (gptr() < egptr()) {
        return traits_type::to_int_type(*gptr());
    }

    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(_physFile, _buf, sizeof(_buf));
    if (bytesRead <= 0) {
        return traits_type::eof();
    }
    setg(_buf, _buf, _buf + bytesRead);

    return traits_type::to_int_type(*gptr());
}

ziStreamBuffer::pos_type ziStreamBuffer::seekpos(pos_type pos, std::ios_base::openmode) {
    if (PHYSFS_seek(_physFile, static_cast<PHYSFS_uint64>(pos)) == 0) {
        return pos_type(off_type(-1));
    }

    setg(_buf, _buf, _buf);
    return pos;
}

ziStreamBuffer::pos_type ziStreamBuffer::seekoff(off_type off, std::ios_base::seekdir dir,
                                                 std::ios_base::openmode mode) {
    off_type pos = off;
    PHYSFS_sint64 ptell = PHYSFS_tell(_physFile);

    switch (dir) {
        case std::ios_base::beg:
            break;
        case std::ios_base::cur:
            if (off == 0) {
                return static_cast<pos_type>(ptell) - static_cast<pos_type>(egptr() - gptr());
            }
            pos += static_cast<off_type>(ptell) - static_cast<off_type>(egptr() - gptr());
            break;
        case std::ios_base::end:
            pos += static_cast<off_type>(PHYSFS_fileLength(_physFile));
            break;
        default:
            LOG_ERROR << "seekoff: unknown direction" << dir << "\n";
            return pos_type(off_type(-1));
    }

    return seekpos(static_cast<pos_type>(pos), mode);
}

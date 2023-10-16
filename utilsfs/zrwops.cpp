// Description:
//   SDL_RWops wrapper for compressed stream.
//
// Copyright (C) 2011 Frank Becker
//
#include "zrwops.hpp"

#include "Trace.hpp"
#include "zStreamBuffer.hpp"

#include <physfs.h>

Sint64 ziStream_seek(struct SDL_RWops* context, Sint64 offset, int whence) {
    PHYSFS_file* physFile = (PHYSFS_file*)context->hidden.unknown.data1;
    int result;
    switch (whence) {
        case SEEK_SET:
            result = PHYSFS_seek(physFile, offset);
            break;
        case SEEK_CUR:
            result = PHYSFS_seek(physFile, PHYSFS_tell(physFile) + offset);
            break;
        case SEEK_END:
            result = PHYSFS_seek(physFile, PHYSFS_fileLength(physFile) + offset);
            break;
        default:
            result = 0;
            break;
    }
    if (!result) {
        LOG_WARNING << "Error seeking: " << PHYSFS_getLastErrorCode() << "\n";
        return -1;
    }

    return PHYSFS_tell(physFile);
}

size_t ziStream_read(struct SDL_RWops* context, void* ptr, size_t size, size_t maxnum) {
    PHYSFS_file* physFile = (PHYSFS_file*)context->hidden.unknown.data1;
    PHYSFS_sint64 bytesRead = PHYSFS_readBytes(physFile, ptr, size * maxnum);
    return bytesRead / size;
}

size_t ziStream_write(struct SDL_RWops*, const void*, size_t, size_t) {
    LOG_ERROR << "Writing ziStream not supported...\n";
    return -1;
}

int ziStream_close(struct SDL_RWops* context) {
    PHYSFS_file* physFile = (PHYSFS_file*)context->hidden.unknown.data1;
    PHYSFS_close(physFile);
    SDL_FreeRW(context);

    return 0;
}

SDL_RWops* RWops_from_ziStream(ziStream& zi) {
    PHYSFS_file* physFile = (PHYSFS_file*)((ziStreamBuffer*)zi.rdbuf())->getHandle();
    SDL_RWops* rwops;

    rwops = SDL_AllocRW();
    if (rwops != NULL) {
        rwops->seek = ziStream_seek;
        rwops->read = ziStream_read;
        rwops->write = ziStream_write;
        rwops->close = ziStream_close;
        rwops->hidden.unknown.data1 = physFile;
    }
    return (rwops);
}

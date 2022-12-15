#include <internal/include/_file.h>
#include <josrt.h>

/* FOR LATER
#define _io_file_fromstring(f, s)\
((f)->_buffer._begin = (f)->_buffer._rp = (const uint8_t*)(s), (f)->_buffer._end = (const uint8_t*)-1, (f)->_buffer._size = 0)
#define _io_file_getch(f) ((f)->_buffer._rp!=(f)->_buffer._end ? *(f)->_buffer._rp++ : 0)
#define _io_file_ungetch(f) ((f)->_buffer._rp!=(f)->_buffer._begin ? (f)->_buffer._rp-- ; (void)0)
#define _io_file_tobuffer(f, buffer, length)\
((f)->_buffer._rp = (void)0, (f)->_buffer._begin = (f)->_buffer._wp = (uint8_t*)(buffer), (f)->_buffer._end = ((f)->_buffer._wp + ((f)->_buffer._size = length)))
#define _io_file_from_buffer(f, b, l)\
((f)->_buffer._begin = (f)->_buffer._wp = (uint8_t*)(b), (f)->_buffer._rp = (const uint8_t*)(b), (f)->_buffer._end = ((const uint8_t*)(b)+l), (f)->_buffer._size = l, (f))
#define _io_file_update_pos(f, l)\
    (f)->_pos += l
*/

static generic_allocator_alloc_t* _file_handle_allocator;

static int _fflush(__FILE* stream) {
    if (stream->_buffer._wp > stream->_buffer._begin) {
        stream->write(stream, stream->_buffer._begin, stream->_buffer._wp - stream->_buffer._begin);
        stream->_buffer._wp = stream->_buffer._begin;
    }
    return 0;
}

static int _fwrite(__FILE *stream, const char* src, size_t elem_size, size_t elem_count) {
    size_t transferred = 0;
    size_t transfer_size = elem_size * elem_count;
    while (transfer_size) {
        size_t stream_buffer_avail = stream->_buffer._end - stream->_buffer._wp;
        size_t chunk_len = min(stream_buffer_avail, transfer_size);
        memcpy(stream->_buffer._wp, src, chunk_len);
        stream->_buffer._wp += chunk_len;
        transfer_size -= chunk_len;
        _io_file_update_pos(stream, chunk_len);
        transferred += chunk_len / elem_size;     //< number of items, not bytes
        if (transfer_size) {
            if (stream->flush) {
                stream->flush(stream);
            }
            else {
                // error; we'll return whatever count we've managed to transfer
                //TODO: errno
                break;
            }
        }
    }
    return (int)transferred;
}

static int _fread(__FILE * stream, void* buffer, size_t elem_size, size_t elem_count) {
    char* wp = (char*)buffer;
    size_t transferred = 0;
    size_t transfer_size = elem_size * elem_count;
    size_t stream_buffer_avail = stream->_buffer._end - stream->_buffer._rp;
    size_t chunk_len = min(stream_buffer_avail, transfer_size);
    memcpy(wp, stream->_buffer._rp, chunk_len);
    stream->_buffer._rp += chunk_len;
    transfer_size -= chunk_len;
    transferred += chunk_len / elem_size;
    _io_file_update_pos(stream, chunk_len);
    //TODO: what to do if full?
    //NOTE: we either got what we asked for or we didn't (blocking?)
    return (int)transferred;
}

static size_t _ftell(__FILE* stream) {
    return stream->_pos;
}

//TODO: initialise these with some internal function
__FILE *const stdin;
__FILE *const stdout;
__FILE *const stderr;

__FILE *fopen(const char *RESTRICT path, const char *RESTRICT flags) {

    //TODO: atomic
    if (!_available_file_handles) {
        //TODO: set error
        return 0;
    }

    __FILE* fp = _file_handle_allocator->alloc(sizeof(__FILE));
    fp->_fd = sys_open(path, flags);
    fp->_pos = 0;

    return fp;
}

#define F_NULLPTR_CHECKED(f)\
if((f)==0) {\
    return -1;\
}

int fprintf (__FILE *f, const char *RESTRICT fmt, ...) {

}

int fclose(__FILE * f) {
    F_NULLPTR_CHECKED(f);
    //TODO: some checks
    fflush(f);
    sys_close(f->_fd);
    _file_handle_allocator->free(f);

    return 0;
}

int feof(__FILE * f) {
    F_NULLPTR_CHECKED(f);
    return (int)__feof(f);
}

int ferror(__FILE * f) {
    F_NULLPTR_CHECKED(f);
    return (int)__ferror(f);
}

int fflush(__FILE * f) {
    F_NULLPTR_CHECKED(f);
    return f->_flush(f);
}

void clearerr(__FILE *) {

}

int fseek(__FILE *, long, int) {

}

long ftell(__FILE *) {

}

size_t fread(void *RESTRICT, size_t, size_t, __FILE *RESTRICT) {

}

size_t fwrite(const void *RESTRICT, size_t, size_t, __FILE *RESTRICT) {


}

int __file_initialize(dynamic_allocation_policy_t* allocator_policy) {

    _file_handle_allocator = allocator_policy->allocator;
    
    return 0;
}

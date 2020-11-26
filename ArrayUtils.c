//
// Created by aless on 21/11/2020.
//

#include "ArrayUtils.h"

#ifndef SIGUSR1
#define SIGUSR1 SIGTERM
#define SIGUSR1ISSIGTERM
#endif
#define uint unsigned int
#define ulong unsigned long
#define uchar unsigned char
#define ll long long
#define STD_CAPACITY 1
#define REALLOC_FACTOR 2

unsigned int std_capacity = STD_CAPACITY;
unsigned int realloc_factor = REALLOC_FACTOR;
int SIGNAL_USR_ArrayUtils = 0;
ArrayUtilsTraceLevel TRACE_LVL = NoTrace;

#define ASSERT_MIN_SIZE_CAPACITY(V, i, error_type, format, ...) {if (i >= V->size) { handle_err(error_type, format, ##__VA_ARGS__); }}

void handle_err(ArrayUtilsErrors error_type, const char *format, ...) {
    va_list args;
    va_start(args, format);
    if (!SIGNAL_USR_ArrayUtils) {
        vprintf(format, args);
        exit(error_type);
    }
    else {
        if (TRACE_LVL != NoTrace)
            fprintf(stderr, "[Warn: triggered exception handled by the user] Err: %s\n", errcodetostr_arrayutils(error_type));
        raise(SIGUSR1);
    }
    va_end(args);
}

struct Vector {
    unsigned char* data;
    unsigned int objsize;
    unsigned long size;
    unsigned long capacity;
};

unsigned char* vdata(Vector* v) {
    return v->data;
}
unsigned int vsize(Vector* v) {
    return v->size;
}
unsigned long vcapacity(Vector *v) {
    return v->capacity;
}
unsigned int vobjsize(Vector* v) {
    return v->objsize;
}

struct AllocatedArrays allocatedArrays = {
    NULL,
    0,
    1
};

#ifdef __GNUC__             // __attribute__((constructor)) is only present in GCC, therefore we need to check this.
#ifndef __clang__
void init(void) __attribute__((constructor));

void init(void) {
    atexit(free_all_arrayutils_structures);
}
#endif
#endif


void* safe_alloc(ulong size) {
    char* ptr = malloc(size);
    if (ptr == NULL) {
        handle_err(AllocationError, "Error in array allocation");
    }
    return ptr;
}

void* safe_realloc(void* ptr, ulong size) {
    ptr = realloc(ptr, size);
    if (ptr == NULL) {
        handle_err(ReallocationError, "Error in array reallocation");
    }
    return ptr;
}

Vector* vector() {
    Vector* v = safe_alloc(sizeof(Vector));
    if (allocatedArrays.nvectors == 0) {
        allocatedArrays.vectors = safe_alloc(sizeof(Vector *) * allocatedArrays.capacity);
    }
    allocatedArrays.nvectors++;
    if (allocatedArrays.nvectors > allocatedArrays.capacity) {
        allocatedArrays.capacity *= realloc_factor;
        allocatedArrays.vectors = safe_realloc(allocatedArrays.vectors, sizeof(Vector *) * allocatedArrays.capacity);
    }
    allocatedArrays.vectors[allocatedArrays.nvectors-1] = v;
    return v;
}
Vector* vector_new(uint objsize) {
    Vector* v = vector();
    v->objsize = objsize;
    v->capacity = std_capacity;
    v->size = 0;
    v->data = safe_alloc(objsize * std_capacity);
    return v;
}
Vector* vector_fromsize(uint objsize, ulong capacity) {
    Vector* v = vector();
    v->objsize = objsize;
    v->capacity = capacity;
    v->size = 0;
    v->data = safe_alloc(objsize * capacity);
    return v;
}
void add(Vector* vect, void* obj) {
    if (vect->size == vect->capacity) {
        vect->data = safe_realloc(vect->data, vect->capacity * realloc_factor * vect->objsize);
        vect->capacity = vect->capacity * realloc_factor;
    }
    memcpy((vect->data)+(vect->objsize * vect->size), obj, vect->objsize);
    vect->size++;
}

void add_range_move(Vector* vect, void* objs, uint nobj) {
    while (vect->size + nobj >= vect->capacity) {
        vect->data = safe_realloc(vect->data, vect->capacity * realloc_factor * vect->objsize);
        vect->capacity = vect->capacity * realloc_factor;
    }
    memmove(vect->data + (vect->size * vect->objsize), objs, vect->objsize * nobj);
    vect->size += nobj;
}

void add_range_copy(Vector* vect, void* objs, uint nobj) {
    while (vect->size + nobj >= vect->capacity) {
        vect->data = safe_realloc(vect->data, vect->capacity * realloc_factor * vect->objsize);
        vect->capacity = vect->capacity * realloc_factor;
    }
    memcpy(vect->data + (vect->size * vect->objsize), objs, vect->objsize * nobj);
    vect->size += nobj;
}

void add_range_move_at(Vector* vect, void* objs, uint nobj, uint at) {
    ASSERT_MIN_SIZE_CAPACITY(vect, at, OutOfBoundsAccessError, "Out of bounds attempt to insert")
    while (vect->size + nobj >= vect->capacity) {
        vect->data = safe_realloc(vect->data, vect->capacity * realloc_factor * vect->objsize);
        vect->capacity = vect->capacity * realloc_factor;
    }
    memmove(vect->data + (vect->objsize * (at + nobj)), vect->data + (vect->objsize * at), vect->objsize * (vect->size - at));
    memmove(vect->data + (vect->objsize * at), objs, vect->objsize * nobj);
    vect->size += nobj;
}

void add_range_copy_at(Vector* vect, void* objs, uint nobj, uint at) {
    ASSERT_MIN_SIZE_CAPACITY(vect, at, OutOfBoundsAccessError, "Out of bounds attempt to insert")
    while (vect->size + nobj >= vect->capacity) {
        vect->data = safe_realloc(vect->data, vect->capacity * realloc_factor * vect->objsize);
        vect->capacity = vect->capacity * realloc_factor;
    }
    memmove(vect->data + (vect->objsize * (at + nobj)), vect->data + (vect->objsize * at), vect->objsize * (vect->size - at));
    memcpy(vect->data + (vect->objsize * at), objs, vect->objsize * nobj);
    vect->size += nobj;
}

void replace_range_move(Vector* vect, void* objs, uint nobj, uint at) {
    if (nobj > vect->size) {
        handle_err(ReplaceMoreThanCurrentSizeError, "Trying to replace more items than what's currently in vector");
    }
    memmove(vect->data + (vect->objsize * at), objs, vect->objsize * nobj);
}

void replace_range_copy(Vector* vect, void* objs, uint nobj, uint at) {
    if (nobj > vect->size) {
        handle_err(ReplaceMoreThanCurrentSizeError, "Trying to replace more items than what's currently in vector");
    }
    memcpy(vect->data + (vect->objsize * at), objs, vect->objsize * nobj);
}

void* access(Vector* v, uint i) {
    ASSERT_MIN_SIZE_CAPACITY(v, i, OutOfBoundsAccessError, "Out of bound access")
    return v->data + (v->objsize * i);
}

void* copy_access(Vector* v, uint i) {
    ASSERT_MIN_SIZE_CAPACITY(v, i, OutOfBoundsAccessError, "Out of bound access")
    void* elem = safe_alloc(v->objsize);
    memcpy(elem, v->data + (v->objsize * i), v->objsize);
    return elem;
}

void* pop(Vector* vect) {
    ASSERT_MIN_SIZE_CAPACITY(vect, 0, EmptyPopError, "Trying to pop from empty array")
    void* cpy = malloc(vect->objsize);
    vect->size--;
    memcpy(cpy, vect->data + (vect->objsize * vect->size), vect->objsize);
    return cpy;
}

void pop_noret(Vector* vect) {
    ASSERT_MIN_SIZE_CAPACITY(vect, 0, EmptyPopError, "Trying to pop from empty array")
    vect->size--;
}

void delete_noret(Vector* vect, uint index) {
    ASSERT_MIN_SIZE_CAPACITY(vect, index, OutOfBoundsAccessError, "Out of bounds delete attempt")
    memmove(vect->data + (vect->objsize * index), vect->data + (vect->objsize * (index + 1)), vect->objsize * (vect->size - index));
    vect->size--;
}

void* delete(Vector* vect, uint index) {
    ASSERT_MIN_SIZE_CAPACITY(vect, index, OutOfBoundsAccessError, "Out of bounds delete attempt")
    void* cpy = malloc(vect->objsize);
    memcpy(cpy, vect->data + (vect->objsize * index), vect->objsize);
    memmove(vect->data + (vect->objsize * index), vect->data + (vect->objsize * (index + 1)), vect->objsize * (vect->size - index));
    vect->size--;
    return cpy;
}

void fill(Vector* vect, void* val, uint nobj) {
    while (vect->capacity < nobj) {
        vect->data = safe_realloc(vect->data, vect->capacity * realloc_factor * vect->objsize);
        vect->capacity = vect->capacity * realloc_factor;
    }
    for (uint i = 0; i < nobj; i++) {
        memcpy(vect->data + (vect->objsize * i), val, vect->objsize);
    }
    vect->size = nobj;
}

int n_matches_from_index(Vector* vect, void* val, int at, unsigned long size) {
    ASSERT_MIN_SIZE_CAPACITY(vect, at, OutOfBoundsAccessError, "Out of bounds access attempted")
    ASSERT_MIN_SIZE_CAPACITY(vect, at + size - 1, OutOfBoundsAccessError, "Out of bounds access attempted")
    int res;
    for (int i = at; i < size; i++) {
        res = memcmp(vect->data + (i * vect->objsize), val, vect->objsize);
        if (res)
            return 0;
    }
    return 1;
}

int any_match_from_index(Vector* vect, void* val, int at, unsigned long size, int* n) {
    ASSERT_MIN_SIZE_CAPACITY(vect, at, OutOfBoundsAccessError, "Out of bounds access attempted")
    ASSERT_MIN_SIZE_CAPACITY(vect, at + size - 1, OutOfBoundsAccessError, "Out of bounds access attempted")
    int res;
    for (int i = at; i < size; i++) {
        res = memcmp(vect->data + (i * vect->objsize), val, vect->objsize);
        if (res == 0) {
            *n = i;
            return 1;
        }
    }
    *n = -1;
    return 0;
}
int all_match(Vector* vect, void* val) {
    return n_matches_from_index(vect, val, 0, vect->size);
}
int any_match(Vector* vect, void* val, int* n) {
    return any_match_from_index(vect, val, 0, vect->size, n);
}

int count_match_from_index(Vector* vect, void* val, int at, unsigned long size) {
    ASSERT_MIN_SIZE_CAPACITY(vect, at, OutOfBoundsAccessError, "Out of bounds access attempted")
    ASSERT_MIN_SIZE_CAPACITY(vect, at + size - 1, OutOfBoundsAccessError, "Out of bounds access attempted")
    int count = 0;
    for (int i = at; i < size; i++) {
        if (memcmp(vect->data + (i * vect->objsize), val, vect->objsize) == 0)
            count++;
    }
    return count;
}

int count_matches(Vector* vect, void* val) {
    return count_match_from_index(vect, val, 0, vect->size);
}
void reverse(Vector* vect) {
    void* objs = safe_alloc(vect->objsize * vect->capacity);
    for (int i = (int)vect->size - 1, j = 0; i >= 0; i--, j++)
        memcpy(objs + (j * vect->objsize), vect->data + (i * vect->objsize), vect->objsize);
    free(vect->data);
    vect->data = objs;
}
void* extract_match(Vector* vect, void* val) {
    int i = 0;
    if (any_match(vect, val, &i))
        return vect->data + (i * vect->objsize);
    return NULL;
}

void print_vect(Vector* v, char* format) {
    for (int i = 0; i < v->size; i++)
        printf(format, *(v->data + (v->objsize * i)));
}

void print_vect_ptr(Vector* v, char* format) {
    for (int i = 0; i < v->size; i++)
        printf(format, *((void**)(v->data + (v->objsize * i))));        // this is horrible
}

void vector_free(Vector* v) {
    free(v->data);
    free(v);
}
void free_all_arrayutils_structures() {
    for (int i = 0; i < allocatedArrays.nvectors; i++)
        vector_free(allocatedArrays.vectors[i]);
    free(allocatedArrays.vectors);
}

AllocatedArrays* expose_internal_arrays() {
    return &allocatedArrays;
}

void set_resize_factor(unsigned int factor) {
    realloc_factor = factor;
}

void set_default_capacity(unsigned int capacity) {
    std_capacity = capacity;
}

unsigned int get_resize_factor() {
    return realloc_factor;
}

unsigned int get_default_capacity() {
    return std_capacity;
}

void override_signal_exception_arrayutils(void (*func)(int)) {
    SIGNAL_USR_ArrayUtils = 1;
    void* ret = signal(SIGUSR1, func);
    if (ret == SIG_ERR) {
        printf("Error in assignment of custom signal handler");
        exit(SignalHandlerError);
    }
}

void set_trace_lvl_arrayutils(ArrayUtilsTraceLevel trace_lvl) {
    TRACE_LVL = trace_lvl;
}

const char* errcodetostr_arrayutils(ArrayUtilsErrors err) {
    switch (err) {
        case OutOfBoundsAccessError:
            return "OutOfBoundsAccessError";
        case AllocationError:
            return "AllocationError";
        case ReallocationError:
            return "ReallocationError";
        case ReplaceMoreThanCurrentSizeError:
            return "ReplaceMoreThanCurrentSizeError";
        case EmptyPopError:
            return "EmptyPopError";
        case SignalHandlerError:
            return "SignalHandlerError";
        default:
            return "InvalidErrorCode";
    }
}

#undef uint
#undef ulong
#undef uchar
#undef ll
#undef STD_CAPACITY
#undef REALLOC_FACTOR
#undef ASSERT_MIN_SIZE_CAPACITY
#ifdef SIGUSR1ISSIGTERM
#undef SIGUSR1
#endif
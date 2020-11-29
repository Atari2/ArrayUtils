//
// Created by aless on 21/11/2020.
//
/**
 * @brief file ArrayUtils.h
 * @author Alessio Rosiello
 * @date 26/11/2020
 * @brief Fixed object size array implementation
 */

#ifndef ARRAYUTILS_ARRAYUTILS_H
#define ARRAYUTILS_ARRAYUTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdarg.h>

/**
 * @brief All the errors defined in the library
 */
typedef enum ArrayUtilsErrors {
    OutOfBoundsAccessError,
    AllocationError,
    ReallocationError,
    ReplaceMoreThanCurrentSizeError,
    EmptyPopError,
    SignalHandlerError,
} ArrayUtilsErrors;

/**
 * @brief The types of tracing available in this library
 * <br> NoTrace is completely silent execution
 * <br> Warn warns on errors (if custom handler is present, it kills the program otherwise)
 */
typedef enum ArrayUtilsTraceLevel {
    NoTrace = 0,
    Warn = 1
} ArrayUtilsTraceLevel;

/**
 * @brief Gets a value of type from returned pointer from func, func should return a void* for this to be used correctly
 * <br> Usage ex: int a = VAL(int, access(vector, 2)); -> Gets the 2nd value from vector and casts it to an int
 */
#define VAL(type, func) *(type*)func

/**
 * @brief Adds a value of type to vector.
 * <br> Usage ex: ADD_VAL(vector, 3) -> will add a 3 to the tail end of the vector (assumes the vector is of the correct type)
 */
#define ADD_VAL(vector, val) {typeof(val) ___insvalue___ = val; add(vector, &___insvalue___);}

/**
 * @brief Creates a new vector of size 0 and type
 * <br> Functionally equal to vector_new()
 * <br> Usage ex: Vector* v = VECTOR(int);
 * @see vector_new()
 */
#define VECTOR(type) vector_new(sizeof(type))

/**
 * @brief Creates a new vector of size cap and type
 * <br> Functionally equal to vector_fromsize()
 * <br> Usage ex: Vector* v = VECTOR_S(int, 10);
 * @see vector_fromsize()
 */
#define VECTOR_S(type, cap) vector_fromsize(sizeof(type), cap)

/**
 * @brief Fills an existing vector with val of type num times
 * <br> Functionally equal to fill()
 * <br> Usage ex: FILL(vector, 10, 5) -> Fills vector of 5 ints of value 10
 * @see fill()
 */
#define FILL(vector, val, n) {typeof(val) ___fillvalue___ = val; fill(vector, &___fillvalue___, n);}

/**
 * @brief ADT that defines the vector class, to access interals use appropriate functions
 * <br> This vector will auto expand as needed when inserting values, but it will not resize down.
 * <br> Each entry in a vector needs to have the same size in order to work. You can't have mixed types.
 * @see vdata()
 * @see vsize()
 * @see vcapacity()
 * @see vobjsize()
 */
typedef struct Vector Vector;

/**
 * @brief Struct that contains all the pointers to allocated arrays, this should not be manipulated, however it is accessible from expose_internal_arrays()
 * @param vectors
 * @param nvectors
 * @param capacity
 * @see expose_interal_arrays()
 */
typedef struct AllocatedArrays {
    Vector** vectors;
    unsigned int nvectors;
    unsigned int capacity;
} AllocatedArrays;

/**
 * @brief Creates a vector with capacity STD_CAPACITY (1 by default) with each entry of size objsize
 * <br> Example: Vector* v = vector_new(sizeof(int));
 * @param objsize -> size of entry
 * @return pointer to created struct
 */
Vector* vector_new(unsigned int objsize);

/**
 * @brief Creates a vector with capacity specified with each entry of size objsize
 * <br> Example: Vector* v = vector_fromsize(sizeof(int), 10);
 * @param objsize -> size of entry
 * @param capacity -> initial capacity of vector
 * @return pointer to created struct
 */
Vector* vector_fromsize(unsigned int objsize, unsigned long capacity);

/**
 * @brief Creates a vector of ints (varargs version)
 * @param n_of_elements
 * @param ... the elements to insert in the vector
 * @return pointer to created struct
 */
Vector* vector_from_args_int(int n_of_elements, ...);

/**
 * @brief Returns the internal data buffer of given array struct
 * @param v -> vector
 * @return data buffer of vector
 */
unsigned char* vdata(Vector* v);

/**
 * @brief Returns the internal size (how many filled out objects the array contains) of given array struct
 * @param v -> vector
 * @return size
 */
unsigned int vsize(Vector* v);

/**
 * @brief Returns the internal capacity (how many items can the array hold before having to expand) of given array struct
 * @param v -> vector
 * @return capacity
 */
unsigned long vcapacity(Vector *v);

/**
 * @brief Returns the internal objsize (size of each entry) of given array struct
 * @param v -> vector
 * @return size of each entry
 */
unsigned int vobjsize(Vector* v);

/**
 * @brief Adds an element to the end of the vector
 * @param vect -> vector
 * @param obj -> the pointer to the object to add
 */
void add(Vector* vect, void* obj);

/**
 * @brief Adds nobj objects to the end of the vector using memcpy.
 * @param vect -> vector
 * @param objs -> pointer to the array of objects to add
 * @param nobj -> number of objects to add
 */
void add_range_copy(Vector* vect, void* objs, unsigned int nobj);

/**
 * @brief Adds nobj objects to the end of the vector using memmove.
 * @param vect -> vector
 * @param objs -> pointer to the array of objects to add
 * @param nobj -> number of objects to add
 */
void add_range_move(Vector* vect, void* objs, unsigned int nobj);

/**
 * @brief Adds nobj objects starting from specified index of the vector using memcpy.
 * @param vect -> vector
 * @param objs -> pointer to the array of objects to add
 * @param nobj -> number of objects to add
 * @param at -> index to start inserting at
 */
void add_range_copy_at(Vector* vect, void* objs, unsigned int nobj, unsigned int at);

/**
 * @brief Adds nobj objects starting from specified index of the vector using memmove.
 * @param vect -> vector
 * @param objs -> pointer to the array of objects to add
 * @param nobj -> number of objects to add
 * @param at -> index to start inserting at
 */
void add_range_move_at(Vector* vect, void* objs, unsigned int nobj, unsigned int at);

/**
 * @brief Replaces nobj objects starting from specified index of the vector using memcpy.
 * @param vect -> vector
 * @param objs -> pointer to the array of objects to add
 * @param nobj -> number of objects to add
 * @param at -> index to start inserting at
 */
void replace_range_copy(Vector* vect, void* objs, unsigned int nobj, unsigned int at);

/**
 * @brief Replaces nobj objects starting from specified index of the vector using memmove.
 * @param vect -> vector
 * @param objs -> pointer to the array of objects to add
 * @param nobj -> number of objects to add
 * @param at -> index to start inserting at
 */
void replace_range_move(Vector* vect, void* objs, unsigned int nobj, unsigned int at);

/**
 * @brief Returns pointer to i-th object of vector
 * @param v -> vector
 * @param i -> index to access
 * @return pointer to data
 */
void* access(Vector* v, unsigned int i);

/**
 * @brief Returns a pointer to a COPY of i-th object of vector
 * @param v -> vector
 * @param i -> index to access
 * @return pointer to copy
 */
void* copy_access(Vector* v, unsigned int i);

/**
 * @brief Removes last item from vector and returns a copy
 * @param vect -> vector
 * @return pointer to copy of removed object
 */
void* pop(Vector* vect);

/**
 * @brief Removes last item from vector without returning it
 * @param vect -> vector
 */
void pop_noret(Vector* vect);

/**
 * @brief Removes item at index and returns a copy
 * @param vect -> vector
 * @param index -> index to remove from
 * @return pointer to copy of removed object
 */
void* delete(Vector* vect, unsigned int index);

/**
 * @brief Removes item at index
 * @param vect -> vector
 * @param index -> index to remove from
 */
void delete_noret(Vector* vect, unsigned int index);

/**
 * @brief Deletes first occurrence of item of value == obj
 * @param vect -> vector
 * @param obj -> obj to delete
 */
void delete_value(Vector* vect, void* obj);

/**
 * @brief Deletes n occurrences of item of value == obj from vector
 * @param vect -> vector
 * @param obj -> obj to delete
 * @param n -> number of occurrences to delete
 * @return number of deleted occurrences
 */
int delete_n_values(Vector* vect, void* obj, int n);

/**
 * @brief Deletes all occurrences of item of value == obj from vector
 * @param vect -> vector
 * @param obj -> obj to delete
 * @return number of deleted occurrences
 */
int delete_values(Vector* vect, void* obj);

/**
 * @brief Frees entire vector structure
 * @param v -> vector to free
 */
void vector_free(Vector* v);

/**
 * @brief Fills pre-allocated vector (resizes if needed) with nobj objects of val.
 * @param vect -> vector to fill
 * @param val -> value of obj to insert
 * @param nobj -> how many objs to insert
 */
void fill(Vector* vect, void* val, unsigned int nobj);

/**
 * @brief Checks if all objs in vector from at to (at + size) have value val.
 * <br> This function compares MEMORY, so be aware of potential issues.
 * @param vect -> vector to check
 * @param val -> val to compare each entry to
 * @param at -> where to start
 * @param size -> how many objects to check
 * @return 1 if true, 0 if false
 */
int n_matches_from_index(Vector* vect, void* val, int at, unsigned long size);

/**
 * @brief Checks if all objs in vector from 0 to vector->size have value val.
 * <br> This function compares MEMORY, so be aware of potential issues.
 * @param vect -> vector to check
 * @param val -> val to compare each entry to
 * @return 1 if true, 0 if false
 */
int all_match(Vector* vect, void* val);

/**
 * @brief Checks if at least one obj in vector from at to (at + size) has value val.
 * <br> Returns in n the index of occurrence
 * <br> This function compares MEMORY, so be aware of potential issues.
 * @param vect -> vector to check
 * @param val -> val to compare each entry to
 * @param at -> where to start
 * @param size -> how many objects to check
 * @param n -> pointer to int to fill with index of occurrence
 * @return 1 if true, 0 if false
 */
int any_match_from_index(Vector* vect, void* val, int at, unsigned long size, int* n);

/**
 * @brief Checks if at least one obj in vector from 0 to vector->size has value val.
 * <br> Returns in n the index of occurrence
 * <br> This function compares MEMORY, so be aware of potential issues.
 * @param vect -> vector to check
 * @param val -> val to compare each entry to
 * @param n -> pointer to int to fill with index of occurrence
 * @return 1 if true, 0 if false
 */
int any_match(Vector* vect, void* val, int* n);

/**
 * @brief Returns how many objs have value val from at to (at + size) in vector.
 * <br> This function compares MEMORY, so be aware of potential issues.
 * @param vect -> vector to check
 * @param val -> val to compare each entry to
 * @param at -> where to start
 * @param size -> how many objs to check
 * @return times val occurred in vect from at to (at + size)
 */
int count_match_from_index(Vector* vect, void* val, int at, unsigned long size);

/**
 * @brief Returns how many objs have value val from 0 to vect->size in vector.
 * <br> This function compares MEMORY, so be aware of potential issues.
 * @param vect -> vector to check
 * @param val -> val to compare each entry to
 * @return how many times val has occurred in vect
 */
int count_matches(Vector* vect, void* val);

/**
 * @brief Reverses entries of a vector in place.
 * @param vect
 */
void reverse(Vector* vect);

/**
 * @brief Returns pointer to first occurrence of val in vect
 * @param vect -> vector to check
 * @param val -> value to equal each entry to
 * @return pointer to entry that's equal to val
 */
void* extract_match(Vector* vect, void* val);

/**
 * @brief Prints each entry of vect with wanted format.
 * <br> This only works with non-pointer values (e.g. int, double, char...)
 * <br> To print pointer values (such as char*, int*) use print_vect_ptr()
 * <br> Example (assuming vect contains 3 ints of value 1, 2 and 3):
 * <br> print_vect(vect, "[%d] ") will print "[1] [2] [3]"
 * @param v -> vector to print
 * @param format -> format to use
 * @see print_vect_ptr()
 */
void print_vect(Vector* v, char* format);

/**
 * @brief Prints each entry of vect with wanted format.
 * <br> This only works with pointer values (such as char*, int*)
 * <br> To print non-pointer values (such as char, int, double) use print_vect()
 * <br> Example (assuming vect contains 3 char* of value "hello", "world" and "!!!"):
 * <br> print_vect(vect, "[%s] ") will print "[hello] [world] [!!!]"
 * @param v -> vector to print
 * @param format -> format to use
 * @see print_vect()
 */
void print_vect_ptr(Vector* v, char* format);

/**
 * @brief This functions frees each and every vector allocated by any function of this header.
 * <br> This is to facilitate memory management, since you just need to call this function at the end of whatever you want.
 * <br> Please do note that if this was compiled with GCC this function will be called automatically at program exit if possible.
 * <br> So make sure to know what you're doing if you call this manually!
 */
void free_all_arrayutils_structures();

/**
 * @brief Exposes internal list of all currently allocated vectors. Use with caution, as this has no guarantees.
 * <br> If you free any vectors from this, make sure to also modify the .nvectors parameter.
 * @return pointer to internal struct of allocated vectors
 */
AllocatedArrays* expose_internal_arrays();

/**
 * @brief Sets resize factor to use when resizing arrays to make room for more, default is 2.
 * @param factor
 */
void set_resize_factor(unsigned int factor);

/**
 * @brief Sets default size to use when allocating, default is 1.
 * @param size
 */
void set_default_capacity(unsigned int size);

/**
 * @brief Returns currently used resize factor
 * @return resize factor
 */
unsigned int get_resize_factor();

/**
 * @brief Returns currently used default capacity
 * @return default capacity
 */
unsigned int get_default_capacity();

/**
 * @brief Converts an error code to the string representation of said error code
 * @param err -> ArrayUtilsError type
 * @return string repr of given error
 */
const char* errcodetostr_arrayutils(ArrayUtilsErrors err);

/**
 * @brief This function's only purpose is to receive another function to handle any exceptions thrown by this library.
 * <br> Defined exceptions can be found in ArrayUtilsError
 * <br> func's prototype should be a void return with a single int argument
 * <br> In POSIX systems, exceptions triggered by this library will be SIGUSR1, on Windows however, it'll trigger SIGTERM, so be aware.
 * <br> If the trace level is on Warn, it'll also print some information, otherwise it'll be silent and just raise.
 * <br> The trace level can be set with set_trace_lvl_arrayutils()
 * @param func (the function to call)
 * @see ArrayUtilsError
 * @see ArrayUtilsTraceLevel
 * @see set_trace_lvl_arrayutils()
 */
void override_signal_exception_arrayutils(void (*func)(int));

/**
 * @brief This function must be called in order to modify the trace level of the library.
 * <br> By default it's set to NoTrace
 * @param trace_lvl (trace level to set, valid values are only NoTrace or Warn)
 * @see ArrayUtilsTraceLevel
 */
void set_trace_lvl_arrayutils(ArrayUtilsTraceLevel trace_lvl);

#endif //ARRAYUTILS_ARRAYUTILS_H

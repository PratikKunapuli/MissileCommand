///////////////////////////////////////////////////////////////////////
// Doubly Linked List Testbench
//
// This file implements an onboard testbench for the the
// doubly_linked_list module. It includes support for input via
// tracefiles from the SD card or local storage, a primitive heap
// validator to check for memory leaks, and tee-like functionality to
// allow simultaneous output to both the screen and a file.
//
// All code is provided as-is, for the students to use at their own
// risk. This system will not be used for grading, but is intended to
// give the students some framework for testing their code.
// 
// Author: Connor Lawson, GEORGIA INSTITUTE OF TECHNOLOGY, FALL 2016
///////////////////////////////////////////////////////////////////////

#include "testbench.h"

#include "globals.h"
#include "doubly_linked_list.h"

#include <mbed.h>
#include <stdarg.h>
#include <ctype.h>

//////////////////////
// Memory monitoring
//////////////////////
struct heap_info_t
{
    const char* HighestAllocBlock;
    const char* HighestFreeBlock;

    unsigned long TotalAlloc;
    unsigned long TotalFree;
};
static int heap_parse_line(void* pvHeapInfo, char const* pFormatString, ...);
#define GET_HEAP_INFO(info)     memset((void*)&info, 0, sizeof(heap_info_t)); \
                                __heapvalid(heap_parse_line, &info, 1);

#define START_HEAP_TRACE()      heap_info_t start_heap, current_heap; \
                                GET_HEAP_INFO(start_heap);
                                
#define UPDATE_HEAP_TRACE()     GET_HEAP_INFO(current_heap);
#define START_HEAP_ALLOC()      (start_heap.TotalAlloc)
#define CURRENT_HEAP_ALLOC()    (current_heap.TotalAlloc)
#define NET_HEAP_ALLOC()        (CURRENT_HEAP_ALLOC() - START_HEAP_ALLOC())

/** Parses the output of __heapvalid line by line to extract structured data.
 *  This function is the workshorse of the GET_HEAP_INFO macro.
 *  Modified from an example found here:
 *  https://developer.mbed.org/forum/mbed/topic/2702/?page=1#comment-13895
 */
static int heap_parse_line(void* pvHeapInfo, char const* pFormatString, ...)
{
    static const char*  pAllocFormatString = "alloc block %p size %3lx";
    static const char*  pFreeFormatString = "free block  %p size %3lx next=%p";
    static const char*  pCompleteFormatString = "------- heap validation complete";
    heap_info_t*          pHeapInfo = (heap_info_t*)pvHeapInfo;
    va_list             valist;

    va_start(valist, pFormatString);

    if (pFormatString == strstr(pFormatString, pAllocFormatString))
    {
        const char* pBlock = va_arg(valist, const char*);
        unsigned long BlockSize = va_arg(valist, unsigned long);
        const char* pBlockLastByte = pBlock + BlockSize - 1;

        if (pBlockLastByte > pHeapInfo->HighestAllocBlock)
        {
            pHeapInfo->HighestAllocBlock = pBlockLastByte;
        }

        pHeapInfo->TotalAlloc += BlockSize;
    }
    else if (pFormatString == strstr(pFormatString, pFreeFormatString))
    {
        const char* pBlock = va_arg(valist, const char*);
        unsigned long BlockSize = va_arg(valist, unsigned long);
        const char* pBlockLastByte = pBlock + BlockSize - 1;

        if (pBlockLastByte > pHeapInfo->HighestFreeBlock)
        {
            pHeapInfo->HighestFreeBlock = pBlockLastByte;
        }

        pHeapInfo->TotalFree += BlockSize;
    }
    else if (pFormatString == strstr(pFormatString, pCompleteFormatString))
    {
        // Ignoring end of dump string.
    }
    else
    {
        // Unrecognized format string.
        printf("Unrecognized format of %s", pFormatString);
    }

    return 1;
}

static void demo_heap_monitoring() {
    // Make sure stdout is already allocated or the stream buffer will show up
    // in the heap allocations
    printf("Start heap monitoring demo\n");
    START_HEAP_TRACE();
    
    printf("Heap trace at start:\n");
    __heapvalid((__heapprt)fprintf, stdout, 1);
    printf("Total allocation: %d\n", START_HEAP_ALLOC());

    printf("Allocating 10 32-byte buffers...\n");
    void* a[10];
    for(int i = 0; i < 10; i++)
        a[i] = malloc(32*i);

    printf("Heap trace after allocation:\n");
    UPDATE_HEAP_TRACE();
    __heapvalid((__heapprt)fprintf, stdout, 1);
    printf("Measured allocation: %x\r\n", NET_HEAP_ALLOC());

    printf("Free all 32-byte buffers...\n");
    for(int i = 0; i < 10; i++)
        free(a[i]);

    printf("Heap trace after free:\n");
    UPDATE_HEAP_TRACE();
    __heapvalid((__heapprt)fprintf, stdout, 1);
    printf("Measured allocation after free: %x\r\n", NET_HEAP_ALLOC());
}

//////////////////////////////
// Test bench implementation
//////////////////////////////
static void tee(FILE* fp, const char* format, ...)
{
    va_list args1, args2;
    va_start( args1, format );
    va_copy ( args2, args1 );
   
    vprintf( format, args1 );
    if (fp) 
        vfprintf(fp, format, args2 );
   
    va_end( args2 );
    va_end( args1 );
}

static void test_createdestroy(FILE* outfile)
{
    START_HEAP_TRACE();
    int result = 1;
    
    DLinkedList* list = create_dlinkedlist();
    if (list != NULL)
        destroyList(list, 1);
    else
        result = 0;
    
    // Check for memory leak
    UPDATE_HEAP_TRACE();
    result &= (NET_HEAP_ALLOC() == 0);
    tee(outfile, "Create/destroy test (check for memory leak): %s\n", (result)?"SUCCESS":"FAIL");
}

static void test_shouldFree(FILE* outfile)
{
    START_HEAP_TRACE();
    int result = 1;
    
    // Allocate data
    void* data = malloc(2048);
    if (data == NULL)
    {
        tee(outfile, "shouldFree test (check for memory leak): %s\n", "malloc failed!");
        return;
    }
    
    // Create list, add data to list
    DLinkedList* dll = create_dlinkedlist();
    if (dll != NULL) 
    {
        // Add data to list
        insertHead(dll, data);
        
        // Destroy list (shouldFree = 1)
        destroyList(dll, 1);
        
        // Check that the data and list were both deleted
        UPDATE_HEAP_TRACE();
    
        result &= (NET_HEAP_ALLOC() == 0);
        tee(outfile, "shouldFree test (check for memory leak): %s\n", (result)?"SUCCESS":"FAIL");        
    }
    else
    {
        result = 0;
    }

    // Clean up if something went wrong
    if (!result) free(data);
}

static void ignore_whitespace(FILE* fp)
{
    int c = fgetc(fp);
    while (c != '\n' && isspace(c)) c = fgetc(fp);
    ungetc(c, fp);
}

static void ignore_comment(FILE* fp)
{
    // Check if there's a comment here
    int c = fgetc(fp);
    if (c != '%') {
        ungetc(c, fp);
        return;
    }
    // There is a comment!
    // Read until the newline, but don't consume it
    while(c != '\n') c = fgetc(fp);
    ungetc(c, fp);
}

static void skip_inconsequential(FILE* fp)
{
    ignore_whitespace(fp);
    ignore_comment(fp);
}

enum Token {
    NEWLINE = -1,
    INSERT_HEAD = -2,
    INSERT_TAIL = -3,
    INSERT_AFTER = -4,
    INSERT_AFTER_SHOULD_FAIL = -5,
    INSERT_BEFORE = -6,
    INSERT_BEFORE_SHOULD_FAIL = -7,
    DELETE_FORWARD = -8,
    DELETE_BACKWARD = -9,
    GET_HEAD = -10,
    GET_TAIL = -11,
    GET_CURRENT = -12,
    GET_NEXT = -13,
    GET_PREVIOUS = -14,
    GET_SIZE = -15,
    CHECK_LIST = -16,
    CLEAR_LIST = -17,
    END_OF_TRACE = -18,
    PARSE_ERROR = 0xFFFF // Make sure we get a whole 16 bits to use here
};

static Token get_next_token(FILE* fp)
{
    int c, sc, tc;

    skip_inconsequential(fp);
    c = fgetc(fp);
    switch (c)
    {
        case '\n': return NEWLINE;
        case 'i':
            sc = fgetc(fp);
            switch (sc)
            {
                case 'h': return INSERT_HEAD;
                case 't': return INSERT_TAIL;
                case 'a': 
                    tc = fgetc(fp);
                    if (tc == 'f') 
                    {
                        return INSERT_AFTER_SHOULD_FAIL;
                    }
                    else
                    {
                        ungetc(tc, fp);
                        return INSERT_AFTER;
                    }
                case 'b':
                    tc = fgetc(fp);
                    if (tc == 'f') 
                    {
                        return INSERT_BEFORE_SHOULD_FAIL;
                    }
                    else
                    {
                        ungetc(tc, fp);
                        return INSERT_BEFORE;
                    }
                default: return PARSE_ERROR;
            }
        case 'd':
            sc = fgetc(fp);
            switch (sc)
            {
                case 'f': return DELETE_FORWARD;
                case 'b': return DELETE_BACKWARD;
                default: return PARSE_ERROR;
            }
        case 'h': return GET_HEAD;
        case 't': return GET_TAIL;
        case 'c': return GET_CURRENT;
        case 'n': return GET_NEXT;
        case 'p': return GET_PREVIOUS;
        case 's': return GET_SIZE;
        case 'e': return CHECK_LIST;
        case 'r': return CLEAR_LIST;
        case EOF: return END_OF_TRACE;
        default:
            // Check to make sure this is actually a value, not an error...
            if (!isdigit(c)) return PARSE_ERROR;
            
            // Read the value
            int value = 0;
            while (isdigit(c)) {
                // Accumulate an integer
                value *= 10;
                value += (c - '0');

                // Read next char
                c = fgetc(fp);
            }
            // Don't consume the last character -- it wasn't a digit
            ungetc(c, fp);
            return (Token)value;
    }
}

static void parse_trace(FILE* out, FILE* trace) 
{
    // Parser state
    Token current_command = NEWLINE;
    int line = 1;

    // Initialize list for testing
    DLinkedList *dll = create_dlinkedlist();

    // Execute tests
    Token next = get_next_token(trace);
    while (next != END_OF_TRACE)
    {
        if (next == PARSE_ERROR)
        {
            // Error condition, warn and quit
            tee(out, "Parse error on line %d! Aborting.\r\n", line);
            break;
        }
        else if (next == NEWLINE)
        {
            //tee("Newline\n");
            line++;
        }
        else if (next < 0)
        {    
            current_command = next;
                
            // Some commands don't take arguments. Execute immediately
            if (current_command == CLEAR_LIST)
            {
                  tee(out, "--- Clear List ---\n");
                  destroyList(dll, 0);
                  dll = create_dlinkedlist();
            }
        }
        else // Every time we have a value (argument)
        {
            int result = 0;
            switch(current_command)
            {
                case INSERT_HEAD:
                  tee(out, "insertHead(%d)", (int)next);
                  if (next != 0) {
                    insertHead(dll, (void*)next);
                    tee(out, "\n");
                  }
                  else
                  {
                    tee(out, ": Cowardly refusing to insert 0\n");
                  }
                  break;
                case INSERT_TAIL:
                  tee(out, "insertTail(%d)", (int)next);
                  if (next != 0) {
                    insertTail(dll, (void*)next);
                    tee(out, "\n");
                  }
                  else
                  {
                    tee(out, ": Cowardly refusing to insert 0\n");
                  }
                  break;
                case INSERT_AFTER_SHOULD_FAIL: 
                  tee(out, "[SHOULD FAIL] ");
                  // Fallthrough
                case INSERT_AFTER:
                  tee(out, "insertAfter(%d)", (int)next);
                  if (next != 0) {
                    result = insertAfter(dll, (void*)next);
                    tee(out, ": %s\n", (result)?"SUCCESS":"FAIL");
                  }
                  else
                  {
                    tee(out, ": Cowardly refusing to insert 0\n");
                  }
                  break;
                case INSERT_BEFORE_SHOULD_FAIL:
                  tee(out, "[SHOULD FAIL] ");
                  // Fallthrough
                case INSERT_BEFORE:
                  tee(out, "insertBefore(%d)", (int)next);
                  if (next != 0) {
                    result = insertBefore(dll, (void*)next);
                    tee(out, ": %s\n", (result)?"SUCCESS":"FAIL");
                  }
                  else
                  {
                    tee(out, ": Cowardly refusing to insert 0\n");
                  }
                  break;
                case DELETE_FORWARD:
                  result = deleteForward(dll, 0) == (void*)next;
                  tee(out, "deleteForward() == %d: %s\n", (int)next, (result)?"SUCCESS":"FAIL");
                  break;
                case DELETE_BACKWARD:
                  result = deleteBackward(dll, 0) == (void*)next;
                  tee(out, "deleteBackward() == %d: %s\n", (int)next, (result)?"SUCCESS":"FAIL");
                  break;
                case GET_HEAD:
                  result = (getHead(dll) == (void*)next);
                  tee(out, "getHead() == %d: %s\n", (int)next, (result)?"SUCCESS":"FAIL");
                  break;
                case GET_TAIL:
                  result = (getTail(dll) == (void*)next);
                  tee(out, "getTail() == %d: %s\n", (int)next, (result)?"SUCCESS":"FAIL");
                  break;
                case GET_CURRENT:
                  result = (getCurrent(dll) == (void*)next);
                  tee(out, "getCurrent() == %d: %s\n", (int)next, (result)?"SUCCESS":"FAIL");
                  break;
                case GET_NEXT:
                  result = (getNext(dll) == (void*)next);
                  tee(out, "getNext() == %d: %s\n", (int)next, (result)?"SUCCESS":"FAIL");
                  break;
                case GET_PREVIOUS:
                  result = (getPrevious(dll) == (void*)next);
                  tee(out, "getPrevious() == %d: %s\n", (int)next, (result)?"SUCCESS":"FAIL");
                  break;
                case GET_SIZE:
                  result = (getSize(dll) == (int)next);
                  tee(out, "getSize() == %d: %s\n", (int)next, (result)?"SUCCESS":"FAIL");
                  break;
                case CHECK_LIST:
                {
                  tee(out, "Check list: ");
                  int correct = 1;
                  int length = 0;
                  void* val = getHead(dll);
                  while ((int)next > 0) // Only read values
                  {
                    correct &= (val == (void*)next);
                    tee(out, "%d ", (int)val);
                    length++;
                    val = getNext(dll);
                    next = get_next_token(trace);
                  }
                  correct &= (getSize(dll) == length);
                  tee(out, ": %s\n", (correct)?"SUCCESS":"FAIL");
                  
                  // Continue here allows proper parsing of next token at the
                  // top of the loop
                  continue;
                }
            }
        }
        
        // Don't forget to grab the next token!
        next = get_next_token(trace);
    }
    
    tee(out, "End of trace\n");

    // Clean up list
    destroyList(dll, 0);
}

void test_tracefile(FILE* out, const char* tracefile)
{
    tee(out, "Beginning trace of file %s\n", tracefile);
    FILE* trace = fopen(tracefile, "r");
    if (trace == NULL)
    {
        tee(out, "Could not open file! Abort.\n");
        return;
    }
    
    parse_trace(out, trace);
    
    // Close file
    fclose(trace);    
}

void test_dlinkedlist(const char* tracefile, const char* outfile)
{
    //demo_heap_monitoring();
    
    // Open output file
    FILE* out = NULL;
    if (outfile != NULL)
        out = fopen(outfile, "w");
    
    // Check if it worked
    if (out == NULL)
        printf("WARNING: Output to screen only!\n");
    else
        printf("Output will be saved to file: %s\n", outfile);

    // Run the tests
    test_createdestroy(out);
    test_shouldFree(out);
    test_tracefile(out, tracefile);
    
    // Clean up
    if (outfile != NULL)
        fclose(out);
}

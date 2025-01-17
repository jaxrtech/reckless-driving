// http://mirror.informatimago.com/next/developer.apple.com/documentation/Carbon/Reference/Memory_Manager/memory_mgr_ref/FunctionGroupIndex.html

#include <Carbon/Carbon.h>

static OSErr g_memory_error = noErr;

/**
 * Determines if an application’s last direct call to a Memory Manager
 * function executed successfully.
 *
 * @returns  A result code. See “Memory Manager Result Codes”.
 *
 * <p>
 * MemError yields the result code produced by the last Memory Manager function
 * your application called directly.
 * </p>
 * <p>
 * MemError is useful during application debugging. You might also use
 * MemError as one part of a memory-management scheme to identify instances
 * in which the Memory Manager rejects overly large memory requests by
 * returning the error code memFullErr.
 * </p>
 * <p>
 * To view the result codes that MemError can produce, see “Memory Manager
 * Result Codes”.
 * </p>
 * <p>
 * Do not rely on MemError as the only component of a memory-management
 * scheme. For example, suppose you call NewHandle or NewPtr and receive the
 * result code noErr, indicating that the Memory Manager was able to allocate
 * sufficient memory. In this case, you have no guarantee that the
 * allocation did not deplete your application’s memory reserves to levels
 * so low that simple operations might cause your application to crash.
 * Instead of relying on MemError, check before making a memory request that
 * there is enough memory both to fulfill the request and to support
 * essential operations.
 * </p>
 */
OSErr
MemError(void)
{
    return g_memory_error;
}

/**
 * Allocates a nonrelocatable block of memory of a specified size.
 *
 * @param byteCount
 *  The requested size (in bytes) of the nonrelocatable block.
 *  On Mac OS X, if you pass a value of zero, this function returns NULL, and
 *  MemError is set to paramErr. On Mac OS 9 and earlier, if you pass a value
 *  of zero, this function returns a valid zero length pointer.
 *
 * @returns  A pointer to the new block. If NewPtr fails to allocate a block of
 * the requested size, it returns NULL.
 *
 * <p>
 * The NewPtr function attempts to reserve space as low in the heap
 * zone as possible for the new block. If it is able to reserve the requested
 * amount of space, NewPtr allocates the nonrelocatable block in the gap
 * ReserveMem creates. Otherwise, NewPtr returns NULL and generates a memFullErr
 * error. Call the function MemError to get the result code. See “Memory
 * Manager Result Codes”. Because NewPtr allocates memory, you should not
 * call it at interrupt time.
 * </p>
 */
Ptr
NewPtr(Size byteCount)
{
    if (byteCount == 0) {
        g_memory_error = paramErr;
        return NULL;
    }

    void *ptr = malloc(byteCount);
    if (errno == ENOMEM) {
        g_memory_error = memFullErr;
        return NULL;
    }

    return ptr;
}

/**
 * Releases memory occupied by a nonrelocatable block.
 *
 * @param p
 *   A pointer to the nonrelocatable block you want to dispose of.
 *
 * <p>
 * When you no longer need a nonrelocatable block, call the DisposePtr function
 * to free it for other uses.
 * </p>
 * <p>
 * Call the function MemError to get the result code. See “Memory Manager
 * Result Codes”.
 * </p>
 * <p>
 * After a call to DisposePtr, all pointers to the released block become
 * invalid and should not be used again. Any subsequent use of a pointer to the
 * released block might cause a system error.
 * </p>
 * <p>
 * Because DisposePtr purges memory, you should not call it at interrupt time.
 * </p>
 */
void
DisposePtr(Ptr p)
{
    if (p == NULL) { return; }
    free(p);
}

/**
 * Copies a sequence of bytes from one location in memory to another.
 *
 * @param srcPtr
 *  The address of the first byte to copy.
 * @param destPtr
 *  The destination address.
 * @param byteCount
 *   The number of bytes to copy. If the value of byteCount is 0, BlockMove
 *   does nothing.
 *
 * <p>
 * The BlockMove function copies the specified number of bytes from the address
 * designated by srcPtr to that designated by destPtr. It updates no pointers.
 * </p>
 * <p>
 * The BlockMove function works correctly even if the source and destination
 * blocks overlap.
 * </p>
 * <p>
 * You can safely call BlockMove at interrupt time. Even though it moves
 * memory, BlockMove does not move relocatable blocks, but simply copies bytes.
 * </p>
 * <p>
 * Call the function MemError to get the result code. See “Memory Manager
 * Result Codes”.
 * </p>
 * <p>
 * The BlockMove function currently flushes the processor caches whenever it
 * moves more than 12 bytes. This behavior can adversely affect your
 * application’s performance. You might want to avoid calling BlockMove to
 * move small amounts of data in memory if there is no possibility of moving
 * stale data or instructions. For more information about stale data and
 * instructions, see the discussion of the processor caches in the chapter
 * “Memory Management Utilities” in this book.
 * </p>
 */
void
BlockMoveData(
        const void *  srcPtr,
        void *        destPtr,
        Size          byteCount)
{
    memcpy(destPtr, srcPtr, byteCount);
}

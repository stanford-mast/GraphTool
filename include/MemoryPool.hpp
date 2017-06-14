/*****************************************************************************
 * GraphTool
 *   Tools for manipulating graphs.
 *****************************************************************************
 * Authored by Samuel Grossman
 * Department of Electrical Engineering, Stanford University
 * Copyright (c) 2016-2017
 *************************************************************************//**
 * @file MemoryPool.hpp
 *   Implementation of a simple templated memory pool for fast allocation.
 *****************************************************************************/

#pragma once

#include <cstddef>
#include <malloc.h>
#include <new>
#include <mutex>
#include <vector>


namespace GraphTool
{
    /// Implements a simple memory pool for single-typed elements.
    /// Lazily allocates a buffer of the specified capacity.
    /// Supports both allocation and deallocation from the buffer at the granularity of a single element.
    /// @tparam TElement Type of element to hold in the memory pool.
    template <typename TElement> class MemoryPool
    {
    private:
        // -------- INSTANCE VARIABLES ------------------------------------- //

        /// Specifies the size of the memory pool, in number of elements.
        const size_t capacity;
        
        /// Holds the list of freed non-contiguous index positions available for re-allocation.
        std::vector<size_t> freeList;

        /// Controls access for concurrency support.
        std::mutex guard;
        
        /// Points to the memory pool itself.
        TElement* memoryBuffer;

        /// Specifies the next free index to be allocated.
        size_t nextAlloc;

        /// Holds the total number of elements allocated.
        size_t numAllocated;


    public:
        // -------- CONSTRUCTION AND DESTRUCTION --------------------------- //

        /// Initialization constructor.
        /// Sets the capacity of the memory pool in number of elements.
        /// @param [in] capacity Number of elements that shall be contained, at maximum, by the memory pool.
        MemoryPool(const size_t capacity) : capacity(capacity), freeList(), guard(), memoryBuffer(NULL), nextAlloc(0), numAllocated(0)
        {
            // Nothing to do here.
        }

        /// Destructor.
        /// Releases the memory held by the memory pool.
        /// Does not destroy the individual objects held within it.
        ~MemoryPool(void)
        {
            std::lock_guard<std::mutex>(guard);

            
            if (NULL != memoryBuffer)
                free((void*)memoryBuffer);
        }


        // -------- INSTANCE METHODS --------------------------------------- //

        /// Requests that a new element be allocated.
        /// The memory buffer itself is not allocated until the first invocation of this method.
        /// Placement-new operator should be called on the result to construct an object at the newly-allocated memory location.
        /// @return Index of the newly-allocated element, or `SIZE_MAX` on failure.
        size_t Allocate(void)
        {
            std::lock_guard<std::mutex>(guard);

            
            size_t allocatedIndex = SIZE_MAX;
            
            if (!(freeList.empty()))
            {
                allocatedIndex = freeList.back();
                freeList.pop_back();
                numAllocated += 1;
            }
            else if (nextAlloc < capacity)
            {
                if (NULL == memoryBuffer)
                    memoryBuffer = (TElement*)malloc(sizeof(TElement) * capacity);

                if (NULL != memoryBuffer)
                {
                    allocatedIndex = nextAlloc;
                    nextAlloc += 1;
                    numAllocated += 1;
                }
            }
            
            return allocatedIndex;
        }

        /// Requests that an element be deallocated.
        /// Invokes the element's destructor.
        /// Does not verify that the element is currently allocated.
        /// @param [in] index Index of the item to be deallocated.
        void Deallocate(const size_t index)
        {
            if (!(index < nextAlloc))
                return;
            
            std::lock_guard<std::mutex>(guard);


            memoryBuffer[index].~TElement();
            numAllocated -= 1;

            if ((nextAlloc - 1) == index)
                nextAlloc -= 1;
            else
                freeList.push_back(index);
        }

        /// Retrieves the number of elements that this memory pool has allocated.
        /// @return Number of allocated elements.
        inline size_t NumAllocated(void) const
        {
            return numAllocated;
        }

        /// Retrieves the number of elements available to be allocated.
        /// @return Number of free elements.
        inline size_t NumFree(void) const
        {
            return (capacity - numAllocated);
        }
    };
}

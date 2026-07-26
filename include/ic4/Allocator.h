
#ifndef IC4_ALLOCATOR_H_INC_
#define IC4_ALLOCATOR_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_QueueSink.h"

#include "ImageType.h"

#include <memory>

namespace ic4
{
    /**
     * @brief %Interface for custom image buffer allocators
     * 
     * Custom image buffer allocators can be used to use specific application-defined memory regions as image buffers.
     * For example, it could be used to let the library write image data directly into a texture.
     * 
     * BufferAllocator instances can be passed to @ref QueueSink::create(), @ref SnapSink::create() or @ref BufferPool::create()
     * to override the default buffer allocation mechanisms of the sinks or buffer pool.
     */
    struct BufferAllocator
    {
        virtual ~BufferAllocator() = default;

        /**
         * @brief Requests the allocator to allocate a memory buffer.
         * 
         * @param[in] buffer_size   The size of the requested memory buffer
         * @param[in] alignment     Requests memory to be allocated on a specific alignment boundary.\n
		 *                          This value is always a power of 2.
         * @param[out] buffer_ptr   A reference to a pointer that receives the address of the newly-allocated buffer
         * @param[out] user_data    User data to attach to the buffer. The user data is passed to @ref free_buffer when
		 *                          the allocated memory is freed again.
         * 
         * @return @c true on success. If the allocation could not be performed, return @c false.
         */
        virtual bool allocate_buffer(size_t buffer_size, size_t alignment, void*& buffer_ptr, void*& user_data) = 0;

        /**
         * @brief Requests the allocator to free a previously-allocated memory buffer.
         * 
         * @param[in] buffer_ptr    Pointer to the memory buffer to be freed
         * @param[in] user_data     The user data that was returned from @ref allocate_buffer when the memory buffer was allocated
        */
        virtual void free_buffer(void* buffer_ptr, void* user_data) = 0;
    };

    namespace detail
    {
        class AllocatorAdapter
        {
        private:
            std::shared_ptr<BufferAllocator> allocator_shared;
            BufferAllocator& allocator;

            AllocatorAdapter(BufferAllocator& alloc)
                : allocator(alloc)
            {
            }
            AllocatorAdapter(const std::shared_ptr<BufferAllocator>& alloc)
                : allocator_shared(alloc)
                , allocator(*alloc)
            {
            }

            static void release(void* context)
            {
                auto adapter = static_cast<AllocatorAdapter*>(context);

                delete adapter;
            }

            static bool allocate_buffer(void* context, size_t buffer_size, size_t alignment, void** buffer_ptr, void** user_data)
            {
                auto adapter = static_cast<AllocatorAdapter*>(context);

                return adapter->allocator.allocate_buffer(buffer_size, alignment, *buffer_ptr, *user_data);
            }

            static void free_buffer(void* context, void* buffer_ptr, void* user_data)
            {
                auto adapter = static_cast<AllocatorAdapter*>(context);

                adapter->allocator.free_buffer(buffer_ptr, user_data);
            }

        public:
            static c_interface::IC4_ALLOCATOR_CALLBACKS callbacks()
            {
                return
                {
                    AllocatorAdapter::release,
                    AllocatorAdapter::allocate_buffer,
                    AllocatorAdapter::free_buffer
                };
            }

            static AllocatorAdapter* wrap(BufferAllocator& allocator)
            {
                return new AllocatorAdapter(allocator);
            }
            static AllocatorAdapter* wrap(const std::shared_ptr<BufferAllocator>& allocator)
            {
                if (allocator == nullptr)
                    return nullptr;

                return new AllocatorAdapter(allocator);
            }
        };
    }
}


#endif // IC4_ALLOCATOR_H_INC_
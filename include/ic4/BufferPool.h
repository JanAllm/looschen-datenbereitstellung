
#ifndef IC4_BUFFERPOOL_H_INC_
#define IC4_BUFFERPOOL_H_INC_

#pragma once

#define IC4_C_IN_NAMESPACE
#include "C_BufferPool.h"

#include "Error.h"
#include "HandleRef.h"
#include "ImageBuffer.h"
#include "ImageType.h"

#include <memory>

namespace ic4
{
    /**
     * @brief The buffer pool allows allocating additional buffers for use by the program.
     * 
     * Most programs will only use buffers provided by one of the sink types.
     * However, some programs require additional buffers, for example to use as destination for image processing.
     *
     * To create additional buffers, first create a buffer pool using @ref BufferPool::create.
     * Then, use @ref BufferPool::getBuffer to create a new buffer with a specified image type.
     * Allocation options can be specified to customize the image buffer's memory alignment, pitch and total buffer size.
     *
     * Image buffers owned by the program are tracked by instances of @c std::shared_ptr<ImageBuffer>.
     * When the image buffer is no longer required, reset all pointers to it. The image buffer will then be returned to the buffer pool.
     *
     * The buffer pool has configurable caching behavior. By default, the buffer pool will cache one image buffer and return it the next
     * time a matching image buffer is requested.
     *
     * Image buffers created by the buffer pool are still valid after the buffer pool itself has been destroyed.
	 *	 
     * Buffer pool objects are neither copyable nor movable, and are only handled via @c std::shared_ptr<BufferPool>.
     */
    class BufferPool
    {
    private:
        detail::FixedHandleRef<c_interface::IC4_BUFFER_POOL, c_interface::ic4_bufferpool_unref> ptr_;
    public:
        /**
         * @brief Configures the buffer pool's caching behavior.
        */
        struct CacheConfig
        {
            size_t frames_max;  ///< Maximum number of frames to keep in the buffer pool's cache
            size_t bytes_max;   ///< Maximum size of the buffer pool cache in bytes, or @c 0 to not limit by size
        };

        /**
         * @brief Create a new buffer pool, specifying a caching behavior and a custom allocator.
         * @param[in] cache_config  The caching behavior of the new buffer pool
         * @param[in] alloc         The custom allocator to be used by the new buffer pool.\n
         *                          The referenced object must be valid while the buffer pool exists.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         * 
         * @return The new buffer pool, or @c nullptr if an error occurs.
         */
        static std::shared_ptr<BufferPool> create(const CacheConfig& cache_config, BufferAllocator& alloc, Error& err = Error::Default())
        {
            return createInternal(&cache_config, detail::AllocatorAdapter::wrap(alloc), err);
        }
        /**
         * @brief Create a new buffer pool, specifying a caching behavior and a custom allocator.
         * @param[in] cache_config  The caching behavior of the new buffer pool
         * @param[in] alloc         The custom allocator to be used by the new buffer pool.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return The new buffer pool, or @c nullptr if an error occurs.
         */
        static std::shared_ptr<BufferPool> create(const CacheConfig& cache_config, const std::shared_ptr<BufferAllocator>& alloc, Error& err = Error::Default())
        {
            return createInternal(&cache_config, detail::AllocatorAdapter::wrap(alloc), err);
        }
        /**
         * @brief Create a new buffer pool, specifying a custom allocator.
         * @param[in] alloc         The custom allocator to be used by the new buffer pool.\n
         *                          The referenced object must be valid while the buffer pool exists.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return The new buffer pool, or @c nullptr if an error occurs.
         */
        static std::shared_ptr<BufferPool> create(BufferAllocator& alloc, Error& err = Error::Default())
        {
            return createInternal(nullptr, detail::AllocatorAdapter::wrap(alloc), err);
        }
        /**
         * @brief Create a new buffer pool, specifying a custom allocator.
         * @param[in] alloc         The custom allocator to be used by the new buffer pool.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return The new buffer pool, or @c nullptr if an error occurs.
         */
        static std::shared_ptr<BufferPool> create(const std::shared_ptr<BufferAllocator>& alloc, Error& err = Error::Default())
        {
            return createInternal(nullptr, detail::AllocatorAdapter::wrap(alloc), err);
        }
        /**
         * @brief Create a new buffer pool, specifying a caching behavior.
         * @param[in] cache_config  The caching behavior of the new buffer pool
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return The new buffer pool, or @c nullptr if an error occurs.
         */
        static std::shared_ptr<BufferPool> create(const CacheConfig& cache_config, Error& err = Error::Default())
        {
            return createInternal(&cache_config, nullptr, err);
        }
        /**
         * @brief Create a new buffer pool, using default caching behavior and the default allocator.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return The new buffer pool, or @c nullptr if an error occurs.
         */
        static std::shared_ptr<BufferPool> create(Error& err = Error::Default())
        {
            return createInternal(nullptr, nullptr, err);
        }

    public:
        /**
         * @brief Contains options to configure the allocation when requesting an image buffer from a buffer pool.
         *
         * @see BufferPool::getBuffer
         */
        struct AllocationOptions
        {
            /**
             * @brief Specifies the alignment of the address of the buffer's memory.
             *
             * Setting this to 0 lets the buffer pool select an alignment automatically.
             *
             * The alignment must be a power of 2.
             */
            size_t alignment;

            /**
             * @brief Specifies the pitch to use when allocating the buffer.
             *
             * A value of 0 lets the buffer pool select a pitch automatically.
             *
             * Setting a pitch that is smaller than the amount of memory required to store one line of image data will lead to an error.
             */
            ptrdiff_t pitch;

            /**
             * @brief Overrides the automatic buffer size calculation.
             *
             * A value of 0 lets the buffer pool calculate the required buffer size automatically.
             *
             * Setting a size that is smaller than the amount of memory required to store an image of a known format will lead to an error.
             */
            size_t buffer_size;
        };

        /**
         * @brief Request a buffer from the buffer pool.
         * 
         * The buffer is either newly allocated, or retrieved from the buffer pool's buffer cache.
         * 
         * @param[in] image_type    Image type of the requested buffer.
         * @param[in] alloc_options Structure containing advanced allocation options.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return The new image buffer, or @c nullptr if an error occurs.
         */
        std::shared_ptr<ImageBuffer> getBuffer(const ImageType& image_type, const AllocationOptions& alloc_options, Error& err = Error::Default()) const
        {
            c_interface::IC4_IMAGE_BUFFER* buffer = nullptr;
            c_interface::IC4_BUFFERPOOL_ALLOCATION_OPTIONS allocation_options =
            {
                alloc_options.alignment,    // .alignment =
                alloc_options.pitch,        // .pitch = 
                alloc_options.buffer_size   // .buffer_size =
            };

            if (!c_interface::ic4_bufferpool_get_buffer(ptr_, &image_type.data, &allocation_options, &buffer))
            {
                return detail::updateFromLastErrorReturn(err, nullptr);
            }

            return detail::clearReturn(err, detail::buffer_wrap(buffer));
        }

        /**
         * @brief Gets a buffer from the buffer pool.
         *
         * The buffer is either newly allocated, or retrieved from the buffer pool's buffer cache.
         *
         * @param[in] image_type    Image type of the requested buffer.
         * @param[out] err          Reference to an error handler. See @ref technical_article_error_handling for details.
         *
         * @return The new image buffer, or @c nullptr if an error occurs.
         */
        std::shared_ptr<ImageBuffer> getBuffer(const ImageType& image_type, Error& err = Error::Default()) const
        {
            return getBuffer(image_type, {}, err);
        }

    private:
        static std::shared_ptr<BufferPool> createInternal(const CacheConfig* cache_config, detail::AllocatorAdapter* allocatorAdapter = nullptr, Error& err = Error::Default())
        {
            c_interface::IC4_ALLOCATOR_CALLBACKS allocator = {};
            void* allocator_context = nullptr;
            if (allocatorAdapter)
            {
                allocator = detail::AllocatorAdapter::callbacks();
                allocator_context = allocatorAdapter;
            };

            CacheConfig default_cache_config =
            {
                1,  // .frames_max = 
                0   // .bytes_max = 
            };
            auto* use_cache_config = cache_config ? cache_config : &default_cache_config;

            c_interface::IC4_BUFFER_POOL_CONFIG config =
            {
                use_cache_config->frames_max,   // .cache_frames_max =
                use_cache_config->bytes_max,    // .cache_bytes_max = 
                allocator,                      // .allocator = 
                allocator_context               // .allocator_context = 
            };

            c_interface::IC4_BUFFER_POOL* ptr = nullptr;

            if (!c_interface::ic4_bufferpool_create(&ptr, &config))
            {
                return detail::updateFromLastErrorReturn(err, nullptr);
            }

            return detail::clearReturn(err, std::shared_ptr<BufferPool>(new BufferPool(ptr)));
        }

    private:
        BufferPool(c_interface::IC4_BUFFER_POOL* ptr)
            : ptr_(ptr)
        {
        }
    };
}


#endif // IC4_BUFFERPOOL_H_INC_

#ifndef IC4_OBJBASE_H_INC_
#define IC4_OBJBASE_H_INC_

#pragma once

#include <cassert>

namespace ic4
{
	namespace detail
	{
		template<typename THandle, void TUnRef( THandle* )>
		class FixedHandleRef
		{
		private:
			THandle* ptr_ = nullptr;

		public:
			explicit FixedHandleRef( THandle* ptr ) noexcept : ptr_( ptr ) {}
			~FixedHandleRef() { TUnRef( ptr_ ); }

			FixedHandleRef( FixedHandleRef& ) = delete;
			FixedHandleRef& operator=( FixedHandleRef& ) = delete;
			FixedHandleRef( FixedHandleRef&& other ) = delete;
			FixedHandleRef& operator=( FixedHandleRef&& other ) = delete;

		public:
			operator THandle* () const noexcept
			{
				return ptr_;
			}
			operator bool() const = delete;
		};

		template<typename THandle, void TUnRef( THandle* )>
		class MovableHandleRef
		{
		protected:
			THandle* ptr_ = nullptr;

		public:
			MovableHandleRef() = default;
			MovableHandleRef( nullptr_t ) = delete;
			explicit MovableHandleRef( THandle* ptr ) noexcept : ptr_( ptr ) {}
			~MovableHandleRef() { TUnRef( ptr_ ); }

			MovableHandleRef( const MovableHandleRef& ) = delete;
			MovableHandleRef& operator=( const MovableHandleRef& ) = delete;

			MovableHandleRef( MovableHandleRef&& other ) noexcept : ptr_( other.ptr_ ) { other.ptr_ = nullptr; }
			MovableHandleRef& operator=( MovableHandleRef&& other ) noexcept
			{
				if( this == &other )	return *this;

				TUnRef( ptr_ );
				ptr_ = other.ptr_;
				other.ptr_ = nullptr;
				return *this;
			}

		public:
			THandle** operator&() noexcept
			{
				assert( ptr_ == nullptr );
				return &ptr_;
			}
			operator THandle* () const noexcept
			{
				return ptr_;
			}
			operator bool() const = delete;

			bool	operator==( const MovableHandleRef& other ) const noexcept { return ptr_ == other.ptr_; }
			bool	operator!=( const MovableHandleRef& other ) const noexcept { return ptr_ != other.ptr_; }
			bool	operator<( const MovableHandleRef& other ) const noexcept { return ptr_ < other.ptr_; }
			bool	operator==( nullptr_t ) const noexcept { return ptr_ == nullptr; }
			bool	operator!=( nullptr_t ) const noexcept { return ptr_ != nullptr; }
		};

		template<typename THandle, THandle* TRef( THandle* ), void TUnRef( THandle* )>
		class CopyableHandleRef : public MovableHandleRef<THandle, TUnRef>
		{
		public:
			CopyableHandleRef() = default;
			CopyableHandleRef( nullptr_t ) = delete;
			explicit CopyableHandleRef( THandle* ptr ) noexcept
				: MovableHandleRef<THandle, TUnRef>( ptr )
			{
			}

			CopyableHandleRef( const CopyableHandleRef& other ) noexcept
				: MovableHandleRef<THandle, TUnRef>( TRef( other.ptr_ ) )
			{
			}
			CopyableHandleRef& operator=( const CopyableHandleRef& other ) noexcept
			{
				if( this == &other )	return *this;

				TUnRef( this->ptr_ );
				this->ptr_ = TRef( other.ptr_ );
				return *this;
			}
		};
	}
}

#endif // IC4_OBJBASE_H_INC_

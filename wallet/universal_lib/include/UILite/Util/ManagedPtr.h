// Author: Chrome, ChenJianjun
#pragma once


namespace se
{
	class linked_ptr_internal
	{
	public:
		// Create a new circle that includes only this instance.
		void join_new()
		{
			next_ = this;
		}

		// Join an existing circle.
		void join(linked_ptr_internal const* ptr)
		{
			next_ = ptr->next_;
			ptr->next_ = this;
		}

		// Leave whatever circle we're part of.  Returns true iff we were the
		// last member of the circle.  Once this is done, you can join() another.
		bool depart()
		{
			if (next_ == this)
				return true;
			linked_ptr_internal const* p = next_;
			while (p->next_ != this)
				p = p->next_;
			p->next_ = next_;
			return false;
		}

	private:
		mutable linked_ptr_internal const* next_;
	};

	template <typename T>
	class managed_ptr
	{
	public:
		typedef T element_type;
		// Take over ownership of a raw pointer.  This should happen as soon as
		// possible after the object is created.
		explicit managed_ptr(T* ptr = NULL, bool bManaged = false)
		{
			capture(ptr, bManaged);
		}

		// Copy an existing linked_ptr<>, adding ourselves to the list of references.
		template <typename U> managed_ptr(managed_ptr<U> const& ptr)
		{
			copy(&ptr);
		}

		managed_ptr(managed_ptr const& ptr)
		{
			if (&ptr != this)
				copy(&ptr);
		}

		~managed_ptr()
		{
			depart();
		}

		// Assignment releases the old value and acquires the new.
		template <typename U> managed_ptr& operator=(managed_ptr<U> const& ptr)
		{
			depart();
			copy(&ptr);
			return *this;
		}

		managed_ptr& operator=(managed_ptr const& ptr)
		{
			if (&ptr != this)
			{
				depart();
				copy(&ptr);
			}
			return *this;
		}

		// Smart pointer members.
		void reset(T* ptr = NULL, bool bManaged = false)
		{
			depart();
			capture(ptr, bManaged);
		}

		T* get() const
		{
			return value_;
		}
		T* operator->() const
		{
			return value_;
		}
		T& operator*() const
		{
			return *value_;
		}

		operator bool() { return !!value_; }

		// Release ownership of the pointed object and returns it.
		// Sole ownership by this linked_ptr object is required.
		T* release()
		{
			bool last = link_.depart();
			T* v = value_;
			value_ = NULL;
			return v;
		}

		bool operator == (const T* p) const
		{
			return value_ == p;
		} // NOLINT (3.3.2 need)
		
		bool operator != (const T* p) const
		{
			return value_ != p;
		} // NOLINT (3.3.2 need)

		template <typename U>
		bool operator==(managed_ptr<U> const& ptr) const
		{
			return value_ == ptr.get();
		}
		template <typename U>
		bool operator != (managed_ptr<U> const& ptr) const
		{
			return value_ != ptr.get();
		}

	private:
		template <typename U>
		friend class managed_ptr;

		T* value_;
		bool bManaged_;
		linked_ptr_internal link_;

		void depart()
		{
			if (link_.depart() && bManaged_)
				delete value_;
		}

		void capture(T* ptr, bool bManaged)
		{
			value_ = ptr;
			bManaged_ = bManaged && ptr;
			link_.join_new();
		}

		template <typename U> void copy(managed_ptr<U> const* ptr)
		{
			value_ = ptr->get();
			bManaged_ = ptr->bManaged_;
			if (value_)
				link_.join(&ptr->link_);
			else
				link_.join_new();
		}
	};

	template<typename T> inline
		bool operator==(T* ptr, const managed_ptr<T>& x)
	{
		return ptr == x.get();
	}

	template<typename T> inline
		bool operator != (T* ptr, const managed_ptr<T>& x)
	{
		return ptr != x.get();
	}

	// A function to convert T* into managed_ptr<T>
	// Doing e.g. make_linked_ptr(new FooBarBaz<type>(arg)) is a shorter notation
	// for managed_ptr<FooBarBaz<type> >(new FooBarBaz<type>(arg))
	template <typename T>
	managed_ptr<T> make_managed_ptr(T* ptr, bool bManaged)
	{
		return managed_ptr<T>(ptr, bManaged);
	}
}

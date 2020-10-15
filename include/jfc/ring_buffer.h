// © 2020 Joseph Cameron - All Rights Reserved

#ifndef JFC_RING_BUFFER_H
#define JFC_RING_BUFFER_H

#include<array>

namespace jfc
{
	/// \brief fixed length, contiguous, linear collection with no begining or end
	///
	/// to access the internal data, use get to construct a ring_buffer::iterator
	/// to the data.
	/// The iterator wraps around the internal array data instead of going out of bounds.
	template<class value_type_template, size_t size_template>
	class ring_buffer final
	{
	public:
		/// \brief type contained in internal array
		using value_type = value_type_template;
		
	private:
		/// \brief internal value array
		std::array<value_type, size_template> m_data;

	public:
		/// \brief iterator to the value array
		class iterator final
		{
			/// \brief a ring_buffer::iterator can only be constructed by a ring_buffer
			friend ring_buffer;

			/// \brief internal iterator to the data
			decltype(m_data.begin()) m_iterator;

			/// \brief beginning of internal array
			decltype(m_data.begin()) m_begin;

			/// \brief end of internal array
			decltype(m_data.end()) m_end;

		public:
			/// \brief increment iterator
			iterator& operator++(int)
			{
				m_iterator++;

				if (m_iterator == m_end) m_iterator = m_begin;
				
				return *this;
			}

			/// \brief decrement iterator
			iterator& operator--(int)
			{
				if (m_iterator != m_begin) m_iterator--;
				else
				{
					m_iterator = m_end;

					m_iterator--;
				}

				return *this;
			}

			/// \brief dereference value
			value_type& operator*()
			{
				return *m_iterator;
			}

			/// \brief indirect member access to value
			value_type* operator->()
			{
				auto a = *m_iterator;

				return &a;
			}

			/// \brief iterator equality
			bool operator==(const iterator& other) const
			{
				return m_iterator == other.m_iterator;
			}

			/// \brief defaulted dtor
			~iterator() = default;

		private:
			/// \brief iterator constructor
			iterator(decltype(m_data.begin()) begin, const decltype(m_data.end()) end)
				: m_iterator(begin)
				, m_begin(begin)
				, m_end(end)
			{}
		};

		/// \brief const iterator can iterate the data but cannot be used to modify its values
		class const_iterator final
		{
			/// \brief a ring_buffer::iterator can only be constructed by a ring_buffer
			friend ring_buffer;

			/// \brief internal iterator to the data
			decltype(m_data.cbegin()) m_iterator;

			/// \brief beginning of internal array
			decltype(m_data.cbegin()) m_begin;

			/// \brief end of internal array
			decltype(m_data.cend()) m_end;

		public:
			/// \brief increment iterator
			const_iterator& operator++(int)
			{
				m_iterator++;

				if (m_iterator == m_end) m_iterator = m_begin;

				return *this;
			}

			/// \brief decrement iterator
			const_iterator& operator--(int)
			{
				if (m_iterator != m_begin) m_iterator--;
				else
				{
					m_iterator = m_end;

					m_iterator--;
				}

				return *this;
			}

			/// \brief dereference value
			const value_type& operator*()
			{
				return *m_iterator;
			}

			/// \brief indirect member access to value
			const value_type *const operator->()
			{
				auto a = *m_iterator;

				return &a;
			}

			/// \brief iterator equality
			bool operator==(const const_iterator& other) const
			{
				return m_iterator == other.m_iterator;
			}

			/// \brief defaulted dtor
			~const_iterator() = default;

		private:
			/// \brief iterator constructor
			const_iterator(decltype(m_data.cbegin()) begin, const decltype(m_data.cend()) end)//, const decltype(m_data.end()) end)
				: m_iterator(begin)
				, m_begin(begin)
				, m_end(end)
			{}
		};

		/// \brief returns an iterator to the data
		[[nodiscard]] iterator get()
		{
			return iterator(m_data.begin(), m_data.end());
		}

		/// \brief returns a const iterator to the data
		[[nodiscard]] const_iterator cget() const
		{
			return const_iterator(m_data.cbegin(), m_data.cend());
		}

		/// \brief copy semantics
		ring_buffer &operator=(const ring_buffer&) = default;
		/// \brief copy semantics
		ring_buffer(const ring_buffer&) = default;

		/// \brief move semantics
		ring_buffer &operator=(ring_buffer&&) = default;
		/// \brief move semantics
		ring_buffer(ring_buffer&&) = default;

		/// \brief ctor
		ring_buffer(std::array<value_type, size_template> &&data)
			: m_data(data)
		{}

		/// \brief dtor
		~ring_buffer() = default;
	};
}

#endif

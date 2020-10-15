// © 2020 Joseph Cameron - All Rights Reserved

#ifndef JFC_RING_BUFFER_H
#define JFC_RING_BUFFER_H

#include<array>

namespace jfc
{
	template<class value_type_template, size_t size_template>
	class ring_buffer
	{
	public:
		using value_type = value_type_template;

	private:
		std::array<value_type, size_template> m_data;

	public:
		class iterator
		{
			friend ring_buffer;

		public:
			ring_buffer& operator++()
			{
				return this;
			}

			ring_buffer& operator--()
			{
				return this;
			}

			bool operator==(const iterator&) const
			{
				return false;
			}

		private:
			iterator()
			{}
		};

		iterator get();

		ring_buffer &operator=(const ring_buffer&) = default;
		//ring_buffer(&) = default;

		//ring_buffer& operator(&&) = default;
		//ring_buffer(&&) = default;

		ring_buffer() = default;

		~ring_buffer() = default;
	};
}

#endif

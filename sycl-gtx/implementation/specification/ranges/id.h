#pragma once

// TODO: 3.7.1.3 ID class

#include "../../common.h"
#include "../../data_ref.h"
#include <initializer_list>

namespace cl {
namespace sycl {

namespace detail {

// Forward declaration
namespace kernel_ {
	class source;
}

static const string_class id_base_name		= "_sycl_id_";
static const string_class id_base_all_name	= "_sycl_id_d";


template <int dimensions>
struct id_ {
protected:
	friend class kernel_::source;

	size_t values[3];

	id_(size_t first, size_t second, size_t third);

public:
	id_ref operator[](size_t n);

	data_ref operator+(size_t n) const;
	data_ref operator-(size_t n) const;
	data_ref operator*(size_t n) const;
	data_ref operator/(size_t n) const;
	data_ref operator%(size_t n) const;

	friend data_ref operator*(size_t n, id_<dimensions> i) {
		return i * n;
	}

	// Return the value of the specified dimension of the id
	size_t get(int n) const {
		return values[n];
	}
};

} // namespace detail

template <int dimensions = 1>
struct id;

template <>
struct id<1> : detail::id_<1> {
	id(size_t size)
		: detail::id_<1>(size, 1, 1) {}
	id(std::initializer_list<size_t> list)
		: id(*(list.begin())) {}
	id(size_t size[1])
		: id(size[0]) {}
};
template <>
struct id<2> : detail::id_<2>{
	id(size_t sizeX, size_t sizeY)
		: detail::id_<2>(sizeX, sizeY, 1) {}
	id(std::initializer_list<size_t> list)
		: id(*(list.begin()), *(list.begin() + 1)) {}
	id(size_t size[2])
		: id(size[0], size[1]) {}
};
template <>
struct id<3> : detail::id_<3>{
	id(size_t sizeX, size_t sizeY, size_t sizeZ)
		: detail::id_<3>(sizeX, sizeY, sizeZ) {}
	id(std::initializer_list<size_t> list)
		: id(*(list.begin()), *(list.begin() + 1), *(list.begin() + 2)) {}
	id(size_t size[3])
		: id(size[0], size[1], size[2]) {}
};

} // namespace sycl
} // namespace cl

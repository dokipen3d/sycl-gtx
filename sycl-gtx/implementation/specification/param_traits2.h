#pragma once

#include "info.h"
#include "../common.h"
#include <CL/cl.h>

namespace cl {
namespace sycl {

// Forward declaration
template <typename EnumClass, EnumClass Value>
struct param_traits2;

namespace detail {

template <typename EnumClass, EnumClass Value, typename ReturnType, typename CLType>
struct param_traits_helper {
	using type = ReturnType;
	using cl_flag_type = CLType;
};

template <typename Contained_, size_t BufferSize, class Container_ = vector_class<Contained_>>
struct traits_helper {
	using Container = Container_;
	using Contained = Contained_;
	static const int BUFFER_SIZE = BufferSize;
	static const size_t type_size = sizeof(Contained);
};

template <typename Contained_, size_t BufferSize = 1024>
struct traits : traits_helper<Contained_, BufferSize> {};

template <size_t BufferSize>
struct traits<string_class, BufferSize> : traits_helper<char, BufferSize, string_class>{};

template <typename cl_input_t>
using opencl_info_f = cl_int(CL_API_CALL*)(cl_input_t, cl_uint, size_t, void*, size_t*);

template <typename cl_input_t, opencl_info_f<cl_input_t> F>
struct info_function_helper {
	template <class... Args>
	static cl_int get(Args... args) {
		return F(args...);
	}
};

template <typename EnumClass>
struct info_function;
template <>
struct info_function<info::context> : info_function_helper<cl_context, clGetContextInfo>{};
template <>
struct info_function<info::device> : info_function_helper<cl_device_id, clGetDeviceInfo>{};
template <>
struct info_function<info::platform> : info_function_helper<cl_platform_id, clGetPlatformInfo>{};

template <class Contained_, class EnumClass, EnumClass param, size_t BufferSize = traits<Contained_>::BUFFER_SIZE>
struct array_traits : traits<Contained_, BufferSize> {
	using Base = array_traits<Contained_, EnumClass, param, BufferSize>;
	static SYCL_THREAD_LOCAL Contained param_value[BUFFER_SIZE];
	static SYCL_THREAD_LOCAL size_t actual_size;

	template <typename cl_input_t>
	static void get(cl_input_t data_ptr) {
		auto error_code = info_function<EnumClass>::get(
			data_ptr, (param_traits2<EnumClass, param>::cl_flag_type)param, BUFFER_SIZE * type_size, param_value, &actual_size
		);
		error::report(error_code);
	}
};

template <class Contained_, class EnumClass, EnumClass param, size_t BufferSize>
typename traits<Contained_, BufferSize>::Contained array_traits<Contained_, EnumClass, param, BufferSize>::param_value[BufferSize];

template <class Contained_, class EnumClass, EnumClass param, size_t BufferSize>
size_t array_traits<Contained_, EnumClass, param, BufferSize>::actual_size = 0;

} // namespace detail


#define SYCL_ADD_TRAIT(EnumClass, Value, ReturnType, CLType)	\
template <>																\
struct param_traits2<EnumClass, Value>									\
	: detail::param_traits_helper<										\
		EnumClass, Value, ReturnType, CLType							\
	> {};


// 3.3.3.2 Context information descriptors
// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clGetContextInfo.html

#define SYCL_ADD_CONTEXT_TRAIT(Value, ReturnType)	\
	SYCL_ADD_TRAIT(info::context, Value, ReturnType, cl_context_info)

SYCL_ADD_CONTEXT_TRAIT(info::context::reference_count,	cl_uint)
SYCL_ADD_CONTEXT_TRAIT(info::context::num_devices,		cl_uint)
SYCL_ADD_CONTEXT_TRAIT(info::context::devices,			vector_class<cl_device_id>)
SYCL_ADD_CONTEXT_TRAIT(info::context::gl_interop,		info::gl_context_interop)

#undef SYCL_ADD_CONTEXT_TRAIT


// 3.3.2.1 Platform information descriptors
// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clGetPlatformInfo.html

#define SYCL_ADD_PLATFORM_TRAIT(Value)	\
	SYCL_ADD_TRAIT(info::platform, Value, string_class, cl_platform_info)

SYCL_ADD_PLATFORM_TRAIT(info::platform::profile)
SYCL_ADD_PLATFORM_TRAIT(info::platform::version)
SYCL_ADD_PLATFORM_TRAIT(info::platform::name)
SYCL_ADD_PLATFORM_TRAIT(info::platform::vendor)
SYCL_ADD_PLATFORM_TRAIT(info::platform::extensions)

#undef SYCL_ADD_PLATFORM_TRAIT


// 3.3.4.2 Device information descriptors
// https://www.khronos.org/registry/cl/sdk/1.2/docs/man/xhtml/clGetDeviceInfo.html

#define SYCL_ADD_DEVICE_TRAIT(Value, ReturnType)	\
	SYCL_ADD_TRAIT(info::device, Value, ReturnType, cl_device_info)

// Forward declaration
template <int dimensions>
struct id;

SYCL_ADD_DEVICE_TRAIT(info::device::device_type,						info::device_type)
SYCL_ADD_DEVICE_TRAIT(info::device::vendor_id,							cl_uint)

SYCL_ADD_DEVICE_TRAIT(info::device::max_compute_units,					cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::max_work_item_dimensions,			cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::max_work_item_sizes,				id<3>)
SYCL_ADD_DEVICE_TRAIT(info::device::max_work_group_size,				size_t)

SYCL_ADD_DEVICE_TRAIT(info::device::preferred_vector_width_char,		cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::preferred_vector_width_short,		cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::preferred_vector_width_int,			cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::preferred_vector_width_long_long,	cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::preferred_vector_width_float,		cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::preferred_vector_width_double,		cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::preferred_vector_width_half,		cl_uint)

SYCL_ADD_DEVICE_TRAIT(info::device::native_vector_width_char,			cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::native_vector_width_short,			cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::native_vector_width_int,			cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::native_vector_width_long_long,		cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::native_vector_width_float,			cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::native_vector_width_double,			cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::native_vector_width_half,			cl_uint)

SYCL_ADD_DEVICE_TRAIT(info::device::max_clock_frequency,				cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::address_bits,						cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::max_mem_alloc_size,					cl_ulong)

SYCL_ADD_DEVICE_TRAIT(info::device::image_support,						cl_bool)
SYCL_ADD_DEVICE_TRAIT(info::device::max_read_image_args,				cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::max_write_image_args,				cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::image2d_max_height,					size_t)
SYCL_ADD_DEVICE_TRAIT(info::device::image2d_max_width,					size_t)
SYCL_ADD_DEVICE_TRAIT(info::device::image3d_max_height,					size_t)
SYCL_ADD_DEVICE_TRAIT(info::device::image3d_max_width,					size_t)
SYCL_ADD_DEVICE_TRAIT(info::device::image3d_max_depth,					size_t)
SYCL_ADD_DEVICE_TRAIT(info::device::image_max_buffer_size,				size_t)
SYCL_ADD_DEVICE_TRAIT(info::device::image_max_array_size,				size_t)

SYCL_ADD_DEVICE_TRAIT(info::device::max_samplers,						cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::max_parameter_size,					size_t)
SYCL_ADD_DEVICE_TRAIT(info::device::mem_base_addr_align,				cl_uint)

SYCL_ADD_DEVICE_TRAIT(info::device::single_fp_config,					info::device_fp_config)
SYCL_ADD_DEVICE_TRAIT(info::device::double_fp_config,					info::device_fp_config)

SYCL_ADD_DEVICE_TRAIT(info::device::global_mem_cache_type,				info::global_mem_cache_type)
SYCL_ADD_DEVICE_TRAIT(info::device::global_mem_cache_line_size,			cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::global_mem_cache_size,				cl_ulong)
SYCL_ADD_DEVICE_TRAIT(info::device::global_mem_size,					cl_ulong)
SYCL_ADD_DEVICE_TRAIT(info::device::max_constant_buffer_size,			cl_ulong)
SYCL_ADD_DEVICE_TRAIT(info::device::max_constant_args,					cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::local_mem_type,						info::local_mem_type)
SYCL_ADD_DEVICE_TRAIT(info::device::local_mem_size,						cl_ulong)

SYCL_ADD_DEVICE_TRAIT(info::device::error_correction_support,			cl_bool)
SYCL_ADD_DEVICE_TRAIT(info::device::host_unified_memory,				cl_bool)
SYCL_ADD_DEVICE_TRAIT(info::device::profiling_timer_resolution,			size_t)
SYCL_ADD_DEVICE_TRAIT(info::device::endian_little,						cl_bool)
SYCL_ADD_DEVICE_TRAIT(info::device::is_available,						cl_bool)
SYCL_ADD_DEVICE_TRAIT(info::device::is_compiler_available,				cl_bool)
SYCL_ADD_DEVICE_TRAIT(info::device::is_linker_available,				cl_bool)

SYCL_ADD_DEVICE_TRAIT(info::device::execution_capabilities,				info::device_exec_capabilities)
SYCL_ADD_DEVICE_TRAIT(info::device::queue_properties,					info::device_queue_properties)
SYCL_ADD_DEVICE_TRAIT(info::device::built_in_kernels,					string_class)
SYCL_ADD_DEVICE_TRAIT(info::device::platform,							cl_platform_id)
SYCL_ADD_DEVICE_TRAIT(info::device::name,								string_class)
SYCL_ADD_DEVICE_TRAIT(info::device::vendor,								string_class)
SYCL_ADD_DEVICE_TRAIT(info::device::driver_version,						string_class)
SYCL_ADD_DEVICE_TRAIT(info::device::profile,							string_class)
SYCL_ADD_DEVICE_TRAIT(info::device::device_version,						string_class)
SYCL_ADD_DEVICE_TRAIT(info::device::opencl_version,						string_class)
SYCL_ADD_DEVICE_TRAIT(info::device::extensions,							string_class)

SYCL_ADD_DEVICE_TRAIT(info::device::printf_buffer_size,					size_t)
SYCL_ADD_DEVICE_TRAIT(info::device::preferred_interop_user_sync,		cl_bool)
SYCL_ADD_DEVICE_TRAIT(info::device::parent_device,						cl_device_id)
SYCL_ADD_DEVICE_TRAIT(info::device::partition_max_sub_devices,			cl_uint)
SYCL_ADD_DEVICE_TRAIT(info::device::partition_properties,				vector_class<info::device_partition_property>)
SYCL_ADD_DEVICE_TRAIT(info::device::partition_affinity_domain,			info::device_affinity_domain)
SYCL_ADD_DEVICE_TRAIT(info::device::partition_type,						vector_class<info::device_partition_type>)
SYCL_ADD_DEVICE_TRAIT(info::device::reference_count,					cl_uint)

#undef SYCL_ADD_DEVICE_TRAIT


#undef SYCL_ADD_TRAIT

} // namespace sycl
} // namespace cl
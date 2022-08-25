#pragma once
#include <string>
#include <vector>
#include <CL/cl.h>


namespace Ranok::Opencl
{
struct DeviceInfo
{
	cl_platform_id   platform_id{};
	cl_device_id     device_id{};
	cl_context       context{};
	cl_command_queue command_queue{};
	cl_uint          ret_num_devices{};
	cl_uint          ret_num_platforms{};
	cl_int           ret = CL_INVALID_CONTEXT;
	cl_program       program{};
	cl_kernel        kernel{};
	cl_uint          localGroupSize{};
};

struct KernelArguments
{
	struct Argument
	{
		Argument(void* ptr, size_t size, size_t count = 1):
			ptr(ptr),
			size(size),
			count(count) {}
		
		size_t TotalSize() const { return size * count; }
		
		void* const ptr;
		const size_t size;
		const size_t count;
	};
	
	KernelArguments(const Argument& output, const std::vector<Argument>& optional = {}):
		output(output),
		optional(optional) {}
	
	Argument output;
	std::vector<Argument> optional;
};

class Executor
{
public:
	static int Init();
	static void Destroy();

	static std::string GetError(int code);
	
	static int ExecuteCurrentKernel(const std::string& functionName, const KernelArguments& args);
	static int Compile(const std::string& code);
	
	inline static DeviceInfo& GetDeviceInfo();
};
}

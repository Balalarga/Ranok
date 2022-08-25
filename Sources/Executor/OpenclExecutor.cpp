#include "OpenclExecutor.h"

namespace Ranok::Opencl
{
int Executor::Init()
{
	if (GetDeviceInfo().ret == CL_SUCCESS)
		return GetDeviceInfo().ret;
	
	GetDeviceInfo().ret = clGetPlatformIDs(1,
									   &GetDeviceInfo().platform_id,
									   &GetDeviceInfo().ret_num_platforms);
	if(GetDeviceInfo().ret != CL_SUCCESS)
		return GetDeviceInfo().ret;
	
	GetDeviceInfo().ret = clGetDeviceIDs(GetDeviceInfo().platform_id,
									 CL_DEVICE_TYPE_GPU,
									 1,
									 &GetDeviceInfo().device_id,
									 &GetDeviceInfo().ret_num_devices);
	if(GetDeviceInfo().ret != CL_SUCCESS)
		return GetDeviceInfo().ret;
	
	GetDeviceInfo().context = clCreateContext(NULL, 1, &GetDeviceInfo().device_id,
										  NULL,
										  NULL,
										  &GetDeviceInfo().ret);
	GetDeviceInfo().command_queue = clCreateCommandQueueWithProperties(GetDeviceInfo().context,
																   GetDeviceInfo().device_id,
																   0,
																   &GetDeviceInfo().ret);
	return GetDeviceInfo().ret;
}

void Executor::Destroy()
{
	GetDeviceInfo().ret = clFlush(GetDeviceInfo().command_queue);
	GetDeviceInfo().ret = clFinish(GetDeviceInfo().command_queue);
	
	if(GetDeviceInfo().program)
		GetDeviceInfo().ret = clReleaseProgram(GetDeviceInfo().program);
	
	if(GetDeviceInfo().kernel)
		GetDeviceInfo().ret = clReleaseKernel(GetDeviceInfo().kernel);
	
	GetDeviceInfo().ret = clReleaseCommandQueue(GetDeviceInfo().command_queue);
	GetDeviceInfo().ret = clReleaseContext(GetDeviceInfo().context);
}

int Executor::ExecuteCurrentKernel(const std::string& functionName, const KernelArguments& args)
{
	 if (!GetDeviceInfo().program)
        return CL_INVALID_PROGRAM;
	
    GetDeviceInfo().kernel = clCreateKernel(GetDeviceInfo().program,
                                        functionName.c_str(),
                                        &GetDeviceInfo().ret);
    if (!GetDeviceInfo().kernel || GetDeviceInfo().ret != CL_SUCCESS)
        return GetDeviceInfo().ret;
	
    // Create gpu buffers
    cl_mem out_mem_obj = clCreateBuffer(GetDeviceInfo().context,
                                        CL_MEM_WRITE_ONLY,
                                        args.output.TotalSize(),
                                        NULL,
                                        &GetDeviceInfo().ret);
    if (GetDeviceInfo().ret != CL_SUCCESS)
        return GetDeviceInfo().ret;
	
    GetDeviceInfo().ret = clSetKernelArg(GetDeviceInfo().kernel, 0, sizeof(cl_mem), &out_mem_obj);
    if (GetDeviceInfo().ret != CL_SUCCESS)
    {
        clReleaseMemObject(out_mem_obj);
        return GetDeviceInfo().ret;
    }
	
    for (size_t i = 0; i < args.optional.size(); ++i)
    {
        GetDeviceInfo().ret = clSetKernelArg(GetDeviceInfo().kernel,
                                         i + 1,
                                         args.optional[i].TotalSize(),
                                         args.optional[i].ptr);
		
        if (GetDeviceInfo().ret != CL_SUCCESS)
        {
            clReleaseMemObject(out_mem_obj);
            return GetDeviceInfo().ret;
        }
    }
	
    // Get the maximum work group size for executing the kernel on the device
    //
    size_t global;  // global domain size for our calculation
    size_t local;   // local domain size for our calculation
    GetDeviceInfo().ret = clGetKernelWorkGroupInfo(GetDeviceInfo().kernel,
                                               GetDeviceInfo().device_id,
                                               CL_KERNEL_WORK_GROUP_SIZE,
                                               sizeof(local),
                                               &local,
                                               NULL);
    if (GetDeviceInfo().ret != CL_SUCCESS)
    {
        clReleaseMemObject(out_mem_obj);
        return GetDeviceInfo().ret;
    }
	
    // Execute the kernel over the entire range of our 1d input data set
    // using the maximum number of work group items for this device
    global = args.output.count;
    GetDeviceInfo().localGroupSize = static_cast<cl_uint>(local);
	
    GetDeviceInfo().ret = clEnqueueNDRangeKernel(GetDeviceInfo().command_queue,
                                             GetDeviceInfo().kernel,
                                             1,
                                             NULL,
                                             &global,
                                             &local,
                                             0, NULL,
                                             NULL);
    if (GetDeviceInfo().ret != CL_SUCCESS)
    {
        clReleaseMemObject(out_mem_obj);
        return GetDeviceInfo().ret;
    }
	
    clFinish(GetDeviceInfo().command_queue);
    GetDeviceInfo().ret = clEnqueueReadBuffer(GetDeviceInfo().command_queue,
                                          out_mem_obj,
                                          CL_TRUE,
                                          0,
                                          args.output.TotalSize(),
                                          args.output.ptr,
                                          0,
                                          NULL,
                                          NULL);
    if (GetDeviceInfo().ret != CL_SUCCESS)
    {
        clReleaseMemObject(out_mem_obj);
        return GetDeviceInfo().ret;
    }
    GetDeviceInfo().ret = clReleaseMemObject(out_mem_obj);
	
    return GetDeviceInfo().ret;
}

int Executor::Compile(const std::string& code)
{
	if (code.empty())
		return false;
	
	if(GetDeviceInfo().program != 0)
		GetDeviceInfo().ret = clReleaseProgram(GetDeviceInfo().program);
	
	if (GetDeviceInfo().kernel != 0)
		GetDeviceInfo().ret = clReleaseKernel(GetDeviceInfo().kernel);
	
	const char* code_src = code.data();
	GetDeviceInfo().program = clCreateProgramWithSource(GetDeviceInfo().context,
													1,
													&code_src,
													NULL,
													&GetDeviceInfo().ret);
	if (GetDeviceInfo().ret != CL_SUCCESS)
		return GetDeviceInfo().ret;
	
	GetDeviceInfo().ret = clBuildProgram(GetDeviceInfo().program,
									 1,
									 &GetDeviceInfo().device_id,
									 NULL,
									 NULL,
									 NULL);
	if (GetDeviceInfo().ret != CL_SUCCESS)
	{
		// size_t len;
		// char buffer[2048];
		// std::cout << "Failed to build program executable!\n";
		// clGetProgramBuildInfo(GetDeviceInfo().program,
		// 					  GetDeviceInfo().device_id,
		// 					  CL_PROGRAM_BUILD_LOG,
		// 					  sizeof(buffer),
		// 					  buffer,
		// 					  &len);
		// std::cout << buffer << "\n";
		clReleaseProgram(GetDeviceInfo().program);
		GetDeviceInfo().program = 0;
		return GetDeviceInfo().ret;
	}
	return GetDeviceInfo().ret;
}

DeviceInfo& Executor::GetDeviceInfo()
{
	static DeviceInfo info;
	return info;
}
}

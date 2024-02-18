#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <sys/stat.h>

#include <iostream>

#include <asm-generic/ioctl.h>

#include <test/compiler_builtin.h>
#include <test/nv_def.h>

struct nv_parameter_os00
{
    NVHandle    rootHandle;
    NVHandle    parentObjHandle;
    NVHandle    oldObjHandle;
    NVi32       status;
};

struct nv_parameter_os21
{
    NVHandle    rootHandle;
    NVHandle    parentObjHandle;
    NVHandle    newObjHandle;
    NVi32       classHandle;
    NVpointer   pAllocParams    ALIGNED(8);
    NVu32       paramSize;
    NVi32       status;
};

struct nv_parameter_os64
{
    /* TODO */
};

#define sniff_print(S, ...) printf("[Cuda Sniff] " S, __VA_ARGS__)

extern "C"
{

int (*real_open)(const char*, int, mode_t);
#undef open
int open(const char* path,
	int flag,
	mode_t mode
)
{
	int fd = 0;

	if(real_open == nullptr)
		real_open = reinterpret_cast<decltype(real_open)>(dlsym(RTLD_NEXT, "open"));

	fd = real_open(path, flag, mode);
	
	sniff_print("fd=%ld, path=%s, flag=%x, mode=%o\n", fd, path, flag, mode);

	return fd;
}

int (*real_open64)(const char*, int, mode_t);
#undef open64
int open64(
	const char* path,
	int flag,
	mode_t mode
)
{
	int fd = 0;

	if(real_open64 == nullptr)
		real_open64 = reinterpret_cast<decltype(real_open64)>(dlsym(RTLD_NEXT, "open64"));

	fd = real_open64(path, flag, mode);

	sniff_print("on 64 -> fd=%ld, path=%s, flag=%x, mode=%o\n", fd, path, flag, mode);

	return fd;
}

int (*real_ioctl)(int, unsigned long, void*);
#undef ioctl
int ioctl(
	int fd,
	unsigned long request,
	void* argp
)
{
	int ret = 0;

	int dir = 0;
	int type = 0;
	int cmd = 0;
	int size = 0;

	dir = _IOC_DIR(request);
	type = _IOC_TYPE(request);
	cmd = _IOC_NR(request);
	size = _IOC_SIZE(request);

	sniff_print("FD=%d, dir=%ld, type=%ld, cmd=%ld, size=%d, argp=%p\n", fd, dir, type, cmd, size, argp);
	if(real_ioctl == nullptr)
		real_ioctl = reinterpret_cast<decltype(real_ioctl)>(dlsym(RTLD_NEXT, "ioctl"));

	ret = real_ioctl(fd, request, argp);

	switch(cmd)
	{
		case NV_FREE:
		{
			nv_parameter_os00* parameter = reinterpret_cast<decltype(parameter)>(argp);
			std::cout << std::endl;
			std::cout << "+ Free NVOS" << std::endl;
			// Error status 49: Object Not valid
			std::cout << "Root Handle: 0x" << std::hex << parameter->rootHandle << std::endl;
			std::cout << "Parent Object Handle: 0x" << parameter->parentObjHandle << std::endl;
			std::cout << "Old Object Handle: 0x" << parameter->oldObjHandle << std::endl;
			std::cout << "Status: " << std::dec << parameter->status << std::endl;
			std::cout << "- Free NVOS\n\n";
		}
		break;

		case NV_ALLOC:
			if(size == sizeof(nv_parameter_os21))
			{
				nv_parameter_os21* parameter = reinterpret_cast<decltype(parameter)>(argp);

				std::cout << std::endl;
				std::cout << "+ Parameter OS21 Info" << std::endl;
            	std::cout << "Root Handle: 0x" << std::hex <<  parameter->rootHandle << std::endl;
            	std::cout << "Parent Object Handle: 0x" << parameter->parentObjHandle << std::endl;
            	std::cout << "New Object Handle: 0x" << parameter->newObjHandle << std::endl;
            	std::cout << "Class Handle: 0x" << parameter->classHandle << std::endl;
            	std::cout << "Alloc params: " << std::dec << parameter->pAllocParams << std::endl;
            	std::cout << "Parameter Size: " << parameter->paramSize << std::endl;
            	std::cout << "Status: " << parameter->status << std::endl;
            	std::cout << "- Parameter OS21 Info\n\n";
			}
			break;
		default:
			break;
	}

	return ret;
}

ssize_t (*real_write)(int, const void*, size_t);
#undef write
ssize_t write(
	int fd,
	const void* buf,
	size_t bytes
)
{
	char path[256];
	sniff_print("FD=%d, buf=%p, bytes=%ld\n", fd, buf, bytes);
	if(real_write == nullptr)
		real_write = reinterpret_cast<decltype(real_write)>(dlsym(RTLD_NEXT, "write"));

	return real_write(fd, buf, bytes);
}

void* (*real_mmap)(void*, size_t, int, int, int, off_t);
#undef mmap
void* mmap(
	void* addr,
	size_t length,
	int prot,
	int flags,
	int fd,
	off_t offset
)
{
	sniff_print("addr=%p, length=%ld, prot=%d, flags=%d, fd=%d, offset=%ld\n", addr, length, prot, flags, fd, offset);
	if(real_mmap == nullptr)
		real_mmap = reinterpret_cast<decltype(real_mmap)>(dlsym(RTLD_NEXT, "mmap"));

	return real_mmap(addr, length, prot, flags, fd, offset);
}

void* (*real_mmap64)(void*, size_t, int, int, int, off_t);
#undef mmap64
void* mmap64(
	void* addr,
	size_t length,
	int prot,
	int flags,
	int fd,
	off_t offset
)
{
	sniff_print("on 64 -> addr=%p, length=%ld, prot=%d, flags=%d, fd=%d, offset=%ld\n", addr, length, prot, flags, fd, offset);
	if(real_mmap64 == nullptr)
		real_mmap64 = reinterpret_cast<decltype(real_mmap64)>(dlsym(RTLD_NEXT, "mmap64"));

	return real_mmap64(addr, length, prot, flags, fd, offset);
}

}

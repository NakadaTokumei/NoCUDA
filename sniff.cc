#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dlfcn.h>
#include <sys/stat.h>

#include <asm-generic/ioctl.h>

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

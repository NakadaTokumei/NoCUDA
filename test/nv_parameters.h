#ifndef __NV_PARAMETER_H__
#define __NV_PARAMETER_H__

#include <iostream>
#include <type_traits>

#include <sys/ioctl.h>

#include <nv_def.h>
#include <compiler_builtin.h>

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

template<typename T>
class NVOS
{
    int fd;             // File Descriptor
    T parameter;        // Parameters (OS21 or OS64)
public:
    NVOS() = default;
    NVOS(int fd) : fd(fd), parameter({}) {}

    bool Alloc()
    {
        bool status = true;
        int ret = 0;
        NVu32 request = 0;

        request = _IOC(
            NV_DEFAULT_DIR,
            NV_DEFAULT_MAGIC,
            NV_ALLOC,
            sizeof(T)
        );
        ret = ioctl(fd, request, &parameter);
        if(ret < 0)
        {
            status = false;
        }

        if constexpr(std::is_same_v<T, nv_parameter_os21>)
        {
            std::cout << "+ Parameter OS21 Info" << std::endl;
            std::cout << "Root Handle: 0x" << std::hex <<  parameter.rootHandle << std::endl;
            std::cout << "Parent Object Handle: 0x" << parameter.parentObjHandle << std::endl;
            std::cout << "New Object Handle: 0x" << parameter.newObjHandle << std::endl;
            std::cout << "Class Handle: 0x" << parameter.classHandle << std::endl;
            std::cout << "Alloc params: " << std::dec << parameter.pAllocParams << std::endl;
            std::cout << "Parameter Size: " << parameter.paramSize << std::endl;
            std::cout << "Status: " << parameter.status << std::endl;
            std::cout << "- Parameter OS21 Info\n\n";
        }

        return status;
    }

    bool Free()
    {
        bool status = true;
        int ret = 0;
        NVu32 request = 0;

        nv_parameter_os00 freeParam = {0,};

        freeParam.rootHandle = parameter.newObjHandle;
        freeParam.parentObjHandle = parameter.newObjHandle;
        freeParam.oldObjHandle = parameter.newObjHandle;

        request = _IOC(
            NV_DEFAULT_DIR,
            NV_DEFAULT_MAGIC,
            NV_FREE,
            sizeof(nv_parameter_os00)
        );
        ret = ioctl(fd, request, &freeParam);
        if(ret < 0)
        {
            status = false;
        }

        std::cout << "+ Free NVOS" << std::endl;
        // Error status 49: Object Not valid
        std::cout << "Status: " << freeParam.status << std::endl;
        std::cout << "- Free NVOS\n\n";

        return status;
    }
};

#endif
#include <nv.h>

#include <string>
#include <iostream>

#include <sys/ioctl.h>

static void
printAllCardInfo(
    const std::vector<nv_ioctl_card_info>& cardInfoVec
)
{
    NVu32 i = 0;
    for(; i < cardInfoVec.size(); i++)
    {
        if(cardInfoVec[i].valid == 1)
        {
            std::cout << "+ " << i << std::endl;
            std::cout << "Card " << i << " Valid: " << static_cast<bool>(cardInfoVec[i].valid) << std::endl;
            std::cout << "Card " << i << ": 0x" << std::hex << cardInfoVec[i].gpuId << std::dec << std::endl;
            std::cout << "Card " << i << ": 0x" << std::hex << cardInfoVec[i].regAddr << std::dec << std::endl;
            std::cout << "Card " << i << ": 0x" << std::hex << cardInfoVec[i].regSize << std::dec << std::endl;
            std::cout << "Card " << i << ": 0x" << std::hex << cardInfoVec[i].fbAddr << std::dec << std::endl;
            std::cout << "Card " << i << ": 0x" << std::hex << cardInfoVec[i].fbSize << std::dec << std::endl;
            std::cout << "- " << i << "\n\n";
        }
    }
}

void
SelfNV::GetMemBlock(void)
{
    NVi8 data[256] = {0,};
    int fd = 0;
    
    fd = open("/sys/devices/system/memory/block_size_bytes", O_RDONLY);
    read(fd, data, 256);

    sysParam.blockSize = static_cast<NVu64>(std::stoi(data, nullptr, 16));

    std::cout << "System Block Size: " << std::hex << sysParam.blockSize << std::dec << std::endl;

    close(fd);
}

bool
SelfNV::SetNVParams(void)
{
    NVBool status = true;

    int ret = 0;
    int fd = 0;

    fd = open64("/dev/nvidiactl", O_RDWR);
    if(fd < 0)
    {
        std::cerr << "Failed to open /dev/nvidiactl" << std::endl;
        status = false;

        return status;
    }

    request = _IOC(NV_DEFAULT_DIR, NV_DEFAULT_MAGIC, NV_SYS_PARAMS, 8);
    ret = ioctl(fd, request, &sysParam);
    if (ret < 0)
    {
        status = false;
    }

    fdVec.push_back(fd);

    return status;
}

bool
SelfNV::GetCardInfo(void)
{
    NVBool status = true;

    int ret = 0;

    cardInfoVec.resize(24);
    std::cout << "Now Card Info Vector Size is " << cardInfoVec.size() << std::endl;

    request = _IOC(NV_DEFAULT_DIR, NV_DEFAULT_MAGIC, NV_CARD_INFO, sizeof(nv_ioctl_card_info) * cardInfoVec.size());
    ret = ioctl(fdVec[0], request, cardInfoVec.data());
    if(ret < 0)
    {
        status = false;
    }
    else
    {
        printAllCardInfo(cardInfoVec);
    }

    return status;
}


void
SelfNV::CloseAllFD(void)
{
    for(auto fd : fdVec)
    {
        close(fd);
    }
}

void
SelfNV::Start(void)
{
    GetMemBlock();
    if(!SetNVParams())
    {
        std::cerr << "Param Setting Failed... : " << errno << std::endl;
        return;
    }

    if(!GetCardInfo())
    {
        std::cerr << "Failed to get card info... : " << errno << std::endl;
        return;
    }
}
#ifndef __NV_H__
#define __NV_H__

#include <asm-generic/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include <vector>

#include <compiler_builtin.h>
#include <nv_def.h>
#include <nv_parameters.h>

struct nv_ioctl_sys_params
{
	NVu64 blockSize ALIGNED(8);
};

// PCI Info for nvidia gpu
struct nv_pci_info
{
	NVu32 	domain;
	NVu8 	bus;
	NVu8	slot;
	NVu8	function;
	NVu16	vendorId;
	NVu16	deviceId;
};

struct nv_ioctl_card_info
{
	NVBool 		valid;
	nv_pci_info pciInfo;
	NVu32 		gpuId;
	NVu16		interruptLine;
	NVu64		regAddr		ALIGNED(8);
	NVu64		regSize		ALIGNED(8);
	NVu64		fbAddr		ALIGNED(8);
	NVu64		fbSize		ALIGNED(8);
	NVu32		minorNum;
	NVu8		devName[10];
};

class SelfNV
{
	NVu32 request;
	int	 ret;

	nv_ioctl_sys_params sysParam;

	std::vector<NVi32> fdVec;
	std::vector<nv_ioctl_card_info> cardInfoVec;

	void GetMemBlock(void);
	bool SetNVParams(void);
	bool GetCardInfo(void);
	bool ControlTest(void);
	void CloseAllFD(void);
public:
	SelfNV() = default;
	void Start();
};

#endif

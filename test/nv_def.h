#ifndef __NV_DEF_H__
#define __NV_DEF_H__

// NV Default IOCTL DIR and Type
#define NV_DEFAULT_DIR (_IOC_READ | _IOC_WRITE)
#define NV_DEFAULT_MAGIC static_cast<NVu32>('F')

// NV Escape COmmand
#define NV_FREE         41
#define NV_CONTROL      42
#define NV_ALLOC        43

// NV Command 
#define NV_CARD_INFO	200
#define NV_SYS_PARAMS	214

typedef unsigned char		NVu8;
typedef char			    NVi8;

typedef unsigned short		NVu16;
typedef short			    NVi16;

typedef unsigned int		NVu32;
typedef int 			    NVi32;

typedef unsigned long long 	NVu64;
typedef long long		    NVi64;

typedef NVu8                NVBool;

typedef NVu32               NVHandle;

typedef void*               NVpointer;

#endif

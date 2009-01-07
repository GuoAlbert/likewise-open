#include "config.h"
#include "lsmbsys.h"

#include "iodriver.h"
#include "ioinit.h"
#include "iostring.h"

#include "lwlist.h"
#include "goto.h"
#include "smbutils.h"

typedef struct _IOP_DRIVER_CONFIG {
    PSTR pszName;
    PSTR pszPath;
    LW_LIST_LINKS Links;
} IOP_DRIVER_CONFIG, *PIOP_DRIVER_CONFIG;

typedef struct _IOP_CONFIG {
    LW_LIST_LINKS DriverConfigList;
    ULONG DriverCount;
} IOP_CONFIG, *PIOP_CONFIG;

typedef struct _IOP_CONFIG_PARSE_STATE {
    PIOP_CONFIG pConfig;
    PIOP_DRIVER_CONFIG pDriverConfig;
    NTSTATUS Status;
} IOP_CONFIG_PARSE_STATE, *PIOP_CONFIG_PARSE_STATE;

typedef struct _IOP_ROOT_STATE {
    PIOP_CONFIG Config;
    // Diagnostics Only
    ULONG DriverCount;
    LW_LIST_LINKS DriverObjectList;
    // Diagnostics Only
    ULONG DeviceCount;
    // Should really be a hash table...
    LW_LIST_LINKS DeviceObjectList;
} IOP_ROOT_STATE, *PIOP_ROOT_STATE;

typedef ULONG IO_DRIVER_OBJECT_FLAGS;

#define IO_DRIVER_OBJECT_FLAG_INITIALIZED 0x00000001
#define IO_DRIVER_OBJECT_FLAG_READY       0x00000002

struct _IO_DRIVER_OBJECT {
    LONG ReferenceCount;
    IO_DRIVER_OBJECT_FLAGS Flags;
    PIOP_ROOT_STATE Root;
    PIOP_DRIVER_CONFIG Config;
    LW_LIST_LINKS DeviceList;
    ULONG DeviceCount;
    LW_LIST_LINKS DriverObjectsListLinks;

    PVOID LibraryHandle;
    PIO_DRIVER_ENTRY DriverEntry;
    struct {
        PIO_DRIVER_SHUTDOWN_CALLBACK Shutdown;
        PIO_DRIVER_DISPATCH_CALLBACK Dispatch;
    } Callback;
    PVOID Context;
};

struct _IO_DEVICE_OBJECT {
    LONG ReferenceCount;
    PSTR DeviceName;
    PIO_DRIVER_OBJECT Driver;
    LW_LIST_LINKS IrpList;
    PVOID Context;

    // For each list to which this device belongs.
    LW_LIST_LINKS RootLinks;
    LW_LIST_LINKS DriverLinks;
};

VOID
IopConfigFreeConfig(
    IN OUT PIOP_CONFIG* ppConfig
    );

NTSTATUS
IopConfigParse(
    OUT PIOP_CONFIG* ppConfig,
    IN PCSTR pszConfigFilePath
    );

VOID
IopRootFree(
    IN OUT PIOP_ROOT_STATE* ppRoot
    );

NTSTATUS
IopRootCreate(
    OUT PIOP_ROOT_STATE* ppRoot,
    IN PCSTR pszConfigFilePath
    );

PIO_DEVICE_OBJECT
IopRootFindDevice(
    IN PIOP_ROOT_STATE pRoot,
    IN PCSTR pszDeviceName
    );

VOID
IopRootInsertDriver(
    IN PIOP_ROOT_STATE pRoot,
    IN PLW_LIST_LINKS pDriverRootLinks
    );

VOID
IopRootRemoveDriver(
    IN PIOP_ROOT_STATE pRoot,
    IN PLW_LIST_LINKS pDriverRootLinks
    );

VOID
IopRootInsertDevice(
    IN PIOP_ROOT_STATE pRoot,
    IN PLW_LIST_LINKS pDeviceRootLinks
    );

VOID
IopRootRemoveDevice(
    IN PIOP_ROOT_STATE pRoot,
    IN PLW_LIST_LINKS pDeviceRootLinks
    );

VOID
IopDriverUnload(
    IN OUT PIO_DRIVER_OBJECT* ppDriverObject
    );

NTSTATUS
IopDriverLoad(
    OUT PIO_DRIVER_OBJECT* ppDriverObject,
    IN PIOP_ROOT_STATE pRoot,
    IN PIOP_DRIVER_CONFIG pDriverConfig
    );

VOID
IopDriverInsertDevice(
    IN PIO_DRIVER_OBJECT pDriver,
    IN PLW_LIST_LINKS pDeviceDriverLinks
    );

VOID
IopDriverRemoveDevice(
    IN PIO_DRIVER_OBJECT pDriver,
    IN PLW_LIST_LINKS pDeviceDriverLinks
    );

BOOLEAN
IopIsSeparator(
    IN wchar16_t Character
    );
#if 0
{
    return (('/' == Character) || ('\\' == Character)) ? TRUE : FALSE;
}
#endif

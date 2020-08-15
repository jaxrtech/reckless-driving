#include "iShockXForceAPI.h"

void iS2F_SimpleJolt(iS2F_DeviceRef_t handle, iS2F_JoltCmd_t *cmd)
{
    // NOT IMPLEMENTED
}

void iS2F_SimpleDirectControl(iS2F_DeviceRef_t handle, iS2F_MotorCmd_t *cmd)
{
    // NOT IMPLEMENTED
}

void iS2F_Final()
{
    // NOT IMPLEMENTED
}

int iS2F_Init()
{
    // NOT IMPLEMENTED
}

OSErr iS2F_GetDevRefList(iS2F_DeviceRef_t *arr)
{
    if (!arr) {
        return paramErr;
    }

    memset(arr, 0xff, iS2F_MAX_ISHOCK2_NUM * sizeof(iS2F_DeviceRef_t));
    return noErr;
}

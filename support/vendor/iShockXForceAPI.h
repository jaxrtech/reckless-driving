#pragma once
// Stub definitions

#include <Carbon/Carbon.h>

#define iS2F_MAX_ISHOCK2_NUM 1

typedef unsigned int iS2F_DeviceRef_t;

typedef struct {
    float leftMotorMagnitude;
    float rightMotorMagnitude;
} iS2F_MotorCmd_t;

typedef struct {
    iS2F_MotorCmd_t motorCmd;
    float duration;
} iS2F_JoltCmd_t;

void iS2F_SimpleJolt(iS2F_DeviceRef_t handle, iS2F_JoltCmd_t *cmd);
void iS2F_SimpleDirectControl(iS2F_DeviceRef_t handle, iS2F_MotorCmd_t *cmd);
void iS2F_Final();
int iS2F_Init();
OSErr iS2F_GetDevRefList(iS2F_DeviceRef_t *arr);

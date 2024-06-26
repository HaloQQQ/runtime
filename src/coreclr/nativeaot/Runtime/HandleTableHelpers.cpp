// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

//
// Helper functions that are p/invoked from redhawkm in order to expose handle table functionality to managed
// code. These p/invokes are special in that the handle table code requires we remain in co-operative mode
// (since these routines mutate the handle tables which are also accessed during garbage collections). The
// binder has special knowledge of these methods and doesn't generate the normal code to transition out of the
// runtime prior to the call.
//
#include "common.h"
#include "gcenv.h"
#include "objecthandle.h"
#include "RestrictedCallouts.h"
#include "gchandleutilities.h"


FCIMPL2(OBJECTHANDLE, RhpHandleAlloc, Object *pObject, int type)
{
    return GCHandleUtilities::GetGCHandleManager()->GetGlobalHandleStore()->CreateHandleOfType(pObject, (HandleType)type);
}
FCIMPLEND

FCIMPL2(OBJECTHANDLE, RhpHandleAllocDependent, Object *pPrimary, Object *pSecondary)
{
    return GCHandleUtilities::GetGCHandleManager()->GetGlobalHandleStore()->CreateDependentHandle(pPrimary, pSecondary);
}
FCIMPLEND

FCIMPL1(void, RhHandleFree, OBJECTHANDLE handle)
{
    GCHandleUtilities::GetGCHandleManager()->DestroyHandleOfUnknownType(handle);
}
FCIMPLEND

FCIMPL1(Object *, RhHandleGet, OBJECTHANDLE handle)
{
    return ObjectFromHandle(handle);
}
FCIMPLEND

FCIMPL2(Object *, RhHandleGetDependent, OBJECTHANDLE handle, Object **ppSecondary)
{
    Object *pPrimary = ObjectFromHandle(handle);
    *ppSecondary = (pPrimary != NULL) ? GetDependentHandleSecondary(handle) : NULL;
    return pPrimary;
}
FCIMPLEND

FCIMPL2(void, RhHandleSetDependentSecondary, OBJECTHANDLE handle, Object *pSecondary)
{
    SetDependentHandleSecondary(handle, pSecondary);
}
FCIMPLEND

FCIMPL2(void, RhHandleSet, OBJECTHANDLE handle, Object *pObject)
{
    GCHandleUtilities::GetGCHandleManager()->StoreObjectInHandle(handle, pObject);
}
FCIMPLEND

FCIMPL2(FC_BOOL_RET, RhRegisterRefCountedHandleCallback, void * pCallout, MethodTable * pTypeFilter)
{
    FC_RETURN_BOOL(RestrictedCallouts::RegisterRefCountedHandleCallback(pCallout, pTypeFilter));
}
FCIMPLEND

FCIMPL2(void, RhUnregisterRefCountedHandleCallback, void * pCallout, MethodTable * pTypeFilter)
{
    RestrictedCallouts::UnregisterRefCountedHandleCallback(pCallout, pTypeFilter);
}
FCIMPLEND

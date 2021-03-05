#pragma once

#ifndef __SLASSERT_H__
#define __SLASSERT_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SLEXCEPTION_MALLOC_FAILED         1114
#define SLEXCEPTION_NULLPOINTER_REFERENCE 1115 

#define slAssert(obj, exceptionCode, ret) do { if (!(obj)) { printf("Assert: " #obj "\nException Details: " #exceptionCode " -- %d\n", (exceptionCode)); return (ret); }} while(0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SLASSERT_H__ */
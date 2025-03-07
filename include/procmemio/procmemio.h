#ifndef PROCMEMIO_H
#define PROCMEMIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define PROCMEMIO_SUCCESS         0
#define PROCMEMIO_ERROR_ATTACH    1
#define PROCMEMIO_ERROR_DETACH    2
#define PROCMEMIO_ERROR_READ      3
#define PROCMEMIO_ERROR_WRITE     4
#define PROCMEMIO_ERROR_READ_REG  5
#define PROCMEMIO_ERROR_WRITE_REG 6
#define PROCMEMIO_ERROR_CONTINUE  7

typedef unsigned char PROCMEMIO_STATUS;

typedef struct
{
	uintptr_t rax;
	uintptr_t rcx;
	uintptr_t rdx;
	uintptr_t rbx;
	uintptr_t rsp;
	uintptr_t rbp;
	uintptr_t rsi;
	uintptr_t rdi;
	uintptr_t rip;
} PROCMEMIO_REGS;

typedef void (*WriteRegistersCallback)(PROCMEMIO_REGS *regs);

const PROCMEMIO_STATUS procmemio_read(const unsigned int pid, const void *addr, const void *buffer, const size_t size);
const PROCMEMIO_STATUS procmemio_write(const unsigned int pid, const void *addr, const void *buffer, const size_t size);

const PROCMEMIO_STATUS procmemio_readRegisters(const unsigned int pid, const void *addr, PROCMEMIO_REGS *regs);
const PROCMEMIO_STATUS procmemio_writeRegisters(const unsigned int pid, const void *addr, WriteRegistersCallback writeRegisters);


#ifdef __cplusplus
}
#endif

#endif

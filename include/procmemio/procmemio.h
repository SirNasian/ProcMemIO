#ifndef PROCMEMIO_H
#define PROCMEMIO_H

#ifdef __cplusplus
extern "C" {
#endif

#define PROCMEMIO_SUCCESS          0
#define PROCMEMIO_ERROR_ATTACH     1
#define PROCMEMIO_ERROR_DETACH     2
#define PROCMEMIO_ERROR_READ_ADDR  3
#define PROCMEMIO_ERROR_WRITE_ADDR 4
#define PROCMEMIO_ERROR_READ_REG   5
#define PROCMEMIO_ERROR_WRITE_REG  6

typedef unsigned char PROCMEMIO_STATUS;

typedef struct
{
	unsigned long long int rax;
	unsigned long long int rcx;
	unsigned long long int rdx;
	unsigned long long int rbx;
	unsigned long long int rsp;
	unsigned long long int rbp;
	unsigned long long int rsi;
	unsigned long long int rdi;
	unsigned long long int rip;
} PROCMEMIO_REGS;

const PROCMEMIO_STATUS procmemio_read(const unsigned int pid, const void* addr, const void* buffer, const unsigned int size);
const PROCMEMIO_STATUS procmemio_write(const unsigned int pid, const void* addr, const void* buffer, const unsigned int size);

const PROCMEMIO_STATUS procmemio_readRegisters(const unsigned int pid, const void* addr, PROCMEMIO_REGS* regs);
const PROCMEMIO_STATUS procmemio_writeRegisters(const unsigned int pid, const void* addr, PROCMEMIO_REGS* regs);


#ifdef __cplusplus
}
#endif

#endif

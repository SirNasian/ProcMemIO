#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <errno.h>

#include "procmemio/procmemio.h"

const PROCMEMIO_STATUS procmemio_read(const unsigned int pid, const void* addr, const void* buffer, const unsigned int size)
{
	int status;
	if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1)
		return PROCMEMIO_ERROR_ATTACH;

	waitpid(pid, &status, 0);

	unsigned long long int data;
	unsigned char* p_data = (unsigned char*)(&data);
	unsigned char* p_buffer = (unsigned char*)(buffer);
	for (unsigned int offset = 0; offset < size; offset += 8)
	{
		errno = 0;
		data = ptrace(PTRACE_PEEKDATA, pid, addr+offset, 0);
		if ((data == -1) && (errno != 0))
			return PROCMEMIO_ERROR_READ_ADDR;

		for (unsigned char i = 0; ((i < 8) && (offset+i < size)); i++)
			p_buffer[offset+i] = p_data[i];
	}

	return (ptrace(PTRACE_DETACH, pid, 0, 0) == -1) ? PROCMEMIO_ERROR_DETACH : PROCMEMIO_SUCCESS;
}

const PROCMEMIO_STATUS procmemio_write(const unsigned int pid, const void* addr, const void* buffer, const unsigned int size)
{
	int status;
	if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1)
		return PROCMEMIO_ERROR_ATTACH;

	waitpid(pid, &status, 0);

	unsigned long long int data;
	unsigned char* p_data = (unsigned char*)(&data);
	unsigned char* p_buffer = (unsigned char*)(buffer);
	for (unsigned int offset = 0; offset < size; offset += 8)
	{
		if (offset+8 > size)
		{
			errno = 0;
			data = ptrace(PTRACE_PEEKDATA, pid, addr+offset, 0);
			if ((data == -1) && (errno != 0))
				return PROCMEMIO_ERROR_READ_ADDR;
		}

		for (unsigned char i = 0; ((i < 8) && (offset+i < size)); i++)
			p_data[i] = p_buffer[offset+i];

		errno = 0;
		if ((ptrace(PTRACE_POKEDATA, pid, addr+offset, data) == -1) && (errno != 0))
			return PROCMEMIO_ERROR_WRITE_ADDR;
	}

	return (ptrace(PTRACE_DETACH, pid, 0, 0) == -1) ? PROCMEMIO_ERROR_DETACH : PROCMEMIO_SUCCESS;
}

const PROCMEMIO_STATUS procmemio_readRegisters(const unsigned int pid, const void* addr, PROCMEMIO_REGS* regs)
{
	long long data_original, data_breakpoint;
	unsigned char* ptr;

	if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) return PROCMEMIO_ERROR_ATTACH;
	waitpid(pid, (int*)(&data_original), 0);

	errno = 0;
	data_original = ptrace(PTRACE_PEEKDATA, pid, addr, 0);
	if ((data_original == -1) && (errno == -1)) return PROCMEMIO_ERROR_READ_ADDR;

	data_breakpoint = data_original;
	ptr = (unsigned char*)(&data_breakpoint);
	ptr[0] = 0xCC;
	if (ptrace(PTRACE_POKEDATA, pid, addr, data_breakpoint) == -1) return PROCMEMIO_ERROR_WRITE_ADDR;

	if (ptrace(PTRACE_CONT, pid, 0, 0) == -1) return PROCMEMIO_ERROR_CONTINUE;
	waitpid(pid, (int*)(&data_breakpoint), 0);

	struct user_regs_struct _regs;
	if (ptrace(PTRACE_GETREGS, pid, 0, &_regs) == -1) return PROCMEMIO_ERROR_READ_REG;
	(*regs).rax = _regs.rax;
	(*regs).rcx = _regs.rcx;
	(*regs).rdx = _regs.rdx;
	(*regs).rbx = _regs.rbx;
	(*regs).rsp = _regs.rsp;
	(*regs).rbp = _regs.rbp;
	(*regs).rsi = _regs.rsi;
	(*regs).rdi = _regs.rdi;
	(*regs).rip = --(_regs.rip);

	if (ptrace(PTRACE_POKEDATA, pid, addr, data_original) == -1) return PROCMEMIO_ERROR_WRITE_ADDR;
	if (ptrace(PTRACE_SETREGS, pid, addr, &_regs) == -1) return PROCMEMIO_ERROR_WRITE_REG;
	return (ptrace(PTRACE_DETACH, pid, 0, 0) == -1) ? PROCMEMIO_ERROR_DETACH : PROCMEMIO_SUCCESS;
}

const PROCMEMIO_STATUS procmemio_writeRegisters(const unsigned int pid, const void* addr, WriteRegistersCallback writeRegisters)
{
	long long data_original, data_breakpoint;
	unsigned char* ptr;

	if (ptrace(PTRACE_ATTACH, pid, 0, 0) == -1) return PROCMEMIO_ERROR_ATTACH;
	waitpid(pid, (int*)(&data_original), 0);

	errno = 0;
	data_original = ptrace(PTRACE_PEEKDATA, pid, addr, 0);
	if ((data_original == -1) && (errno == -1)) return PROCMEMIO_ERROR_READ_ADDR;

	data_breakpoint = data_original;
	ptr = (unsigned char*)(&data_breakpoint);
	ptr[0] = 0xCC;
	if (ptrace(PTRACE_POKEDATA, pid, addr, data_breakpoint) == -1) return PROCMEMIO_ERROR_WRITE_ADDR;

	if (ptrace(PTRACE_CONT, pid, 0, 0) == -1) return PROCMEMIO_ERROR_CONTINUE;
	waitpid(pid, (int*)(&data_breakpoint), 0);

	PROCMEMIO_REGS regs;
	struct user_regs_struct _regs;
	if (ptrace(PTRACE_GETREGS, pid, 0, &_regs) == -1) return PROCMEMIO_ERROR_READ_REG;

	regs.rax = _regs.rax;
	regs.rcx = _regs.rcx;
	regs.rdx = _regs.rdx;
	regs.rbx = _regs.rbx;
	regs.rsp = _regs.rsp;
	regs.rbp = _regs.rbp;
	regs.rsi = _regs.rsi;
	regs.rdi = _regs.rdi;
	regs.rip = --(_regs.rip);

	writeRegisters(&regs);

	_regs.rax = regs.rax;
	_regs.rcx = regs.rcx;
	_regs.rdx = regs.rdx;
	_regs.rbx = regs.rbx;
	_regs.rsp = regs.rsp;
	_regs.rbp = regs.rbp;
	_regs.rsi = regs.rsi;
	_regs.rdi = regs.rdi;
	_regs.rip = regs.rip;

	if (ptrace(PTRACE_POKEDATA, pid, addr, data_original) == -1) return PROCMEMIO_ERROR_WRITE_ADDR;
	if (ptrace(PTRACE_SETREGS, pid, addr, &_regs) == -1) return PROCMEMIO_ERROR_WRITE_REG;
	return (ptrace(PTRACE_DETACH, pid, 0, 0) == -1) ? PROCMEMIO_ERROR_DETACH : PROCMEMIO_SUCCESS;
}

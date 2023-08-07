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

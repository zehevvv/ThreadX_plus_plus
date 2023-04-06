#include "MemoryManager.h"
#include "tx_api.h"
#include "HW.h"
#include <stdio.h>

#ifdef MEMORY_MANAGER_ENABLE

static TX_BYTE_POOL m_my_pool;

/// @brief Create pool of memory bytes.
///
/// @param first_unused_memory	- The start of the memory pool
void Memory_manager(VOID *first_unused_memory)
{
	UINT status;

	ULONG pool_size = MEMORY_MANAGER_POOL_SIZE;
	status = tx_byte_pool_create(&m_my_pool, (char*)"Main_heap_pool",
	    first_unused_memory, pool_size);

	if ( status != TX_SUCCESS )
	{
		printf("Error: failed to init memory, error - %d \n", status);
		while(1);
	}
}

/// @brief override of the new function
void * operator new(size_t size)
{
	void* memory_ptr;
	UINT status = tx_byte_allocate(&m_my_pool, (VOID **) &memory_ptr,size, TX_NO_WAIT);
	if ( status != TX_SUCCESS )
	{
		printf("Error: failed to allocate memory, error - %d \n", status);
		while(1);
	}

    return memory_ptr;
}

/// @brief override of the new function
void * operator new[](size_t size)
{
	void* memory_ptr;
	UINT status = tx_byte_allocate(&m_my_pool, (VOID **) &memory_ptr,size, TX_NO_WAIT);
	if ( status != TX_SUCCESS )
	{
		printf("Error: failed to allocate memory, error - %d \n", status);
		while(1);
	}

    return memory_ptr;
}

/// @brief override of the delete function
void operator delete(void * memory_ptr)
{
	UINT status = tx_byte_release((VOID *) memory_ptr);
	if ( status != TX_SUCCESS )
	{
		printf("Error: failed to release memory, error - %d \n", status);
		while(1);
	}
}

/// @brief override of the delete function
void operator delete[](void * memory_ptr)
{
	UINT status = tx_byte_release((VOID *) memory_ptr);
	if ( status != TX_SUCCESS )
	{
		printf("Error: failed to release memory, error - %d \n", status);
		while(1);
	}
}

#endif

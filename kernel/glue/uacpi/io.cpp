#include <uacpi_libc.h>
#include <uacpi/status.h>
#include <uacpi/kernel_api.h>

#include <libs/mmio.hpp>
#include <arch.hpp>

uacpi_status uacpi_kernel_raw_io_read(uacpi_io_addr address, uacpi_u8 byte_width,
									  uacpi_u64* out_value)
{
	const uint16_t port = address;

	switch(byte_width)
	{
		case 1:
			*out_value = arch::inp<uint8_t>(port);
			break;
		case 2:
			*out_value = arch::inp<uint16_t>(port);
			break;
		case 4:
			*out_value = arch::inp<uint32_t>(port);
			break;
		default:
			return UACPI_STATUS_INVALID_ARGUMENT;
	}

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_raw_io_write(uacpi_io_addr address, uacpi_u8 byte_width,
									   uacpi_u64 in_value)
{
	const uint16_t port = address;

	switch(byte_width)
	{
		case 1:
			arch::outp<uint8_t>(port, in_value);
			break;
		case 2:
			arch::outp<uint16_t>(port, in_value);
			break;
		case 4:
			arch::outp<uint32_t>(port, in_value);
			break;
		default:
			return UACPI_STATUS_INVALID_ARGUMENT;
	}

	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_raw_memory_read(uacpi_phys_addr address, uacpi_u8 byte_width,
										  uacpi_u64* out_value)
{
	void* ptr = uacpi_kernel_map(address, byte_width);

	switch(byte_width)
	{
		case 1:
			*out_value = mmio_in<uint8_t>(ptr);
			break;
		case 2:
			*out_value = mmio_in<uint16_t>(ptr);
			break;
		case 4:
			*out_value = mmio_in<uint32_t>(ptr);
			break;
		case 8:
			*out_value = mmio_in<uint64_t>(ptr);
			break;
		default:
			uacpi_kernel_unmap(ptr, byte_width);
			return UACPI_STATUS_INVALID_ARGUMENT;
	}

	uacpi_kernel_unmap(ptr, byte_width);
	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_raw_memory_write(uacpi_phys_addr address, uacpi_u8 byte_width,
										   uacpi_u64 in_value)
{
	void* ptr = uacpi_kernel_map(address, byte_width);

	switch(byte_width)
	{
		case 1:
			mmio_out<uint8_t>(ptr, in_value);
			break;
		case 2:
			mmio_out<uint16_t>(ptr, in_value);
			break;
		case 4:
			mmio_out<uint32_t>(ptr, in_value);
			break;
		case 8:
			mmio_out<uint64_t>(ptr, in_value);
			break;
		default:
			uacpi_kernel_unmap(ptr, byte_width);
			return UACPI_STATUS_INVALID_ARGUMENT;
	}

	uacpi_kernel_unmap(ptr, byte_width);
	return UACPI_STATUS_OK;
}

uacpi_status uacpi_kernel_io_read(uacpi_handle handle, uacpi_size offset, uacpi_u8 byte_width,
								  uacpi_u64* value)
{
	const uacpi_io_addr addr = reinterpret_cast<uacpi_io_addr>(handle);
	return uacpi_kernel_raw_io_read(addr + offset, byte_width, value);
}

uacpi_status uacpi_kernel_io_write(uacpi_handle handle, uacpi_size offset, uacpi_u8 byte_width,
								   uacpi_u64 value)
{
	const uacpi_io_addr addr = reinterpret_cast<uacpi_io_addr>(handle);
	return uacpi_kernel_raw_io_write(addr + offset, byte_width, value);
}

uacpi_status uacpi_kernel_pci_read(uacpi_pci_address* address, uacpi_size offset,
								   uacpi_u8 byte_width, uacpi_u64* value)
{
	return UACPI_STATUS_UNIMPLEMENTED;
}

uacpi_status uacpi_kernel_pci_write(uacpi_pci_address* address, uacpi_size offset,
									uacpi_u8 byte_width, uacpi_u64 value)
{
	return UACPI_STATUS_UNIMPLEMENTED;
}
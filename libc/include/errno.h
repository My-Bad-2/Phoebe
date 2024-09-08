#ifndef _SYS_ERRNO_H
#define _SYS_ERRNO_H 1

// Indicates an operation was successful.
#define SYSTEM_OK (0)

// The system encountered an otherwise unspecified error while performing the
// operation.
#define SYSTEM_ERR_INTERNAL (-1)

// The operation is not implemented, supported, or enabled.
#define SYSTEM_ERR_NOT_SUPPORTED (-2)

// The system was not able to allocate some resource needed for the operation.
#define SYSTEM_ERR_NO_RESOURCES (-3)

// The system was not able to allocate memory needed for the operation.
#define SYSTEM_ERR_NO_MEMORY (-4)

// The system call was interrupted, but should be retried. This should not be
// seen outside of the VDSO.
#define SYSTEM_ERR_INTERNAL_INTR_RETRY (-6)

// An argument is invalid. For example, a null pointer when a null pointer is
// not permitted.
#define SYSTEM_ERR_INVALID_ARGS (-10)

// A specified handle value does not refer to a handle.
#define SYSTEM_ERR_BAD_HANDLE (-11)

// The subject of the operation is the wrong type to perform the operation.
//
// For example: Attempting a message_read on a thread handle.
#define SYSTEM_ERR_WRONG_TYPE (-12)

// The specified syscall number is invalid.
#define SYSTEM_ERR_BAD_SYSCALL (-13)

// An argument is outside the valid range for this operation.
#define SYSTEM_ERR_OUT_OF_RANGE (-14)

// The caller-provided buffer is too small for this operation.
#define SYSTEM_ERR_BUFFER_TOO_SMALL (-15)

// The operation failed because the current state of the object does not allow
// it, or a precondition of the operation is not satisfied.
#define SYSTEM_ERR_BAD_STATE (-20)

// The time limit for the operation elapsed before the operation completed.
#define SYSTEM_ERR_TIMED_OUT (-21)

// The operation cannot be performed currently but potentially could succeed if
// the caller waits for a prerequisite to be satisfied, like waiting for a
// handle to be readable or writable.
//
// Example: Attempting to read from a channel that has no messages waiting but
// has an open remote will return `SYSTEM_ERR_SHOULD_WAIT`. In contrast, attempting
// to read from a channel that has no messages waiting and has a closed remote
// end will return `SYSTEM_ERR_PEER_CLOSED`.
#define SYSTEM_ERR_SHOULD_WAIT (-22)

// The in-progress operation, for example, a wait, has been canceled.
#define SYSTEM_ERR_CANCELED (-23)

// The operation failed because the remote end of the subject of the operation
// was closed.
#define SYSTEM_ERR_PEER_CLOSED (-24)

// The requested entity is not found.
#define SYSTEM_ERR_NOT_FOUND (-25)

// An object with the specified identifier already exists.
//
// Example: Attempting to create a file when a file already exists with that
// name.
#define SYSTEM_ERR_ALREADY_EXISTS (-26)

// The operation failed because the named entity is already owned or controlled
// by another entity. The operation could succeed later if the current owner
// releases the entity.
#define SYSTEM_ERR_ALREADY_BOUND (-27)

// The subject of the operation is currently unable to perform the operation.
//
// This is used when there's no direct way for the caller to observe when the
// subject will be able to perform the operation and should thus retry.
#define SYSTEM_ERR_UNAVAILABLE (-28)

// The caller did not have permission to perform the specified operation.
#define SYSTEM_ERR_ACCESS_DENIED (-30)

// Otherwise-unspecified error occurred during I/O.
#define SYSTEM_ERR_IO (-40)

// The entity the I/O operation is being performed on rejected the operation.
//
// Example: an I2C device NAK'ing a transaction or a disk controller rejecting
// an invalid command, or a stalled USB endpoint.
#define SYSTEM_ERR_IO_REFUSED (-41)

// The data in the operation failed an integrity check and is possibly
// corrupted.
//
// Example: CRC or Parity error.
#define SYSTEM_ERR_IO_DATA_INTEGRITY (-42)

// The data in the operation is currently unavailable and may be permanently
// lost.
//
// Example: A disk block is irrecoverably damaged.
#define SYSTEM_ERR_IO_DATA_LOSS (-43)

// The device is no longer available (has been unplugged from the system,
// powered down, or the driver has been unloaded).
#define SYSTEM_ERR_IO_NOT_PRESENT (-44)

// More data was received from the device than expected.
//
// Example: a USB "babble" error due to a device sending more data than the
// host queued to receive.
#define SYSTEM_ERR_IO_OVERRUN (-45)

// An operation did not complete within the required timeframe.
//
// Example: A USB isochronous transfer that failed to complete due to an
// overrun or underrun.
#define SYSTEM_ERR_IO_MISSED_DEADLINE (-46)

// The data in the operation is invalid parameter or is out of range.
//
// Example: A USB transfer that failed to complete with TRB Error
#define SYSTEM_ERR_IO_INVALID (-47)

// Path name is too long.
#define SYSTEM_ERR_BAD_PATH (-50)

// The object is not a directory or does not support directory operations.
//
// Example: Attempted to open a file as a directory or attempted to do
// directory operations on a file.
#define SYSTEM_ERR_NOT_DIR (-51)

// Object is not a regular file.
#define SYSTEM_ERR_NOT_FILE (-52)

// This operation would cause a file to exceed a filesystem-specific size
// limit.
#define SYSTEM_ERR_FILE_BIG (-53)

// The filesystem or device space is exhausted.
#define SYSTEM_ERR_NO_SPACE (-54)

// The directory is not empty for an operation that requires it to be empty.
//
// For example, non-recursively deleting a directory with files still in it.
#define SYSTEM_ERR_NOT_EMPTY (-55)

// An indicate to not call again.
//
// The flow control values `SYSTEM_ERR_STOP`, `SYSTEM_ERR_NEXT`, and `SYSTEM_ERR_ASYNC` are
// not errors and will never be returned by a system call or public API. They
// allow callbacks to request their caller perform some other operation.
//
// For example, a callback might be called on every event until it returns
// something other than `SYSTEM_OK`. This status allows differentiation between
// "stop due to an error" and "stop because work is done."
#define SYSTEM_ERR_STOP (-60)

// Advance to the next item.
//
// The flow control values `SYSTEM_ERR_STOP`, `SYSTEM_ERR_NEXT`, and `SYSTEM_ERR_ASYNC` are
// not errors and will never be returned by a system call or public API. They
// allow callbacks to request their caller perform some other operation.
//
// For example, a callback could use this value to indicate it did not consume
// an item passed to it, but by choice, not due to an error condition.
#define SYSTEM_ERR_NEXT (-61)

// Ownership of the item has moved to an asynchronous worker.
//
// The flow control values `SYSTEM_ERR_STOP`, `SYSTEM_ERR_NEXT`, and `SYSTEM_ERR_ASYNC` are
// not errors and will never be returned by a system call or public API. They
// allow callbacks to request their caller perform some other operation.
//
// Unlike `SYSTEM_ERR_STOP`, which implies that iteration on an object
// should stop, and `SYSTEM_ERR_NEXT`, which implies that iteration
// should continue to the next item, `SYSTEM_ERR_ASYNC` implies
// that an asynchronous worker is responsible for continuing iteration.
//
// For example, a callback will be called on every event, but one event needs
// to handle some work asynchronously before it can continue. `SYSTEM_ERR_ASYNC`
// implies the worker is responsible for resuming iteration once its work has
// completed.
#define SYSTEM_ERR_ASYNC (-62)

// The specified protocol is not supported.
#define SYSTEM_ERR_PROTOCOL_NOT_SUPPORTED (-70)

// The host is unreachable.
#define SYSTEM_ERR_ADDRESS_UNREACHABLE (-71)

// Address is being used by someone else.
#define SYSTEM_ERR_ADDRESS_IN_USE (-72)

// The socket is not connected.
#define SYSTEM_ERR_NOT_CONNECTED (-73)

// The remote peer rejected the connection.
#define SYSTEM_ERR_CONNECTION_REFUSED (-74)

// The connection was reset.
#define SYSTEM_ERR_CONNECTION_RESET (-75)

// The connection was aborted.
#define SYSTEM_ERR_CONNECTION_ABORTED (-76)

// A task was killed during an operation. This is a private error that should
// not be seen outside of the VDSO.
#define SYSTEM_ERR_INTERNAL_INTR_KILLED (-502)

typedef int error_t;

#endif // _ERRNO_H
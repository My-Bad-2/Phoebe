#include <assert.h>
#include <logger.h>

__NO_RETURN
void __assert_fail(const char* assertion, const char* file, int line, const char* function)
{
	log_panic("\"%s\": Failed assertion: line %d: function %s", file, line, function);
	log_panik("assertion: %s", assertion);

	__UNREACHABLE();
}
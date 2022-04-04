#include "sched.h"

#include <trace/events/power.h>

const struct sched_class freezer_sched_class
	__section("__freezer_sched_class") = {
#ifdef CONFIG_SMP
#endif
};
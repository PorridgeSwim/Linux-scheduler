#include "sched.h"

#include <trace/events/power.h>

static inline struct task_struct *fz_task_of(struct sched_freezer_entity *fz_se)
{
	return container_of(fz_se, struct task_struct, fz);
}

static inline struct rq *rq_of_fz_se(struct sched_freezer_entity *fz_se)
{
	struct task_struct *p = fz_task_of(fz_se);

	return task_rq(p);
}

static inline struct freezer_rq *fz_rq_of_se(struct sched_freezer_entity *fz_se)
{
	struct rq *rq = rq_of_fz_se(fz_se);

	return &rq->fz;
}

static void __enqueue_freezer_entity(struct sched_freezer_entity *fz_se, unsigned int flags)
{
	struct freezer_rq *fz_rq = fz_rq_of_se(fz_se);
	struct list_head *queue = fz_rq->fz_list;

	if (move_entity(flags)) {
		WARN_ON_ONCE(fz_se->on_list);
		if (flags & ENQUEUE_HEAD) // ENQUEUE_HEAD place at front of runqueue
			list_add(&fz_se->run_list, queue);
		else
			list_add_tail(&fz_se->run_list, queue);

		fz_se->on_list = 1;
	}
	fz_se->on_rq = 1;

	// inc_fz_tasks(fz_se, fz_rq);
	fz_rq->fz_nr_running += 1;
}

static void enqueue_freezer_entity(struct sched_freezer_entity *fz_se, unsigned int flags)
{
	// get the runqueue of a task
	// struct rq *rq = rq_of_fz_se(fz_se);

	// dequeue_rt_stack(fz_se, flags);
	for (; fz_se; fz_se = NULL)
		__enqueue_freezer_entity(fz_se, flags);
	// enqueue_top_rt_rq(&rq->fz); used for group?
}

static void dequeue_fz_entity(struct sched_freezer_entity *fz_se, unsigned int flags)
{
	// struct rq *rq = rq_of_rt_se(rt_se);

	// dequeue_rt_stack(rt_se, flags);

	for (; fz_se; fz_se = NULL) {
		struct freezer_rq *fz_rq = fz_rq_of_se(fz_se);

		if (fz_rq && fz_rq->fz_nr_running) {
			list_del_init(&fz_se->run_list);
			fz_se->on_list = 0;
			--fz_rq->fz_nr_running;
		}
			// __enqueue_rt_entity(rt_se, flags);
	}
	// enqueue_top_rt_rq(&rq->rt);
}

static void
enqueue_task_freezer(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_freezer_entity *fz_se = &p->fz;

	// check the defination of ENQUEUE_WAKEUP
	// check the defination of flags
	if (flags & ENQUEUE_WAKEUP)
		fz_se->timeout = 0;

	enqueue_freezer_entity(fz_se, flags);

}

static void dequeue_task_freezer(struct rq *rq, struct task_struct *p, int flags)
{
	// get freezer entity
	struct sched_freezer_entity *fz_se = &p->fz;
	dequeue_fz_entity(fz_se, flags);
}
const struct sched_class freezer_sched_class
	__section("__freezer_sched_class") = {
	/* no enqueue/yield_task for idle tasks */

	.enqueue_task		= enqueue_task_freezer,
	.dequeue_task		= dequeue_task_freezer,

	.check_preempt_curr	= check_preempt_curr_idle,

	.pick_next_task		= pick_next_task_idle,
	.put_prev_task		= put_prev_task_idle,
	.set_next_task          = set_next_task_idle,

#ifdef CONFIG_SMP
	.balance		= balance_idle,
	.select_task_rq		= select_task_rq_idle,
	.set_cpus_allowed	= set_cpus_allowed_common,
#endif

	.task_tick		= task_tick_idle,

	.prio_changed		= prio_changed_idle,
	.switched_to		= switched_to_idle,
	.update_curr		= update_curr_idle,
#ifdef CONFIG_SMP
#endif
};
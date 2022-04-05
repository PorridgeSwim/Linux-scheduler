#include "sched.h"

#include <trace/events/power.h>

int sched_freezer_timeslice = FREEZER_TIMESLICE;

const struct sched_class freezer_sched_class;

/*
 * Change rt_se->run_list location unless SAVE && !MOVE
 *
 * assumes ENQUEUE/DEQUEUE flags match
 */
static inline bool move_entity(unsigned int flags)
{
	if ((flags & (DEQUEUE_SAVE | DEQUEUE_MOVE)) == DEQUEUE_SAVE)
		return false;

	return true;
}


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
	struct list_head *queue = &(fz_rq->fz_list);
	struct rq *rq = rq_of_fz_se(fz_se);

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
	rq->nr_running += 1;
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
	struct rq *rq = rq_of_fz_se(fz_se);

	// dequeue_rt_stack(rt_se, flags);

	for (; fz_se; fz_se = NULL) {
		struct freezer_rq *fz_rq = fz_rq_of_se(fz_se);

		if (fz_rq && fz_rq->fz_nr_running) {
			list_del_init(&fz_se->run_list);
			fz_se->on_list = 0;
			fz_rq->fz_nr_running -= 1;
			rq->nr_running -= 1;
		}
			// __enqueue_rt_entity(rt_se, flags);
	}
	// enqueue_top_rt_rq(&rq->rt);
}

static void
enqueue_task_freezer(struct rq *rq, struct task_struct *p, int flags)
{
	struct sched_freezer_entity *fz_se = &p->fz;

	enqueue_freezer_entity(fz_se, flags);
}

static void dequeue_task_freezer(struct rq *rq, struct task_struct *p, int flags)
{
	// get freezer entity
	struct sched_freezer_entity *fz_se = &p->fz;

	dequeue_fz_entity(fz_se, flags);
}

#ifdef CONFIG_SMP
static int select_task_rq_freezer(struct task_struct *p, int cpu, int sd_flag, int flags)
{
	int i;
	unsigned int tmp;
	unsigned int min;
	int count = 0;
	int cur_cpu; 

	for_each_possible_cpu(i) {
		if (count == 0) {
			min = cpu_rq(i)->fz.fz_nr_running;
			cur_cpu = i;
			count = 1;
		} else{
			tmp = cpu_rq(i)->fz.fz_nr_running;
			if (min > tmp) {
				min = tmp;
				cur_cpu = i;
			}
		}
	}

	return cur_cpu;
}

static int
balance_freezer(struct rq *rq, struct task_struct *prev, struct rq_flags *rf)
{
	return WARN_ON_ONCE(1);
}
 #endif

 /*
  * Update the current task's runtime statistics. Skip current tasks that
  * are not in our scheduling class.
  */
 static void update_curr_freezer(struct rq *rq)
 {
 	struct task_struct *curr = rq->curr;
 	// struct sched_freezer_entity *fz_se = &curr->fz;
 	u64 delta_exec;
 	u64 now;

 	if (curr->sched_class != &freezer_sched_class)
 		return;

 	now = rq_clock_task(rq);
 	delta_exec = now - curr->se.exec_start;
 	if (unlikely((s64)delta_exec <= 0))
 		return;

 	schedstat_set(curr->se.statistics.exec_max,
 		      max(curr->se.statistics.exec_max, delta_exec));

 }

 static void task_tick_freezer(struct rq *rq, struct task_struct *p, int queued)
 {
 	struct sched_freezer_entity *fz_se = &p->fz;

 	update_curr_freezer(rq);

 	if (--p->fz.time_slice)
 		return;

 	p->fz.time_slice = sched_freezer_timeslice;

 	if (fz_se->run_list.prev != fz_se->run_list.next) {
 		list_move_tail(&fz_se->run_list, &(fz_rq_of_se(fz_se)->fz_list)); //need list_head *
 		resched_curr(rq);
 		return;
 	}
 }

 void init_fz_rq(struct freezer_rq *fz_rq)
 {
 	INIT_LIST_HEAD(&(fz_rq->fz_list));
 	fz_rq->fz_nr_running = 0;
 }

static struct sched_freezer_entity *pick_next_fz_entity(struct rq *rq,

						   struct freezer_rq *fz_rq)
{
	struct sched_freezer_entity *next = NULL;
	struct list_head *queue;

	queue = &fz_rq->fz_list;
	next = list_entry(queue->next, struct sched_freezer_entity, run_list);

	return next;
}

static struct task_struct *_pick_next_task_fz(struct rq *rq)
{
	struct sched_freezer_entity *fz_se;
	struct freezer_rq *fz_rq  = &rq->fz;


	do {
		fz_se = pick_next_fz_entity(rq, fz_rq);
		BUG_ON(!fz_se);
		fz_rq = NULL;
	} while (fz_rq);

	return fz_task_of(fz_se);
}

static struct task_struct *pick_next_task_freezer(struct rq *rq)
{
	struct task_struct *p;

	if (!sched_freezer_runnable(rq))
		return NULL;

	p = _pick_next_task_fz(rq);
	// set_next_task_freezer(rq, p, true);
	return p;
}


static void yield_task_freezer(struct rq *rq)
{
}

static void check_preempt_curr_freezer(struct rq *rq, struct task_struct *p, int flags)
{

}

static void put_prev_task_freezer(struct rq *rq, struct task_struct *prev)
{
}

static void set_next_task_freezer(struct rq *rq, struct task_struct *next, bool first)
{
}


static void
prio_changed_freezer(struct rq *rq, struct task_struct *p, int oldprio)
{

}

static void switched_to_freezer(struct rq *rq, struct task_struct *p)
{
}


const struct sched_class freezer_sched_class
	__section("__freezer_sched_class") = {
	/* no enqueue/yield_task for idle tasks */

	.enqueue_task		= enqueue_task_freezer,
	.dequeue_task		= dequeue_task_freezer,
	.yield_task		= yield_task_freezer,

	.check_preempt_curr	= check_preempt_curr_freezer,

	.check_preempt_curr	= check_preempt_curr_freezer,
	.put_prev_task		= put_prev_task_freezer,
	.pick_next_task		= pick_next_task_freezer,
	.put_prev_task		= put_prev_task_freezer,
	.set_next_task          = set_next_task_freezer,

#ifdef CONFIG_SMP

	.balance		= balance_freezer,
	.select_task_rq		= select_task_rq_freezer,
	.set_cpus_allowed	= set_cpus_allowed_common,
#endif

	.task_tick		= task_tick_freezer,

	.prio_changed		= prio_changed_freezer,
	.switched_to		= switched_to_freezer,
	.update_curr		= update_curr_freezer,
	.prio_changed	= prio_changed_freezer,
	.update_curr	= update_curr_freezer,
#ifdef CONFIG_SMP
#endif
};
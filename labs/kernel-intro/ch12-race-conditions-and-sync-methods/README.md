
# Chapter 12 - Race conditions and synchronization methos


- Concurrency and Synchronization Methods
- Atomic Operations
- Bit Operations
- Spinlocks
- Seqlocks
- Disabling Preemption
- Mutexes
- Semaphores
- Completion Functions
- Read-Copy-Update (RCU)
- Reference counts


`Atomic operations` are used when we require to modify a variable and we
don't want to sleep.

When more instructions are needed, we can use `spinlocks`. Spinlocks busy
wait to get the log. The spinlocks can suspend interrupts.

`seqlocks` are used when we want to read values that are hardly modified.

If we require to go to sleep while waiting for a resource, we can use `mutexes`
or `semaphores`.

`Completion funtions` can be used instead of semaphores when contention is
expected to be high.


Before many of these sync constructions, there was the `Big Lock Kernel`. It
was removed completely in the 2.6.39 kernel.


## Atomic operations

The `atomic_t` structure is defined in `include/linux/types.h`.

```c
typedef struct {
	int counter;
} atomic_t;

```

The atomic API consists of:


```c
#include <linux/atomic.h>

#define ATOMIC_INIT(i) { (i) }
#define atomic_read(v) ACCESS_ONCE((v)->counter)
#define atomic_set(v, i) (((v)->counter) = (i))
void atomic_add (int i, atomic_t *v);
void atomic_sub (int i, atomic_t *v);
void atomic_inc (atomic_t *v);
void atomic_dec (atomic_t *v);
int atomic_dec_and_test (atomic_t *v);
int atomic_inc_and_test_greater_zero (atomic_t *v);
int atomic_sub_and_test (int i, atomic_t *v);
int atomic_add_negative (int i, atomic_t *v);
int atomic_sub_return (int i, atomic_t *v);
int atomic_add_return (int i, atomic_t *v);
int atomic_inc_return (int i, atomic_t *v);
int atomic_dec_return (int i, atomic_t *v);
```

## Bit operations

When we want to modify bits in variables and have a lock while we do it, we
can use the functions defined in `asm/bitops.h`.


```c
#include <asm/bitops.h>

void set_bit (int nr, volatile unsigned long *addr);
void clear_bit (int nr, volatile unsigned long *addr);
void change_bit (int nr, volatile unsigned long *addr);
int test_bit (int nr, volatile unsigned long *addr);
int test_and_set_bit (int nr, volatile unsigned long *addr);
int test_and_clear_bit (int nr, volatile unsigned long *addr);
int test_and_change_bit (int nr, volatile unsigned long *addr);


// Find first zero.
unsigned long ffz (unsigned long word);
// Find first set bit.
unsigned long ffs (int x);
// Find last set bit.
unsigned long fls (int x);
```

The functions below are not atomit:


```c
long find_first_zero_bit (const unsigned long *addr, unsigned long size);
long find_next_zero_bit (const unsigned long *addr, long size, long offset);
long find_first_bit ( const unsigned long *addr, unsigned long size);
```

## Spin locks

Spinlocks is a construction that helps to protect the critical sections of code.
It doesn't sleep and busy waits till the resource is free. The spinlocks are
architecture dependant. The signature functions are defined in `include/linux/spinlock.h`.


```c
// Declare the lock.
spinlock_t my_lock;

// Initialize the lock
spin_lock_init (&my_lock);


// Get the lock.
spin_lock (&my_lock);

...... critical region ........

// Release the lock.
spin_unlock (&my_lock);
```

When we require to suspend interrupts while in the critical region, we can use:


```c
// Holds the interrupt state.
unsigned long interrupts_state;
// Declare the lock.
spinlock_t my_lock;

// Initialize the lock.
spin_lock_init (&my_lock);

// Save and suspend interrupts, get lock.
spin_lock_irqsave (&my_lock, interrupts_state);

...... critical region ........

// Restore and enable interrupts, release lock.
spin_unlock_irqrestore (&my_lock, interrupts_state);

```

When we have more readers than writers and we want them to enter to the critical
region, we can use the functions below:

```c
unsigned long interrupts_state;
rwlock_t my_lock;

rw_lock_init (&my_lock);



read_lock_irqsave (&my_lock, interrupts_state);
...... critical code , reads only
read_unlock_irqrestore (&my_lock, interrupts_state);


write_lock_irqsave (&my_lock,flags);
...... critical region , exclusive read and write access
write_unlock_irqrestore (&my_lock, interrupts_state);

```

Another spinlocks functions are:

```c
spin_unlock_wait (spinlock_t *lock);
int spin_is_locked (spinlock_t *lock);
int spin_trylock (spinlock_t *lock);
```

The basic implementation of a spinlock is below:

- https://elixir.bootlin.com/linux/v5.16.5/source/include/linux/spinlock_api_smp.h#L130

```c
static inline void __raw_spin_lock(raw_spinlock_t *lock)
{
	preempt_disable();
	spin_acquire(&lock->dep.map, 0, 0, _RET_IP_);
	LOCK_CONTENDED(lock, do_raw_spin_trylock, do_raw_spin_lock);
}


static inline void __raw_spin_unlock(raw_spinlock_t *lock)
{
	spin_release(&lock->dep_map, 1, _RET_IP_);
	do_raw_spin_unlock(lock);
	preempt_enable();
}

```

## Seqlocks

`Seqlocks` are used when we have the conditions below:

- The critical region is small.
- No side effects are produced by altering the data.


The main API definitions are below:

```c
#include <linux/seqlock.h>

typedef struct {
unsigned sequence;
spinlock_t lock;
} seqlock_t;

void write_seqlock (seqlock_t *sl);
void write_sequnlock (seqlock_t *sl);
unsigned read_seqbegin (const seqlock_t *sl);
int read_seqretry (const seqlock_t *sl, unsigned iv);
```

And example of how to use it is below:


```c
unsigned int seq;
do {
seq = read_seqbegin (&my_seqlock);

// Perform changes using the protected code.

} while (read_seqretry (&my_seqlock, seq));

```

The while loop will proceed under these 2 circunstances:


1. If `seq` is an odd number, it means that the write was in the middle of updation
the value when the loop began.
2. If the sequence number read by `read_seqretry()` does not match the one
obtained from `read_seqbegin()`, then a writer modified the sequence.


## Mutexes

A `mutex` is a lock mechanism that sleeps while waiting to get the lock. The API
is defined in `include/linux/mutex.h`.


```c
#include <linux/mutex.h>

struct mutex {
  spinlock_t wait_lock;
  struct list_head wait_list;
};

DEFINE_MUTEX (name);

void mutex_init (struct mutex *lock);
void mutex_lock (struct mutex *lock);
int mutex_lock_interruptible (struct mutex *lock);
int mutex_lock_killable (struct mutex *lock);
void mutex_unlock (struct mutex *lock);
int mutex_trylock (struct mutex *lock);
int mutex_is_locked (struct mutex *lock);
```
The main mutex's tradeoffs are:

- The mutex must be released by the original owner.
– The mutex cannot be applied recursively.
– The mutex cannot be locked or unlocked from interrupt context.


## Semaphores

Mutexes allows to only one thread to protect a critical region. When we require
the ability to allow several threads to enter to a critical region, we use
semaphores.


- `include/linux/semaphore.h`
- `include/linux/rwsem.h`


```c
#include <linux/semaphore.h>


struct semaphore {
  unsigned int count;
  int sleepers;
  wait_queue_head_t wait;
};

struct rw_semaphore {
  rwsem_count_t count;
  spinlock_t wait_lock;
  struct list_head wait_list;
};


DEFINE_SEMAPHORE(name);
DECLARE_RWSEM(name);

void sema_init (struct semaphore *sem, int val);
void down (struct semaphore *sem);
int down_interruptible (struct semaphore *sem);
int down_killable (struct semaphore *sem);
int down_trylock (struct semaphore *sem);
int down_timeout(struct semaphore *sem, long jiffies);
void up (struct semaphore *sem);
void down_read (struct rw_semaphore *sem);
void down_write (struct rw_semaphore *sem);
void up_read (struct rw_semaphore *sem);
void up_write (struct rw_semaphore *sem);
```

The down() function checks to see if someone else has already entered the
critical code section; if the value of the semaphore is greater than zero, it
decrements it and returns. If it is already zero, it will sleep and try again
later.

The up() function increments the semaphore value, waking up any processes
waiting on the semaphore. It does not require any _interruptible form.


## Completion Functions

The completation functions is a mechanism that allows to wait for a certain
event and it is optimized for high contention cases.

```c
#include <linux/completion.h>

struct completion {
  unsigned int done;
  wait_queue_head_t wait;
};

DECLARE_COMPLETION(name);

void init_completion (struct completion *c);
void wait_for_completion (struct completion *c);
int wait_for_completion_interruptible (struct completion *c);
void complete (struct completion *c);
void complete_and_exit (struct completion *c, long code);
unsigned long wait_for_completion_timeout (struct completion *c, unsigned long timeout);
unsigned long wait_for_completion_interruptible_timeout (struct completion *c, unsigned longtimeout);

```

The correspondence with the APIs for mutexes, semaphores and completion functions
is below:

```
struct semaphore <--->  struct completion     <---> struct mutex
sema_init()      <--->  init_completion()     <---> mutex_init()
down()           <--->  wait_for_completion() <---> mutex_lock()
up()             <--->  complete()            <---> mutex_unlock()
```


## Read-Copy-Update (RCU)

The Read-Copy-Update mechanism is designed for the cases where:

- There are little writes.
- There are more reads.
- We want to avoid excessive locking.

```
#include <linux/rcupdate.h>

struct my_struct *data;

rcu_read_lock();
... critial region ...
rcu_read_unlock();
```

## Reference counts

When we want to count references, we use the `kref` API, defined in
`include/linux/kref.h`.


```c
struct kref {
  atomic_t refcount;
};

void kref_init (struct kref *kref);
unsigned int kref_read (const struct kref *kref);
void kref_get (struct kref *kref);
int kref_put (struct kref *kref, void (*release) (struct kref *kref));
```

To release the reference, we use code similar to:


```c
struct my_dev_data *md;

kref_put(md->my_refcount, my_release);

static void my_release (struct kref *kr){
  struct my_dev_data *md = container_of (kr, struct my_dev_data, my_refcount);
  my_dev_free(md);
}
```

For more information, see: https://www.kernel.org/doc/html/latest/core-api/kref.html


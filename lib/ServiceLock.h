
#ifndef LIB_SERVICE_LOCK_H_
#define LIB_SERVICE_LOCK_H_

/*
 * Services locks are used to indicate if the user have control
 * of the resource (manual, unlocked) or if the microcontroller
 * has it (automatic, locked).
 *
 * !!!! All the options are atomic in order to avoid race conditions between
 *      different threads
 */

#define SERVICE_LOCKS_SIZE 4

atomic_bool Service_locked[SERVICE_LOCKS_SIZE];

typedef enum {
	SERVICE_LOCK_PUMPS = 0,
	SERVICE_LOCK_VALVES = 1,
	SERVICE_LOCK_HEATERS = 2,
	SERVICE_LOCK_SENSOR_SELECT = 3
} Service_LockId;

bool Service_isLocked(Service_LockId id) {
	if (id < SERVICE_LOCKS_SIZE)
		return atomic_load(&Service_locked[id]);
	else
		return false;
}

void Service_unlock(Service_LockId id) {
	if (id < SERVICE_LOCKS_SIZE)
		atomic_store(&Service_locked[id], false);
}

void Service_lock(Service_LockId id) {
	if (id < SERVICE_LOCKS_SIZE)
		atomic_store(&Service_locked[id], true);
}

void Service_Lock_setup() {
	Service_lock(SERVICE_LOCK_PUMPS);
	Service_lock(SERVICE_LOCK_VALVES);
	Service_lock(SERVICE_LOCK_HEATERS);
	Service_lock(SERVICE_LOCK_SENSOR_SELECT);
}

#endif

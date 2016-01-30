
#ifndef LIB_MACHINE_H_
#define LIB_MACHINE_H_

enum {
	SAMPLING,
	CALIBRATING
} Machine_State;

// TODO: Implement this function
Machine_State_sampling() {
	// Lock All Services

	// Open sampling valve
	// Pump sample inside the chamber
	// Wait until sample detector detects that the sample is inside the chamber

	// Turn pumps off

	// Unlock sampling (Enable sampling service)
	// Wait some time (Sample Timeout)
	// Block sampling

	// Open wash valve
	// Pump the wash to the chamber
	// Wait some time (Wash Timeout)

	// Turn pumps off

	// Unlock All Services
}

// TODO: Implement this function
Machine_State_calibrating() {


}

#endif /* LIB_MACHINE_H_ */

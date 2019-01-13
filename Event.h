/***************************************************************************
/  Written by: Alan Doose
/  27 October 2018
/  CS 433 Assignment 3
/
/  Purpose:
/   This is the Event class header file. The purpose of this class is
/   to provide a container for different event types that are used in
/	a DES simulation.
/
/  Assumptions:
/	It is assumed that when an instance of this class is created, the client
/	program will provide the event type, the time of the event, and a pointer
/	to the pcb that owns the event
/   
***************************************************************************/

#ifndef _EVENT_H
#define _EVENT_H

#include "pcb.h"
#include <iostream>
#include <string>

enum e_type {
	process_arrival,		//process arrives in the system
	cpu_burst_completion,	//process completes its cpu burst
	io_completion,			//process completes its io burst
	timer_exp				//max failsafe timer expires
};

class Event
{
public:
	Event(); //default constructor - should not be used
	Event(pcb* process, e_type eventType); //constructor to set the event type and point to the owning process
	Event(pcb* process, e_type eventType, int time); //constructor to set the event type and time, and point to the owning process
	~Event();

	void printEvent() const; //prints event details - use for debugging
	std::string typeToString() const; //converts event enum type to a printable string

	//less than operator overloaded for Event class
	// returns true if LHS's time is less than RHS's time
	friend bool operator<(const Event& LHS, const Event& RHS);

	//Each event will have a pointer to the process that owns it, but will also hold its own local variable of the process' start time and ID
	pcb* this_events_PCB; //pointer to the pcb that owns this event
	int _time; //time the event occurs
	int _procID; //process ID that owns the event (for easier/faster access than going through the pcb pointer)
	e_type _type; //type of event
};

//This class is implemented here with Event to be used to less-than compare Events based on time
//  within a STL priority queue
class EventComparator
{
	public:
		int operator() (const Event& e1, const Event& e2)
		{
			return e1._time > e2._time;
		}
};

#endif
/***************************************************************************
/  Written by: Alan Doose
/  27 October 2018
/  CS 433 Assignment 3
/
/  Purpose:
/   This is the Event class implementation file. The purpose of this class is
/   to provide a container for different event types that are used in
/	a DES simulation.
/
/  Assumptions:
/	It is assumed that when an instance of this class is created, the client
/	program will provide the event type, the time of the event, and a pointer
/	to the pcb that owns the event
/
***************************************************************************/

#include "Event.h"

Event::Event()
{
	//default constructor for Event will be unused since I
	//  need to specify the parameters of each event
}

//Constructor for seeding event queue with process arrival events (time is dependent on the arrival time of the process)
Event::Event(pcb* process, e_type eventType)
{
	_time = process->getStartTime();
	_type = eventType;
	_procID = process->getID();
	this_events_PCB = process;
}

//Standard Constructor for adding an event into the event queue during simulation
Event::Event(pcb* process, e_type eventType, int time)
{
	_time = time; //time the event occurs
	_type = eventType; //tupe of event
	_procID = process->getID();
	this_events_PCB = process; //pointer reference to the pcb that owns this event
}

Event::~Event()
{
	this_events_PCB = NULL;
}

void Event::printEvent() const
{
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "Event type: " << typeToString() 
			<< ", occuring at time: " << (double)_time / 1000 << " s"
			<< ", for process: " << _procID << std::endl;
	
}

std::string Event::typeToString() const
{
	switch (_type)
	{
	case process_arrival: return "Process Arrival";
	case cpu_burst_completion: return "CPU Burst Completion";
	case io_completion: return "IO Burst Completion";
	case timer_exp: return "Timer Expiration";
	default: return "Invalid State detected in event!";
	}
}

bool operator<(const Event& LHS, const Event& RHS)
{
	return (LHS._time < RHS._time);
}
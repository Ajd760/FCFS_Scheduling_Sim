/***************************************************************************
/  Written by: Alan Doose
/  27 October 2018
/  CS 433 Assignment 3
/
/  Purpose:
/   This is the Scheduler class header file. The purpose of this class is
/   to simulate a short term CPU scheduler using a FCFS algorithm.
/	
/	This class inherits from PCBTable in order to give it direct access to 
/	the pcbs in the simulation and to allow it to dynamically create a PCB
/	Table of sufficient size to accomodate an amount of processes given to 
/	it at run time (via command line or other method)
/
/  Assumptions:
/   Inherits publicly from PCBTable.h, this file must be included in the same
/		directory as this file (or modify the include path)
/
***************************************************************************/

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <queue>
#include "PCBTable.h"
#include "Event.h"

class Scheduler : public PCBTable
{ 
public:
	Scheduler(); //default Scheduler constructor will create a scheduler with a PCBTable with the default size specified in PCBTable.h
	Scheduler(int tableCap); //overloaded Scheduler constructor creates a scheduler with a PCBTable of size tableCap
	~Scheduler();

	bool init(); //Fills the PCB table with random processes

	//Called after every event - checks if CPU is idle and dispatches a process if so
	void schedule(const Event& e);

	//Event handling functions
	void handle_proc_arrival(const Event& e);
	void handle_CPU_completion(const Event& e);
	void handle_IO_completion(const Event& e);
	void handle_timer_expiration(const Event& e);

	//Prints statistics relevant to overall algorithm performance
	void printStats(int endTime, int totalProcesses);

	//Calculates the total waiting time for processes that did not finish their entire CPU time
	void calcUnfinishedWait(int endTime);

	/* Member variables */
	bool CPU_is_idle; //flag to show if the CPU is idle or not (true if idle)
	std::priority_queue<Event, std::vector<Event>, EventComparator> event_queue; //priority queue based on time stamp of generated events
	std::queue<pcb*> ready_queue; //standard queue for FCFS
	

private:
	pcb* _current_process; //pointer to the current process running on the CPU

	/* Statistics Variables */
	int sThroughput; //will increment by 1 each time a process completes
};

#endif
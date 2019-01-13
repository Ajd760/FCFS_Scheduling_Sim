/***************************************************************************
/  Written by: Alan Doose
/  27 October 2018
/  CS 433 Assignment 3
/
/  Purpose:
/   This is the Scheduler class implementation file. The purpose of this class is
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

#include "Scheduler.h"

//Creates a scheduler with a PCBTable with the default capacity as set in PCBTable.h
Scheduler::Scheduler()
{
	CPU_is_idle = true;
	_current_process = NULL;
	sThroughput = 0;
}

//Creates a scheduler with a PCBTable that has given capacity
Scheduler::Scheduler(int tableCap) : PCBTable(tableCap)
{
	CPU_is_idle = true;
	_current_process = NULL;
	sThroughput = 0;
}

//Destructor
Scheduler::~Scheduler()
{
	_current_process = NULL; //all pcb pointer objects are freed from memory in their own destructor
}

//Initializes the scheduler's PCB table by filling it to capactity with random processes
bool Scheduler::init()
{
	for (int i = 0; i < _capacity; i++)
	{
		pcb* newProc = new pcb(); //create a new random process
		if (!addToTable(newProc)) //add process to the scheduler's PCBTable
		{
			std::cout << "Error adding pcb to PCBTable in Scheduler.init()" << std::endl;
			return false;
		}
	}
	return true;
}

//The main scheduling algorithm - checks if the CPU is idle, and if so will dispatch the next process from the ready 
//	queue to run on the CPU. Generates a new CPU completion event that is added to the scheduler's event queue to
//	show that the process completed its CPU burst and is ready to give up the CPU. 
void Scheduler::schedule(const Event& e)
{
	if (CPU_is_idle && !ready_queue.empty())
	{
		//take front process off the ready queue and assign to the CPU
		_current_process = ready_queue.front();
		ready_queue.pop();

		//run the process on the CPU
		CPU_is_idle = false;
		//std::cout << "Process " << _current_process->getID() << " executing on CPU..." << std::endl;
		_current_process->executeCPUBurst();
		
		//calc the time at which the process will complete its CPU burst
		int CPUCompletionTime = e._time + _current_process->getNextCPUBurst(); //calculate the time at which this process will complete its CPU burst foir the Event
		
		//std::cout << "Process " << _current_process->getID() << " will complete CPU burst at time: " << CPUCompletionTime << std::endl;
		//std::cout << "Process " << _current_process->getID() << " has " << _current_process->getRemainingCPUBurst() << " CPU time remaining" << std::endl;

		//create a CPU completion event for when this process will complete its CPU burst and add to the event queue
		Event CPUcompletion(_current_process, cpu_burst_completion, CPUCompletionTime);
		event_queue.push(CPUcompletion);
	}
}

//Handles process arrival in the system by setting its next CPU burst time and adding it to the ready queue
//	Calls schedule() to check if CPU is idle and ready to run the next process in the ready queue
void Scheduler::handle_proc_arrival(const Event& e)
{
	e.this_events_PCB->setNextCPUBurst(); //calculate the process' next CPU burst
	e.this_events_PCB->setState(READY); //show that the process is ready to run
	ready_queue.push(e.this_events_PCB); //push the process into the ready queue to wait for CPU burst time
	schedule(e); //call the main scheduler function to check if the CPU is idle 

}

//Handles the event generated when a process completes its CPU burst and is ready to either terminate or enter
//	an IO Burst phase. If the process completes, it is not added back into the ready and statistics on the 
//	process' execution are gathered. If the process is not complete, it will generate an IO Burst time and
//	add an event to the event queue to signal when it completes this IO burst and is ready to re-enter the 
//  ready queue for its next CPU Burst
void Scheduler::handle_CPU_completion(const Event& e)
{
	if (e.this_events_PCB->getRemainingCPUBurst() <= 0)
	{
		//remove the process from the system (process will not be reinserted to the ready queue)
		e.this_events_PCB->setState(TERMINATED);

		sThroughput++; //job completed, increment stat
		e.this_events_PCB->sFinishTime = e._time; //record the finish time for this pcb

		//calculate the processes waiting time as: (Completion time - running time - IO time - arrival time)
		e.this_events_PCB->sWaitingTime = e._time - e.this_events_PCB->sServiceTime - e.this_events_PCB->sIOTime - e.this_events_PCB->getStartTime(); 
		
		//calculate the turnaround time as: (Completion time - arrivalTime)
		e.this_events_PCB->sTurnaroundTime = e._time - e.this_events_PCB->getStartTime();
	}
	else //else process still needs CPU time to complete
	{
		//process enters IO burst
		int IOBurstTime = ranInt(30, 100); //random IO burst between 30-100 ms per project spec
		int IOCompletionTime = e._time + IOBurstTime; //time when the IO burst will complete for this process
		e.this_events_PCB->sIOTime += IOBurstTime; //add IO time to statistic

		//Set the process' state to WAITING while it is executing its IO burst
		e.this_events_PCB->setState(WAITING);

		//Create the IOCompletion event and add to the event queue
		Event IOCompletion(_current_process, io_completion, IOCompletionTime);
		event_queue.push(IOCompletion);
	}

	_current_process = NULL; //clear pointer to current process
	CPU_is_idle = true; //swap flag indicating CPU is now idle
	schedule(e);
}

//Handles the event generated by a process when it completes its IO Burst phase and is ready to generate its
//	next CPU Burst time and re-enter the ready queue to wait for CPU time. Calls schedule() to check if CPU 
//	is idle and ready to run the next process in the ready queue 
void Scheduler::handle_IO_completion(const Event& e)
{
	e.this_events_PCB->setNextCPUBurst(); //calculate the process' next CPU burst
	e.this_events_PCB->setState(READY); //show that the process is ready to run
	ready_queue.push(e.this_events_PCB); //push the process into the ready queue to wait for CPU burst time
	schedule(e); //call the main scheduler function to check if the CPU is idle 
}

//Handles the event generated when an internal timer expires to signal that the process currently on the CPU has 
//	run out of time and must surrender the CPU. The process' state is saved and it is added back into the ready queue
//	Calls schedule() to check if CPU is idle and ready to run the next process in the ready queue
void Scheduler::handle_timer_expiration(const Event& e)
{
	_current_process->setState(READY); //show that process is stopping execution and going back to ready state

	//take the current process off the CPU, update its remaining CPU time, and add back to the ready queue
	int timeElapsed = e._time - _current_process->getRemainingCPUBurst();
	_current_process->setRemainingTime(timeElapsed);

	//calc the length of the process' next CPU burst
	_current_process->setNextCPUBurst();

	//put the process back in the ready queue
	ready_queue.push(_current_process);

	//get next process on the CPU
	_current_process = NULL;
	CPU_is_idle = true;
	schedule(e); //call the scheduler to dispatch the next process
}

//Calculates waiting time for processes that did not complete their full execution during the simulation
// @param endTime - provide the time at which the simulation ended execution in the driver program
void Scheduler::calcUnfinishedWait(int endTime)
{
	//for every PCB in the system
	for (int i = 0; i < _count; i++)
	{
		//if the process did not complete
		if (_table[i]->getState() != TERMINATED) //a process has its state set to TERMINATED when it completes its entire CPU time (i.e. finishes execution)
		{//calculate its time in the ready queue as: (endTime - arrival time - service time - IO time)
			_table[i]->sWaitingTime = endTime - _table[i]->getStartTime() - _table[i]->sServiceTime - _table[i]->sIOTime;
		}
	}
}

//Prints statistics for each process that ran on the virtual system as well as overall statistics for the
//	DES algorithm
void Scheduler::printStats(int endTime, int totalProcesses)
{
	int sAvgTurnaround = 0; //ensure containers are empty before summing up times
	int sAvgWait = 0; //ensure containers are empty before summing up times
	int sCPU_util = 0; //ensure containers are empty before summing up times
	int totalProcessesCompleted = 0;

	//Loop to print the statistics for each individual process and sum up average turnaround and wait times
	for (int i = 0; i <= _count; i++)
	{
		std::cout << "-----------------------" << std::endl;
		_table[i]->printStats();
		std::cout << "-----------------------" << std::endl;

		//Sum up average stats turnaround and waiting time, as well as total service time to calculate CPU Utilization
		sAvgTurnaround += _table[i]->sTurnaroundTime;
		sAvgWait += _table[i]->sWaitingTime;
		sCPU_util += _table[i]->sServiceTime;
		if (_table[i]->getState() == TERMINATED)
			totalProcessesCompleted++;
	}

	//Print overall stats for the algorithm simulation
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "=================================" << std::endl;
	std::cout << "Overall algorithm statistics:" << std::endl;
	std::cout << "CPU Utilization: " << ((float)sCPU_util/endTime)/.01 << "%" << std::endl;
	std::cout << "Throughput: " << (float)sThroughput/(endTime/1000.0f) << " jobs / s (" << totalProcessesCompleted << " jobs completed in " << (float)endTime/1000 << " s)" << std::endl;
	
	if(totalProcessesCompleted == 0)
		std::cout << "Average Turnaround time: No processes completed execution" << std::endl;
	else
		std::cout << "Average Turnaround time: " << (float)(sAvgTurnaround/1000.0f)/totalProcessesCompleted << " s" <<  std::endl;
	
	std::cout << "Average Waiting time: " << (float)(sAvgWait/1000.0f)/totalProcesses << " s" << std::endl;
	std::cout << "=================================" << std::endl;

	//debugging
	//std::cout << "sAvgTurnaround: " << sAvgTurnaround << std::endl;
	//std::cout << "totalProcessesCompleted: " << totalProcessesCompleted << std::endl;
}
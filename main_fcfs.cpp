/**************************************************************************************************************
Written by: Alan Doose
CS 433 HW 3
Oct 31 2018

Purpose: This program simulates a FCFS scheduling algorithm over a span of 5 minutes (simulated) while
	collecting statistics to evaluate the algorithm. The user will specify the number of processes to 
	simulate executing on the virtual system via a command line argument when executing the program.
		i.e. in a unix environment: "./a.out 20"
	The program will display a promt before beginning the simulation, showing the number of processes
	that were entered and telling the user to press the enter key to begin. Once the simulation is complete,
	the program will print out statistics for each individual process followed by a block of statistics on the
	overall performance of the simulation.

	*Note that while the number of processes to simulate can be specified, this program (in its current state) 
	does not allow the user to specify the details and times for each process - these are assigned randomly when 
	each simulated process is created.

Assumptions: This driver file depends on multiple classes and it is assumed that it will be compiled with those
	class header and implementation files in the same directory. The classes are:
		random.h
		pcb.h - depends on random.h
		PCBTable.h - depends on pcb.h
		Event.h - depends on pcb.h
		Scheduler.h - depends on PCBTable.h and Event.h
*************************************************************************************************************/


#include <iostream>
#include <queue>
#include "random.h"
#include "Event.h"
#include "Scheduler.h"

#define MAX_TIME 300000 //max time for simulation in milliseconds, 300000 ms = 5 mins

//Prototypes
void handle_the_event(const Event& e, Scheduler& scheduler);

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		std::cout << "To run this program, specify an integer amount of processes to simulate with:" << std::endl;
		std::cout << "e.g.: ./doose 20" << std::endl;
		exit(1);
	}

	std::cout << "Alan Doose" << std::endl;
	std::cout << "This is a FCFS Scheduler simulation that will run for approximately 5 minutes (simulated)" << std::endl;
	std::cout << "Program running with " << argv[1] << " processes." << std::endl;
	std::cout << "Performance statistics will print at the end" << std::endl;
	std::cout << "Press enter key to begin...";
	std::cin.get();

	int pcbTableSize = atoi(argv[1]); //convert command line argument to int

	//create scheduler with a PCB table sized to match command line parameter
	Scheduler scheduler(pcbTableSize); //Note - scheduler also contains the event priority queue
	
	//fill the scheduler's internal PCB table with random Processes
	if (!scheduler.init()) //init() fills the PCBTable in the scheduler with random processes
	{
		std::cout << "Error initializing Scheduler in main, aborting..." << std::endl;
	}
	else
	{	
		//seed the event queue with process arrival events (1 for each process in the scheduler's PCBTable)
		for (int i = 0; i < scheduler.getCapacity(); i++)
		{
			Event temp(scheduler[i], process_arrival);
			scheduler.event_queue.push(temp);
		}

		/* SIMULATION BEGINS HERE */
		int quitTime = MAX_TIME; //default 5 mins or 300,000 ms
		int currentTime = 0;
		Event e; //container for events removed from the event queue

		//The main DES Loop
		while (!scheduler.event_queue.empty() && currentTime < quitTime)
		{
			//take the next event
			e = scheduler.event_queue.top(); 
			scheduler.event_queue.pop();

			currentTime = e._time; //set the current time to that of the next event

			handle_the_event(e, scheduler); //handle the next event
		}

		//calculate waiting time for processes that did not complete their entire CPU time (if a process completed its entire CPU time its waiting time is calculated at completion time)
		scheduler.calcUnfinishedWait(currentTime);

		//Print all the simulation statistics for the algorithm
		scheduler.printStats(currentTime, pcbTableSize);

		std::cout << std::fixed << std::setprecision(2);
		std::cout << "Simulation completed at time: " << (float)currentTime / 1000.0f << " s" << std::endl;

	}

	return 0;
}

//This function processes events sent to it from the event queue by dispatching the 
//	scheduler to handle the specific event type using a simple switch statement
// \const Event& e: the event to be handled
// \Scheduler& scheduler: the scheduler that is running the simulation in main
void handle_the_event(const Event& e, Scheduler& scheduler)
{
	switch (e._type)
	{
	case process_arrival: scheduler.handle_proc_arrival(e);
		break;
	case cpu_burst_completion: scheduler.handle_CPU_completion(e);
		break;
	case io_completion: scheduler.handle_IO_completion(e);
		break;
	case timer_exp: scheduler.handle_timer_expiration(e);
		break;

	default: std::cout << "Invalid event type!\n\n";  break;
	}
}
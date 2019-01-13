/***********************************************************************
/  Written by: Alan Doose
/  27 October 2018
/  CS 433 Assignment 3
/
/  Purpose:
/   This is the PCB class implementation file. The purpose of this class is to
/   provide a container that mimics a process in a computer system.
/
/  Assumptions:
/   This class does not check for exceptions when setting the values of
/   the pid, priority, or state. It is assumed the compiler will type
/   check these values in any client program, and that a client will choose
/   sane values for the integer IDs and priority values.
/
*************************************************************************/

#include "pcb.h"

//Static member variables
int pcb::min_priority = 50;
int pcb::max_priority = 1;
int pcb::nextPid = 1;

//Default constructor - creates a Process with random values as specified in assignment
pcb::pcb() : _pid(nextPid++), _start_time(ranInt(0, 300000)), _total_duration(ranInt(1000, 60000))
{
  _state = NEW;
  _remaining_cpu_burst = _total_duration; //at creation remaining CPU time is equal to total CPU time
  _avg_cpu_burst = ranInt(5, 100); //per specification this is a random time between 5-100 ms
  _next_cpu_burst = _io_burst = 0; //these will be determined by a Scheduler

  //Initialize all statistic data to 0
  sFinishTime = sServiceTime = sIOTime = sTurnaroundTime = sWaitingTime = 0;
}

//This overloaded constructor is used for creating a PCB with exact start time, 
//  duration, and avg CPU burst length
pcb::pcb(int startTime, int totDuration, int avgCPUBurst) : _pid(nextPid++), _start_time(startTime), _total_duration(totDuration)
{
	_state = NEW;
	_avg_cpu_burst = avgCPUBurst;
	_next_cpu_burst = _io_burst = 0; //these will be determined by a Scheduler

	//Initialize all statistic data to 0
	sFinishTime = sServiceTime = sIOTime = sTurnaroundTime = sWaitingTime = 0;
}

//Destructor (nothing to do)
pcb::~pcb()
{ }

//sets the next CPU burst for a process based on its average CPU burst - should be called by a scheduler
void pcb::setNextCPUBurst()
{
	_next_cpu_burst = CPUBurstRandom(_avg_cpu_burst);
}

//sets a random IO Burst length between min-max in ms - should be called by a scheduler
void pcb::setIOBurst(int min, int max)
{
	_io_burst = ranInt(min, max); //random duration betwen min-max in ms
}

void pcb::executeCPUBurst()
{
	_state = RUNNING;
	//if the next CPU burst is more than the time the process needs to complete, reduce the next cpu burst to just complete the process and not hold the CPU longer than necessary
	if (_next_cpu_burst > _remaining_cpu_burst)
	{
		_next_cpu_burst = _remaining_cpu_burst;
	}

	sServiceTime += _next_cpu_burst; //add time process runs on CPU to statistic
	_remaining_cpu_burst -= _next_cpu_burst; //process uses its CPU time
}

void pcb::setRemainingTime(int timeExpired)
{
	_remaining_cpu_burst -= timeExpired;
}

//Returns pcb state as a C++ string
std::string pcb::getStateString() const
{
  switch(_state)
    {
    case NEW: return "New"; break;
    case READY: return "Ready"; break;
    case RUNNING: return "Running"; break;
    case WAITING: return "Waiting"; break;
    case TERMINATED: return "Terminated"; break;
    default: return "Invalid State!"; //error handle
    }
}

//Prints the pcb info on a single line
void pcb::printProcess() const
{
  std::cout << "PID: " << std::left << std::setw(10) << _pid
	    << "State: " << std::left << std::setw(12) << getStateString() << std::endl;
}

void pcb::printStats() const
{
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "Process " << _pid << ":" << std::endl;
	std::cout << "arrival time: " << (float)_start_time / 1000 << " s" <<  std::endl;

	if(sFinishTime > 0)
		std::cout << "finish time: " << (float)sFinishTime / 1000 << " s" << std::endl;
	else
		std::cout << "finish time: Process did not finish" << std::endl;
	
	std::cout << "service time: " << (float)sServiceTime / 1000 << " s" << std::endl;
	std::cout << "I/O time: " << (float)sIOTime / 1000 << " s" << std::endl;

	if (sFinishTime > 0)
		std::cout << "turnaround time: " << (float)sTurnaroundTime / 1000 << " s" << std::endl;
	else
		std::cout << "turnaround time: Process did not finish, required " << (float)(_total_duration- sServiceTime)/1000.0f << " s more service time to finish" << std::endl;
	
	
	std::cout << "waiting time: " << (float)sWaitingTime / 1000 << " s" << std::endl;
	//std::cout << "total cpu time: " << (float)_total_duration / 1000 << " s" << std::endl; //debugging
}
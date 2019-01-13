/***********************************************************************
/  Written by: Alan Doose
/  27 October 2018
/  CS 433 Assignment 3
/
/  Purpose:
/   This is the PCB class header file. The purpose of this class is to 
/   provide a container that mimics a process in a computer system.
/
/  Assumptions:
/   This class does not check for exceptions when setting the values of 
/   the pid, priority, or state. It is assumed the compiler will type
/   check these values in any client program, and that a client will choose
/   sane values for the integer IDs and priority values.
/
*************************************************************************/

#ifndef PCB_H
#define PCB_H
#include <string>
#include <iostream>
#include <iomanip>
#include "random.h"

//valid process states
enum STATE {
  NEW,
  READY,
  RUNNING,
  WAITING,
  TERMINATED
};

class pcb
{
 public:
  pcb();                      //default constructor sets pid to nextPid and priority to random value in range of min_priorty to max_priority
  pcb(int startTime, int totDuration, int avgCPUBurst);
  ~pcb();
							/***** Setters *****/
  void setState(STATE state) { _state = state; } //sets state of pcb to given state NEW, READY, etc.
  void setNextCPUBurst();		//sets the next CPU burst for a process based on its internal average CPU burst which was set at creation
  void setIOBurst(int min, int max); //sets a random IO Burst length between min-max in ms
  void executeCPUBurst(); //The process "uses" its CPU time and changes its internal state timers accordingly - returns the time at which it completes its CPU burst
  void setRemainingTime(int timeExpired); //this is used when a process is pulled off the CPU early due to preemption or timer expiration
							/***** Getters *****/
  std::string getStateString() const;		 //returns state as a string
  STATE getState() const { return _state; }  //returns state as enum
  int getID()  const { return _pid; }             //returns the pid of the pcb
  int getStartTime() const { return _start_time; }
  int getTotalDuration() const { return _total_duration; }
  int getRemainingCPUBurst() const { return _remaining_cpu_burst; }
  int getAvgCPUBurst() const { return _avg_cpu_burst; }
  int getNextCPUBurst() const { return _next_cpu_burst; }
  int getIOBurst() const { return _io_burst; }

  //prints the ID, priority, and state of the process on one line
  void printProcess() const;
  void printStats() const;

  static int min_priority; //set to 50 in implementation file
  static int max_priority; //set to 1 in implementation file
  
	/* Statistics Variables  - all times collected as ms, convert to seconds for printing */
  int sFinishTime;
  int sServiceTime;
  int sIOTime;
  int sTurnaroundTime;
  int sWaitingTime;

 private:
	 /* Variable data that can change during simulation */
  STATE _state;				//process state - will be NEW on creation, and READY when it enters a ready queue, etc
  int _remaining_cpu_burst;	//real-time value of remaining cpu burst time - when reaches 0, process is terminated
  int _avg_cpu_burst;		//length of process' next cpu burst
  int _next_cpu_burst;		//length of this process' next cpu burst
  int _io_burst;			//generated when a process completes cpu burst and moves into io queue
  
  //int _time_waiting; //will collect the time that the process waits in the ready queue

	/* Constant data */
  const int _pid;			//process ID
  const int _start_time;	//arrival time of the process in ms
  const int _total_duration; //total cpu burst length in ms

	

  // Private static for giving each process a unique ID#
  static int nextPid;      //starts at 1 and is incremented when processes are created
};
#endif

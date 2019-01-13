/***************************************************************************
/  Written by: Alan Doose
/  27 October 2018
/  CS 433 Assignment 3
/
/  Purpose:
/   This is the PCBTable class header file. The purpose of this class is
/   to provide a container that holds pcbs (see pcb.h) and mimic a process
/   control block table in a computer system.
/
/	This class is publicly inherited by Scheduler.h
/
/  Assumptions:
/   This class is NOT templated and will only hold pointers to pcb objects
/   (see my pcb.h). This class is inherited by Scheduler.h
/
***************************************************************************/

#ifndef PCBTABLE_H
#define PCBTABLE_H

#include "pcb.h"
#include <iostream>
#include <iomanip>

class PCBTable
{ 
 public:
  PCBTable();               //Default - creates a PCB table with capacity == DEFAULT_CAP (see below)
  PCBTable(int capactity);  //Overloaded - creates a PCB table with given capacity
  ~PCBTable();              //Destructor
  
  int getSize() const;      //Returns the qty of pcbs in the table
  bool addToTable(pcb* proc);      //Adds a pointer to a pcb into the table at the end. Returns true on success, false otherwise (ie duplicate pid or table full)
  void printTable() const;  //Prints the contents of the pcb table
  void clearTable();        //Remove all the entries in the table
  void resetAllProcesses(); //"Resets" all  pcb's, changing their state to NEW
  int giveRandomProc();     //Returns the index of a random proc that is not already in a queue
  int getCapacity() const;
  

  //pcb table indexes are accessed with 0 through _count in my implementation of overloaded bracket operators
  pcb* operator[](int i); //Returns a pointer to the pcb at index i in the table
 
 protected:
#define DEFAULT_CAP 100 //Max size of table by default

  pcb** _table;  //Array of pointers to pcb's
  int _capacity; //Max capacity of table
  int _count;    //Current count of pcb's in the table

  void destroyTable(); //Called by the Destructor to clean up pointers
};
#endif

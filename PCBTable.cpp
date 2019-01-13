/***************************************************************************
/  Written by: Alan Doose
/  27 October 2018
/  CS 433 Assignment 3
/
/  Purpose:
/   This is the PCBTable class implementation file. The purpose of this class is
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

#include "PCBTable.h"

//Default constructor initializes the count to -1 to indicate the table is empty on creation
// Creates a table with DEFAULT_CAP capacity (set in header)
PCBTable::PCBTable()
{
  _count = -1; //-1 indicates empty table
  _capacity = DEFAULT_CAP;
  _table = new pcb*[DEFAULT_CAP];
}

//Overloaded constructor creates a table with given capacity
PCBTable::PCBTable(int capacity)
{
  _count = -1;
  _capacity = capacity;
  _table = new pcb*[capacity];
}

//Destructor calls destroyTable() to clean up pointers before going out of scope
PCBTable::~PCBTable()
{
  destroyTable();
}

//Cleanup pcb pointers
void PCBTable::destroyTable()
{
  for(int i = 0; i < _count; i++)
    {
      delete _table[i];
	  _table[i] = NULL;
    }
}



//Adds a pcb entry to the table, checking that the table is not full
// returns true on success, false otherwise
bool PCBTable::addToTable(pcb* proc)
{
  if(_count > _capacity)
    {
      return false;
    }
  else
    {
      _table[++_count] = proc;
      return true;
    }
}

//Prints every pcb in the table, one entry per line
void PCBTable::printTable() const
{
  std::cout << "============= Printing pcb table =============\n";

  for(int i = 0; i <= _count; i++)
    {
      _table[i]->printProcess();
    }

  std::cout << "==============================================\n\n";
}

//Allows accessing entries in the table with bracket operators, similar to an array - however:
// Caller supplies real index (i.e. 1 for first entry, 2 for second, etc)
pcb* PCBTable::operator[](int i)
{
  if(i >= 0 && i <= _capacity)
    return _table[i];
  else return NULL;
}

//Resets state of all pcb's in table to NEW
void PCBTable::resetAllProcesses()
{
  for(int i = 0; i < _count; i++)
    {
      _table[i]->setState(NEW);
    }
}

//Returns an index of a random process from the table which is not currently in a ready queue
// A pcb has READY state if it is already in a queue
int PCBTable::giveRandomProc()
{
  int openArray[DEFAULT_CAP]; //array to hold indices of all pcbs in the table which are not in a queue. Max size will be _count which is if every pcb in the table is not in a ready queue
  int totalOpen = 0; //quantity of pcbs in the openArray after checking the table
  
  for(int i = 0; i < _count; i++)
  {
      if(_table[i]->getState() != READY)
		{
			openArray[totalOpen++] = i; //add the index of the non-Ready pcb into the openArray and increment the amount of total pcbs that are not in a queue
		}
  }

  if(totalOpen == 0) //if totalOpen was never incremented, all pcb's are already in a queue
    return -1; //-1 indicates all processes in table are READY i.e. already in a queue
  else //else there is at least one process in the table that is not ina queue
    {
      int randomIndex = rand() % totalOpen; //random index from 0 to totalOpen-1
      return (openArray[randomIndex] + 1); //return the table index + 1 (clients access table indexes with overloaded [] operators using 1 through _count as index
    }
}

//Returns number of entries in the table
int PCBTable::getSize() const
{
	return _count + 1; //count is -1 to indicate empty, 0 to indicate one item in list, etc
}

int PCBTable::getCapacity() const
{
	return _capacity;
}

//Removes all pcbs from the table - this is public and not called by the destructor
void PCBTable::clearTable()
{
  for(int i = 0; i < _count; i++)
    {
      delete _table[i];
      _count--;
    }

#if 0 //debugging
  if(_count == -1)
    std::cout << "Table Cleared\n";
  else std::cout << "Table count still: " << _count << std::endl;
#endif
}

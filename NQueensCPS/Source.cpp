#include <time.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <mutex>
#include <omp.h>
using namespace std;


const int n = 6; //The N in N-Queens

int threadcount =n; // number of threads to use for processing
int runs = 10; //number of times to run algorithm for performance testing.



struct spile
{
	void Add(vector<int> c)
	{
		lock_guard<std::mutex> guard(mutex);
		s.push_back(c);
	}

	void Clear()
	{
		s.clear();
	}

	bool Contains(vector<int> c)
	{
		lock_guard<std::mutex> guard(mutex);
		if (std::find(s.begin(), s.end(), c) != s.end())
		{
			return true;
		}
		else
		{
			s.push_back(c);
			return false;
		}
	}

	void Combine(vector<vector<int>> q)
	{
		lock_guard<std::mutex> guard(mutex);
		for(vector<int> var : q)
		{
			s.push_back(var);
		}
	}

	int Count()
	{
		return s.size();
	}
private:
	std::mutex mutex;
	vector<vector<int>> s;

};

spile results;

bool CheckDiagonals(int x, int y, vector<int> q)
{
	
	for (int i =1; i <= x; i++)
	{
		if (q[x - i] == y - i || q[x - i] == y + i)
		{
			return false;
		}
	}

	return true;

}

int sum(bool q[n], int x)
{
	int s = 0;
	for (int i = 0; i < x; i++)
	{
		if (q[i])
		{
			s++;
		}
	}
	return s;
}


void Navigate(int p)
{
	vector<vector<int>> solutions; //store the solutions found by this thread
	bool finished = false; //flag if this thread is finished
	bool taken[n] = { false }; // store if this row/column has been populated
	bool tried[n][n] = { false }; //store the squares that have been checked
	vector<int> queens; //store where we have placed a queen

	queens.push_back(p);
	taken[p] = true;
	
	int x = 1, y = 0; //initialise navigation variables

	//select a random square
	
	while (!finished)
	{
		y = rand() % n;		
		//if the row is clear and the square has not been tried	
		if (!taken[y] && !tried[x][y])
		{
			tried[x][y] = true; //mark the square as tried

			if (CheckDiagonals(x, y, queens)) //if the diaganols are clear 
			{
				//place the queen
				queens.push_back(y); 
				//mark the row as taken
				taken[y] = true; 
				//move on to the next column
				x++;
				if (x == n) //if we are at the end of the board
				{
					if (std::find(solutions.begin(), solutions.end(), queens) != solutions.end()) //if we have found the solution before
					{
						//remove the last queen we placed
						taken[queens.back()] = false;
						queens.pop_back();
						
						//step back
						x--;
					}
					else //if we have not found this solution before
					{
						//add the solution to the list	
						solutions.push_back(queens);
						//reset and find another solution
						queens.clear();
						for (int i = 0; i < n; i++) //y values
						{
							taken[i] = false;
							for (int j = 1; j < n; j++) //x values
							{
								tried[j][i] = false;
							}
						}
						queens.push_back(p);
						taken[p] = true;
						x = 1;
					}
				}
			}
		}

		if (sum(tried[x], n) + sum(taken, n) == n) //if we can not try any more locations
		{
			if (x == 1) { finished = true; }
			taken[queens.back()] = false;

			queens.pop_back();//remove the last queen we placed

			for (int i = 0; i < n; i++)
			{
				tried[x][i] = false;
			}

			x--; //step back

		}
	}

	results.Combine(solutions);
	
}



int main(int argc, char** argv)
{
	float meanTime = 0;
	for(int r= 0; r<runs; r++)
	{
	auto start = chrono::high_resolution_clock::now();
/*
	thread threads[n];
	for (int i = 0; i < n; i++)
	{
		threads[i] = thread(Navigate, i);

	}

	for (int i = 0; i < n; i++)
{
	Navigate(i);
}

	for (auto& t : threads)
	{
		t.join();
	}
	
	
	*/
	

#pragma omp parallel for num_threads(threadcount)
	for (int i = 0; i < n; i++)
	{
		Navigate(i);
	}	
	auto end = chrono::high_resolution_clock::now();
	auto time = end - start;

	cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << endl;
	
	results.Clear();
}
	cout << "Mean Time: " << meanTime << endl;
		return 0;
	
}




	//check if row is free, and if the if the square has been tried before

	//mark position as tried

	//check if diaganols are free

	//add queen to the solution

	//set row as used

	//increment the column we are working on


//function to check for diagonal queens

//function to check for queens on same row

//check for column queens



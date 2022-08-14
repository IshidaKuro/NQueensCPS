#include <time.h>
#include <stdio.h>
#include <iostream>

#include <vector>
#include <mutex>
#include <omp.h>
#include <cuda_runtime.h>
#include <curand.h>
#include <curand_kernel.h>

#include ""

using namespace std;


const int n = 12;

int threadcount = n;
int runs = 10;

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
			return false;
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


spile solutions;

 bool CheckDiagonals(int x, int y, vector<int> q)
{

	for (int i = 1; i <= x; i++)
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


__global__ void kernel(int p)
{
	bool finished = false;
	bool taken[n] = { false }; // store if this row/column has been populated
	bool tried[n][n] = { false }; //store the squares that have been checked
	vector<int> queens;

	queens.push_back(p);
	taken[p] = true;
	for (int i = 0; i < n; i++)
	{
		tried[0][i] = true;
	}
	int x = 1, y = 0; //initialise navigation variables

	//select a random square

	while (!finished)
	{
		y = rand() % n;
		//cout << "(" << x << ", " << y << ")" << endl;
		//if the row is clear and the square has not been tried	
		if (!taken[y] && !tried[x][y])
		{
			tried[x][y] = true; //mark the square as tried


			if (CheckDiagonals(x, y, queens)) //if the diaganols are clear, place the queen and move on to the next column
			{
				queens.push_back(y);
				taken[y] = true;
				x++;
				y = 0;
				if (x == n) //if we are at the end of the board
				{
					if (solutions.Contains(queens))
					{
						taken[queens.back()] = false;

						queens.pop_back();//remove the last queen we placed

						x--; //step back
					}
					else
					{
						//add the solution to the list	
						solutions.Add(queens);
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

}



int main(int argc, char** argv)
{
	for (int r = 0; r < runs; r++)
	{
		auto start = chrono::system_clock::now();
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


		auto end = chrono::system_clock::now();

		cout << "N=" << n << endl;
		cout << "number of solutions = " << solutions.Count() << endl;
		cout << "time taken: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << endl;
		solutions.Clear();
	}
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



#include <time.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <mutex>
#include <omp_llvm.h>

using namespace std;

const int n = 12;// The N in N - Queens
const int threadcount = 16; //number of threads to target
const int runs = 10; //number of times to run algorithm for performance testing.

vector<vector<int>> solutions; //somewhere to store our solutions

//helper function to make sure diagonal squares are not occupied
bool CheckDiagonals(int x, int y, vector<int> q)
{	
	for (int i =x; i >= 1; i--)
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
	for (int i = n - 1; i >= 0; i--)
	{
		if (q[i])
		{
			s++;
		}
	}
	return s;
}


int TryNext(bool t[n], bool x[n])
{
	for (int i = n-1; i >=0; i--)
	{
		if (!t[i]&&!x[i]) { return i; }
	}
	return -2;
}

void NavigateNoRand(int p, int q) {
	bool finished = false;
	bool taken[n] = { false }; // store if this row/column has been populated
	bool tried[n][n] = { {false} }; //store the squares that have been checked


	vector<int> queens;
	vector<vector<int>> s;

	queens.push_back(p);
	queens.push_back(q);

	taken[p] = true;
	taken[q] = true;
	
	//initialise navigation variables
	int x = 2, y = 0; 

	while (!finished)
	{

		//select the next square 
		y = TryNext(taken,tried[x]);
		//if we can try a square
		if (y != -2)
		{
			tried[x][y] = true; //mark the square as tried
			if (CheckDiagonals(x, y, queens)) //if the diaganols are clear, place the queen and move on to the next column
			{
				queens.push_back(y);
				taken[y] = true;
				x++;
				if (x == n) //if we are at the end of the board
				{
					//if the solution has already been recorded
					if (find(s.begin(), s.end(), queens) != s.end())
					{
						taken[queens.back()] = false;
						queens.pop_back();//remove the last queen we placed
						x--; //step back
					}
					//if this is a new solution
					else
					{
						//add the solution to the list	
						s.push_back(queens);
						//reset and find another solution
						queens.clear();
						for (int i = n - 1; i >= 0; i--) //y values
						{
							
								taken[i] = false;
							
							for (int j = n - 1; j >= 2; j--) //x values
							{
								tried[j][i] = false;
							}
						}
						queens.push_back(p);
						queens.push_back(q);
						taken[p] = true;
						taken[q] = true;
						
						

						x = 2;
					}
				}

			}
		}

		//if we can not try any more locations
		else{

			//if we are back at the start and can't try any other squares, we are finished
			if (x ==2)
			{
				finished = true;
			}

			//remove the last queen we placed
			taken[queens.back()] = false;
			queens.pop_back();

			//reset the squares that have been tried
			for (int i = n - 1; i >= 0; i--)
			{
				tried[x][i] = false;
			}

			x--; //step back

		}
	}
#pragma omp critical
	{
		for (vector<int> v : s)
		{
			solutions.push_back(v);
		}
	}
}

void Navigate(int p, int q)
{
	bool finished = false;
	bool taken[n] = { false }; // store if this row/column has been populated
	bool tried[n][n] = { {false} }; //store the squares that have been checked

	vector<int> queens;
	vector<vector<int>> s;
	queens.reserve(n);

	queens.emplace_back(p);
	queens.emplace_back(q);

	taken[p] = true;
	taken[q] = true;

	for (int i = 0; i < n; i++)
	{
		tried[0][i] = true;
		tried[1][i] = true;
	}
	int x = 2, y = 0; //initialise navigation variables

	while (!finished)
	{
		//select a random square
		y = rand() % n;
		//if the row is clear and the square has not been tried	
		if (!taken[y] && !tried[x][y])
		{
			tried[x][y] = true; //mark the square as tried

			if (CheckDiagonals(x, y, queens)) //if the diaganols are clear, place the queen and move on to the next column
			{
				queens.emplace_back(y);
				taken[y] = true;
				x++;
				if (x == n) //if we are at the end of the board
				{
					//if the solution has already been recorded
					if (find(s.begin(), s.end(), queens) != s.end())
					{
						taken[queens.back()] = false;

						queens.pop_back();//remove the last queen we placed

						x--; //step back
					}

					//if this is a new solution
					else
					{
						//add the solution to the list	
						s.emplace_back(queens);
						//reset and find another solution
						queens.clear();
						for (int i = 0; i < n; i++) //y values
						{
							taken[i] = false;
							for (int j = 2; j < n; j++) //x values
							{
								tried[j][i] = false;
							}
						}

						queens.emplace_back(p);
						queens.emplace_back(q);
						taken[q] = true;
						taken[p] = true;

						x = 2;
					}
				}
			}
		}

		//if we can not try any more locations
		if (sum(tried[x], n) + sum(taken, n) == n)
		{
			if (x == 2)
			{
				finished = true;
			}
			taken[queens.back()] = false;

			queens.pop_back();//remove the last queen we placed

			for (int i = 0; i < n; i++)
			{
				tried[x][i] = false;
			}

			x--; //step back

		}
	}
#pragma omp critical
	{
		for (vector<int> x : s)
		{



			solutions.push_back(x);

		}
	}
}




int main(int argc, char** argv)
{
	//for (threadcount; threadcount < 17; threadcount *= 2)
	{
		long meantime = 0;
		for (int r = runs - 1; r >= 0; r--)
		{
			auto start = chrono::system_clock::now();

#pragma omp parallel for schedule(static) num_threads(threadcount) collapse(2)
			for (int i = n - 1; i >= 0; i--)
			{
				for (int j = n - 1; j >= 0; j--)
				{
					if (j<i - 1 || j>i + 1)
					{
						NavigateNoRand(i, j);
					}
				}
			}
			auto end = chrono::system_clock::now();
			meantime += chrono::duration_cast<chrono::microseconds>(end - start).count();
			
			//debug, output the number of solutions found this run
			//std::cout << "Solutions Found = " << solutions.size() << " Time Taken = " << chrono::duration_cast<chrono::microseconds>(end - start).count() << endl;
			
			solutions.clear();
		}
		std::cout << "Threads = " << threadcount << " Mean time to complete =" << meantime / runs << " microseconds" << endl;
	}
		return 0;
	
}
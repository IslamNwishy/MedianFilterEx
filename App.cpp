#include <iostream>
#include <string>
#include <fstream>
using namespace std;

//Name: Islam Osama Nwishy
//ID#: 900170200
//Assignment 6 (Bonus)
//File: App.cpp


//PS: The program only works properly on square images


//global input and output file streams
ifstream input;
ofstream output;

//Functions prototyping
void GetInput(int** &A, int& Arows, int& Acolmn);
void Median_Filter(int**& pic, int& S, int**& FinalPic, int& A_row, int& A_col);
void ZeroPad(int**& pic, int S, int**& B, int A_row, int A_col, int B_row, int B_col);
void GetRegion(int**& B, int S, int i, int j, int* &V);
void heapSort(int*& arr, int n);
void heapify(int*& arr, int n, int i);

void main() {
	
	//A is a 2D dynamic array that holds the values of the inputed pgm image
	//Arows and Acolmn hold the row and column values of the Array "A"
	//S is the filter size, F is the array that holds the values for the final Pic
	int** A, Arows, Acolmn, S = 3, ** F;

	//interface variables
	bool choice = true;
	char c;
	string filename;

	while (choice) {

		//Take and check the input file destination
		cout << endl << "Enter a .pmg pic file destination to filter" << endl;
		getline(cin, filename);
		input.open(filename);
		if (!input) {
			cout << "Couldn't open the file! Please Try again" << endl;
			input.close();
			continue;
		}

		//take and check the output file destination
		cout << "Enter an ouput file destination including the .pmg pic name" << endl;
		getline(cin, filename);
		output.open(filename);
		if (!output) {
			cout << "Couldn't open the file! Please Try again" << endl;
			output.close();
			continue;
		}

		//Store the input from the .pgm file to the array "A"
		GetInput(A, Arows, Acolmn);

		//Apply the filter
		Median_Filter(A, S, F, Arows, Acolmn);

		//Output the filtered picture
		output << "P2" << endl << "# After Filtering" << endl << Arows << " " << Acolmn << endl << "255" << endl;
		for (int i = 0; i < Arows; i++) {
			for (int j = 0; j < Acolmn; j++)
				output << F[i][j] << " ";
			output << endl;
		}
		output.close();

		//Delete the array A
		delete[] * A;
		delete[] A;
		
		//Delete the array F
		delete[] * F;
		delete[] F;

		//Check if the user wants another run
		cout << "Done!" << endl << endl << "Do you want to filter a new picture (yes is the default) [Y/N]" << endl;
		cin >> c;
		getline(cin, filename); //Dummy getline to getrid of cin leftovers

		//Close the program if the user does not want another run
		if (toupper(c) == 'N')
			choice = false;
	}
	

}


//Takes the input from the .pgm file, creates a 2D array of the correct size and stores the data into the array "A"
void GetInput(int** &A, int& Arows, int& Acolmn) {
	

	string word;								//a temporary variable to store the inputed data
	getline(input, word, '\n');					//Ignore the first 2 lines
	getline(input, word, '\n');

	input >> word;								//input and store the rows size
	Arows = atoi(word.c_str());

	input >> word;								//input and store the columns size
	Acolmn = atoi(word.c_str());
	getline(input, word, '\n');

	A = new int* [Arows];						//Create the 2D array with rows = Arows and columns = Acolmn
	for (int i = 0; i < Arows; i++)
		A[i] = new int[Acolmn];

	getline(input, word, '\n');					//Ignore the 4th line


	for(int i=0; i<Arows; i++)					//Store the color intensity values into the array "A" 
		for (int j = 0; j < Acolmn; j++) {
			input >> word;
			A[i][j] = atoi(word.c_str());
		}

	input.close();								//Close the file
}


//Applies the filter
void Median_Filter(int**& pic, int& S, int **& FinalPic, int& A_row, int& A_col) {

	int D = S / 2;											
	int B_row = A_row + 2 * D;								//row size of array B
	int B_col = A_col + 2 * D;								//Column size of array B

	FinalPic = new int* [A_row];							//Create the 2D array that will hold the final picture
	for (int i = 0; i < A_row; i++)
		FinalPic[i] = new int[A_col];

	int** B;								//Create the 2D array B that will be the padded version of the original image
	B = new int* [B_row];
	for (int i = 0; i < B_row; i++)
		B[i] = new int[B_col];



	int* V;													//Create the 1D array V that will hold each SxS square to sample
	int V_size = S * S;
	V = new int[V_size];
	int index_of_median = V_size / 2;						//V's median

	ZeroPad(pic, S, B, A_row, A_col, B_row, B_col);			//store values in B

	for (int i = 0; i < A_row; i++)							//Run through each pixel and apply the filter to it
		for (int j = 0; j < A_col; j++) {
			GetRegion(B, S, i, j, V);
			heapSort(V, V_size);
			FinalPic[i][j] = V[index_of_median];
		}
	
	
}


//Fills B with 0 and copies A to it
void ZeroPad(int**& pic, int S, int**& B, int A_row, int A_col, int B_row, int B_col) {
	
	//Fill B with 0
	for (int i = 0; i < B_row; i++) 
		for (int j = 0; j < B_col; j++) 
			B[i][j] = 0;
		
	//Copy A into B
	for (int i = (S/2); i < A_row; i++)
		for (int j = (S/2); j < A_col; j++)
			B[i][j] = pic[i - (S/2)][j - (S/2)];
	
}


//Gets the square of size SxS in the array B with the point (i,j) as its center and stores the square in the 1D array V
void GetRegion(int**& B, int S, int i, int j, int* &V) {
	
	//Ending points of the square
	int K = i + S;							
	int L = j + S;

	//RS and CS are the starting points of the square
	//loop through the given square and store it in V
	int m = 0;
	for (int RS = i; RS < K; RS++) {
		for (int CS = j; CS < L; CS++) {
			V[m] = B[RS][CS];
			m++;
		}
		
	}
}

//builds a heap and sorts it
//arr is a 1D array, n is the size of it
void heapSort(int* &arr, int n)
{
	// Build heap (rearrange array) 
	for (int i = n / 2 ; i > 0; i--)
		heapify(arr, n, i);

	// One by one extract an element from heap 
	for (int i = n; i > 0; i--)
	{
		// Move current root to end 
		swap(arr[0], arr[i]);

		// call max heapify on the reduced heap 
		heapify(arr, i, 0);
	}
}

// sorts a heap
//arr is a 1D array, n is the size of it, i is the initial value
void heapify(int*& arr, int n, int i)
{
	int largest = i; // Initialize largest as root 
	int l = 2 * i + 1; // left = 2*i + 1 
	int r = 2 * i + 2; // right = 2*i + 2 

	// If left child is larger than root 
	if (l < n && arr[l] > arr[largest])
		largest = l;

	// If right child is larger than largest so far 
	if (r < n && arr[r] > arr[largest])
		largest = r;

	// If largest is not root 
	if (largest != i)
	{
		swap(arr[i], arr[largest]);

		// Recursively heapify the affected sub-tree 
		heapify(arr, n, largest);
	}
}


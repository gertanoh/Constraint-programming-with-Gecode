#include <gecode/int.hh>
#include <gecode/driver.hh>
#include <gecode/gist.hh>
#include <gecode/minimodel.hh>
#include <time.h>
using namespace Gecode;


/*
 * Answers to questions are at the end of the file
 */

namespace {
	
static int examples[][9][9] = {
    {
	{0,0,0, 2,0,5, 0,0,0},
	{0,9,0, 0,0,0, 7,3,0},
	{0,0,2, 0,0,9, 0,6,0},
	
	{2,0,0, 0,0,0, 4,0,9},
	{0,0,0, 0,7,0, 0,0,0},
	{6,0,9, 0,0,0, 0,0,1},
    
	{0,8,0, 4,0,0, 1,0,0},
	{0,6,3, 0,0,0, 0,8,0},
	{0,0,0, 6,0,8, 0,0,0}
    },{
	{3,0,0, 9,0,4, 0,0,1},
	{0,0,2, 0,0,0, 4,0,0},
	{0,6,1, 0,0,0, 7,9,0},

	{6,0,0, 2,4,7, 0,0,5},
	{0,0,0, 0,0,0, 0,0,0},
	{2,0,0, 8,3,6, 0,0,4},
    
	{0,4,6, 0,0,0, 2,3,0},
	{0,0,9, 0,0,0, 6,0,0},
	{5,0,0, 3,0,9, 0,0,8}
    },{
	{0,0,0, 0,1,0, 0,0,0},
	{3,0,1, 4,0,0, 8,6,0},
	{9,0,0, 5,0,0, 2,0,0},
    
	{7,0,0, 1,6,0, 0,0,0},
	{0,2,0, 8,0,5, 0,1,0},
	{0,0,0, 0,9,7, 0,0,4},
    
	{0,0,3, 0,0,4, 0,0,6},
	{0,4,8, 0,0,6, 9,0,7},
	{0,0,0, 0,8,0, 0,0,0}
    },{	// Fiendish puzzle April 21, 2005 Times London
	{0,0,4, 0,0,3, 0,7,0},
	{0,8,0, 0,7,0, 0,0,0},
	{0,7,0, 0,0,8, 2,0,5},
    
	{4,0,0, 0,0,0, 3,1,0},
	{9,0,0, 0,0,0, 0,0,8},
	{0,1,5, 0,0,0, 0,0,4},
    
	{1,0,6, 9,0,0, 0,3,0},
	{0,0,0, 0,2,0, 0,6,0},
	{0,2,0, 4,0,0, 5,0,0}
    },{	// This one requires search
	{0,4,3, 0,8,0, 2,5,0},
	{6,0,0, 0,0,0, 0,0,0},
	{0,0,0, 0,0,1, 0,9,4},
    
	{9,0,0, 0,0,4, 0,7,0},
	{0,0,0, 6,0,8, 0,0,0},
	{0,1,0, 2,0,0, 0,0,3},
    
	{8,2,0, 5,0,0, 0,0,0},
	{0,0,0, 0,0,0, 0,0,5},
	{0,3,4, 0,9,0, 7,1,0}
    },{	// Hard one from http://www.cs.mu.oz.au/671/proj3/node5.html
	{0,0,0, 0,0,3, 0,6,0},
	{0,0,0, 0,0,0, 0,1,0},
	{0,9,7, 5,0,0, 0,8,0},

	{0,0,0, 0,9,0, 2,0,0},
	{0,0,8, 0,7,0, 4,0,0},
	{0,0,3, 0,6,0, 0,0,0},

	{0,1,0, 0,0,2, 8,9,0},
	{0,4,0, 0,0,0, 0,0,0},
	{0,5,0, 1,0,0, 0,0,0}
    },{ // Puzzle 1 from http://www.sudoku.org.uk/bifurcation.htm
	{1,0,0, 9,0,7, 0,0,3},
	{0,8,0, 0,0,0, 0,7,0},
	{0,0,9, 0,0,0, 6,0,0},
	{0,0,7, 2,0,9, 4,0,0},
	{4,1,0, 0,0,0, 0,9,5},
	{0,0,8, 5,0,4, 3,0,0},
	{0,0,3, 0,0,0, 7,0,0},
	{0,5,0, 0,0,0, 0,4,0},
	{2,0,0, 8,0,6, 0,0,9}
    },{ // Puzzle 2 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,0, 3,0,2, 0,0,0},
	{0,5,0, 7,9,8, 0,3,0},
	{0,0,7, 0,0,0, 8,0,0},
	{0,0,8, 6,0,7, 3,0,0},
	{0,7,0, 0,0,0, 0,6,0},
	{0,0,3, 5,0,4, 1,0,0},
	{0,0,5, 0,0,0, 6,0,0},
	{0,2,0, 4,1,9, 0,5,0},
	{0,0,0, 8,0,6, 0,0,0}
    },{ // Puzzle 3 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,0, 8,0,0, 0,0,6},
	{0,0,1, 6,2,0, 4,3,0},
	{4,0,0, 0,7,1, 0,0,2},
	{0,0,7, 2,0,0, 0,8,0},
	{0,0,0, 0,1,0, 0,0,0},
	{0,1,0, 0,0,6, 2,0,0},
	{1,0,0, 7,3,0, 0,0,4},
	{0,2,6, 0,4,8, 1,0,0},
	{3,0,0, 0,0,5, 0,0,0}
    },{ // Puzzle 4 from http://www.sudoku.org.uk/bifurcation.htm
	{3,0,5, 0,0,4, 0,7,0},
	{0,7,0, 0,0,0, 0,0,1},
	{0,4,0, 9,0,0, 0,3,0},
	{4,0,0, 0,5,1, 0,0,6},
	{0,9,0, 0,0,0, 0,4,0},
	{2,0,0, 8,4,0, 0,0,7},
	{0,2,0, 0,0,7, 0,6,0},
	{8,0,0, 0,0,0, 0,9,0},
	{0,6,0, 4,0,0, 2,0,8}
    },{ // Puzzle 5 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,0, 7,0,0, 3,0,0},
	{0,6,0, 0,0,0, 5,7,0},
	{0,7,3, 8,0,0, 4,1,0},
	{0,0,9, 2,8,0, 0,0,0},
	{5,0,0, 0,0,0, 0,0,9},
	{0,0,0, 0,9,3, 6,0,0},
	{0,9,8, 0,0,7, 1,5,0},
	{0,5,4, 0,0,0, 0,6,0},
	{0,0,1, 0,0,9, 0,0,0}
    },{ // Puzzle 6 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,0, 6,0,0, 0,0,4},
	{0,3,0, 0,9,0, 0,2,0},
	{0,6,0, 8,0,0, 7,0,0},
	{0,0,5, 0,6,0, 0,0,1},
	{6,7,0, 3,0,1, 0,5,8},
	{9,0,0, 0,5,0, 4,0,0},
	{0,0,6, 0,0,3, 0,9,0},
	{0,1,0, 0,8,0, 0,6,0},
	{2,0,0, 0,0,6, 0,0,0}
    },{ // Puzzle 7 from http://www.sudoku.org.uk/bifurcation.htm
	{8,0,0, 0,0,1, 0,4,0},
	{2,0,6, 0,9,0, 0,1,0},
	{0,0,9, 0,0,6, 0,8,0},
	{1,2,4, 0,0,0, 0,0,9},
	{0,0,0, 0,0,0, 0,0,0},
	{9,0,0, 0,0,0, 8,2,4},
	{0,5,0, 4,0,0, 1,0,0},
	{0,8,0, 0,7,0, 2,0,5},
	{0,9,0, 5,0,0, 0,0,7}
    },{ // Puzzle 8 from http://www.sudoku.org.uk/bifurcation.htm
	{6,5,2, 0,4,8, 0,0,7},
	{0,7,0, 2,0,5, 4,0,0},
	{0,0,0, 0,0,0, 0,0,0},
	{0,6,4, 1,0,0, 0,7,0},
	{0,0,0, 0,8,0, 0,0,0},
	{0,8,0, 0,0,4, 5,6,0},
	{0,0,0, 0,0,0, 0,0,0},
	{0,0,8, 6,0,7, 0,2,0},
	{2,0,0, 8,9,0, 7,5,1}
    },{ // Puzzle 9 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,6, 0,0,2, 0,0,9},
	{1,0,0, 5,0,0, 0,2,0},
	{0,4,7, 3,0,6, 0,0,1},
	{0,0,0, 0,0,8, 0,4,0},
	{0,3,0, 0,0,0, 0,7,0},
	{0,1,0, 6,0,0, 0,0,0},
	{4,0,0, 8,0,3, 2,1,0},
	{0,6,0, 0,0,1, 0,0,4},
	{3,0,0, 4,0,0, 9,0,0}
    },{ // Puzzle 10 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,4, 0,5,0, 9,0,0},
	{0,0,0, 0,7,0, 0,0,6},
	{3,7,0, 0,0,0, 0,0,2},
	{0,0,9, 5,0,0, 0,8,0},
	{0,0,1, 2,0,4, 3,0,0},
	{0,6,0, 0,0,9, 2,0,0},
	{2,0,0, 0,0,0, 0,9,3},
	{1,0,0, 0,4,0, 0,0,0},
	{0,0,6, 0,2,0, 7,0,0}
    },{ // Puzzle 11 from http://www.sudoku.org.uk/bifurcation.htm
	{0,0,0, 0,3,0, 7,9,0},
	{3,0,0, 0,0,0, 0,0,5},
	{0,0,0, 4,0,7, 3,0,6},
	{0,5,3, 0,9,4, 0,7,0},
	{0,0,0, 0,7,0, 0,0,0},
	{0,1,0, 8,2,0, 6,4,0},
	{7,0,1, 9,0,8, 0,0,0},
	{8,0,0, 0,0,0, 0,0,1},
	{0,9,4, 0,1,0, 0,0,0}
    },{ // From http://www.sudoku.org.uk/discus/messages/29/51.html?1131034031
	{2,5,8, 1,0,4, 0,3,7},
	{9,3,6, 8,2,7, 5,1,4},
	{4,7,1, 5,3,0, 2,8,0},

	{7,1,5, 2,0,3, 0,4,0},
	{8,4,9, 6,7,5, 3,2,1},
	{3,6,2, 4,1,0, 0,7,5},

	{1,2,4, 9,0,0, 7,5,3},
	{5,9,3, 7,4,2, 1,6,8},
	{6,8,7, 3,5,1, 4,9,2}
    }
};
	
}
class Sudoku : public Space {
	
protected:

	/*
	 * we have 81 variables
	 * each has only one value 
	 */
	IntVarArray pos;
	
	
public:
	Sudoku(void) : pos(*this, 81, 1, 9) {
		
		
		srand (time(NULL));
		/* index to select the 9 * 9 array in the array example
		 * put the value of index to zero to solve the the 9 * 9 sudoku display in the assignement
		 */
		 
		int index = 0;//rand() % 18;
		Matrix<IntVarArray> mat(pos, 9, 9 );
		// set initial values
		for(int i=0;i<9;i++){
			for(int j=0;j<9;j++){
				if( int v = getField(examples[0], i, j) ){
					rel(*this, mat(i,j), IRT_EQ, v);
				}
			}
		}
	
		// distinct row
		for(int i=0; i<9; i++){
			distinct(*this, mat.row(i), IPL_DEF);
		}
		
		// distinct column
		for(int i=0; i<9; i++){
			distinct(*this, mat.col(i), IPL_DEF);
		}
		
		//distinct in 3 * 3 boxes
		for(int i=0; i<9; i+=3){
			for(int j=0; j<9; j+=3){
				distinct(*this, mat.slice(i, i+3, j, j+3), IPL_DEF);
			}
		}
		// branching
		branch(*this, pos, INT_VAR_SIZE_MIN(), INT_VAL_MIN());
		
	}
	// search support
	Sudoku(bool share, Sudoku& s) : Space(share, s) {
		pos.update(*this, share, s.pos);
	}
	virtual Space* copy(bool share) {
		return new Sudoku(share, *this);
	}
	// print solution
	void print(std::ostream& os) const {
		
		Matrix<IntVarArray> mat(pos, 9, 9 );
		os << mat << std::endl;
	}
	// get value in 9 * 9 boxes

	int getField (int example[][9], int i, int j){
			
		return  example[j][i] ;
	}

	
};


// main function
int main(int argc, char* argv[]) {
	
	Sudoku* s = new Sudoku;
	/*DFS<Sudoku> e(s);
	// search and print all solutions
	while( Sudoku* s = e.next()){
		s->print();
		delete s;
	}*/
	Gist::Print<Sudoku> p("Print solution");
	Gist::Options o;
	o.inspect.click(&p);
	Gist::dfs(s,o);
	delete s;
	return 0;
}


/*
 * Answers to questions 
 * Experimentation has been done with index 0 on the 9 * 9 squares displayed in the assignement
 */
 
 /*
  * With default propagator strength IPL_DEF and branch with SIZE_MIN and VAL_MIN, the tree has a depth of 16 and 623 nodes.
  * It is the same with propagator IPL_VAL because IPL_VAL is a naive propagation.
  * With Bound propagation, we obtained depth of 7 and 25 nodes. It is obvioulsy better because the propagation removes bound values 
  * can not be part of a solution, therefore the search is done on a simplified tree.
  * Domain propagation is completely suited for this problem as it resulted with depth of 0 and 1 sucessful node. No need for a search.
  * "INT_VAR_SIZE_MIN(), INT_VAL_MIN()" seems to be the branching option for this problem. With the default propagator strength,
  * It resulted in a smaller tree.
  * Also we noticed a difference between "INT_VAR_NONE(), INT_VAL_SPLIT_MIN" and "INT_VAR_NONE(), INT_VAL_SPLIT_MAX". The latter
  * resulted in a small tree (17) than the first one(19).
  */
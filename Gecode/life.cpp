#include <gecode/minimodel.hh>
#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <math.h>


using namespace Gecode;

class Life : public Script {
	
public:	

	// we have n * n variables 
	// which can take values 0 , 1
	BoolVarArray l;
	
	// sum of individual cells
	//IntVarArray sum;
	
	
	Life(const SizeOptions& opt): Script(opt), l(*this, (opt.size()+4)*(opt.size()+4), 0, 1) {
		
		const int increasedBoard = opt.size() + 4;
		const int size = opt.size();
		
		Matrix<BoolVarArray> life(l, increasedBoard, increasedBoard );
		
		// constraints
		
		// set the first/last two rows/columns to dead (0)
		for(int i=0;i < increasedBoard; i++){
			rel(*this, life(0,i), IRT_EQ, 0);
			rel(*this, life(1,i), IRT_EQ, 0);
			rel(*this, life(size+2,i), IRT_EQ, 0);
			rel(*this, life(size+3,i), IRT_EQ, 0);
			rel(*this, life(i,0), IRT_EQ, 0);
			rel(*this, life(i,1), IRT_EQ, 0);
			rel(*this, life(i, size+2), IRT_EQ, 0);
			rel(*this, life(i, size+3), IRT_EQ, 0);
		}
	
		// game life constraints
		// the constraints has also to be for the inner constraints 
		
		for(int i=1; i <= size+2; i++){
			for(int j=1; j <= size+2; j++){

				BoolVar b;
				BoolVarArgs nei(8);
				nei[0] = life(i-1,j-1);
				nei[1] = life(i-1,j);
				nei[2] = life(i-1,j+1);
				nei[3] = life(i,j-1);
				nei[4] = life(i,j+1);
				nei[5] = life(i+1,j-1);
				nei[6] = life(i+1,j);
				nei[7] = life(i+1,j+1);
				
				//dom(*this, nei, 2, 3, b);
				Reify r(life(i,j));
				Reify r1(life(i,j));
				linear(*this, nei, IRT_LQ, 3, r);
				linear(*this, nei, IRT_GQ, 2, r1);
				
			}
		}
		
		// maximise the sum of individual values
		/*for(int i=2; i < size + 2; i++){
			for(int j=2; j < size+2; j++){
				
			}
		}*/
		// symetry breaking
		rel(*this, life(2,2) >= life(size+1, size+1) );
		rel(*this, life(2,size+1) >= life(size+1, 2) );
		
		// branching val_max in order to maximize values
		branch(*this, l, INT_VAR_NONE(), INT_VAL_MIN());
	
	}	
	Life(bool share, Life& li): Script(share, li){
		l.update(*this, share, li.l);
		
	}
	virtual Space* copy(bool share) {
		return new Life(share, *this);
	}
	// print solution
	void print(std::ostream& os) const {
		
		int s = sqrt(l.size());
		Matrix<BoolVarArray> mat(l, s, s );
		
		for(int i=2; i < s-2; i++){
			for(int j=2; j < s-2; j++){
				std::cout << mat(i,j) << " ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
		
	}
	
};

// main function
int main(int argc, char* argv[]) {
	
	SizeOptions opt("Life");
	opt.size(5);
	opt.parse(argc, argv);
	// create model and search engine
	Script::run<Life, DFS, SizeOptions>(opt);
	return 0;
}
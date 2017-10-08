#include <gecode/minimodel.hh>
#include <gecode/driver.hh>
#include <gecode/int.hh>
#include <math.h>


using namespace Gecode;



class SquarePacking : public Script {
	
public:	
	IntVar s;
	IntVarArray x;
	IntVarArray y;
	
	SquarePacking(const SizeOptions& opt): Script(opt),
				x(*this, opt.size(), 0, s.max()-1),
				y(*this, opt.size(), 0, s.max()-1),
				s(*this, 0, 30){
		
		
		// Array size
		const int n = opt.size();
		
		// max size of the enclosed rectangle, therefor one can put any value in the constructor
		int Size = 0 ;
		for(int i = 0; i < n; i++){
			Size += size(i);
		}
		IntArgs sizes(n);
		for(int i=0; i < n; i++){
			sizes[i] = size(i);
		}
		// restrict according to size
		for(int i=0; i < n; i++){
			rel(*this, x[i] + size(i) <= s);
			rel(*this, y[i] + size(i) <= s);
		}		
		//no overlap
		for(int i = 0 ; i <n ; i++){
			//i is s1 and j is s2
			for(int j=i+1; j < n ; j++){
				rel(*this, (x[i] + size(i) <= x[j]) || (x[j]+size(j) <= x[i]) ||
				    (y[i] + size(i) <= y[j]) || (y[j] + size(j) <= y[i]) );
			}
		}
		// sum of columns must be less or equal to s.max();
		for(int cx=0; cx < Size; cx++){
			BoolVarArgs bx(*this, n,0,1);
			for(int i=0; i<n ; i++){
				dom(*this, x[i], cx-size(i)+1,cx, bx[i]);
			}
			linear(*this,sizes, bx, IRT_LQ, s);
		}
		// sum of rows must be less or equal to s.max();
		for(int cy=0; cy < Size; cy++){
			BoolVarArgs by(*this, n,0,1);
			for(int i=0; i < n; i++){
				dom(*this, y[i], cy-size(i)+1,cy, by[i]);
			}
			linear(*this,sizes, by, IRT_LQ, s);
		}
		
		// Problem decompostion, restrict the size s to sqrt(sum(i^2)
		rel(*this, s , IRT_GQ, ceil(sqrt( n*(2*n+1)*(n+1)/6)));
		
		// Symetry removal, restriction of the largest square
		dom(*this, x[0], 0, 1 + ( (size(0) - n)/ 2 ) );
		rel(*this, y[0], IRT_LQ, x[0]);
		
		//Empty strip dominace
		// remove the partial solution X = d or Y = d where it is possible to fill squares 1*1 ... d* d. Same goes for Y
		// remove the largest square because it will be at (0, 0) from constraint above
		for(int k=n-1,i=1; k > 1; k--,i++){
			for(int d=1 ; d < Size - size(i); d++){
				int square = k * d;
				int fillup = 0;
				for(int j=1; j <= d ; j++){
					fillup += j * j; 
				}
				if( fillup <= square){
					// remove d from X and Y
					rel(*this, x[i], IRT_NQ, d);
					rel(*this, y[i], IRT_NQ, d);
				}
			}
		}
		
		// Branching
		
		// branch on s first
		branch(*this, s , INT_VAL_MIN());
		// x first and from left to right so value selection is val_min
		branch(*this, x, INT_VAR_NONE(), INT_VAL_MIN());
		// y from top to bottom so value selection is val_max
		branch(*this, y, INT_VAR_NONE(), INT_VAL_MAX());
		// due to the fact that the squares are ordered in decreasing order in x and y
		// INT_VAR_NONE() assures that the first, i.e the largest is assigned first
	}
	
	
	SquarePacking(bool share, SquarePacking& sp): Script(share, sp){
		x.update(*this, share, sp.x);
		s.update(*this, share, sp.s);
		y.update(*this, share, sp.y);
		
		
	}
	virtual Space* copy(bool share) {
		return new SquarePacking(share, *this);
	}
	int size(int index){
		return (x.size() - index);
	}
	virtual void print(std::ostream& os) const {
		os << "\t";
		for (int i=0; i<x.size(); i++)
			os << "(" << x[i] << "," << y[i] << ") ";
		os << std::endl;
		
		os << "Size of the enclosed Square is " << s << std::endl;
	}
};


int main(int argc, char *argv[]){
	
	
	SizeOptions opt("SquarePacking");
	opt.size(6);
	opt.parse(argc, argv);
	// create model and search engine
	Script::run<SquarePacking, DFS, SizeOptions>(opt);
	return 0;
}

/*
 * Comments
 * The runtime is pretty fast for n = 6...16 however i have tried N = 17 and so on
 * and i haven't been able to see the end of the execution.
 * I guess there is a faster solution
 */
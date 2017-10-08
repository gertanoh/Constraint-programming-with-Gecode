#include <gecode/search.hh>
#include <gecode/minimodel.hh>
#include <gecode/gist.hh>
#include <gecode/driver.hh>
#include <math.h>


using namespace Gecode;






/*
 * Answers to questions are at the end of the file
 */

class Queens : public Script {
public:
	BoolVarArray q;
	IntVar size;
	
	Queens(const SizeOptions& opt): Script(opt), q(*this, opt.size()*opt.size(), 0, 1) {
		const int n = opt.size();
		Matrix<BoolVarArray> qm(q, n, n );
		
		// sum of rows and colums must be 1
		for(int i=0;i<n;i++){
			rel(*this, sum(qm.row(i)) == 1);
		}
		for(int i=0;i<n;i++){
			rel(*this, sum(qm.col(i)) == 1);
		}
		
		// get the diagonal 
		for(int k = 0; k < n - 1; k++){
			const int s = n - k;
			BoolVarArgs d(s);
			BoolVarArgs d2(s);
			BoolVarArgs d4(s);
			for(int i = 0 , j= k; j < n ; i++, j++){
				d[i] = qm(i,j);
			}
			for(int j = k, i= n-1; j < n ; i--, j++){
				d2[j - k] = qm(i,j);
			}
			for(int i = k , j= 0; i < n ; i++, j++){
				d4[i - k] = qm(i,j);
			}
			linear(*this, d, IRT_LQ, 1);
			linear(*this, d2, IRT_LQ, 1);
			linear(*this, d4, IRT_LQ, 1);

		}
		for(int k = n - 1; k > 0; k--){
			BoolVarArgs d1(k + 1);
			for(int i = k , j= 0; i >= 0 ; i--, j++){
				d1[j] = qm(i,j);
			}
			linear(*this, d1, IRT_LQ, 1);
		}
		// branching
		branch(*this, q, INT_VAR_NONE(), INT_VAL_MAX());
	}
	
	Queens(bool share, Queens& s): Script(share, s){
		q.update(*this, share, s.q);
		
	}
	virtual Space* copy(bool share) {
		return new Queens(share, *this);
	}
	// print solution
	void print(std::ostream& os) const {
		
		int l = sqrt(q.size());
		Matrix<BoolVarArray> mat(q, l, l );
		os << mat << std::endl;
	}
	
};

// main function
int main(int argc, char* argv[]) {
	
	SizeOptions opt("Queens");
	opt.size(8);
	opt.parse(argc, argv);
	// create model and search engine
	Script::run<Queens, DFS, SizeOptions>(opt);
	return 0;
}

/*
 * Answers to questions 
 * Experimentation has been done with index 0 on the 9 * 9 squares displayed in the assignement
 */
 
 
 /*
  *  - Branching: the most efficient strategy is to assign the max value which is 1, which constraints the row,
  * colum and diagonal.
  * so INT_VAL_NONE() and INT_VAL_MAX is an efficient branching with 45 nodes.
  */
  
  /*
   * - Advantages of this model.
   * This model is with less propagators and then less propagation.( For 8 queens, 455 vs 782).
   * The model allows the n-queens problem to be scalable, the number of propagation and nodes
   * with n incresing grow less faster compared to the model seen in the lecture.
   * 
   * - Disadvantages
   * n^2 variables
   * Impossibility to use symetrix when posting constraints.
   */
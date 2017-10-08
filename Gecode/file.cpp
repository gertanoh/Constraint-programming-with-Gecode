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
	
	
	Life(const SizeOptions& opt): Script(opt), l(*this, opt.size()*opt.size(), 0, 1) {
		const int n = opt.size();

		
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
		os << mat << std::endl;
	}
	
};

// main function
int main(int argc, char* argv[]) {
	
	SizeOptions opt("Life");
	opt.size(8);
	opt.parse(argc, argv);
	// create model and search engine
	Script::run<Queens, DFS, SizeOptions>(opt);
	return 0;
}
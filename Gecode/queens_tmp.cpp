#include <gecode/int.hh>
#include <gecode/search.hh>
#include <gecode/minimodel.hh>
#include <gecode/driver.hh>

using namespace Gecode;


class Queens : public Script {
	
	protected:
		IntVarArray q;
		
		
	public:
		Queens(const SizeOptions& opt)
		: Script(opt), q(*this, opt.size(), 0, opt.size() - 1) {
			
			const int n = q.size();			
			// not in the same diagonal
			for(int i= 0 ; i < n; i++){
				for(int j=i+1; j<n;j++){
					rel(*this, q[i] - i != q[j] - j);
					rel(*this, q[i] - j != q[j] - i);

				}
			}
			// One queen per column
			distinct(*this, q);
			// post branching
			branch(*this, q, INT_VAR_NONE(),INT_VAL_MIN());
		
		}
		
		// search support
		Queens(bool share, Queens& s): Script(share, s){
			q.update(*this, share, s.q);
		}
		virtual Space* copy(bool share) {
			return new Queens(share, *this);
		}
		// print solution
		void print(std::ostream& os) const {
		
			os << q << std::endl;
		}
};

// main function
int main(int argc, char* argv[]) {
	SizeOptions opt("Queens");
	opt.size(16);
	opt.parse(argc, argv);
	// create model and search engine
	Script::run<Queens, DFS, SizeOptions>(opt);
	return 0;
}
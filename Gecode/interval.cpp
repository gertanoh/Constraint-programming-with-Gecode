/*
 *  Main authors:
 *     Christian Schulte <cschulte@kth.se>
 *
 *  Copyright:
 *     Christian Schulte, 2009
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <gecode/int.hh>
#include <math.h>
using namespace Gecode;
using namespace Gecode::Int;

/*
 * Custom brancher for forcing mandatory parts
 *
 */
class IntervalBrancher : public Brancher {
protected:
  // Views for x-coordinates (or y-coordinates)
  ViewArray<IntView> x;
  // Width (or height) of rectangles
  int* w;
  // Percentage for obligatory part
  double p;
  // Cache of first unassigned view
  mutable int start;
  // Description
  class Description : public Choice {
  public:
    // Position of view
    int pos;
    // You might need more information, please add here
	int* first_inter;
	int size;
    /* Initialize description for brancher b, number of
     *  alternatives a, position p, and ???. 
     */
    Description(const Brancher& b, unsigned int a, int p, int* inter, int s)
      : Choice(b,a), pos(p), first_inter(heap.alloc<int>(s)), size(s) {
		  for(int i=0; i<s; i++){
			  first_inter[i] = inter[i];
		  }
	  }
    // Report size occupied
    virtual size_t size(void) const {
      return sizeof(Description);
    }
    // Archive the choice's information in e
    virtual void archive(Archive& e) const {
      Choice::archive(e);
      // You must also archive the additional information
      e << alternatives() << pos << size;
	  for(int i=0; i<s; i++){
		   e << first_inter[i];
	  }
    }
	
	virtual ~Description(void){
		heap.free<int>(first_inter, size);
	}
  };
public:
  // Construct branching
  IntervalBrancher(Home home, 
                   ViewArray<IntView>& x0, int w0[], double p0)
    : Brancher(home), x(x0), w(w0), p(p0), start(0) {}
  // Post branching
  static void post(Home home, ViewArray<IntView>& x, int w[], double p) {
    (void) new (home) IntervalBrancher(home,x,w,p);
  }

  // Copy constructor used during cloning of b
  IntervalBrancher(Space& home, bool share, IntervalBrancher& b)
    : Brancher(home, share, b), p(b.p), start(b.start) {
    x.update(home,share,b.x);
    w = home.alloc<int>(x.size());
    for (int i=x.size(); i--; )
      w[i]=b.w[i];
  }
  // Copy brancher
  virtual Actor* copy(Space& home, bool share) {
    return new (home) IntervalBrancher(home, share, *this);
  }

  // Check status of brancher, return true if alternatives left
  virtual bool status(const Space& home) const {

    // FILL IN HERE
	for(int i= start; i < x.size(); i++){
		if(!x[i].assigned()){
			start = i;
			return true;
		}
	}
	return false;

  }
  // Return choice as description
  virtual const Choice* choice(Space& home) {

    // FILL IN HERE
	int obligatory = 0;
	// remenber to split only large intervals
	for(int i=0; i< x.size(); i++){
		if(x[i].max() < w[i] ){
			// There exists an obligatory part
			int sizeObligatory = ceil((w[i] - x[i].max()) * p);
			
		}
		
	}
	

  }
  // Construct choice from archive e
  virtual const Choice* choice(const Space&, Archive& e) {
    // Again, you have to take care of the additional information
    int pos, val;
	unsigned int a;
    e >> pos >> val >> a;
    return new Description(*this, a, pos, val);
  }
  // Perform commit for choice c and alternative a
  virtual ExecStatus commit(Space& home, 
                            const Choice& c,
                            unsigned int a) {
    const Description& d = static_cast<const Description&>(c);

    // FILL IN HERE

  }
  // Print some information on stream o (used by Gist, from Gecode 4.0.1 on)
  virtual void print(const Space& home, const Choice& c, unsigned int b,
                     std::ostream& o) const {

    // FILL IN HERE

  }
};

// This posts the interval branching
void interval(Home home, const IntVarArgs& x, const IntArgs& w, double p) {
  // Check whether arguments make sense
  if (x.size() != w.size())
    throw ArgumentSizeMismatch("interval");
  // Never post a branching in a failed space
  if (home.failed()) return;
  // Create an array of integer views
  ViewArray<IntView> vx(home,x);
  // Create an array of integers
  int* wc = static_cast<Space&>(home).alloc<int>(x.size());
  for (int i=x.size(); i--; )
    wc[i]=w[i];
  // Post the brancher
  IntervalBrancher::post(home,vx,wc,p);
}


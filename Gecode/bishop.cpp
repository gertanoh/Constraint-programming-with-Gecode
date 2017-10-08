/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */
00002 /*
00003  *  Main authors:
00004  *     Christian Schulte <schulte@gecode.org>
00005  *     Mikael Lagerkvist <lagerkvist@gecode.org>
00006  *
00007  *  Copyright:
00008  *     Christian Schulte, 2001
00009  *     Mikael Lagerkvist, 2005
00010  *
00011  *  Last modified:
00012  *     $Date: 2016-04-19 17:19:45 +0200 (Tue, 19 Apr 2016) $ by $Author: schulte $
00013  *     $Revision: 14967 $
00014  *
00015  *  This file is part of Gecode, the generic constraint
00016  *  development environment:
00017  *     http://www.gecode.org
00018  *
00019  *  Permission is hereby granted, free of charge, to any person obtaining
00020  *  a copy of this software and associated documentation files (the
00021  *  "Software"), to deal in the Software without restriction, including
00022  *  without limitation the rights to use, copy, modify, merge, publish,
00023  *  distribute, sublicense, and/or sell copies of the Software, and to
00024  *  permit persons to whom the Software is furnished to do so, subject to
00025  *  the following conditions:
00026  *
00027  *  The above copyright notice and this permission notice shall be
00028  *  included in all copies or substantial portions of the Software.
00029  *
00030  *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
00031  *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
00032  *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
00033  *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
00034  *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
00035  *  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
00036  *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
00037  *
00038  */
00039 
00040 #include <gecode/driver.hh>
00041 #include <gecode/int.hh>
00042 #include <gecode/minimodel.hh>
00043 
00044 using namespace Gecode;
00045 
00050 const int kval[] = {
00051   0,   0,  0,  0,  5,
00052   9,  15, 21, 29, 37,
00053   47, 57, 69, 81, 94,
00054   109
00055 };
00056 
00057 namespace {
00061   TupleSet bishops;
00065   class Bishops : public Space {
00066   public:
00067     const int n;
00068     BoolVarArray k;
00069     bool valid_pos(int i, int j) {
00070       return (i >= 0 && i < n) && (j >= 0 && j < n);
00071     }
00072     Bishops(int size)
00073       : n(size), k(*this,n*n,0,1) {
00074       Matrix<BoolVarArray> kb(k, n, n);
00075       for (int l = n; l--; ) {
00076         const int il = (n-1) - l;
00077         BoolVarArgs d1(l+1), d2(l+1), d3(l+1), d4(l+1);
00078         for (int i = 0; i <= l; ++i) {
00079           d1[i] = kb(i+il, i);
00080           d2[i] = kb(i, i+il);
00081           d3[i] = kb((n-1)-i-il, i);
00082           d4[i] = kb((n-1)-i, i+il);
00083         }
00084 
00085         linear(*this, d1, IRT_LQ, 1);
00086         linear(*this, d2, IRT_LQ, 1);
00087         linear(*this, d3, IRT_LQ, 1);
00088         linear(*this, d4, IRT_LQ, 1);
00089       }
00090 
00091       linear(*this, k, IRT_EQ, 2*n - 2);
00092       // Forced bishop placement from crowded chess model
00093       rel(*this, kb(n-1,   0), IRT_EQ, 1);
00094       rel(*this, kb(n-1, n-1), IRT_EQ, 1);
00095       branch(*this, k,
00096              tiebreak(INT_VAR_DEGREE_MAX(),INT_VAR_SIZE_MIN()), INT_VAL_MAX());
00097     }
00098     Bishops(bool share, Bishops& s) : Space(share,s), n(s.n) {
00099       k.update(*this, share, s.k);
00100     }
00101     virtual Space* copy(bool share) {
00102       return new Bishops(share,*this);
00103     }
00104   };
00108   void init_bishops(int size) {
00109     Bishops* prob = new Bishops(size);
00110     DFS<Bishops> e(prob); IntArgs ia(size*size);
00111     delete prob;
00112 
00113     while (Bishops* s = e.next()) {
00114       for (int i = size*size; i--; )
00115         ia[i] = s->k[i].val();
00116       bishops.add(ia);
00117       delete s;
00118     }
00119 
00120     bishops.finalize();
00121   }
00122 }
00187 class CrowdedChess : public Script {
00188 protected:
00189   const int n;          
00190   IntVarArray s;        
00191   IntVarArray queens,   
00192     rooks;              
00193   BoolVarArray knights; 
00194 
00198   enum
00199     {Q,   
00200      R,   
00201      B,   
00202      K,   
00203      E,   
00204      PMAX 
00205     } piece;
00206 
00207   // Is (i,j) a valid position on the board?
00208   bool valid_pos(int i, int j) {
00209     return (i >= 0 && i < n) &&
00210       (j >= 0 && j < n);
00211   }
00212 
00214   void knight_constraints(void) {
00215     static const int kmoves[4][2] = {
00216       {-1,2}, {1,2}, {2,-1}, {2,1}
00217     };
00218     Matrix<BoolVarArray> kb(knights, n, n);
00219     for (int x = n; x--; )
00220       for (int y = n; y--; )
00221         for (int i = 4; i--; )
00222           if (valid_pos(x+kmoves[i][0], y+kmoves[i][1]))
00223             rel(*this,
00224                 kb(x, y),
00225                 BOT_AND,
00226                 kb(x+kmoves[i][0], y+kmoves[i][1]),
00227                 0);
00228   }
00229 
00230 
00231 public:
00232   enum {
00233     PROP_TUPLE_SET, 
00234     PROP_DECOMPOSE  
00235   };
00236 
00238   CrowdedChess(const SizeOptions& opt)
00239     : Script(opt),
00240       n(opt.size()),
00241       s(*this, n*n, 0, PMAX-1),
00242       queens(*this, n, 0, n-1),
00243       rooks(*this, n, 0, n-1),
00244       knights(*this, n*n, 0, 1) {
00245     const int nkval = sizeof(kval)/sizeof(int);
00246     const int nn = n*n, q = n, r = n, b = (2*n)-2,
00247       k = n <= nkval ? kval[n-1] : kval[nkval-1];
00248     const int e = nn - (q + r + b + k);
00249 
00250     assert(nn == (e + q + r + b + k));
00251 
00252     Matrix<IntVarArray> m(s, n);
00253 
00254     // ***********************
00255     // Basic model
00256     // ***********************
00257 
00258     count(*this, s, E, IRT_EQ, e, opt.ipl());
00259     count(*this, s, Q, IRT_EQ, q, opt.ipl());
00260     count(*this, s, R, IRT_EQ, r, opt.ipl());
00261     count(*this, s, B, IRT_EQ, b, opt.ipl());
00262     count(*this, s, K, IRT_EQ, k, opt.ipl());
00263 
00264     // Collect rows and columns for handling rooks and queens
00265     for (int i = 0; i < n; ++i) {
00266       IntVarArgs aa = m.row(i), bb = m.col(i);
00267 
00268       count(*this, aa, Q, IRT_EQ, 1, opt.ipl());
00269       count(*this, bb, Q, IRT_EQ, 1, opt.ipl());
00270       count(*this, aa, R, IRT_EQ, 1, opt.ipl());
00271       count(*this, bb, R, IRT_EQ, 1, opt.ipl());
00272 
00273       // Connect (queens|rooks)[i] to the row it is in
00274       element(*this, aa, queens[i], Q, IPL_DOM);
00275       element(*this, aa,  rooks[i], R, IPL_DOM);
00276     }
00277 
00278     // N-queens constraints
00279     distinct(*this, queens, IPL_DOM);
00280     distinct(*this, IntArgs::create(n,0,1), queens, IPL_DOM);
00281     distinct(*this, IntArgs::create(n,0,-1), queens, IPL_DOM);
00282 
00283     // N-rooks constraints
00284     distinct(*this,  rooks, IPL_DOM);
00285 
00286     // Collect diagonals for handling queens and bishops
00287     for (int l = n; l--; ) {
00288       const int il = (n-1) - l;
00289       IntVarArgs d1(l+1), d2(l+1), d3(l+1), d4(l+1);
00290       for (int i = 0; i <= l; ++i) {
00291         d1[i] = m(i+il, i);
00292         d2[i] = m(i, i+il);
00293         d3[i] = m((n-1)-i-il, i);
00294         d4[i] = m((n-1)-i, i+il);
00295       }
00296 
00297       count(*this, d1, Q, IRT_LQ, 1, opt.ipl());
00298       count(*this, d2, Q, IRT_LQ, 1, opt.ipl());
00299       count(*this, d3, Q, IRT_LQ, 1, opt.ipl());
00300       count(*this, d4, Q, IRT_LQ, 1, opt.ipl());
00301       if (opt.propagation() == PROP_DECOMPOSE) {
00302         count(*this, d1, B, IRT_LQ, 1, opt.ipl());
00303         count(*this, d2, B, IRT_LQ, 1, opt.ipl());
00304         count(*this, d3, B, IRT_LQ, 1, opt.ipl());
00305         count(*this, d4, B, IRT_LQ, 1, opt.ipl());
00306       }
00307     }
00308     if (opt.propagation() == PROP_TUPLE_SET) {
00309       IntVarArgs b(s.size());
00310       for (int i = s.size(); i--; )
00311         b[i] = channel(*this, expr(*this, (s[i] == B)));
00312       extensional(*this, b, bishops, opt.ipl());
00313     }
00314 
00315     // Handle knigths
00316     // Connect knigths to board
00317     for(int i = n*n; i--; )
00318       knights[i] = expr(*this, (s[i] == K));
00319     knight_constraints();
00320 
00321 
00322     // ***********************
00323     // Redundant constraints
00324     // ***********************
00325 
00326     // Queens and rooks not in the same place
00327     // Faster than going through the channelled board-connection
00328     for (int i = n; i--; )
00329       rel(*this, queens[i], IRT_NQ, rooks[i]);
00330 
00331     // Place bishops in two corners (from Schimpf and Hansens solution)
00332     // Avoids some symmetries of the problem
00333     rel(*this, m(n-1,   0), IRT_EQ, B);
00334     rel(*this, m(n-1, n-1), IRT_EQ, B);
00335 
00336 
00337     // ***********************
00338     // Branching
00339     // ***********************
00340     // Place each piece in turn
00341     branch(*this, s, INT_VAR_MIN_MIN(), INT_VAL_MIN());
00342   }
00343 
00345   CrowdedChess(bool share, CrowdedChess& e)
00346     : Script(share,e), n(e.n) {
00347     s.update(*this, share, e.s);
00348     queens.update(*this, share, e.queens);
00349     rooks.update(*this, share, e.rooks);
00350     knights.update(*this, share, e.knights);
00351   }
00352 
00354   virtual Space*
00355   copy(bool share) {
00356     return new CrowdedChess(share,*this);
00357   }
00358 
00360   virtual void
00361   print(std::ostream& os) const {
00362     Matrix<IntVarArray> m(s, n);
00363     char names[PMAX];
00364     names[E] = '.'; names[Q] = 'Q'; names[R] = 'R';
00365     names[B] = 'B'; names[K] = 'K';
00366     const char* sep   = n < 8 ? "\t\t" : "\t";
00367 
00368     for (int r = 0; r < n; ++r){
00369       // Print main board
00370       os << '\t';
00371       for (int c = 0; c < n; ++c) {
00372         if (m(r, c).assigned()) {
00373           os << names[m(r, c).val()];
00374         } else {
00375           os << " ";
00376         }
00377       }
00378       // Print each piece on its own
00379       for (int p = 0; p < PMAX; ++p) {
00380         if (p == E) continue;
00381         os << sep;
00382         for (int c = 0; c < n; ++c) {
00383           if (m(r, c).assigned()) {
00384             if (m(r, c).val() == p)
00385               os << names[p];
00386             else
00387               os << names[E];
00388           } else {
00389             os << " ";
00390           }
00391         }
00392       }
00393       os << std::endl;
00394     }
00395     os << std::endl;
00396   }
00397 };
00398 
00402 int
00403 main(int argc, char* argv[]) {
00404   SizeOptions opt("CrowdedChess");
00405   opt.propagation(CrowdedChess::PROP_TUPLE_SET);
00406   opt.propagation(CrowdedChess::PROP_TUPLE_SET,
00407                   "extensional",
00408                   "Use extensional propagation for bishops-placement");
00409   opt.propagation(CrowdedChess::PROP_DECOMPOSE,
00410                   "decompose",
00411                   "Use decomposed propagation for bishops-placement");
00412   opt.ipl(IPL_DOM);
00413   opt.size(8);
00414   opt.parse(argc,argv);
00415   if (opt.size() < 5) {
00416     std::cerr << "Error: size must be at least 5" << std::endl;
00417     return 1;
00418   }
00419   init_bishops(opt.size());
00420   Script::run<CrowdedChess,DFS,SizeOptions>(opt);
00421   return 0;
00422 }
00423 
00424 // STATISTICS: example-any
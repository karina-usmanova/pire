#ifndef PIRE_GLUE_H
#define PIRE_GLUE_H


#include "stub/stl.h"
#include "partition.h"

namespace Pire {
namespace Impl {
	template <class Scanner>
	class LettersEquality: public ybinary_function<Char, Char, bool> {
	public:
		LettersEquality(typename Scanner::Letter* lhs, typename Scanner::Letter* rhs): m_lhs(lhs), m_rhs(rhs) {}

		bool operator()(Char a, Char b) const
		{
			return m_lhs[a] == m_lhs[b] && m_rhs[a] == m_rhs[b];
		}

	private:
		typename Scanner::Letter* m_lhs;
		typename Scanner::Letter* m_rhs;
	};
	
	typedef ypair<Scanner::State, Scanner::State> GluedState;
}

// This lookup table is used instead of std::map.
// The key idea is to specify size which is a power of 2 in order to use >> and | instead of
// divisions and remainders.
// NB: it mimics limited std::map<> behaviour, hence stl-like method names and typedefs.
template <size_t N>
class GluedStateLookupTable {
public:
	static const size_t MaxSize = N;
	typedef Impl::GluedState key_type;
	typedef size_t mapped_type;
	typedef ypair<const Impl::GluedState, size_t> value_type;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;

	GluedStateLookupTable()
		: mMap(new value_type[N])
		, mFilled(N, false)
	{}

	~GluedStateLookupTable()
	{
		delete [] mMap;
	}
	
	const_iterator end() const {
		return mMap + MaxSize;
	}
	// Note that in fact mMap is sparsed and traditional [begin,end)
	// traversal is unavailable; hence no begin() method here.
	// end() is only valid for comparing with find() result.
	const_iterator find(const key_type& st) const {
		size_t ind = Search(st);
		return mFilled[ind] ? (mMap + ind) : end();
	}

	ypair<iterator, bool> insert(const value_type& v) {
		size_t ind = Search(v.first);
		if (!mFilled[ind]) {
			new(mMap + ind) value_type(v);
			mFilled[ind] = true;
			return ymake_pair(mMap + ind, true);
		} else
			return ymake_pair(mMap + ind, false);
	}

private:
	size_t Search(const Impl::GluedState& st) const {
		size_t startInd = (Hash(st) % N);
		for (size_t ind = startInd; ind != (startInd + N - 1) % N; ind = (ind + 1) % N) {
			if (!mFilled[ind] || mMap[ind].first == st) {
				return ind;
			}
		}
		return -1;
	}

	static size_t Hash(const Impl::GluedState& st) {
		return size_t((st.first >> 2) ^ (st.second >> 4) ^ (st.second << 10));
	}

	value_type*   mMap;        // not using vector here to avoid initialization
	yvector<bool> mFilled;

// Noncopyable
GluedStateLookupTable(const GluedStateLookupTable&);
GluedStateLookupTable& operator = (const GluedStateLookupTable&);
};

template<class Scanner>
class ScannerGlueCommon {
public:
	typedef Partition< Char, Impl::LettersEquality<Scanner> > LettersTbl;
	
	typedef ypair<typename Scanner::InternalState, typename Scanner::InternalState> State;
	ScannerGlueCommon(const Scanner& lhs, const Scanner& rhs, const LettersTbl& letters)
		: m_lhs(lhs)
		, m_rhs(rhs)
		, m_letters(letters)
	{
		// Form a new letters partition
		for (unsigned ch = 0; ch < MaxChar; ++ch)
			if (ch != Epsilon)
				m_letters.Append(ch);
	}

	const LettersTbl& Letters() const { return m_letters; }
	
	const Scanner& Lhs() const { return m_lhs; }
	const Scanner& Rhs() const { return m_rhs; }

	State Initial() const { return State(Lhs().m.initial, Rhs().m.initial); }

	State Next(State state, Char letter) const
	{
		Lhs().Next(state.first, letter);
		Rhs().Next(state.second, letter);
		return state;
	}
	
	bool IsRequired(const State& /*state*/) const { return true; }

	typedef Scanner Result;
	const Scanner& Success() const { return *m_result; }
	Scanner Failure() const { return Scanner(); }

protected:
	Scanner& Sc() { return *m_result; }
	void SetSc(Scanner* sc) { m_result.reset(sc); }

private:
	const Scanner& m_lhs;
	const Scanner& m_rhs;
	LettersTbl m_letters;
	yauto_ptr<Scanner> m_result;
};

	
}

#endif
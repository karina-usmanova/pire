#include <algorithm>
#include <map>
#include <list>
#include <set>
#include <vector>
#include <utility>
#include "stub/singleton.h"
#include "stub/noncopyable.h"
#include "stub/utf8.h"
#include "re_lexer.h"
#include "stub/stl.h"

namespace Pire {

namespace {

	/*
	* A class providing a function which returns a character
	* whose glyph resembles that of given char, if any;
	* otherwise returns given char itself.
	*/
	class GlyphTable {
	private:
		ylist< yvector<wchar32> > m_classes;
		ymap<wchar32, yvector<wchar32>*> m_map;

		struct GlyphClass {
			yvector<wchar32>* m_class;
			ymap<wchar32, yvector<wchar32>*> *m_map;

			GlyphClass& operator << (wchar32 c)
			{
				m_class->push_back(c);
				m_map->insert(ymake_pair(c, m_class));
				return *this;
			}
		};

		GlyphClass Class()
		{
			GlyphClass cl;
			m_classes.push_back(yvector<wchar32>());
			cl.m_class = &m_classes.back();
			cl.m_map = &m_map;
			return cl;
		}

	public:

		const yvector<wchar32>& Klass(wchar32 x) const
		{
			ymap<wchar32, yvector<wchar32>*>::const_iterator i = m_map.find(x);
			if (i != m_map.end())
				return *i->second;
			else
				return DefaultValue< yvector<wchar32> >();
		}

		GlyphTable()
		{
			Class() << 'A' << 0x0410;
			Class() << 'B' << 0x0412;
			Class() << 'C' << 0x0421;
			Class() << 'E' << 0x0415 << 0x0401;
			Class() << 'H' << 0x041D;
			Class() << 'K' << 0x041A;
			Class() << 'M' << 0x041C;
			Class() << 'O' << 0x041E;
			Class() << 'P' << 0x0420;
			Class() << 'T' << 0x0422;
			Class() << 'X' << 0x0425;

			Class() << 'a' << 0x0430;
			Class() << 'c' << 0x0441;
			Class() << 'e' << 0x0435 << 0x0451;
			Class() << 'm' << 0x0442;
			Class() << 'o' << 0x043E;
			Class() << 'p' << 0x0440;
			Class() << 'u' << 0x0438;
			Class() << 'x' << 0x0445;
			Class() << 'y' << 0x0443;
		}
	};

	class GlueSimilarGlyphsImpl: public Feature {
	public:
		GlueSimilarGlyphsImpl(): m_table(Singleton<GlyphTable>()) {}
		int Priority() const { return 9; }

		void Alter(Term& t)
		{
			if (t.Value().IsA<Term::CharacterRange>()) {
				const Term::CharacterRange& range = t.Value().As<Term::CharacterRange>();
				typedef Term::CharacterRange::first_type CharSet;
				const CharSet& old = range.first;
				CharSet altered;
				for (CharSet::const_iterator i = old.begin(), ie = old.end(); i != ie; ++i) {
					const yvector<wchar32>* klass = 0;
					if (i->size() == 1 && !(klass = &m_table->Klass((*i)[0]))->empty())
						for (yvector<wchar32>::const_iterator j = klass->begin(), je = klass->end(); j != je; ++j)
							altered.insert(Term::String(1, *j));
					else
						altered.insert(*i);
				}

				t = Term(t.Type(), Term::CharacterRange(altered, range.second));
			}
		}

	private:
		GlyphTable* m_table;
	};
}

namespace Features {
	Feature* GlueSimilarGlyphs() { return new GlueSimilarGlyphsImpl; }
}

}

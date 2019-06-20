/*
 * unicode_support.cpp -- implementation for the EnableUnicodeSequences feature.
 *
 * Copyright (c) 2018 YANDEX LLC
 * Author: Andrey Logvin <andry@logvin.net>
 *
 * This file is part of Pire, the Perl Incompatible
 * Regular Expressions library.
 *
 * Pire is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Pire is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 * You should have received a copy of the GNU Lesser Public License
 * along with Pire.  If not, see <http://www.gnu.org/licenses>.
 */


#include "unicode_support.h"
#include "read_unicode.h"

#include "re_lexer.h"

namespace Pire {
	
namespace {
    class EnableUnicodeSequencesImpl : public UnicodeReader {
    public:
        bool Accepts(wchar32 c) const {
            return c == (Control | 'x');
        }

        Term Lex() {
            return Term::Character(ReadUnicodeCharacter());
        }
    };
}
	
namespace Features {
    Feature::Ptr EnableUnicodeSequences() { return Feature::Ptr(new EnableUnicodeSequencesImpl); }
};
}

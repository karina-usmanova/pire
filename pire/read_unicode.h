/*
 * read_unicode.h -- declaration of the UnicodeReader class.
 *
 * Copyright (c) 2018-2019 YANDEX LLC, Andrey Logvin <andry@logvin.net>
 *                                     Karina Usmanova <usmanova.karin@yandex.ru>
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


#ifndef PIRE_READ_UNICODE_H
#define PIRE_READ_UNICODE_H


#include <re_lexer.h>

namespace Pire {
	class UnicodeReader : public Feature {
	public:
		wchar32 ReadUnicodeCharacter();

	private:
		static const wchar32 MaxUnicode = 0x10FFFF;

		bool IsHexDigit(wchar32 ch);
		ystring ReadHexDigit(std::function<bool(wchar32, size_t)> shouldStop);
		wchar32 HexToDec(const ystring& hexStr);
	};
}

#endif

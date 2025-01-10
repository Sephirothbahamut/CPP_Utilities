#pragma once

#include "base.h"
#include "output.h"
#include "../../storage.h"
#include "definitions.h"
#include "memberwise_operators.h"

namespace utils::details::vector
	{
	template<typename T, size_t SIZE, template <typename, size_t> class unspecialized_derived_t, const char* name>
	struct utils_oop_empty_bases base :
		concept_common_flag_type,
		definitions<T, SIZE, unspecialized_derived_t>,
		utils::storage::multiple<T, SIZE, false>,
		memberwise_operators<definitions<T, SIZE, unspecialized_derived_t>>,
		output<name>
		{
		using typename definitions<T, SIZE, unspecialized_derived_t>::self_t       ;
		using typename definitions<T, SIZE, unspecialized_derived_t>::nonref_self_t;
		using typename definitions<T, SIZE, unspecialized_derived_t>::storage_t    ;

		using utils::storage::multiple<T, SIZE, false>::multiple;
		using storage_t::extent;
		using storage_t::size;
		using storage_t::storage_type;
		using memberwise_operators<definitions<T, SIZE, unspecialized_derived_t>>::operator=;

		//Forward declare to prevent clang specifically from attempting to instantiate the operator= for base classes that inherit from this one
		//while the base classes are still incomplete, which causes some concepts to fail.
		//Thanks #include discord for the help in understanding what was going on here.
		//The same is done in the base class as well (vec/rgb)
		//Note that MSVC and Gcc will not report errors without this line, but clang's behaviour is the correct one.
		//Gcc explicitly states its own behaviour on the matter is non-conforming (see: https://cplusplus.github.io/CWG/issues/1594.html)
		base<T, SIZE, unspecialized_derived_t, name>& operator=(const base<T, SIZE, unspecialized_derived_t, name>&) noexcept;

		using typename storage_t::value_type            ;
		using typename storage_t::const_aware_value_type;
		using typename storage_t::iterator              ;
		using typename storage_t::const_iterator        ;
		using typename storage_t::reverse_iterator      ;
		using typename storage_t::const_reverse_iterator;
		};
	}
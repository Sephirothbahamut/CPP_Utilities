#pragma once

#include "details/vec/all.h"

namespace utils::math
	{
	template<typename T, size_t SIZE>
	struct utils_oop_empty_bases vec : 
		::utils::details::vector::base<T, SIZE, vec, details::name_vec>,
		details::vec_sized_specialization   <T, SIZE, vec>,
		details::vec_typed_specialization   <T, SIZE, vec>
		{
		using base_t = ::utils::details::vector::base<T, SIZE, vec, details::name_vec>;

		using base_t::size;
		using base_t::extent;
		using base_t::storage_type;
		using typename base_t::self_t;
		using typename base_t::value_type;
		using typename base_t::const_aware_value_type;
		using typename base_t::template_type;
		using typename base_t::nonref_self_t;

		using ::utils::details::vector::base<T, SIZE, vec, details::name_vec>::base;
		using base_t::operator=;
		utils_gpu_available constexpr vec() noexcept requires(storage_type.is_owner()) : base_t{} {}; //for some reason it doesn't use base_t's default constructor with = default

		//Forward declare to prevent clang specifically from attempting to instantiate the operator= for base classes that inherit from this one
		//while the base classes are still incomplete, which causes some concepts to fail.
		//Thanks #include discord for the help in understanding what was going on here.
		//The same is done in the base class as well (vec/rgb)
		//Note that MSVC and Gcc will not report errors without this line, but clang's behaviour is the correct one.
		//Gcc explicitly states its own behaviour on the matter is non-conforming (see: https://cplusplus.github.io/CWG/issues/1594.html)
		vec<T, SIZE>& operator=(const vec<T, SIZE>&) noexcept;

		#pragma region fields
		utils_gpu_available constexpr const const_aware_value_type& x() const noexcept requires(extent >= 1) { return (*this)[0]; }
		utils_gpu_available constexpr       const_aware_value_type& x()       noexcept requires(extent >= 1) { return (*this)[0]; }
		utils_gpu_available constexpr const const_aware_value_type& y() const noexcept requires(extent >= 2) { return (*this)[1]; }
		utils_gpu_available constexpr       const_aware_value_type& y()       noexcept requires(extent >= 2) { return (*this)[1]; }
		utils_gpu_available constexpr const const_aware_value_type& z() const noexcept requires(extent >= 3) { return (*this)[2]; }
		utils_gpu_available constexpr       const_aware_value_type& z()       noexcept requires(extent >= 3) { return (*this)[2]; }
		utils_gpu_available constexpr const const_aware_value_type& w() const noexcept requires(extent >= 4) { return (*this)[3]; }
		utils_gpu_available constexpr       const_aware_value_type& w()       noexcept requires(extent >= 4) { return (*this)[3]; }
		#pragma endregion fields

		#pragma region swizzle
		utils_gpu_available constexpr const vec<const       value_type&, 2> xy () const noexcept requires(extent >= 2) { return {(*this)[0], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> xy ()       noexcept requires(extent >= 2) { return {(*this)[0], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> xz () const noexcept requires(extent >= 3) { return {(*this)[0], (*this)[2]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> xz ()       noexcept requires(extent >= 3) { return {(*this)[0], (*this)[2]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> xw () const noexcept requires(extent >= 4) { return {(*this)[0], (*this)[3]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> xw ()       noexcept requires(extent >= 4) { return {(*this)[0], (*this)[3]}; }

		utils_gpu_available constexpr const vec<const       value_type&, 2> yx () const noexcept requires(extent >= 3) { return {(*this)[1], (*this)[0]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> yx ()       noexcept requires(extent >= 3) { return {(*this)[1], (*this)[0]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> yz () const noexcept requires(extent >= 3) { return {(*this)[1], (*this)[2]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> yz ()       noexcept requires(extent >= 3) { return {(*this)[1], (*this)[2]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> yw () const noexcept requires(extent >= 4) { return {(*this)[1], (*this)[3]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> yw ()       noexcept requires(extent >= 4) { return {(*this)[1], (*this)[3]}; }

		utils_gpu_available constexpr const vec<const       value_type&, 2> zx () const noexcept requires(extent >= 3) { return {(*this)[2], (*this)[0]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> zx ()       noexcept requires(extent >= 3) { return {(*this)[2], (*this)[0]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> zy () const noexcept requires(extent >= 2) { return {(*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> zy ()       noexcept requires(extent >= 2) { return {(*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> zw () const noexcept requires(extent >= 4) { return {(*this)[2], (*this)[3]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> zw ()       noexcept requires(extent >= 4) { return {(*this)[2], (*this)[3]}; }

		utils_gpu_available constexpr const vec<const       value_type&, 2> wx () const noexcept requires(extent >= 3) { return {(*this)[3], (*this)[0]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> wx ()       noexcept requires(extent >= 3) { return {(*this)[3], (*this)[0]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> wy () const noexcept requires(extent >= 2) { return {(*this)[3], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> wy ()       noexcept requires(extent >= 2) { return {(*this)[3], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 2> wz () const noexcept requires(extent >= 4) { return {(*this)[3], (*this)[2]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 2> wz ()       noexcept requires(extent >= 4) { return {(*this)[3], (*this)[2]}; }
		
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> xyz()       noexcept requires(extent >= 3) { return {(*this)[0], (*this)[1], (*this)[2]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> xyz() const noexcept requires(extent >= 3) { return {(*this)[0], (*this)[1], (*this)[2]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> xzy()       noexcept requires(extent >= 3) { return {(*this)[0], (*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> xzy() const noexcept requires(extent >= 3) { return {(*this)[0], (*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> yxz()       noexcept requires(extent >= 3) { return {(*this)[1], (*this)[0], (*this)[2]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> yxz() const noexcept requires(extent >= 3) { return {(*this)[1], (*this)[0], (*this)[2]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> yzx()       noexcept requires(extent >= 3) { return {(*this)[1], (*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> yzx() const noexcept requires(extent >= 3) { return {(*this)[1], (*this)[2], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> zxy()       noexcept requires(extent >= 3) { return {(*this)[2], (*this)[0], (*this)[1]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> zxy() const noexcept requires(extent >= 3) { return {(*this)[2], (*this)[0], (*this)[1]}; }
		utils_gpu_available constexpr       vec<const_aware_value_type&, 3> zyx()       noexcept requires(extent >= 3) { return {(*this)[2], (*this)[1], (*this)[0]}; }
		utils_gpu_available constexpr const vec<const       value_type&, 3> zyx() const noexcept requires(extent >= 3) { return {(*this)[2], (*this)[1], (*this)[0]}; }
		#pragma endregion swizzle

#pragma region distances
		utils_gpu_available constexpr value_type get_length2() const noexcept 
			{
			value_type ret{0}; 
			base_t::for_each([&ret](const auto& value) { ret += value * value; });
			return ret; 
			}
		utils_gpu_available constexpr value_type get_length () const noexcept
			requires (std::floating_point<value_type>)
			{
			return std::sqrt(get_length2()); 
			}

		utils_gpu_available constexpr self_t& set_length(value_type value) noexcept
			requires(!storage_type.is_const() && std::floating_point<value_type>)
			{
			*this = normalize() * value;
			return *this; 
			}

		utils_gpu_available constexpr nonref_self_t normalize() const noexcept 
			requires (std::floating_point<value_type>)
			{
			const nonref_self_t copy{*this};
			const auto length2{get_length2()};
			if (length2 != value_type{0} && length2 != value_type{1})
				{
				const auto length{std::sqrt(length2)};
				const auto length_inverse{value_type{1} / length};
				const auto ret{copy * length_inverse};
				return ret;
				}
			return copy;
			}
		utils_gpu_available constexpr self_t& normalize_self() noexcept 
			requires(!storage_type.is_const() && std::floating_point<value_type>)
			{ 
			return *this = normalize(); 
			}
		
		/// <summary> Evaluate distance^2 in the size of this vec. Missing coordinates are considered 0. </summary>
		template <utils::details::vector::concepts::compatible_vector<self_t> a_T, utils::details::vector::concepts::compatible_vector<self_t> b_T>
		utils_gpu_available static constexpr value_type distance2(const a_T& a, const b_T& b) noexcept
			requires (std::floating_point<typename a_T::value_type> && std::floating_point<typename b_T::value_type>)
			{
			constexpr auto sizes{details::pair_sizes<a_T, b_T>()};

			value_type ret{0};
			size_t i{0};
			for (; i < sizes.min; i++)
				{
				value_type tmp{a[i] - b[i]};
				ret += tmp * tmp;
				}
					
			     if constexpr (sizes.a > sizes.b) { for (; i < sizes.a; i++) { ret += a[i] * a[i]; } }
			else if constexpr (sizes.a < sizes.b) { for (; i < sizes.b; i++) { ret += b[i] * b[i]; } }

			return ret;
			}

		template <utils::details::vector::concepts::compatible_vector<self_t> a_T, utils::details::vector::concepts::compatible_vector<self_t> b_T>
		utils_gpu_available static constexpr value_type distance2_shared(const a_T& a, const b_T& b) noexcept
			requires (std::floating_point<typename a_T::value_type>&& std::floating_point<typename b_T::value_type>)
			{
			constexpr auto sizes{details::pair_sizes<a_T, decltype(b)>()};

			value_type ret{0};
			size_t i{0};
			for (; i < sizes.min; i++)
				{
				value_type tmp{a[i] - b[i]};
				ret += tmp * tmp;
				}

			return ret;
			}

		/// <summary> Evaluate distance in the size of this vec. Missing coordinates are considered 0. </summary>
		template <utils::details::vector::concepts::compatible_vector<self_t> a_T, utils::details::vector::concepts::compatible_vector<self_t> b_T>
		utils_gpu_available static constexpr value_type distance(const a_T& a, const b_T& b) noexcept
			requires (std::floating_point<typename a_T::value_type>&& std::floating_point<typename b_T::value_type>)
			{
			return std::sqrt(distance2(a, b)); 
			}

		/// <summary> Evaluate distance in all the axes of the smaller vec. </summary>
		template <utils::details::vector::concepts::compatible_vector<self_t> a_T, utils::details::vector::concepts::compatible_vector<self_t> b_T>
		utils_gpu_available static constexpr value_type distance_shared(const a_T& a, const b_T& b) noexcept
			requires (std::floating_point<typename a_T::value_type>&& std::floating_point<typename b_T::value_type>)
			{
			return std::sqrt(distance_shared2(a, b)); 
			}

		utils_gpu_available static constexpr nonref_self_t slerp_fast(const self_t& a, const self_t& b, value_type t) noexcept
			requires (std::floating_point<value_type>)
			{
			return utils::math::lerp(a, b, t).normalize() * (utils::math::lerp(a.get_length(), b.get_length(), t));
			}
		utils_gpu_available static constexpr nonref_self_t tlerp_fast(const self_t& a, const self_t& b, value_type t) noexcept
			requires (std::floating_point<value_type>)
			{
			return utils::math::lerp(a, b, t).normalize() * std::sqrt(utils::math::lerp(a.get_length2(), b.get_length2(), t));
			}
		utils_gpu_available static constexpr nonref_self_t slerp(const self_t& a, const self_t& b, value_type t) noexcept //TODO test
			requires (std::floating_point<value_type>)
			{
			value_type dot = utils::math::clamp(self_t::dot(a, b), -1.0f, 1.0f);
			value_type theta = std::acos(dot) * t;
			nonref_self_t relative_vec = (b - a * dot).normalize();
			return ((a * std::cos(theta)) + (relative_vec * std::sin(theta)));
			}

		struct create : ::utils::oop::non_constructible
			{
			utils_gpu_available static constexpr self_t zero    () noexcept requires(storage_type.is_owner() && extent >= 1) { return {value_type{ 0}}; }

			utils_gpu_available static constexpr self_t rr      () noexcept requires(storage_type.is_owner() && extent == 1) { return {value_type{ 1}}; }
			utils_gpu_available static constexpr self_t ll      () noexcept requires(storage_type.is_owner() && extent == 1) { return {value_type{-1}}; }
			utils_gpu_available static constexpr self_t rr      () noexcept requires(storage_type.is_owner() && extent >  1) { return {value_type{ 1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t ll      () noexcept requires(storage_type.is_owner() && extent >  1) { return {value_type{-1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t right   () noexcept requires(storage_type.is_owner() && extent >= 1) { return rr(); }
			utils_gpu_available static constexpr self_t left    () noexcept requires(storage_type.is_owner() && extent >= 1) { return ll(); }

			utils_gpu_available static constexpr self_t up      () noexcept requires(storage_type.is_owner() && extent == 2) { return {value_type{ 0}, value_type{-1}}; }
			utils_gpu_available static constexpr self_t dw      () noexcept requires(storage_type.is_owner() && extent == 2) { return {value_type{ 0}, value_type{ 1}}; }
			utils_gpu_available static constexpr self_t up      () noexcept requires(storage_type.is_owner() && extent >  2) { return {value_type{ 0}, value_type{-1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t dw      () noexcept requires(storage_type.is_owner() && extent >  2) { return {value_type{ 0}, value_type{ 1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t down    () noexcept requires(storage_type.is_owner() && extent >= 2) { return dw(); }

			utils_gpu_available static constexpr self_t fw      () noexcept requires(storage_type.is_owner() && extent == 3) { return {value_type{ 0}, value_type{ 0}, value_type{ 1}}; }
			utils_gpu_available static constexpr self_t bw      () noexcept requires(storage_type.is_owner() && extent == 3) { return {value_type{ 0}, value_type{ 0}, value_type{-1}}; }
			utils_gpu_available static constexpr self_t fw      () noexcept requires(storage_type.is_owner() && extent >  3) { return {value_type{ 0}, value_type{ 0}, value_type{ 1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t bw      () noexcept requires(storage_type.is_owner() && extent >  3) { return {value_type{ 0}, value_type{ 0}, value_type{-1}, value_type{ 0}}; }
			utils_gpu_available static constexpr self_t forward () noexcept requires(storage_type.is_owner() && extent >= 3) { return fw(); }
			utils_gpu_available static constexpr self_t backward() noexcept requires(storage_type.is_owner() && extent >= 3) { return bw(); }
			};
				
		template <utils::details::vector::concepts::compatible_vector<self_t> b_t>
		utils_gpu_available static constexpr value_type dot(const self_t& a, const b_t& b) noexcept
			{
			value_type ret{0};
			for (size_t i{0}; i < extent; i++)
				{
				ret += a[i] * b[i];
				} 
			return ret;
			}

		utils_gpu_available constexpr auto& scale_self(const float& scaling) noexcept;
		utils_gpu_available constexpr auto& rotate_self(const angle::concepts::angle auto& rotation) noexcept;
		utils_gpu_available constexpr auto& translate_self(const vec2f& translation) noexcept;
		};
	

	namespace operators
		{
		inline constexpr struct _dot
			{
			template <concepts::vec a_t>
			class _inner;

			template <concepts::vec a_t>
			utils_gpu_available inline friend _inner<a_t> operator<(const a_t& lhs, _dot proxy) noexcept { return {lhs}; }

			template <concepts::vec a_t>
			class _inner
				{
				public:
					template <concepts::vec b_t>
					typename a_t::value_type operator>(const b_t& rhs) const noexcept  { return a_t::dot(lhs, rhs); }
					utils_gpu_available _inner(const a_t& lhs) noexcept : lhs{lhs} {}
				private:
					const a_t& lhs;
				};

			} dot;

		inline constexpr struct _cross
			{
			template <concepts::vec a_t>
			class _inner;

			template <concepts::vec a_t>
			utils_gpu_available inline friend _inner<a_t> operator<(const a_t& lhs, _cross proxy) noexcept { return {lhs}; }

			template <concepts::vec a_t>
			class _inner
				{
				public:
					template <concepts::vec b_t>
					a_t::nonref_derived_t operator>(const b_t& rhs) const noexcept { return lhs * rhs; }
					utils_gpu_available _inner(const a_t& lhs) noexcept : lhs{lhs} {}
				private:
					const a_t& lhs;
				};

			} cross;
		}
	}

#pragma once

#include <span>
#include <array>
#include <vector>
#include <memory>
#include <cassert>
#include <concepts>
#include <type_traits>

#include "math/math.h"
#include "compilation/debug.h"
#include "oop/disable_move_copy.h"
#include "concepts.h"

namespace utils::storage
	{
	struct type
		{
		utils_gpu_available consteval bool is_observer() const noexcept { return !owner_value; }
		utils_gpu_available consteval bool is_owner   () const noexcept { return  owner_value; }
		utils_gpu_available consteval bool is_const   () const noexcept { return  const_value; }
		utils_gpu_available consteval bool can_construct_from_const() const noexcept { return owner_value || const_value; }

		struct create : utils::oop::non_constructible
			{
			template <typename T2>
			utils_gpu_available static consteval type from          (                        ) noexcept { return {.owner_value{!std::is_reference_v<T2>}, .const_value{std::is_const_v<std::remove_reference_t<T2>>}}; }
			utils_gpu_available static consteval type owner         (bool const_value = false) noexcept { return {.owner_value{true                    }, .const_value{const_value                                 }}; }
			utils_gpu_available static consteval type observer      (bool const_value = false) noexcept { return {.owner_value{false                   }, .const_value{const_value                                 }}; }
			utils_gpu_available static consteval type const_observer(                        ) noexcept { return {.owner_value{false                   }, .const_value{true                                        }}; }
			};

		const bool owner_value;
		const bool const_value;
		};

	template <typename A, typename B>
	struct constness_matching
		{
		static constexpr bool other_const{std::remove_cvref_t<B>::storage_type.is_const() || std::is_const_v<B>};
		static constexpr bool compatible_constness{std::remove_cvref_t<A>::storage_type.is_const() || std::remove_cvref_t<A>::storage_type.is_owner() || !other_const};
		};

	template <typename T, type storage_type>
	using storage_type_for = std::conditional_t
		/**/<
		/**/storage_type.is_owner(),
		/**/std::conditional_t
		/**//**/<
		/**//**/storage_type.is_const(),
		/**//**/const T,
		/**//**/T
		/**//**/>,
		/**/std::conditional_t
		/**//**/<
		/**//**/storage_type.is_const(),
		/**//**/const T&,
		/**//**/T&
		/**//**/>
		/**/>;

	template <typename T>
	struct single
		{
		using template_type = T;
		using value_type = std::remove_reference_t<T>;
		inline static constexpr type storage_type{utils::storage::type::create::from<T>()};

		using inner_storage_t = std::conditional_t
			<
			storage_type.is_owner(),
			template_type,
			std::conditional_t
				<
				storage_type.is_const(),
				std::reference_wrapper<const value_type>,
				std::reference_wrapper<      value_type>
				>
			>;

		inner_storage_t storage;

		utils_gpu_available constexpr single() = default;
		template <typename ...Args>
		utils_gpu_available constexpr single(Args&&... args) : storage{std::forward<Args>(args)...} {}

		utils_gpu_available constexpr          const value_type& value     () const noexcept                                    { return static_cast<const value_type&>(storage); }
		utils_gpu_available constexpr                value_type& value     ()       noexcept requires(!storage_type.is_const()) { return static_cast<      value_type&>(storage); }
		utils_gpu_available constexpr operator const value_type&           () const noexcept                                    { return  value(); }
		utils_gpu_available constexpr operator       value_type&           ()       noexcept requires(!storage_type.is_const()) { return  value(); }
		utils_gpu_available constexpr          const value_type* operator->() const noexcept                                    { return &value(); }
		utils_gpu_available constexpr                value_type* operator->()       noexcept requires(!storage_type.is_const()) { return &value(); }
		};

	template <typename T, size_t extent, bool sequential_observer>
	struct multiple;
	
	namespace concepts
		{
		template <typename T>
		concept single = std::derived_from<std::remove_cvref_t<T>, utils::storage::single<typename std::remove_cvref_t<T>::template_type>>;

		template <typename storage_type, typename value_type>
		concept storage_compatible_with_type = std::same_as<typename utils::remove_const_reference_t<storage_type>::value_type, utils::remove_const_reference_t<value_type>>;
		template <typename value_type, typename storage_type>
		concept type_compatible_with_storage = std::same_as<utils::remove_const_reference_t<                     value_type>, typename utils::remove_const_reference_t<storage_type>::value_type> ||
		                (single<value_type> && std::same_as<utils::remove_const_reference_t<typename value_type::value_type>, typename utils::remove_const_reference_t<storage_type>::value_type>);

		template <typename T, typename value_type>
		concept can_construct_value_type = std::constructible_from<value_type, T>;

		template <typename T>
		concept vector = std::same_as<std::remove_cvref_t<T>, std::vector<typename std::remove_cvref_t<T>::value_type>>;
		template <typename T>
		concept array  = std::same_as<std::remove_cvref_t<T>, std::array<typename std::remove_cvref_t<T>::value_type, std::tuple_size<std::remove_cvref_t<T>>::value>>; //Note: using tuple_size_t won't work
		template <typename T>
		concept span   = std::same_as<std::remove_cvref_t<T>, std::span<typename std::remove_cvref_t<T>::element_type, std::remove_cvref_t<T>::extent>>;

		template <typename T>
		concept multiple = std::derived_from<std::remove_cvref_t<T>, utils::storage::multiple<typename std::remove_cvref_t<T>::template_type, std::remove_cvref_t<T>::extent, std::remove_cvref_t<T>::sequential_observer>>;
		
		template <typename T, typename other_T>
		concept compatible_multiple = multiple<T> && multiple<other_T> && storage_compatible_with_type<T, typename std::remove_cvref_t<other_T>::value_type>;

		template <typename T, typename storage_type>
		concept operator_parameter = compatible_multiple<T, std::remove_cvref_t<storage_type>> || type_compatible_with_storage<T, std::remove_cvref_t<storage_type>>;

		template <typename T>
		concept non_const = utils::concepts::non_const<T> || !T::storage_type.is_const();
		}

	struct construct_flag_data_t {};
	struct construct_flag_size_t {};

	inline static constexpr construct_flag_data_t construct_flag_data;
	inline static constexpr construct_flag_size_t construct_flag_size;


	/// <summary>
	/// Abstracts storage as static, dynamic owners, static and dynamic observers transparently. 
	/// A single templated class implementation inheriting from or containing one multiple can be used with each storage type across the program program.
	/// Given a class that inherits from multiple, the available operators will return a multiple with the same properties if "owner_self_t" is not defined.
	/// Given a class that inherits from multiple and defines owner_self_t as itself, the available operators will return that same class.
	///		For this to work reliably, that class should also expose multiple's constructors
	/// </summary>
	/// <typeparam name="T">
	/// T can be a pure value type, but also have constness defined and/or be a reference.
	/// In case T is a reference, the internal storage will use std::reference_wrapper&lt;T&gt;, however the access operators and iterators of the class will
	/// transparently behave as though they were pure references.
	/// </typeparam>
	/// <typeparam name="EXTENT"></typeparam>
	/// <typeparam name="SEQUENTIAL_OBSERVER">If the storage is an observer, specify wether the observed content is sequential in memory (std::span&lt;T&gt;) or spread in arbitrary locations (std::vector&lt;std::reference_wrapper&lt;T&gt;&gt;)</typeparam>
	template <typename T, size_t EXTENT = std::dynamic_extent, bool SEQUENTIAL_OBSERVER = true>
	struct multiple
		{
		using template_type = T;
		using const_aware_value_type = std::remove_reference_t<T>;
		using value_type = std::remove_cvref_t<T>;
		inline static constexpr type storage_type{utils::storage::type::create::from<T>()};
		inline static constexpr bool sequential_observer{SEQUENTIAL_OBSERVER};
		inline static constexpr size_t extent{EXTENT};
		using multiple_t      = multiple<T                      , extent, sequential_observer>;
		using self_t          = multiple<T                      , extent, sequential_observer>;
		//using owner_self_t    = multiple<value_type             , extent, sequential_observer>;
		//using observer_self_t = multiple<const_aware_value_type&, extent, sequential_observer>;

		template <typename T2>
		using owner_storage_t = std::conditional_t<extent == std::dynamic_extent, std::vector<T2>, std::array<T2, extent>>;

		using inner_storage_t = std::conditional_t
			<
			storage_type.is_owner(),
			owner_storage_t<const_aware_value_type>,
			std::conditional_t
			/**/<
			/**/sequential_observer,
			/**/std::span<const_aware_value_type, extent>,
			/**/owner_storage_t<std::reference_wrapper<const_aware_value_type>>
			/**/>
			>;

		inner_storage_t storage;

		utils_gpu_available        constexpr size_t size() const noexcept requires(extent == std::dynamic_extent) { return storage.size(); }
		utils_gpu_available static consteval size_t size()       noexcept requires(extent != std::dynamic_extent) { return extent; }

		utils_gpu_available constexpr       bool   empty() const noexcept { return storage.empty(); }
		utils_gpu_available constexpr const auto   data () const noexcept { return storage.data (); }
		utils_gpu_available constexpr       auto   data ()       noexcept { return storage.data (); }

		utils_gpu_available constexpr const       value_type& operator[](size_t index) const noexcept                                    { return static_cast<const       value_type&>(storage   [index]); }
		utils_gpu_available constexpr const_aware_value_type& operator[](size_t index)       noexcept requires(!storage_type.is_const()) { return static_cast<const_aware_value_type&>(storage   [index]); }
		utils_gpu_available constexpr const       value_type& at        (size_t index) const                                             { return static_cast<const       value_type&>(storage.at(index)); }
		utils_gpu_available constexpr const_aware_value_type& at        (size_t index)                requires(!storage_type.is_const()) { return static_cast<const_aware_value_type&>(storage.at(index)); }
 
		utils_gpu_available constexpr void rebind(size_t index, const_aware_value_type& new_value) noexcept requires(storage_type.is_observer())
			{
			if constexpr (utils::compilation::debug) { assert(index < size()); }
			storage[index] = new_value;
			}

		template <typename inner_iterator_t, typename T2>
		struct base_iterator
			{
			using self_t            = base_iterator<inner_iterator_t, T2>;
			using iterator_category = std::random_access_iterator_tag;
			using difference_type   = std::ptrdiff_t;
			using value_type        = T2;
			using pointer           = T2*;
			using reference         = T2&;

			inner_iterator_t inner_iterator;

			utils_gpu_available constexpr self_t  operator+ (difference_type value) const noexcept { return {inner_iterator + value}; }
			utils_gpu_available constexpr self_t  operator- (difference_type value) const noexcept { return {inner_iterator - value}; }
			utils_gpu_available constexpr self_t& operator+=(difference_type value) const noexcept { inner_iterator += value; return *this; }
			utils_gpu_available constexpr self_t& operator-=(difference_type value) const noexcept { inner_iterator += value; return *this; }

			utils_gpu_available constexpr self_t& operator++(   ) noexcept { inner_iterator++; return *this; }
			utils_gpu_available constexpr self_t& operator--(   ) noexcept { inner_iterator--; return *this; }
			utils_gpu_available constexpr self_t  operator++(int) noexcept { self_t tmp{*this}; ++(*this); return tmp; }
			utils_gpu_available constexpr self_t  operator--(int) noexcept { self_t tmp{*this}; --(*this); return tmp; }

			utils_gpu_available constexpr auto operator<=>(const self_t& other) const noexcept { return inner_iterator <=> other.inner_iterator; }
			utils_gpu_available constexpr bool operator== (const self_t& other) const noexcept { return inner_iterator ==  other.inner_iterator; }

			utils_gpu_available constexpr reference operator* () noexcept { return  *inner_iterator; }
			utils_gpu_available constexpr pointer   operator->() noexcept { return &*inner_iterator; }
			};

		using iterator               = base_iterator<typename inner_storage_t::iterator              ,       const_aware_value_type>;
		using reverse_iterator       = base_iterator<typename inner_storage_t::reverse_iterator      ,       const_aware_value_type>;
		//TODO When CUDA supports C++23 this will suffice for all containers. Until then, std::span doesn't have const_iterator defined in C++20
		//using const_iterator         = base_iterator<typename inner_storage_t::const_iterator        , const const_aware_value_type>;
		//using const_reverse_iterator = base_iterator<typename inner_storage_t::const_reverse_iterator, const const_aware_value_type>;
		//until then, keep this uglyness
		template <typename       T2> struct inner_storage_const_iterator             { using type =       typename T2::const_iterator        ; };
		template <typename       T2> struct inner_storage_const_reverse_iterator     { using type =       typename T2::const_reverse_iterator; };
		template <concepts::span T2> struct inner_storage_const_iterator        <T2> { using type = const typename T2::      iterator        ; };
		template <concepts::span T2> struct inner_storage_const_reverse_iterator<T2> { using type = const typename T2::      reverse_iterator; };

		template <typename T2> using inner_storage_const_iterator_t         = typename inner_storage_const_iterator        <T2>::type;
		template <typename T2> using inner_storage_const_reverse_iterator_t = typename inner_storage_const_reverse_iterator<T2>::type;

		using const_iterator         = base_iterator<inner_storage_const_iterator_t        <inner_storage_t>, const const_aware_value_type>;
		using const_reverse_iterator = base_iterator<inner_storage_const_reverse_iterator_t<inner_storage_t>, const const_aware_value_type>;
		
		utils_gpu_available constexpr       iterator           begin()       noexcept { return {storage. begin ()}; }
		utils_gpu_available constexpr const_iterator           begin() const noexcept { return {storage. begin ()}; }
		utils_gpu_available constexpr const_iterator          cbegin()       noexcept { if constexpr (!concepts::span<inner_storage_t>) { return {storage.cbegin()}; } else { return {storage.begin()}; } }
		utils_gpu_available constexpr       iterator           end  ()       noexcept { return {storage. end   ()}; }
		utils_gpu_available constexpr const_iterator           end  () const noexcept { return {storage. end   ()}; }
		utils_gpu_available constexpr const_iterator          cend  ()       noexcept { if constexpr (!concepts::span<inner_storage_t>) { return {storage.cend  ()}; } else { return {storage.end  ()}; } }
			 
		utils_gpu_available constexpr       reverse_iterator  rbegin()       noexcept { return {storage. rbegin()}; }
		utils_gpu_available constexpr const_reverse_iterator  rbegin() const noexcept { return {storage. rbegin()}; }
		utils_gpu_available constexpr const_reverse_iterator crbegin()       noexcept { if constexpr (!concepts::span<inner_storage_t>) { return {storage.crbegin()}; } else { return {storage.rbegin()}; } }
		utils_gpu_available constexpr       reverse_iterator  rend  ()       noexcept { return {storage. rend  ()}; }
		utils_gpu_available constexpr const_reverse_iterator  rend  () const noexcept { return {storage. rend  ()}; }
		utils_gpu_available constexpr const_reverse_iterator crend  ()       noexcept { if constexpr (!concepts::span<inner_storage_t>) { return {storage.crend  ()}; } else { return {storage.crend  ()}; } }

		//struct create : utils::oop::non_constructible, utils::oop::non_copyable, utils::oop::non_movable
		//	{
		//	using ret_t = multiple<T, extent, sequential_observer>;
		//	utils_gpu_available inline static constexpr ret_t _default() noexcept requires(storage_type.is_owner()) { return ret_t{}; }
		//	utils_gpu_available inline static constexpr ret_t from_other_storage(const concepts::multiple auto& other) noexcept requires(storage_type.can_construct_from_const()) { return ret_t{.storage{inner_create(other)}}; }
		//	utils_gpu_available inline static constexpr ret_t from_other_storage(      concepts::multiple auto& other) noexcept { return ret_t{.storage{inner_create(other)}}; }
		//	utils_gpu_available inline static constexpr ret_t size(const size_t& size) noexcept
		//		requires(storage_type.is_owner() && concepts::vector<inner_storage_t>)
		//		{
		//		return ret_t{.storage(size)};
		//		}
		//	utils_gpu_available inline static constexpr ret_t size(const size_t& size) noexcept
		//		requires(storage_type.is_owner() && concepts::array<inner_storage_t>)
		//		{
		//		assert(size == extent);
		//		return ret_t{};
		//		}
		//	
		//	template <concepts::can_construct_value_type<typename inner_storage_t::value_type> ...Args>
		//	utils_gpu_available inline static constexpr ret_t data(Args&&... args) noexcept
		//		requires
		//			(
		//			concepts::vector<inner_storage_t> || 
		//				(
		//				concepts::array<inner_storage_t> &&
		//					(
		//					(storage_type.is_owner   () && sizeof...(Args) <= extent) ||
		//					(storage_type.is_observer() && sizeof...(Args) == extent)
		//					)
		//				)
		//			) 
		//		{
		//		return ret_t{.storage{std::forward<Args>(args)...}};
		//		}
		//	};


		utils_gpu_available constexpr multiple() requires(storage_type.is_owner()) = default;
		
		utils_gpu_available constexpr multiple(inner_storage_t&& storage) : storage{storage} {}
		
		//utils_gpu_available constexpr multiple(const multiple<T, extent, sequential_observer>&  copy) noexcept : storage{          copy.storage } {}
		//utils_gpu_available constexpr multiple(      multiple<T, extent, sequential_observer>&& move) noexcept : storage{std::move(move.storage)} {}
		//utils_gpu_available constexpr multiple& operator=(const multiple<T, extent, sequential_observer>&  copy) noexcept { storage =           copy.storage ; return *this; }
		//utils_gpu_available constexpr multiple& operator=(      multiple<T, extent, sequential_observer>&& move) noexcept { storage = std::move(move.storage); return *this; }
		
		utils_gpu_available constexpr multiple(size_t size)
			requires(storage_type.is_owner() && (concepts::array<inner_storage_t> || concepts::vector<inner_storage_t>)) :
			multiple{utils::storage::construct_flag_size, size}
			{};
		
		template <concepts::can_construct_value_type<typename inner_storage_t::value_type> ...Args>
			requires((!std::same_as<utils::storage::construct_flag_data_t, Args>) && ...)
		utils_gpu_available constexpr multiple(Args&&... args) : multiple{utils::storage::construct_flag_data, std::forward<Args>(args)...}
			{}
		
		utils_gpu_available constexpr multiple(utils::storage::construct_flag_size_t, size_t size)
			requires(storage_type.is_owner() && concepts::vector<inner_storage_t>) :
			storage(size)
			{};
		utils_gpu_available constexpr multiple(utils::storage::construct_flag_size_t, size_t size)
			requires(storage_type.is_owner() && concepts::array<inner_storage_t>)
			{
			assert(size == extent);
			};
		
		template <concepts::can_construct_value_type<typename inner_storage_t::value_type> ...Args>
		utils_gpu_available constexpr multiple(utils::storage::construct_flag_data_t, Args&&... args)
			requires
				(
				concepts::vector<inner_storage_t> || 
					(
					concepts::array<inner_storage_t> &&
						(
						(storage_type.is_owner   () && sizeof...(Args) <= extent) ||
						(storage_type.is_observer() && sizeof...(Args) == extent)
						)
					)
				) 
			:
			storage{std::forward<Args>(args)...} {}
				
		template <typename iterator_t>
		utils_gpu_available constexpr multiple(iterator_t first, size_t count)
			requires(concepts::span<inner_storage_t>) :
			storage{first, count} {}

		template <bool allow_memberwise_cast, concepts::multiple other_t>
		static constexpr inner_storage_t inner_create(other_t& other) noexcept
			requires 
				(
				std::constructible_from<typename inner_storage_t::value_type, typename other_t::inner_storage_t::reference> 
				&& constness_matching<self_t, other_t>::compatible_constness
				)
			{
			using inner_value_type = typename inner_storage_t::value_type;

			// If owner can construct each element from each other's element freely.
			// If sparse observer with const value can construct without issues
			// If sparse observer without const value, the source mustn't be const and mustn't have const values
			static constexpr bool other_const          {other_t::storage_type.is_const() || std::is_const_v<other_t>};
			static constexpr bool compatible_constness {storage_type.is_const() || storage_type.is_owner() || !other_const};
			static constexpr bool initialize_memberwise{storage_type.is_owner() || (storage_type.is_observer() && !sequential_observer)};
			static constexpr bool other_may_have_less  {storage_type.is_owner()};

			if constexpr (storage_type.is_observer() && extent != std::dynamic_extent)
				{
				if constexpr (other_t::extent != std::dynamic_extent)
					{
					static_assert(extent <= other_t::extent);
					}
				else
					{
					assert(extent <= other.size());
					}
				}

			if constexpr (compatible_constness)
				{
				if constexpr (initialize_memberwise)
					{
					if constexpr (utils::storage::concepts::vector<inner_storage_t> && utils::storage::concepts::vector<typename other_t::inner_storage_t>)
						{
						inner_storage_t ret; 
						ret.reserve(other.storage.size());
						for (auto& element : other.storage) 
							{
							ret.emplace_back(element); 
							}
						return ret;
						}
					else
						{
						static constexpr size_t ret_extent{extent == std::dynamic_extent ? other_t::extent : extent};

						auto getter{[&other](size_t i)
							{
							if constexpr (other_may_have_less)
								{
								if (i >= other.storage.size()) 
									{
									return inner_value_type{};
									}
								}
							if constexpr (allow_memberwise_cast)
								{
								return static_cast<inner_value_type>(other[i]);
								}
							else if constexpr (!allow_memberwise_cast)
								{
								return other[i];
								}
							}};

						return[&]<std::size_t... is>(std::index_sequence<is...>)
							{
							return inner_storage_t{getter(is)...};
							//
							//if constexpr (other_may_have_less) { return inner_storage_t{(is < other.storage.size() ? static_cast<inner_value_type>(other[is]) : inner_value_type{})...}; }
							//else                               { return inner_storage_t{                             static_cast<inner_value_type>(other[is])                      ...}; }
							}(std::make_index_sequence<ret_extent>());
						}
					}
				else if constexpr (storage_type.is_observer() && sequential_observer)
					{
					if constexpr (extent != std::dynamic_extent) { assert(extent == other.size()); }
					return inner_storage_t(&(*(other.begin())), other.size());
					}
				}
			}
		
		//TODO set inner_create<true> to inner_create<false>
		//force static_cast if user wants inner_create<true>
		utils_gpu_available constexpr multiple(const concepts::compatible_multiple<self_t> auto& other) noexcept requires(storage_type.can_construct_from_const()) : storage{inner_create<true>(other)} {}
		utils_gpu_available constexpr multiple(      concepts::compatible_multiple<self_t> auto& other) noexcept : storage{inner_create<true>(other)} {}
			
		utils_gpu_available constexpr void for_each(this auto& self, auto callback) noexcept { for (auto& value : self) { callback(value); } }

		template <typename self_t>
		utils_gpu_available constexpr auto for_each_to_new(this self_t& self, auto callback) noexcept
			{
			typename self_t::owner_self_t ret;
			for (size_t i{0}; i < extent; i++)
				{
				ret[i] = callback(self[i]);
				}
			return ret;
			}

		template <typename self_t>
		utils_gpu_available constexpr auto& operator_self_assign(this self_t& self, const concepts::type_compatible_with_storage<self_t> auto& other, auto callback) noexcept
			{
			for (auto& value : self) { callback(value, other); }
			return self;
			}

		template <typename self_t, typename T2, size_t EXTENT2, bool SEQUENTIAL_OBSERVER2>
		utils_gpu_available constexpr auto& operator_self_assign(this self_t& self, const multiple<T2, EXTENT2, SEQUENTIAL_OBSERVER2>& other, auto callback) noexcept
			requires (concepts::compatible_multiple<self_t, multiple<T2, EXTENT2, SEQUENTIAL_OBSERVER2>>)
			{
			const size_t indices{utils::math::min(self.multiple::size(), other.size())};
			for (size_t i{0}; i < indices; i++)
				{
				callback(self[i], other[i]);
				}
			return self;
			}

		template <typename self_t>
		utils_gpu_available constexpr auto operator_to_new(this const self_t& self, const concepts::type_compatible_with_storage<self_t> auto& other, auto callback) noexcept
			{
			typename self_t::owner_self_t ret;
			const size_t indices{self.multiple::size()};
			for (size_t i{0}; i < indices; i++)
				{
				ret[i] = callback(self[i], other);
				}
			return ret;
			}

		template <typename self_t, typename T2, size_t EXTENT2, bool SEQUENTIAL_OBSERVER2>
		utils_gpu_available constexpr auto operator_to_new(this const self_t& self, const multiple<T2, EXTENT2, SEQUENTIAL_OBSERVER2>& other, auto callback) noexcept
			requires (concepts::compatible_multiple<self_t, multiple<T2, EXTENT2, SEQUENTIAL_OBSERVER2>>)
			{
			typename self_t::owner_self_t ret;
			const size_t indices{utils::math::min(self.multiple::size(), other.size())};
			for (size_t i{0}; i < indices; i++)
				{
				ret[i] = callback(self[i], other[i]);
				}
			return ret;
			}

		//template <utils::concepts::non_const self_t>
		//utils_gpu_available constexpr self_t& operator=(this self_t& self, const concepts::type_compatible_with_storage<self_t> auto& other) noexcept
		//	requires(!storage_type.is_const())
		//	{
		//	self.operator_self_assign(other, [](auto& a, const auto& b) { a = b; });
		//	return self;
		//	}
		//
		//template <utils::concepts::non_const self_t, typename T2, size_t EXTENT2, bool SEQUENTIAL_OBSERVER2>
		//utils_gpu_available constexpr self_t& operator=(this self_t& self, const multiple<T2, EXTENT2, SEQUENTIAL_OBSERVER2>& other) noexcept
		//	requires (!storage_type.is_const() && concepts::compatible_multiple<self_t, multiple<T2, EXTENT2, SEQUENTIAL_OBSERVER2>>)
		//	{
		//	self.operator_self_assign(other, [](auto& a, const auto& b) { a = b; });
		//	return self;
		//	}
		
		utils_gpu_available constexpr auto& operator+=(this concepts::non_const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_self_assign(other, [](      auto& a, const auto& b) constexpr {        a += b; }); }
		utils_gpu_available constexpr auto& operator-=(this concepts::non_const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_self_assign(other, [](      auto& a, const auto& b) constexpr {        a -= b; }); }
		utils_gpu_available constexpr auto& operator*=(this concepts::non_const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_self_assign(other, [](      auto& a, const auto& b) constexpr {        a *= b; }); }
		utils_gpu_available constexpr auto& operator/=(this concepts::non_const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_self_assign(other, [](      auto& a, const auto& b) constexpr {        a /= b; }); }
		utils_gpu_available constexpr auto& operator%=(this concepts::non_const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_self_assign(other, [](      auto& a, const auto& b) constexpr {        a %= b; }); }
		utils_gpu_available constexpr auto& operator|=(this concepts::non_const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_self_assign(other, [](      auto& a, const auto& b) constexpr {        a |= b; }); }
		utils_gpu_available constexpr auto& operator&=(this concepts::non_const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_self_assign(other, [](      auto& a, const auto& b) constexpr {        a &= b; }); }
		utils_gpu_available constexpr auto  operator+ (this               const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_to_new     (other, [](const auto& a, const auto& b) constexpr { return a +  b; }); }
		utils_gpu_available constexpr auto  operator- (this               const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_to_new     (other, [](const auto& a, const auto& b) constexpr { return a -  b; }); }
		utils_gpu_available constexpr auto  operator* (this               const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_to_new     (other, [](const auto& a, const auto& b) constexpr { return a *  b; }); }
		utils_gpu_available constexpr auto  operator/ (this               const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_to_new     (other, [](const auto& a, const auto& b) constexpr { return a /  b; }); }
		utils_gpu_available constexpr auto  operator% (this               const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_to_new     (other, [](const auto& a, const auto& b) constexpr { return a %  b; }); }
		utils_gpu_available constexpr auto  operator| (this               const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_to_new     (other, [](const auto& a, const auto& b) constexpr { return a |  b; }); }
		utils_gpu_available constexpr auto  operator& (this               const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return self.operator_to_new     (other, [](const auto& a, const auto& b) constexpr { return a &  b; }); }
		utils_gpu_available constexpr bool  operator!=(this               const auto& self, const concepts::operator_parameter<decltype(self)> auto& other) noexcept { return !self.operator==(other); }
		
		#pragma region scalar
		#pragma endregion scalar

		#pragma region self
			utils_gpu_available constexpr auto operator!(this const auto& self) noexcept { return self.for_each_to_new([](const auto& value) constexpr { return !value; }); }
			utils_gpu_available constexpr auto operator-(this const auto& self) noexcept { return self.for_each_to_new([](const auto& value) constexpr { return -value; }); }
		#pragma endregion self

		#pragma region array
			template <typename self_t, concepts::compatible_multiple<self_t> other_t> 
			utils_gpu_available constexpr bool  operator==(this const self_t& self, const other_t& other) noexcept
				{
				const size_t indices{utils::math::min(self.size(), other.size())};
				size_t i{0};
				for (; i < indices; i++)
					{
					if (self[i] != other[i]) { return false; }
					}

					 if(self.multiple::size() > other.size()) { for (; i < self.multiple::size(); i++) { if (self [i] != typename self_t ::value_type{}) { return false; } } }
				else if(self.multiple::size() < other.size()) { for (; i < other         .size(); i++) { if (other[i] != typename other_t::value_type{}) { return false; } } }

				return true;
				}
		#pragma endregion array
		};
	}


//Note: intellisense doesn't seem to find the operators in the class, but not finding them doesn't crash it
//defining the operators outside intellisense just crashes, so... let's keep the errors i guess
// 
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto& operator+=(      multiple_t& storage, const value_t& value) noexcept requires(!multiple_t::storage_type.is_const()) { return storage.operator_self_assign(value, [](      auto& a, const auto& b) {        a += b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto& operator-=(      multiple_t& storage, const value_t& value) noexcept requires(!multiple_t::storage_type.is_const()) { return storage.operator_self_assign(value, [](      auto& a, const auto& b) {        a -= b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto& operator*=(      multiple_t& storage, const value_t& value) noexcept requires(!multiple_t::storage_type.is_const()) { return storage.operator_self_assign(value, [](      auto& a, const auto& b) {        a *= b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto& operator/=(      multiple_t& storage, const value_t& value) noexcept requires(!multiple_t::storage_type.is_const()) { return storage.operator_self_assign(value, [](      auto& a, const auto& b) {        a /= b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto& operator%=(      multiple_t& storage, const value_t& value) noexcept requires(!multiple_t::storage_type.is_const()) { return storage.operator_self_assign(value, [](      auto& a, const auto& b) {        a %= b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto& operator|=(      multiple_t& storage, const value_t& value) noexcept requires(!multiple_t::storage_type.is_const()) { return storage.operator_self_assign(value, [](      auto& a, const auto& b) {        a |= b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto& operator&=(      multiple_t& storage, const value_t& value) noexcept requires(!multiple_t::storage_type.is_const()) { return storage.operator_self_assign(value, [](      auto& a, const auto& b) {        a &= b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto  operator+ (const multiple_t& storage, const value_t& value) noexcept                                                { return storage.operator_to_new     (value, [](const auto& a, const auto& b) { return a +  b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto  operator- (const multiple_t& storage, const value_t& value) noexcept                                                { return storage.operator_to_new     (value, [](const auto& a, const auto& b) { return a -  b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto  operator* (const multiple_t& storage, const value_t& value) noexcept                                                { return storage.operator_to_new     (value, [](const auto& a, const auto& b) { return a *  b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto  operator/ (const multiple_t& storage, const value_t& value) noexcept                                                { return storage.operator_to_new     (value, [](const auto& a, const auto& b) { return a /  b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto  operator% (const multiple_t& storage, const value_t& value) noexcept                                                { return storage.operator_to_new     (value, [](const auto& a, const auto& b) { return a %  b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto  operator| (const multiple_t& storage, const value_t& value) noexcept                                                { return storage.operator_to_new     (value, [](const auto& a, const auto& b) { return a |  b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr auto  operator& (const multiple_t& storage, const value_t& value) noexcept                                                { return storage.operator_to_new     (value, [](const auto& a, const auto& b) { return a &  b; }); }
//template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::operator_parameter<multiple_t> value_t> utils_gpu_available constexpr bool  operator!=(const multiple_t& storage, const value_t& value) noexcept                                                { return !storage.operator==(other); }

	
template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::type_compatible_with_storage<multiple_t> value_t> utils_gpu_available constexpr auto  operator+ (const value_t& value, const multiple_t& storage) noexcept { return storage.operator_to_new(value, [](const auto& a, const auto& b) { return b + a; }); }
template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::type_compatible_with_storage<multiple_t> value_t> utils_gpu_available constexpr auto  operator- (const value_t& value, const multiple_t& storage) noexcept { return storage.operator_to_new(value, [](const auto& a, const auto& b) { return b - a; }); }
template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::type_compatible_with_storage<multiple_t> value_t> utils_gpu_available constexpr auto  operator* (const value_t& value, const multiple_t& storage) noexcept { return storage.operator_to_new(value, [](const auto& a, const auto& b) { return b * a; }); }
template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::type_compatible_with_storage<multiple_t> value_t> utils_gpu_available constexpr auto  operator/ (const value_t& value, const multiple_t& storage) noexcept { return storage.operator_to_new(value, [](const auto& a, const auto& b) { return b / a; }); }
template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::type_compatible_with_storage<multiple_t> value_t> utils_gpu_available constexpr auto  operator% (const value_t& value, const multiple_t& storage) noexcept { return storage.operator_to_new(value, [](const auto& a, const auto& b) { return b % a; }); }
template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::type_compatible_with_storage<multiple_t> value_t> utils_gpu_available constexpr auto  operator| (const value_t& value, const multiple_t& storage) noexcept { return storage.operator_to_new(value, [](const auto& a, const auto& b) { return b | a; }); }
template <utils::storage::concepts::multiple multiple_t, utils::storage::concepts::type_compatible_with_storage<multiple_t> value_t> utils_gpu_available constexpr auto  operator& (const value_t& value, const multiple_t& storage) noexcept { return storage.operator_to_new(value, [](const auto& a, const auto& b) { return b & a; }); }

#pragma region scalar
#pragma endregion scalar
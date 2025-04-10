#pragma once

#include <tuple>
#include <vector>
#include <ranges>
#include <limits>
#include <cassert>
#include <algorithm>

#include "../memory.h"
#include "../optional.h"
#include "../compilation/gpu.h"
#include "../oop/disable_move_copy.h"

#include "../details/warnings_pre.inline.h"

namespace utils::containers
	{
	/// <summary> 
	/// Container that represent a distribution of values along a 1d range of values with pseudo-indices from 0 to std::numeric_limits<size_t>::max().
	/// Adding the same value in subsequent regions merges them in one region (add value X to 1-3, then add X to 4-8, will make a single region with value X in positions 1-8).
	/// Adding the a different value in a sequential region will split that region (add X to 1-9, then add Y to 5-6, will split everything into: X 1-4, Y 5-6, X 7-9).
	///</summary>
	struct region
		{
		size_t begin{0};
		size_t count{1};
		utils_gpu_available inline constexpr size_t end() const noexcept { return begin + count; }

		struct create : utils::oop::non_constructible
			{
			inline static constexpr region full_range() noexcept { return region{0, std::numeric_limits<size_t>::max()}; };
			inline static constexpr region from(size_t start) noexcept { return region{start, std::numeric_limits<size_t>::max() - start}; };
			};

		bool operator==(const region& other) const noexcept = default;
		};

	template <typename T>
	class regions;

	namespace concepts
		{
		template <typename T>
		concept regions = std::same_as<std::remove_cvref_t<T>, containers::regions<typename std::remove_cvref_t<T>::value_type>>;
		}

	template <typename T>
	class regions
		{
		bool opt_eq(const std::optional<T>& a, const T& b) const noexcept
			{
			if (!a.has_value()) { return false; }
			return a == b;
			}

		public:
			using value_type = T;

			struct slot
				{
				size_t begin{0};
				std::optional<T> value_opt{std::nullopt}; 
				};
			template <bool is_const>
			struct read_slot
				{
				using value_type = T;
				const region region;
				utils::observer_ptr<std::conditional_t<is_const, const T, T>> value_ptr;
				};
			std::vector<slot> slots;

			constexpr void set_full_range(const T& new_value) noexcept
				{
				slots.clear();
				slots.reserve(2);
				slots.emplace_back(0, new_value);
				slots.emplace_back(std::numeric_limits<size_t>::max(), std::nullopt);
				}

			constexpr void add(const T& new_value, region fill_region) noexcept
				{
				if (slots.size() == 0)
					{
					auto& emplaced_begin{slots.emplace_back(fill_region.begin, new_value)};
					auto& emplaced_end  {slots.emplace_back(fill_region.end(), std::nullopt)};
					//return {fill_region, emplaced_begin.value_opt.value()};
					return;
					}

				const auto regions_indices{get_region_insert_reference_indices(fill_region)};

				//lbb = last_before_begin
				//lbe = last_before_end
				//fae = first_after_end

				const size_t lbb_index{regions_indices.last_before_begin_index};
				const size_t fab_index{regions_indices.first_after_begin_index};
				const size_t lbe_index{regions_indices.last_before_end_index  };
				const size_t fae_index{regions_indices.first_after_end_index  };

				const bool lbb_is_valid{lbb_index != slots.size()};
				const bool lbe_is_valid{lbe_index != slots.size()};
				const bool fae_is_valid{fae_index != slots.size()};

				const bool at_least_one_element_after_region_before_fae{!fae_is_valid || slots[fae_index].begin > fill_region.end()};
				
				bool extend_lbb {false};

				if (lbb_is_valid)
					{
					const auto& lbb{slots[lbb_index]};
					const bool lbb_ends_after{!lbe_is_valid};
					const bool lbb_shares_value{opt_eq(lbb.value_opt, new_value)};
					if (lbb_shares_value)
						{
						if (lbb_ends_after)
							{
							//If we're here we can early exit
							return;
							}
						extend_lbb = true;
						}
					else if (lbb_ends_after)
						{
						if (fae_is_valid)
							{
							auto& fae{slots[fae_index]};
							const bool fae_shares_value{opt_eq(fae.value_opt, new_value)};
							if (fae.begin == fill_region.end() && fae_shares_value)
								{
								fae.begin = fill_region.begin;
								return;
								}
							}

						//If we're here, lbe isn't valid so we can early exit
						inner_add(lbb_index + 1, fill_region.begin, new_value);
						if (at_least_one_element_after_region_before_fae)
							{
							//slots[lbb_index], not lbb, cause reference invalidated by previous inner_add
							inner_add(lbb_index + 2, fill_region.end(), slots[lbb_index].value_opt);
							}
						else if (lbb_index + 1 == slots.size() - 1)
							{
							inner_add(lbb_index + 2, fill_region.end(), std::nullopt);
							}
							
						return;
						}
					}

				if (lbe_is_valid)
					{
					auto& lbe{slots[lbe_index]};
					const bool lbe_shares_value{opt_eq(lbe.value_opt, new_value)};

					if (!extend_lbb && lbe_shares_value)
						{
						//Extend
						lbe.begin = fill_region.begin;
						return;
						}
					else
						{
						//if fae is strictly after fill.region.end(), lbe has at least 1 element after fill_region.end()
						//if fae isn't valid, lbe extends indefinitely
						if (at_least_one_element_after_region_before_fae)
							{
							// Advance last
							lbe.begin = fill_region.end();

							if (fab_index == lbe_index)
								{
								if (!extend_lbb)
									{
									inner_add(lbb_index + 1, fill_region.begin, new_value);
									}
								return;
								}
							}

						const bool erase_first{!extend_lbb};
						const bool erase_last{!at_least_one_element_after_region_before_fae};

						if (extend_lbb)
							{
							const size_t erase_from{fab_index};
							const size_t erase_to  {lbe_index - (erase_last  ? 0 : 1)};
							slots.erase(slots.begin() + erase_from, slots.begin() + erase_to + 1);
							return;
							}
						else
							{
							auto& fab{slots[fab_index]};
							fab.value_opt = new_value;

							const size_t erase_from{fab_index + 1};
							const size_t erase_to{lbe_index - (erase_last ? 0 : 1)};
							slots.erase(slots.begin() + erase_from, slots.begin() + erase_to + 1);
							}
						}
					}
				
				if (!lbb_is_valid && !lbe_is_valid)
					{
					//assert(fae_is_valid); //impossible otherwise, if all are invalid slots.size() == 0
					
					auto& fae{slots[fae_index]};
					const bool fae_shares_value{opt_eq(fae.value_opt, new_value)};
					if (fae.begin == fill_region.end() && fae_shares_value)
						{
						fae.begin = fill_region.begin;
						return;
						}

					inner_add(0, fill_region.begin, new_value);
					if (fae.begin != fill_region.end())
						{
						inner_add(1, fill_region.end(), std::nullopt);
						}
					}
				}

			constexpr utils::observer_ptr<const value_type> at(size_t index) const noexcept
				{
				if (empty())
					{
					return nullptr; 
					}

				if (index < slots[0].begin)
					{
					return nullptr;
					}

				for (size_t i{1}; i < slots.size(); i++)
					{
					const slot& current_slot{slots[i]};
					if (current_slot.begin > index)
						{
						const slot& ret_slot{slots[i - 1]};
						if (ret_slot.value_opt.has_value())
							{
							return &(*ret_slot.value_opt);
							}
						else
							{
							return nullptr;
							}
						}
					}
				return nullptr;
				}
			
			constexpr const read_slot<true> slot_at(size_t index) const noexcept
				{
				if (empty()) 
					{
					return read_slot<true>
						{
						.region{region::create::full_range()},
						.value_ptr{nullptr}
						};
					}

				if (index < slots[0].begin)
					{
					return read_slot<true>
						{
						.region{.begin{0}, .count{slots[0].begin}},
						.value_ptr{nullptr}
						};
					}

				for (size_t i{1}; i < slots.size(); i++)
					{
					const slot& current_slot{slots[i]};
					if (current_slot.begin > index)
						{
						const slot& ret_slot{slots[i - 1]};

						return read_slot<true>
							{
							.region{.begin{ret_slot.begin}, .count{current_slot.begin - ret_slot.begin}},
							.value_ptr{utils::optional_to_observer_ptr(ret_slot.value_opt)}
							};
						}
					}

				return read_slot<true>
					{
					.region{region::create::from(slots.rbegin()->begin)},
					.value_ptr{nullptr}
					};
				}

			constexpr region region_at(size_t index) const noexcept
				{
				if (empty()) 
					{
					return region::create::full_range();
					}

				if (index < slots[0].begin)
					{
					return {.begin{0}, .count{slots[0].begin}};
					}

				for (size_t i{1}; i < slots.size(); i++)
					{
					const slot& current_slot{slots[i]};
					if (current_slot.begin > index)
						{
						const slot& ret_slot{slots[i - 1]};

						return {.begin{ret_slot.begin}, .count{current_slot.begin - ret_slot.begin}};
						}
					}
				return region::create::from(slots.rbegin()->begin);
				}

			constexpr size_t size() const noexcept
				{
				if (slots.empty()) { return 0; }
				return slots[slots.size() - 1].begin;
				}
			
			constexpr read_slot<true> slot_at_index_of_slots(size_t index) const noexcept
				{
				assert(index < slots_count());

				auto& slot     {slots[index    ]};
				auto& slot_next{slots[index + 1]};

				return read_slot<true>
					{
					.region
						{
						.begin{slot.begin},
						.count{slot_next.begin - slot.begin}
						},
					.value_ptr{utils::optional_to_observer_ptr(slot.value_opt)}
					};
				}
			constexpr read_slot<false> slot_at_index_of_slots(size_t index) noexcept
				{
				assert(index < slots_count());

				auto& slot     {slots[index    ]};
				auto& slot_next{slots[index + 1]};

				return read_slot<false>
					{
					.region
						{
						.begin{slot.begin},
						.count{slot_next.begin - slot.begin}
						},
					.value_ptr{utils::optional_to_observer_ptr(slot.value_opt)}
					};
				}

			constexpr size_t slots_count() const noexcept
				{
				const size_t inner_size{slots.size()};
				return (inner_size == 0 ? 0 : (inner_size - 1));
				}

			/// <summary> They're sorted </summary>
			constexpr std::vector<size_t> split_indices() const noexcept
				{
				std::vector<size_t> ret;
				for (const auto& slot : slots)
					{
					ret.emplace_back(slot.begin);
					}
				return ret;
				}

			constexpr bool empty() const noexcept { return slots.empty(); }

			constexpr void clear() noexcept { slots.clear(); }

			//TODO the view should skip nullopt regions
			//inline auto get_slots() noexcept //inline auto cause views return types are... heh
			//	{
			//	return std::views::transform(slots, [](slot& slot)
			//		{
			//		return read_slot{slot., val.b};
			//		});
			//	}
			//const std::span<slot> get_slots() const noexcept
			//	{
			//	return std::span<slot>{regions.begin(), regions.end()};
			//	}
			
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

				utils_gpu_available constexpr self_t& operator++() noexcept { inner_iterator++; return *this; }
				utils_gpu_available constexpr self_t& operator--() noexcept { inner_iterator--; return *this; }
				utils_gpu_available constexpr self_t  operator++(int) noexcept { self_t tmp{*this}; ++(*this); return tmp; }
				utils_gpu_available constexpr self_t  operator--(int) noexcept { self_t tmp{*this}; --(*this); return tmp; }

				utils_gpu_available constexpr auto operator<=>(const self_t& other) const noexcept { return inner_iterator <=> other.inner_iterator; }
				utils_gpu_available constexpr bool operator== (const self_t& other) const noexcept { return inner_iterator ==  other.inner_iterator; }

				utils_gpu_available constexpr reference operator* ()       noexcept { return  *(inner_iterator->value_opt); }
				utils_gpu_available constexpr pointer   operator->()       noexcept { return &*(inner_iterator->value_opt); }
				utils_gpu_available constexpr reference operator* () const noexcept { return  *(inner_iterator->value_opt); }
				utils_gpu_available constexpr pointer   operator->() const noexcept { return &*(inner_iterator->value_opt); }
				utils_gpu_available constexpr region    region    () const noexcept 
					{
					const auto next_inner_iterator{inner_iterator + 1};
					return {inner_iterator->begin, next_inner_iterator->begin - inner_iterator->begin};
					}
				utils_gpu_available constexpr bool      has_value() const noexcept { return inner_iterator->value_opt.has_value(); }
				};

			using iterator               = base_iterator<typename std::vector<slot>::iterator              ,       value_type>;
			using reverse_iterator       = base_iterator<typename std::vector<slot>::reverse_iterator      ,       value_type>;
			using const_iterator         = base_iterator<typename std::vector<slot>::const_iterator        , const value_type>;
			using const_reverse_iterator = base_iterator<typename std::vector<slot>::const_reverse_iterator, const value_type>;
		
			utils_gpu_available constexpr       iterator           begin()       noexcept { return {slots. begin ()}; }
			utils_gpu_available constexpr const_iterator           begin() const noexcept { return {slots. begin ()}; }
			utils_gpu_available constexpr const_iterator          cbegin()       noexcept { return {slots.cbegin()}; }
			utils_gpu_available constexpr       iterator           end  ()       noexcept { return {slots. end   ()}; }
			utils_gpu_available constexpr const_iterator           end  () const noexcept { return {slots. end   ()}; }
			utils_gpu_available constexpr const_iterator          cend  ()       noexcept { return {slots.cend   ()}; }

			utils_gpu_available constexpr       reverse_iterator  rbegin()       noexcept { return {slots. rbegin()}; }
			utils_gpu_available constexpr const_reverse_iterator  rbegin() const noexcept { return {slots. rbegin()}; }
			utils_gpu_available constexpr const_reverse_iterator crbegin()       noexcept { return {slots.crbegin()}; }
			utils_gpu_available constexpr       reverse_iterator  rend  ()       noexcept { return {slots. rend  ()}; }
			utils_gpu_available constexpr const_reverse_iterator  rend  () const noexcept { return {slots. rend  ()}; }
			utils_gpu_available constexpr const_reverse_iterator crend  ()       noexcept { return {slots.crend  ()}; }

			iterator find(size_t index) noexcept
				{
				for (auto it{slots.begin()}; it != (slots.end() - 1); it++)
					{
					const slot& slot{*it};
					if (slot.begin >= index && slot.value_opt.has_value()) { return {it}; }
					}
				return {slots.end()};
				}
			const_iterator find(size_t index) const noexcept
				{
				for (auto it{slots.begin()}; it != (slots.end() - 1); it++)
					{
					const slot& slot{*it};
					if (slot.begin >= index && slot.value_opt.has_value()) { return {it}; }
					}
				return {slots.end()};
				}

		private:
			struct region_insert_reference_indices_t
				{
				size_t last_before_begin_index;
				size_t first_after_begin_index; //Not guaranteed to be valid (unnecessary information), only valid if last_before_end_index is valid
				size_t last_before_end_index;
				size_t first_after_end_index;
				};
			constexpr region_insert_reference_indices_t get_region_insert_reference_indices(region region) const noexcept
				{
				size_t slot_index{0};

				region_insert_reference_indices_t ret
					{
					.last_before_begin_index{slots.size()},
					.last_before_end_index  {slots.size()},
					.first_after_end_index  {slots.size()},
					};

				//must check if there is at least one
				if (slots[slot_index].begin < region.begin)
					{
					while (slot_index < slots.size() && slots[slot_index].begin < region.begin)
						{
						slot_index++;
						}
					ret.last_before_begin_index = slot_index - 1;
					}
				ret.first_after_begin_index = slot_index;

				//must check if there is at least one
				if (slot_index < slots.size() && slots[slot_index].begin < region.end())
					{
					while (slot_index < slots.size() && slots[slot_index].begin < region.end())
						{
						slot_index++;
						}
					ret.last_before_end_index = slot_index - 1;
					}
				ret.first_after_end_index = slot_index;

				return ret;
				}

			constexpr void inner_add(size_t index, size_t begin, const T& new_value) noexcept
				{
				slots.emplace(slots.begin() + index, begin, new_value);
				}
			constexpr void inner_add(size_t index, size_t begin, const std::optional<T>& new_value_opt) noexcept
				{
				slots.emplace(slots.begin() + index, begin, new_value_opt);
				}
		};



	}

#include "../details/warnings_post.inline.h"
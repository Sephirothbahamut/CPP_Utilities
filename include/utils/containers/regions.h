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

			//auto slot_at(this auto&& self, size_t index) noexcept -> read_slot<std::is_const_v<decltype(self)>>
			//	{
			//	using ret_t = read_slot<std::is_const_v<decltype(self)>>;
			//
			//	if (empty())
			//		{
			//		return ret_t
			//			{
			//			.region{.begin{0}, .count{std::numeric_limits<size_t>::max()}},
			//			.value_opt_ref{std::nullopt}
			//			};
			//		}
			//
			//	if (index < slots[0].begin)
			//		{
			//		return ret_t
			//			{
			//			.region{.begin{0}, .count{slots[0].begin}},
			//			.value_opt_ref{std::nullopt}
			//			};
			//		}
			//
			//	size_t slot_index{0};
			//	while (slot_index < slots.size())
			//		{
			//		const regions::slot& slot{slots[slot_index]};
			//		const size_t next_slot_begin{(slot_index < slots.size() - 1) ? slots[slot_index + 1].begin : std::numeric_limits<size_t>::max()};
			//		if (slot.begin <= index && next_slot_begin > index) { break; }
			//		slot_index++;
			//		}
			//
			//	const size_t next_slot_begin{(slot_index < slots.size() - 1) ? slots[slot_index + 1].begin : std::numeric_limits<size_t>::max()};
			//	auto& slot{slots[slot_index]};
			//	const size_t region_begin{slot.begin};
			//
			//	std::optional<std::reference_wrapper<typename ret_t::const_aware_value_type>> value_opt_ref{std::nullopt};
			//	if (slot.value_opt.has_value()) 
			//		{
			//		std::reference_wrapper<typename ret_t::const_aware_value_type> value_ref{slot.value_opt.value()};
			//		value_opt_ref.emplace(value_ref);
			//		}
			//
			//	return read_slot<is_const>
			//		{
			//		.region
			//			{
			//			.begin{region_begin},
			//			.count{next_slot_begin - region_begin}
			//			},
			//		.value_opt_ref{value_opt_ref}
			//		};
			//	}

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



	//
	//namespace details
	//	{
	//	template <typename T, typename regions_of_aggregate_of_optionals_t, typename aggregate_of_optionals_t, auto ...component_getters>
	//	regions_of_aggregate_of_optionals_t generate_regions_of_aggregate_of_optionals(const T& struct_of_regions) noexcept
	//		{
	//		regions_of_aggregate_of_optionals_t ret;
	//
	//		const size_t end{std::max(component_getters(struct_of_regions).values_size(), ...)};
	//
	//		size_t element_index{0};
	//		while (element_index != end)
	//			{
	//			const size_t region_begin{element_index};
	//			const size_t region_end{std::min(component_getters(struct_of_regions).region.end(), ...)};
	//
	//			const bool fully_nullopt
	//				{
	//				component_getters(struct_of_regions).value_at(element_index).has_value() && ...
	//				};
	//			
	//			if(!fully_nullopt)
	//				{
	//				const aggregate_of_optionals_t new_aggregate{[&]()
	//					{
	//					aggregate_of_optionals_t ret;
	//					((component_getters(ret) = component_getters(struct_of_regions).value_at(element_index).value_opt_ref), ...);
	//					}()};
	//	
	//				ret.add(new_aggregate, {region_begin, region_end - region_begin});
	//				}
	//
	//			element_index = region_end;
	//			}
	//
	//		return ret;
	//		}
	//
	//	template <typename aggregate_t, typename aggregate_of_optionals_t, auto ...component_getters>
	//		requires(concepts::regions_of_aggregate_of_optionals_functor<component_getters, aggregate_t, aggregate_of_optionals_t> && ...)
	//	struct regions_of_aggregate_of_optionals : regions<aggregate_of_optionals_t>
	//		{
	//		aggregate_t value_at(const size_t& index, const aggregate_t& defaults) const noexcept
	//			{
	//			const auto aggregate_of_optionals{regions.value_at(index)};
	//			aggregate_t ret;
	//			(component_getters(ret) = component_getters(aggregate_of_optionals).value_or(component_getters(defaults)), ...);
	//			return ret;
	//			}
	//		};
	//
	//	template <typename aggregate_t, typename aggregate_of_optionals_t, auto ...component_getters>
	//		requires(concepts::regions_of_aggregate_of_optionals_functor<component_getters, aggregate_t, aggregate_of_optionals_t> && ...)
	//	struct regions_of_aggregate_of_optionals_with_defaults : regions_of_aggregate_of_optionals<aggregate_t, aggregate_of_optionals_t, component_getters...>
	//		{
	//		aggregate_t defaults;
	//
	//		aggregate_t value_at(const size_t& index) const noexcept
	//			{
	//			return value_at(index, defaults);
	//			}
	//		};
	//	}
	//
	///// <summary> 
	///// There must be one components_getter for each member of the aggregate.
	///// It is a functor with:
	///// A value_type alias for its value type
	///// One function takes aggregate_of_optionals_t as reference and returns a reference to its optional.
	///// One function takes regions_per_each_field_t as reference and returns a reference to its region.
	///// aggregate_of_optionals_t, regions_per_each_field_t should share the same fields, with optional<T> and region<T> as types respectively.
	///// </summary>
	//template <typename aggregate_of_optionals_t, typename regions_per_each_field_t, auto ...component_getters>
	//	requires(concepts::aggregate_regions_functor<component_getters, aggregate_of_optionals_t, regions_per_each_field_t> && ...)
	//struct aggregate_regions_optional_only
	//	{
	//	regions_per_each_field_t regions_per_each_field;
	//
	//	auto generate_regions_of_aggregate_of_optionals() const noexcept
	//		{
	//		return details::generate_regions_of_aggregate_of_optionals<decltype(*this), regions<aggregate_of_optionals_t>, aggregate_of_optionals_t, component_getters...>(*this);
	//		}
	//	};
	//
	///// <summary> 
	///// There must be one components_getter for each member of the aggregate.
	///// It is a functor with:
	///// A value_type alias for its value type
	///// One function takes aggregate_t              as reference and returns a reference to its field.
	///// One function takes aggregate_of_optionals_t as reference and returns a reference to its optional.
	///// One function takes regions_per_each_field_t as reference and returns a reference to its region.
	///// aggregate_t, aggregate_of_optionals_t, regions_per_each_field_t should share the same fields, with T, optional<T> and region<T> as types respectively.
	///// </summary>
	//template <typename aggregate_t, typename aggregate_of_optionals_t, typename regions_per_each_field_t, auto ...component_getters>
	//	requires(concepts::aggregate_regions_with_defaults_functor<component_getters, aggregate_t, aggregate_of_optionals_t, regions_per_each_field_t> && ...)
	//struct aggregate_regions : aggregate_regions<aggregate_t, aggregate_of_optionals_t, regions_per_each_field_t, component_getters...>
	//	{
	//	auto generate_regions_of_aggregate_of_optionals() const noexcept
	//		{
	//		return details::generate_regions_of_aggregate_of_optionals<decltype(*this), details::regions_of_aggregate_of_optionals<aggregate_t, aggregate_of_optionals_t>, aggregate_of_optionals_t, component_getters...>(*this);
	//		}
	//	};
	//
	///// <summary> 
	///// There must be one components_getter for each member of the aggregate.
	///// It is a functor with:
	///// A value_type alias for its value type
	///// One function takes aggregate_t              as reference and returns a reference to its field.
	///// One function takes aggregate_of_optionals_t as reference and returns a reference to its optional.
	///// One function takes regions_per_each_field_t as reference and returns a reference to its region.
	///// aggregate_t, aggregate_of_optionals_t, regions_per_each_field_t should share the same fields, with T, optional<T> and region<T> as types respectively.
	///// </summary>
	//template <typename aggregate_t, typename aggregate_of_optionals_t, typename regions_per_each_field_t, auto ...component_getters>
	//	requires(concepts::aggregate_regions_with_defaults_functor<component_getters, aggregate_t, aggregate_of_optionals_t, regions_per_each_field_t> && ...)
	//struct aggregate_regions_with_defaults : aggregate_regions<aggregate_t, aggregate_of_optionals_t, regions_per_each_field_t, component_getters...>
	//	{
	//	aggregate_t defaults;
	//
	//	auto generate_regions_of_aggregate_of_optionals() const noexcept
	//		{
	//		auto ret{details::generate_regions_of_aggregate_of_optionals<decltype(*this), details::regions_of_aggregate_of_optionals_with_defaults<aggregate_t, aggregate_of_optionals_t>, aggregate_of_optionals_t, component_getters...>(*this)};
	//		ret.defaults = defaults;
	//		return;
	//		}
	//	};
	}

#include "../details/warnings_post.inline.h"
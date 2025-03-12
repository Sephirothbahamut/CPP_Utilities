#pragma once

#include <vector>
#include <ranges>
#include <limits>
#include <cassert>
#include <algorithm>

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
			};
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

			struct slot            { size_t begin{0}; std::optional<T> value_opt{std::nullopt}; };
			struct read_slot       { using value_type = T; const region region; std::optional<std::reference_wrapper<T      >> value_opt_ref; };
			struct read_const_slot { using value_type = T; const region region; std::optional<std::reference_wrapper<const T>> value_opt_ref; };
			std::vector<slot> slots;

			void add(const T& new_value, region fill_region) noexcept
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

			read_slot value_at(size_t index) noexcept
				{
				if (empty())
					{
					return read_slot
						{
						.region{.begin{0}, .count{std::numeric_limits<size_t>::max()}},
						.value_opt_ref{std::nullopt}
						};
					}

				if (index < slots[0].begin)
					{
					return read_slot
						{
						.region{.begin{0}, .count{slots[0].begin}},
						.value_opt_ref{std::nullopt}
						};
					}

				size_t slot_index{0};
				while (slot_index < slots.size())
					{
					const regions::slot& slot{slots[slot_index]};
					const size_t next_slot_begin{(slot_index < slots.size() - 1) ? slots[slot_index + 1].begin : std::numeric_limits<size_t>::max()};
					if (slot.begin <= index && next_slot_begin > index) { break; }
					slot_index++;
					}

				const size_t next_slot_begin{(slot_index < slots.size() - 1) ? slots[slot_index + 1].begin : std::numeric_limits<size_t>::max()};
				auto& slot{slots[slot_index]};
				const size_t region_begin{slot.begin};

				std::optional<std::reference_wrapper<T>> value_opt_ref{std::nullopt};
				if (slot.value_opt.has_value()) 
					{
					std::reference_wrapper<T> value_ref{slot.value_opt.value()};
					value_opt_ref.emplace(value_ref);
					}

				return read_slot
					{
					.region
						{
						.begin{region_begin},
						.count{next_slot_begin - region_begin}
						},
					.value_opt_ref{value_opt_ref}
					};
				}

			read_const_slot value_at(size_t index) const noexcept
				{
				if (empty())
					{
					return read_const_slot
						{
						.region{.begin{0}, .count{std::numeric_limits<size_t>::max()}},
						.value_opt_ref{std::nullopt}
						};
					}

				if (index < slots[0].begin)
					{
					return read_const_slot
						{
						.region{.begin{0}, .count{slots[0].begin}},
						.value_opt_ref{std::nullopt}
						};
					}

				size_t slot_index{0};
				while (slot_index < slots.size())
					{
					const regions::slot& slot{slots[slot_index]};
					const size_t next_slot_begin{(slot_index < slots.size() - 1) ? slots[slot_index + 1].begin : std::numeric_limits<size_t>::max()};
					if (slot.begin <= index && next_slot_begin > index) { break; }
					slot_index++;
					}

				const size_t next_slot_begin{(slot_index < slots.size() - 1) ? slots[slot_index + 1].begin : std::numeric_limits<size_t>::max()};
				auto& slot{slots[slot_index]};
				const size_t region_begin{slot.begin};

				std::optional<std::reference_wrapper<const T>> value_opt_ref{std::nullopt};
				if (slot.value_opt.has_value()) 
					{
					std::reference_wrapper<const T> value_ref{slot.value_opt.value()};
					value_opt_ref.emplace(value_ref);
					}

				return read_const_slot
					{
					.region
						{
						.begin{region_begin},
						.count{next_slot_begin - region_begin}
						},
					.value_opt_ref{value_opt_ref}
					};
				}

			size_t values_size() const noexcept
				{
				if (slots.empty()) { return 0; }
				return slots[slots.size() - 1].begin;
				}

			read_slot slot_at(size_t index)
				{
				assert(index < slots_size());

				auto& slot{slots[index]};
				auto& slot_next{slots[index + 1]};

				return read_slot
					{
					.region
						{
						.begin{slot.begin},
						.count{slot_next.begin - slot.begin}
						},
					.value_opt_ref{slot.value_opt.has_value() ? std::optional<std::reference_wrapper<T>>{slot.value_opt.value()} : std::optional<std::reference_wrapper<T>>{std::nullopt}}
					};
				}

			read_const_slot slot_at(size_t index) const
				{
				assert(index < slots_size());

				const auto& slot{slots[index]};
				const auto& slot_next{slots[index + 1]};

				return read_const_slot
					{
					.region
						{
						.begin{slot.begin},
						.count{slot_next.begin - slot.begin}
						},
					.value_opt_ref{slot.value_opt}
					};
				}

			size_t slots_size() const noexcept
				{
				const size_t inner_size{slots.size()};
				return (inner_size == 0 ? 0 : (inner_size - 1));
				}

			bool empty() const noexcept { return slots.empty(); }

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
			region_insert_reference_indices_t get_region_insert_reference_indices(region region) const noexcept
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

			void inner_add(size_t index, size_t begin, const T& new_value) noexcept
				{
				slots.emplace(slots.begin() + index, begin, new_value);
				}
			void inner_add(size_t index, size_t begin, const std::optional<T>& new_value_opt) noexcept
				{
				slots.emplace(slots.begin() + index, begin, new_value_opt);
				}
		};
	}

#include "../details/warnings_post.inline.h"
#pragma once

#include <tuple>
#include <vector>
#include <ranges>
#include <limits>
#include <cassert>
#include <algorithm>

#include "../compilation/debug.h"
#include "../memory.h"
#include "../optional.h"
#include "../compilation/gpu.h"
#include "../oop/disable_move_copy.h"
#include "../math/math.h"

#include "../details/warnings_pre.inline.h"


namespace utils::containers
	{
	struct region
		{
		size_t begin{0};
		size_t count{1};
		utils_gpu_available inline constexpr size_t end() const noexcept 
			{
			assert(!utils::math::will_overflow_sum(begin, count));
			return begin + count; 
			}

		struct create : utils::oop::non_constructible
			{
			inline static constexpr region full_range() noexcept { return region{0, std::numeric_limits<size_t>::max()}; }
			inline static constexpr region from_to(size_t start, size_t end) noexcept
				{
				assert(end > start);
				return region{start, end - start};
				}
			inline static constexpr region from(size_t start) noexcept { return from_to(start, std::numeric_limits<size_t>::max()); }
			};

		bool operator==(const region& other) const noexcept = default;
		};

	/// <summary> 
	/// Container that represent a distribution of values along a 1d range of values with pseudo-indices from 0 to std::numeric_limits<size_t>::max().
	/// Adding the same value in subsequent regions merges them in one region (add value X to 1-3, then add X to 4-8, will make a single region with value X in positions 1-8).
	/// Adding the a different value in a sequential region will split that region (add X to 1-9, then add Y to 5-6, will split everything into: X 1-4, Y 5-6, X 7-9).
	///</summary>
	template <typename T>
	class regions
		{
		public:
			using value_type = T;

			template <bool IS_CONST>
			struct read_slot
				{
				inline static constexpr const bool is_const{IS_CONST};

				using value_type = regions::value_type;
				const region region;
				std::conditional_t<is_const, const value_type&, value_type&> value;
				};

		private:
			struct inner_slot_t
				{
				value_type value;
				size_t end{std::numeric_limits<size_t>::max()};
				};
			using inner_slots_t = std::vector<inner_slot_t>;

			inner_slots_t inner_slots;

			size_t element_index_to_slot_index(const size_t& index, const size_t& inner_slots_starting_index = 0) const noexcept
				{
				for (size_t index_in_slots{inner_slots_starting_index}; index_in_slots < inner_slots.size(); index_in_slots++)
					{
					const inner_slot_t& inner_slot{inner_slots[index_in_slots]};
					if (inner_slot.end > index) 
						{
						return index_in_slots;
						}
					}
				return inner_slots.size();
				}
			const inner_slot_t& inner_slot_for_value_at(const size_t& index) const noexcept { return inner_slots[element_index_to_slot_index(index)]; }
			/***/ inner_slot_t& inner_slot_for_value_at(const size_t& index) /***/ noexcept { return inner_slots[element_index_to_slot_index(index)]; }

		public:
			regions(const value_type& starting_value) noexcept :
				inner_slots{inner_slot_t
					{
					.value{starting_value},
					.end{std::numeric_limits<size_t>::max()}
					}}
				{
				}

			regions() noexcept 
				requires(std::is_default_constructible_v<value_type>)
				:
				inner_slots{inner_slot_t
					{
					.value{},
					.end{std::numeric_limits<size_t>::max()}
					}}
				{
				}

			void reset(const value_type& starting_value) noexcept
				{
				inner_slots.clear();
				inner_slots.emplace_back(inner_slot_t
					{
					.value{starting_value},
					.end{std::numeric_limits<size_t>::max()}
					});
				}
			void reset() noexcept
				{
				inner_slots.clear();
				inner_slots.emplace_back(inner_slot_t
					{
					.value{},
					.end{std::numeric_limits<size_t>::max()}
					});
				}





////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Iterators begin //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			template <bool IS_CONST>
			class forward_iterator
				{
				private:
					inline static constexpr const bool is_const{IS_CONST};
					friend class regions;

					using difference_type = std::ptrdiff_t;
					using value_type = regions::value_type;
					using inner_iterator_t = std::conditional_t<is_const, typename inner_slots_t::const_iterator, typename inner_slots_t::iterator>;

					size_t begin{0};
					inner_iterator_t inner_it;

					forward_iterator(size_t begin, inner_iterator_t inner_it) :
						begin   {begin   },
						inner_it{inner_it}
						{
						}

				public:
					forward_iterator<is_const>& operator++() noexcept
						{
						begin = inner_it->end;
						++inner_it;
						return *this;
						}
					forward_iterator<is_const> operator++(int) noexcept
						{
						auto tmp{*this};
						++(*this);
						return tmp;
						}

					bool operator==(const forward_iterator<is_const>& other) const noexcept
						{
						return inner_it == other.inner_it;
						}

					const read_slot<true> operator*() const noexcept
						{
						const read_slot<is_const> ret
							{
							.region{region()},
							.value {value()}
							};
						return ret;
						}
					read_slot<false> operator*() noexcept
						requires(!is_const)
						{
						const read_slot<is_const> ret
							{
							.region{region()},
							.value {value()}
							};
						return ret;
						}

					const value_type& value() const noexcept                     { return inner_it->value; }
					      value_type& value()       noexcept requires(!is_const) { return inner_it->value; }

					const region region() const noexcept
						{
						const auto ret{utils::containers::region::create::from_to(begin, inner_it->end)};
						return ret;
						}

					bool is_end() const noexcept 
						{
						const bool ret{begin == std::numeric_limits<size_t>::max()};
						return ret;
						}
				};

			forward_iterator<true> begin() const noexcept
				{
				const forward_iterator<true> ret
					{
					static_cast<size_t>(0),
					inner_slots.begin()
					};
				return ret;
				}
			forward_iterator<true> end() const noexcept
				{
				const forward_iterator<true> ret
					{
					std::numeric_limits<size_t>::max(),
					inner_slots.end()
					};
				return ret;
				}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Iterators end ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Views begin //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			class slot_index_view_t
				{
				public:
					slot_index_view_t(const regions<value_type>& regions) : _regions{regions} {}

					class at_t
						{
						friend class slot_index_view_t;
						public:
							const value_type& value() const noexcept
								{
								return _regions.inner_slots[_index].value;
								}
							size_t region_begin() const noexcept
								{
								if (_index == 0) { return 0; }
								return _regions.inner_slots[_index - 1].end;
								}
							size_t region_end() const noexcept
								{
								return _regions.inner_slots[_index].end;
								}
							containers::region region() const noexcept
								{
								const inner_slot_t& inner_slot{_regions.inner_slots[_index]};
								if (_index == 0)
									{
									const containers::region ret
										{
										.begin{0},
										.count{inner_slot.end}
										};
									return ret;
									}
								else
									{
									const inner_slot_t& previous_inner_slot{_regions.inner_slots[_index - 1]};
									const containers::region ret{region::create::from_to(previous_inner_slot.end, inner_slot.end)};
									return ret;
									}
								}

							read_slot<true> slot() const noexcept
								{
								const read_slot<true> ret
									{
									.region{region()},
									.value {value ()}
									};
								return ret;
								}

						private:
							at_t(size_t index, const regions<value_type>& regions) : _index{index}, _regions{regions} {}
							size_t _index;
							const regions<value_type>& _regions;
						};

					at_t at(const size_t& index) const 
						{
						if (index >= _regions.inner_slots.size())
							{
							throw std::out_of_range{"Regions access out of bounds."};
							}
						return {index, _regions}; 
						}
					at_t operator[](const size_t& index) const noexcept
						{
						#ifdef utils_is_debug
						return at(index);
						#endif
						return {index, _regions};
						}

					size_t size() const noexcept { return _regions.inner_slots.size(); }
					/// <summary> Will never be empty, there's at least one slot.</summary>
					bool empty() const noexcept { return false; }

					forward_iterator<true> begin() const noexcept { return _regions.begin(); }
					forward_iterator<true> end  () const noexcept { return _regions.end  (); }

				private:
					const regions<value_type>& _regions;
				};
			friend class slot_index_view;
			slot_index_view_t slot_index_view() const noexcept { return {*this}; }
			slot_index_view_t::at_t at_slot_index(const size_t& index) const noexcept
				{
				return slot_index_view().at(index);
				}

			class elements_index_view_t
				{
				public:
					elements_index_view_t(const regions<value_type>& regions) : _regions{regions} {}

					class at_t
						{
						friend class elements_index_view_t;
						public:
							size_t slot_index() const noexcept
								{
								const size_t ret{_regions.element_index_to_slot_index(_index)};
								return ret;
								}

							const value_type& value() const noexcept
								{
								return _regions.slot_index_view().at(slot_index()).value();
								}
							size_t region_begin() const noexcept
								{
								return _regions.slot_index_view().at(slot_index()).region_begin();
								}
							size_t region_end() const noexcept
								{
								return _regions.slot_index_view().at(slot_index()).region_end();
								}
							containers::region region() const noexcept
								{
								return _regions.slot_index_view().at(slot_index()).region();
								}

							read_slot<true> slot() const noexcept
								{
								return _regions.slot_index_view().at(slot_index()).slot();
								}

						private:
							at_t(size_t index, const regions<value_type>& regions) : _index{index}, _regions{regions} {}
							size_t _index;
							const regions<value_type>& _regions;
						};

					at_t at(const size_t& index) const
						{
						if (index == std::numeric_limits<size_t>::max())
							{
							throw std::out_of_range{"Regions access out of bounds."};
							}
						return {index, _regions};
						}
					at_t operator[](const size_t& index) const noexcept
						{
						#ifdef utils_is_debug
						return at(index);
						#endif
						return {index, _regions};
						}

					/// <summary> Will always be std::numeric_limits<size_t>::max().</summary>
					size_t size() const noexcept { return std::numeric_limits<size_t>::max(); }
					/// <summary> Will always be full.</summary>
					bool empty() const noexcept { return false; }

				private:
					const regions<value_type>& _regions;
				};
			friend class elements_index_view_t;
			elements_index_view_t elements_index_view() const noexcept { return {*this}; }
			elements_index_view_t::at_t at_element_index(const size_t& index) const noexcept 
				{
				return elements_index_view().at(index);
				}


			/// <summary> 
			/// Unsafe APIs allow you to access slot's value (via both accessors and iterators) as non-constant.
			/// Modifying those values without making sure that no two consecutive slots hold the same values is UB. 
			/// Unsupported. I don't deal with it. Sorry not sorry. If you do it, it's on you.
			/// </summary>
			class unsafe_slot_view_t
				{
				public:
					unsafe_slot_view_t(regions<value_type>& regions) : _regions{regions} {}

					class at_t
						{
						friend class slot_index_view_t;
						public:
							value_type& value() noexcept
								{
								return _regions.inner_slots[_index].value;
								}
							size_t region_begin()  noexcept
								{
								if (_index == 0) { return 0; }
								return _regions.inner_slots[_index - 1].end;
								}
							size_t region_end() const noexcept
								{
								return _regions.inner_slots[_index].end;
								}
							containers::region region() const noexcept
								{
								const inner_slot_t& inner_slot{_regions.inner_slots[_index]};
								if (_index == 0)
									{
									const containers::region ret
										{
										.begin{0},
										.count{inner_slot.end}
										};
									return ret;
									}
								else
									{
									const inner_slot_t& previous_inner_slot{_regions.inner_slots[_index - 1]};
									const containers::region ret{region::create::from_to(previous_inner_slot.end, inner_slot.end)};
									return ret;
									}
								}

							read_slot<false> slot() noexcept
								{
								const read_slot<false> ret
									{
									.region{region()},
									.value {value ()}
									};
								return ret;
								}

						private:
							at_t(size_t index, regions<value_type>& regions) : _index{index}, _regions{regions} {}
							size_t _index;
							regions<value_type>& _regions;
						};

					at_t at(const size_t& index) const noexcept 
						{
						if (index >= _regions.inner_slots.size())
							{
							throw std::out_of_range{"Regions access out of bounds."};
							}
						return {index, _regions}; 
						}
					at_t operator[](const size_t& index) const noexcept
						{
						#ifdef utils_is_debug
						return at(index);
						#endif
						return {index, _regions};
						}

					size_t size() const noexcept { return _regions.inner_slots.size(); }
					/// <summary> Will never be empty, there's at least one slot.</summary>
					bool empty() const noexcept { return false; }

					forward_iterator<false> begin() noexcept 
						{
						const forward_iterator<false> ret
							{
							static_cast<size_t>(0),
							_regions.inner_slots.begin()
							};
						return ret;
						}
					forward_iterator<false> end() noexcept 
						{
						const forward_iterator<false> ret
							{
							std::numeric_limits<size_t>::max(),
							_regions.inner_slots.end()
							};
						return ret;
						}

				private:
					regions<value_type>& _regions;
				};
			friend class unsafe_slot_view_t;
			/// <summary> 
			/// Unsafe APIs allow you to access slot's value (via both accessors and iterators) as non-constant.
			/// Modifying those values without making sure that no two consecutive slots hold the same values is UB. 
			/// Unsupported. I don't deal with it. Sorry not sorry. If you do it, it's on you.
			/// </summary>
			unsafe_slot_view_t unsafe_slots_index_view() noexcept { return {*this}; }
			/// <summary> 
			/// Unsafe APIs allow you to access slot's value (via both accessors and iterators) as non-constant.
			/// Modifying those values without making sure that no two consecutive slots hold the same values is UB. 
			/// Unsupported. I don't deal with it. Sorry not sorry. If you do it, it's on you.
			/// </summary>
			unsafe_slot_view_t::at_t unsafe_at_slot_index(const size_t& index) noexcept
				{
				return unsafe_slots_index_view().at(index);
				}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// Views end ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			std::vector<size_t> split_indices() const noexcept
				{
				std::vector<size_t> ret; 
				ret.reserve(inner_slots.size() - 1);

				typename inner_slots_t::const_iterator it{inner_slots.cbegin()};
				while (it->end != std::numeric_limits<size_t>::max())
					{
					ret.emplace_back(it->end);
					it++;
					}
				return ret;
				}
			std::vector<value_type> values() const noexcept
				{
				std::vector<value_type> ret; ret.reserve(inner_slots.size());
				for (const inner_slot_t& inner_slot : inner_slots)
					{
					ret.emplace(inner_slot.value);
					}
				}

			void add(const value_type& new_value, const region& in_region)
				{
				assert(in_region.begin != std::numeric_limits<size_t>::max());


				const std::optional<size_t> index_of_slot_starting_before_begin_opt{in_region.begin > 0 ? std::optional<size_t>{element_index_to_slot_index(in_region.begin - 1)} : std::optional<size_t>{std::nullopt}};

				if (index_of_slot_starting_before_begin_opt)
					{
					const size_t& index_of_slot_starting_before_begin{*index_of_slot_starting_before_begin_opt};
					inner_slot_t& slot_starting_before_begin{inner_slots[index_of_slot_starting_before_begin]};

					if(slot_starting_before_begin.end > in_region.begin)
						{
						const size_t begin_of_slot_starting_before_begin{(index_of_slot_starting_before_begin > 0) ? inner_slots[index_of_slot_starting_before_begin - 1].end : static_cast<size_t>(0)};

						if (slot_starting_before_begin.end > in_region.end())
							{
							//Existing slot wraps around new slot.
							if (slot_starting_before_begin.value != new_value)
								{
								const inner_slot_t first_half_of_existing
									{
									.value{slot_starting_before_begin.value},
									.end{in_region.begin}
									};
								const inner_slot_t new_slot
									{
									.value{new_value},
									.end{in_region.end()}
									};
								inner_slots.insert(inner_slots.begin() + index_of_slot_starting_before_begin, new_slot);
								inner_slots.insert(inner_slots.begin() + index_of_slot_starting_before_begin, first_half_of_existing);
								}
							return;
							}
						else if (slot_starting_before_begin.end == in_region.end())
							{
							if (slot_starting_before_begin.value != new_value)
								{
								slot_starting_before_begin.end = in_region.begin;
								const inner_slot_t new_slot
									{
									.value{new_value},
									.end{in_region.end()}
									};
								inner_slots.insert(inner_slots.begin() + index_of_slot_starting_before_begin + 1, new_slot);
								}
							return;
							}
						else 
							{
							slot_starting_before_begin.end = in_region.begin;
							//NOT return
							}
						}
					}

				//From now on it's guaranteed that this region starts exactly at in_region.begin


				const size_t index_of_slot_starting_at_begin{element_index_to_slot_index(in_region.begin, (index_of_slot_starting_before_begin_opt ? (*index_of_slot_starting_before_begin_opt) : size_t{0}))};
				const size_t index_of_slot_starting_before_or_at_end{element_index_to_slot_index(in_region.end(), index_of_slot_starting_at_begin)}; //Guaranteed to have at least 1 element at end or be vector's end() index, will never need to be removed, also unsafe to dereference
				inner_slot_t& slot_starting_at_begin{inner_slots[index_of_slot_starting_at_begin]};

				if (index_of_slot_starting_at_begin == index_of_slot_starting_before_or_at_end)
					{
					if (slot_starting_at_begin.value == new_value) 
						{
						return; 
						}
					else
						{
						if (index_of_slot_starting_before_begin_opt)
							{
							const size_t& index_of_slot_starting_before_begin{*index_of_slot_starting_before_begin_opt};
							inner_slot_t& slot_starting_before_begin{inner_slots[index_of_slot_starting_before_begin]};
							if (slot_starting_before_begin.value == new_value)
								{
								slot_starting_before_begin.end = in_region.end();
								return;
								}
							}

						const inner_slot_t new_slot
							{
							.value{new_value},
							.end{in_region.end()}
							};
						inner_slots.insert(inner_slots.begin() + index_of_slot_starting_at_begin, new_slot);
						return;
						}
					}
				else
					{
					if (index_of_slot_starting_before_or_at_end != inner_slots.size())
						{//If a slot after exists
						const inner_slot_t slot_starting_before_or_at_end{inner_slots[index_of_slot_starting_before_or_at_end]};
						if (slot_starting_before_or_at_end.value == new_value)
							{//And it also has the same value, we must erase slots that come before

							//If the previous slot also had the same value, we erase that one too since we'll take the value and "end" of the slot from the next slot.
							const size_t erase_from{[&]()
								{
								if (index_of_slot_starting_before_begin_opt)
									{
									const size_t& index_of_slot_starting_before_begin{*index_of_slot_starting_before_begin_opt};
									const inner_slot_t& slot_starting_before_begin{inner_slots[index_of_slot_starting_before_begin]};
									if (slot_starting_before_begin.value == new_value) { return index_of_slot_starting_before_begin; }
									}
								return index_of_slot_starting_at_begin;
								}()};

							inner_slots.erase(inner_slots.begin() + erase_from, inner_slots.begin() + index_of_slot_starting_before_or_at_end);
							return;
							}
						}

					// From now on it's guaranteed that the next slot doesn't exist or it has a different value

					if (index_of_slot_starting_before_begin_opt)
						{
						const size_t& index_of_slot_starting_before_begin{*index_of_slot_starting_before_begin_opt};
						inner_slot_t& slot_starting_before_begin{inner_slots[index_of_slot_starting_before_begin]};
						if (slot_starting_before_begin.value == new_value) 
							{
							slot_starting_before_begin.end = in_region.end();
							inner_slots.erase(inner_slots.begin() + index_of_slot_starting_at_begin, inner_slots.begin() + index_of_slot_starting_before_or_at_end);
							return;
							}
						}

					slot_starting_at_begin.value = new_value;
					slot_starting_at_begin.end = in_region.end();
					inner_slots.erase(inner_slots.begin() + index_of_slot_starting_at_begin + 1, inner_slots.begin() + index_of_slot_starting_before_or_at_end);
					}
				}

			size_t count_slots_if(auto callback) const noexcept
				{
				size_t count{0};
				for (const auto& slot : slot_index_view())
					{
					if (callback(slot.value)) 
						{
						count++;
						}
					}
				return count;
				}
			size_t count_values_if(auto callback) const noexcept
				{
				size_t count{0};
				for (const auto& slot : slot_index_view())
					{
					if (callback(slot.value)) 
						{
						count += slot.region.count; 
						}
					}
				return count;
				}
		};
	}

#include "../details/warnings_post.inline.h"
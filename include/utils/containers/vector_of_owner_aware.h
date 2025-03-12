#pragma once

#include <ranges>
#include <memory>
#include <vector>
#include <utility>
#include <stdexcept>

#include "../memory.h"
#include "../compilation/debug.h"

namespace utils::owner_aware
	{
	namespace details
		{
		struct owner_aware_flag {};
		struct owner_flag {};
		}

	namespace concepts
		{
		template <typename T> concept owner_aware = std::derived_from<T, details::owner_aware_flag>;
		template <typename T> concept owner       = std::derived_from<T, details::owner_flag      >;
		}

	namespace owner
		{
		template <concepts::owner_aware T>
		class unique;

		namespace details
			{
			struct owned_metadata {};

			template <utils::owner_aware::concepts::owner_aware T>
			struct base : public owner_aware::details::owner_flag
				{
				protected:
					unique<T> replace(T& old_element, T& new_element) utils_if_debug(noexcept)
						{
						if constexpr (utils::compilation::debug)
							{
							if (old_element.owner_ptr != this) { throw std::runtime_error{"Trying to remove owner_aware object from wrong owner."}; }
							}
						return extract_impl(old_element, new_element);
						}
					virtual unique<T> replace(T& old_element, T& new_element) = 0;

					unique<T> extract(T& owned) utils_if_debug(noexcept)
						{
						if constexpr (utils::compilation::debug)
							{
							if (owned.owner_ptr != this) { throw std::runtime_error{"Trying to remove owner_aware object from wrong owner."}; }
							}
						return extract(owned.owned_metadata);
						}
					virtual unique<T> extract_impl(owned_metadata& owned_metadata) = 0;
				};
			}
		}

	template <typename/*concepts::owner_aware*/ owners_pointee_type>
	class base : public details::owner_aware_flag
		{
		template <concepts::owner_aware T>
		friend struct utils::owner_aware::owner::details::base;
		public:
			/// <summary> Move operations update the source instance's owner (if any) to own the target instance instead. </summary>
			base(base&& move) noexcept
				{
				if (move.knows_owner())
					{
					owner_ptr->replace(move, *this);
					}
				}

			/// <summary> Move operations update the source instance's owner (if any) to own the target instance instead. </summary>
			base& operator=(base&& move) noexcept 
				{
				if (move.knows_owner())
					{
					move.owner_ptr->replace(move, *this);
					}
				}

			/// <summary> Copy operations do nothing to owner information. The instance data is copied to remains unowned by a vector_of_owner_aware. </summary>
			base(const base& copy) noexcept : owner_ptr{nullptr} {}

			/// <summary> Copy operations do nothing to owner information. The instance data is copied to remains unowned by a vector_of_owner_aware. </summary>
			base& operator=(const base& copy) noexcept {}

			bool knows_owner() const noexcept { return owner_ptr; }

			owner::unique<owners_pointee_type> extract_from_owner() utils_if_debug(noexcept)
				{
				if constexpr (utils::compilation::debug)
					{
					if (!knows_owner()) { throw std::out_of_range{"Trying to remove owner_aware object from its owner while it is not owned by a vector_of_owner_aware."}; }
					}
				return owner_ptr->extract(iterator_in_owner);
				}

			owner::unique<owners_pointee_type> replace_with(owners_pointee_type& new_element)
				{
				if (knows_owner() && new_element.knows_owner())
					{
					auto new_element_own{new_element.extract_from_owner()};
					return owner_ptr->replace(*this, new_element)
					}
				throw std::runtime_error{"Trying to replace objects that don't know their owners."};
				}

		private:
			size_t index_in_owner;
			utils::observer_ptr<owner_t> owner_ptr{nullptr};
		};
	}











namespace utils::owner_aware
	{
	namespace details
		{
		struct flag {};
		}
	namespace concepts
		{
		template <typename T>
		concept owner_aware = std::derived_from<T, details::flag>;
		}
	};


namespace utils::containers
	{
	namespace details
		{
		struct owner_aware_flag {};
		}
	namespace concepts
		{
		template <typename T>
		concept owner_aware = std::derived_from<T, details::owner_aware_flag>;
		}

	template <typename/*concepts::owner_aware*/ owners_pointee_type>
	class owner_aware;

	template <concepts::owner_aware T>
	class vector_of_owner_aware;
	template <concepts::owner_aware T>
	class unique_owner_of_owner_aware;
	
	template <typename/*concepts::owner_aware*/ owners_pointee_type>
	class owner_aware : public details::owner_aware_flag
		{
		template <concepts::owner_aware T>
		friend struct vector_of_owner_aware;
		using owner_t = vector_of_owner_aware<owners_pointee_type>;
		public:
			/// <summary> Move operations update the source instance's owner (if any) to own the target instance instead. </summary>
			owner_aware(owner_aware&& move) noexcept
				{
				if (move.knows_owner())
					{
					owner_ptr->replace(move, *this);
					}
				}

			/// <summary> Move operations update the source instance's owner (if any) to own the target instance instead. </summary>
			owner_aware& operator=(owner_aware&& move) noexcept 
				{
				if (move.knows_owner())
					{
					move.owner_ptr->replace(move, *this);
					}
				}

			/// <summary> Copy operations do nothing to owner information. The instance data is copied to remains unowned by a vector_of_owner_aware. </summary>
			owner_aware(const owner_aware& copy) noexcept : owner_ptr{nullptr} {}

			/// <summary> Copy operations do nothing to owner information. The instance data is copied to remains unowned by a vector_of_owner_aware. </summary>
			owner_aware& operator=(const owner_aware& copy) noexcept {}

			bool knows_owner() const noexcept { return owner_ptr; }

			std::unique_ptr<owners_pointee_type> extract_from_owner() utils_if_debug(noexcept)
				{
				if constexpr (utils::compilation::debug)
					{
					if (!knows_owner()) { throw std::out_of_range{"Trying to remove owner_aware object from its owner while it is not owned by a vector_of_owner_aware."}; }
					}
				return owner_ptr->extract(iterator_in_owner);
				}

			std::unique_ptr<owners_pointee_type> replace_with(std::unique_ptr<owners_pointee_type>&& new_element)
				{
				if (new_element.knows_owner())
					{
					auto new_element_own{new_element.extract_from_owner()};
					}
				}

		private:
			size_t index_in_owner;
			utils::observer_ptr<owner_t> owner_ptr{nullptr};
		};

	template <concepts::owner_aware T>
	class vector_of_owner_aware
		{
		template <typename/*concepts::owner_aware*/ owners_pointee_type>
		friend class owner_aware;

		private:
			using inner_vector_t = std::vector<std::unique_ptr<T>>;
			inline static constexpr auto dereference_view{std::views::transform([](auto&& ptr) -> auto& { return *ptr; })};

		public:
			using value_type = T;

			vector_of_owner_aware(vector_of_owner_aware&& move) : inner_vector{std::move(move.inner_vector)}
				{
				//No need to update owned's iterators, since the new owner is using the previous owner's storage
				update_owned_owner();
				}
			vector_of_owner_aware& operator=(vector_of_owner_aware&& move)
				{
				inner_vector = std::move(move.inner_vector);
				//No need to update owned's iterators, since the new owner is using the previous owner's storage
				update_owned_owner();
				}
			
			vector_of_owner_aware(const vector_of_owner_aware& copy) : inner_vector{copy.inner_vector}
				{
				update_owned_all();
				}
			vector_of_owner_aware& operator=(const vector_of_owner_aware& copy)
				{
				inner_vector = copy.inner_vector;
				update_owned_all();
				}

			size_t size         (               ) const noexcept { return inner_vector.size(); }
			bool   empty        (               ) const noexcept { return inner_vector.empty(); }
			void   reserve      (size_t capacity)       noexcept { inner_vector.reserve(capacity); }
			size_t max_size     (               ) const noexcept { return inner_vector.max_size(); }
			size_t capacity     (               ) const noexcept { return inner_vector.capacity(); }
			void   shrink_to_fit(               )       noexcept { return inner_vector.shrink_to_fit(); }
			void   clear        (               )       noexcept { return inner_vector.clear(); }

			      value_type& operator[](size_t index)       noexcept { return *inner_vector[index]; }
			const value_type& operator[](size_t index) const noexcept { return *inner_vector[index]; }

			      value_type& at(size_t index)       { return *inner_vector.at(index); }
			const value_type& at(size_t index) const { return *inner_vector.at(index); }

			std::unique_ptr<T> extract(T& owner_aware_value) noexcept
				{
				return extract(owner_aware_value.index_in_owner);
				}
			
			std::unique_ptr<T> extract(size_t index) noexcept
				{
				typename inner_vector_t::iterator iterator{inner_vector.begin() + index};
				std::unique_ptr<T> ret_ptr{std::move(*iterator)};

				auto it{inner_vector.erase(iterator)};
				update_owned_indices(index);

				return std::move(ret_ptr);
				}

			void resize(size_t new_size) noexcept
				requires(std::default_constructible<T>)
				{
				const auto old_size{size()};
				inner_vector.resize(new_size);
				
				for (size_t i{old_size}; i < new_size; i++)
					{
					inner_vector[i] = std::make_unique<T>();
					}
				}

			auto begin()       noexcept { return (inner_vector | dereference_view).begin(); }
			auto end  ()       noexcept { return (inner_vector | dereference_view).end  (); }
			auto begin() const noexcept { return (inner_vector | dereference_view).begin(); }
			auto end  () const noexcept { return (inner_vector | dereference_view).end  (); }
			
			auto rbegin()       noexcept { return (inner_vector | std::views::reverse | dereference_view).begin(); }
			auto rend  ()       noexcept { return (inner_vector | std::views::reverse | dereference_view).end  (); }
			auto rbegin() const noexcept { return (inner_vector | std::views::reverse | dereference_view).begin(); }
			auto rend  () const noexcept { return (inner_vector | std::views::reverse | dereference_view).end  (); }

		private:
			inner_vector_t inner_vector;


			void update_owned_owner() noexcept 
				{
				for (auto& owned : (*this))
					{
					owned.owner_ptr = this;
					}
				}
			void update_owned_indices() noexcept { update_owned_indices(0); }
			void update_owned_indices(size_t from) noexcept
				{
				for (size_t i{from}; i < size(); i++)
					{
					auto& element{operator[](i)};
					element.index_in_owner = i;
					}
				}
			void update_owned_all() noexcept
				{
				for (size_t i{0}; i < size(); i++)
					{
					auto& element{operator[](i)};
					element.owner_ptr = this;
					element.index_in_owner = i;
					}
				}
		};
	}
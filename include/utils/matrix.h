#pragma once

#include "storage.h"
#include "math/vec.h"
#include "compilation/gpu.h"
#include "compilation/compiler.h"
#include "oop/disable_move_copy.h"
#include "oop/conditional_inheritance.h"

namespace utils
	{
	struct matrix_size //TODO turn into vec2s if C++ ever supports types with inheritance as template parameters
		{
		struct create : utils::oop::non_constructible
			{
			utils_gpu_available inline static consteval matrix_size dynamic() noexcept 
				{
				return matrix_size{std::dynamic_extent, std::dynamic_extent}; 
				}
			};
		utils_gpu_available consteval bool is_dynamic() const noexcept 
			{
			return width == std::dynamic_extent || height == std::dynamic_extent; 
			}

		size_t width {1}; 
		size_t height{1}; 
		
		utils_gpu_available constexpr size_t x() const noexcept { return width ; }
		utils_gpu_available constexpr size_t y() const noexcept { return height; }
		}; 

	template <typename T, matrix_size EXTENTS = matrix_size::create::dynamic()>
	struct utils_oop_empty_bases matrix;

	namespace concepts
		{
		template <typename T>
		concept matrix = std::same_as<std::remove_cvref_t<T>, utils::matrix<typename std::remove_cvref_t<T>::template_type, std::remove_cvref_t<T>::_extents>>;
		}
	

	namespace details
		{
		template <typename T, matrix_size extents>
		using evaluate_multiple_t = utils::storage::multiple<T, extents.is_dynamic() ? std::dynamic_extent : extents.width * extents.height>;

		template <typename T, matrix_size extents>
		struct matrix_sizes_interface
			{
			utils_gpu_available static consteval utils::math::vec2s sizes () noexcept { return {width(), height()}; }
			utils_gpu_available static consteval size_t             width () noexcept { return extents.x(); }
			utils_gpu_available static consteval size_t             height() noexcept { return extents.y(); }
			
			utils_gpu_available static constexpr bool validate_x     (size_t      x     ) noexcept { return x < width (); }
			utils_gpu_available static constexpr bool validate_y     (size_t      y     ) noexcept { return y < height(); }
			utils_gpu_available static constexpr bool validate_coords(math::vec2s coords) noexcept { return validate_x(coords.x()), validate_y(coords.y()); }
			};

		template <typename T>
		struct matrix_sizes_interface<T, matrix_size{std::dynamic_extent, std::dynamic_extent}>
			{
			private:
				utils::math::vec2s inner_sizes;

			public:
				matrix_sizes_interface(utils::math::vec2s inner_sizes) : inner_sizes{inner_sizes} {}

				utils_gpu_available constexpr utils::math::vec2s sizes () const noexcept { return inner_sizes; }
				utils_gpu_available constexpr size_t             width () const noexcept { return inner_sizes.x(); }
				utils_gpu_available constexpr size_t             height() const noexcept { return inner_sizes.y(); }
				
				utils_gpu_available constexpr bool validate_x     (size_t      x     ) const noexcept { return x < width (); }
				utils_gpu_available constexpr bool validate_y     (size_t      y     ) const noexcept { return y < height(); }
				utils_gpu_available constexpr bool validate_coords(math::vec2s coords) const noexcept { return validate_x(coords.x()), validate_y(coords.y()); }
			};
		}

	template <typename T, matrix_size EXTENTS>
	struct utils_oop_empty_bases matrix : details::evaluate_multiple_t<T, EXTENTS>, details::matrix_sizes_interface<T, EXTENTS>
		{
		utils_gpu_available inline static constexpr matrix_size _extents{EXTENTS};
		utils_gpu_available inline static constexpr utils::math::vec2s extents{EXTENTS.width, EXTENTS.height};
		using multiple_t = details::evaluate_multiple_t<T, EXTENTS>;
		using sizes_inerface_t = details::matrix_sizes_interface<T, EXTENTS>;

		using typename multiple_t::value_type;
		using typename multiple_t::const_aware_value_type;
		using multiple_t      ::size;
		using multiple_t      ::storage_type;
		using sizes_inerface_t::sizes;
		using sizes_inerface_t::validate_coords;
		
		using self_t          = matrix<T                      , EXTENTS>;
		using owner_self_t    = matrix<value_type             , EXTENTS>;
		using observer_self_t = matrix<const_aware_value_type&, EXTENTS>;

		utils_gpu_available constexpr matrix(utils::math::vec2s sizes) requires(EXTENTS.is_dynamic() && storage_type.is_owner()) :
			multiple_t(sizes.sizes_to_size()),
			details::matrix_sizes_interface<T, EXTENTS>{sizes}
			{
			}
			
		utils_gpu_available constexpr matrix(utils::math::vec2s sizes, const T default_value) requires(EXTENTS.is_dynamic() && storage_type.is_owner()) :
			multiple_t(sizes.sizes_to_size()),
			details::matrix_sizes_interface<T, EXTENTS>{sizes}
			{
			for (size_t i = 0; i < sizes.sizes_to_size(); i++)
				{
				operator[](i) = default_value;
				}
			}

		template <typename ...Args>
		utils_gpu_available constexpr matrix(utils::math::vec2s sizes, Args&&... args) requires(EXTENTS.is_dynamic() && storage_type.is_owner()) :
			multiple_t(utils::storage::construct_flag_data, std::forward<Args>(args)...),
			details::matrix_sizes_interface<T, EXTENTS>{sizes}
			{
			assert(sizes.sizes_to_size() == size());
			}
		
		template <typename ...Args>
		utils_gpu_available constexpr matrix(Args&&... args) requires(!EXTENTS.is_dynamic() && storage_type.is_owner()) :
			multiple_t(utils::storage::construct_flag_data, std::forward<Args>(args)...)
			{}

		using multiple_t::operator[];
		using multiple_t::at;
		utils_gpu_available constexpr const       value_type& operator[](math::vec2s coords) const noexcept                                    { return operator[](sizes().coords_to_index(coords)); }
		utils_gpu_available constexpr const_aware_value_type& operator[](math::vec2s coords)       noexcept requires(!storage_type.is_const()) { return operator[](sizes().coords_to_index(coords)); }
		utils_gpu_available constexpr const       value_type& at        (math::vec2s coords) const                                             { if (!validate_coords(coords)) { throw std::out_of_range{"Matrix access out of bounds."}; }; return at(sizes().coords_to_index(coords)); }
		utils_gpu_available constexpr const_aware_value_type& at        (math::vec2s coords)                requires(!storage_type.is_const()) { if (!validate_coords(coords)) { throw std::out_of_range{"Matrix access out of bounds."}; }; return at(sizes().coords_to_index(coords)); }
		};
	}
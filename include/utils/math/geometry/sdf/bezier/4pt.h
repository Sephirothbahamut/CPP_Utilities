#pragma once

#include "../common.h"
#include "../../shape/bezier.h"
#include "../../../vec.h"

namespace utils::math::geometry::sdf::details::bezier::_4pt
	{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Slower but comprehensible
	// https://www.shadertoy.com/view/sdjXRy
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	struct closest_precalculated_information_t
		{
		float              distance2{utils::math::constants::finf};
		float              t{0.f};
		utils::math::vec2f point;
		utils_gpu_available inline float distance() const noexcept { return std::sqrt(distance2); } //TODO constexpr when std::sqrt is constexpr
		};

	template <typename T, size_t capacity>
	struct simple_static_vector
		{
		size_t size{0};
		std::array<T, capacity> data{};

		utils_gpu_available inline constexpr void push_back(T value) noexcept
			{
			assert(size < capacity);
			data[size] = value;
			size++;
			}
		};

	// https://www.shadertoy.com/view/st33Wj
	utils_gpu_available inline constexpr utils::math::vec2f cmul(utils::math::vec2f z, utils::math::vec2f w) noexcept
		{
		//return mat2(z, -z.y(), z.x()) * w; 
		// [z.x(), -z.y()] * [w.x()] = [z.x() * w.x() - z.y() * w.y()]
		// [z.y(),  z.x()]   [w.y()]   [z.y() * w.x() + z.x() * w.y()]
		return
			{
			z.x() * w.x() - z.y() * w.y(), 
			z.y() * w.x() + z.x() * w.y()
			};
		}

	utils_gpu_available inline constexpr utils::math::vec2f cdiv(utils::math::vec2f z, utils::math::vec2f w) noexcept
		{
		return cmul(z, utils::math::vec2f(w.x(), -w.y())) / utils::math::vec2f::dot(w, w);
		}

	utils_gpu_available inline constexpr simple_static_vector<float, 5> solveQuintic(float a, float b, float c, float d, float e, float f) noexcept
		{
		float p = (a * c *                5.f - b * b *               2.f                                                                                 ) / (a * a *                5.f);
		float q = (a * a * d *           25.f - a * b * c *          15.f + b * b * b *           4.f                                                     ) / (a * a * a *           25.f);
		float r = (a * a * a * e *      125.f - a * a * b * d *      50.f + a * b * b * c *      15.f - b * b * b * b *      3.f                          ) / (a * a * a * a *      125.f);
		float s = (a * a * a * a * f * 3125.f - a * a * a * b * e * 625.f + a * a * b * b * d * 125.f - a * b * b * b * c * 25.f + b * b * b * b * b * 4.f) / (a * a * a * a * a * 3125.f);

		float bound = 1.f + utils::math::max(1.f, utils::math::max(utils::math::abs(p), utils::math::max(utils::math::abs(q), utils::math::max(utils::math::abs(r), utils::math::abs(s)))));
		//bound *= 0.414213562373; // Correction if perturbing with random([-1...1])
		bound *= 0.5;

		// Added by me.
		if (bound == utils::math::constants::finf || bound == -utils::math::constants::finf || bound == utils::math::constants::fnan)
			{
			return {};
			}

		std::array<utils::math::vec2f, 5> roots
			{
			utils::math::vec2f{           bound,  0.f            }, 
			utils::math::vec2f{ 0.309016994375f,  0.951056516295f} * bound, 
			utils::math::vec2f{-0.809016994375f,  0.587785252292f} * bound, 
			utils::math::vec2f{-0.809016994375f, -0.587785252292f} * bound, 
			utils::math::vec2f{ 0.309016994375f, -0.951056516295f} * bound
			};

		for (size_t iter = 0; iter < 25; iter++)
			{
			float maxEval{-1e20f};
			for (size_t root = 0; root < 5; root++)
				{
				utils::math::vec2f z = roots[root];
				utils::math::vec2f quinticVal = cmul(cmul(cmul(cmul(z, z) + utils::math::vec2f(p, 0.f), z) + utils::math::vec2f(q, 0.f), z) + utils::math::vec2f(r, 0.f), z) + utils::math::vec2f(s, 0.f);
				maxEval = utils::math::max(maxEval, utils::math::max(utils::math::abs(quinticVal.x()), utils::math::abs(quinticVal.y())));

				utils::math::vec2f denom = z - roots[(root + 1) % 5];
				denom = cmul(denom, z - roots[(root + 2) % 5]);
				denom = cmul(denom, z - roots[(root + 3) % 5]);
				denom = cmul(denom, z - roots[(root + 4) % 5]);

				roots[root] -= cdiv(quinticVal, denom);
				}

			if (maxEval < 1e-7) break;
			}

		simple_static_vector<float, 5> ret;

		float offs = b / (5.f * a);
		for (int root = 0; root < 5; root++)
			{
			utils::math::vec2f z = roots[root];
			if (utils::math::abs(z.y()) < 1e-7f)
				{
				ret.push_back(z.x() - offs);
				}
			}

		return ret;
		}

	utils_gpu_available inline constexpr float dot2(utils::math::vec2f v) noexcept { return utils::math::vec2f::dot(v, v); }

	utils_gpu_available inline constexpr simple_static_vector<float, 5> solved_quintic(const utils::math::vec2f point, const utils::math::vec2f v1, const utils::math::vec2f v2, const utils::math::vec2f v3, const utils::math::vec2f v4) noexcept
		{
		// Convert to power basis
		const utils::math::vec2f a = v4 + (v2 - v3) * 3.f - v1;
		const utils::math::vec2f b = (v1 - v2 * 2.f + v3) * 3.f;
		const utils::math::vec2f c = (v2 - v1) * 3.f;
		const utils::math::vec2f d = v1 - point;

		// Quintic coefficients (derivative of distance-for-t with 2 factored out)
		const float qa = 3.f * utils::math::vec2f::dot(a, a);
		const float qb = 5.f * utils::math::vec2f::dot(a, b);
		const float qc = 4.f * utils::math::vec2f::dot(a, c) + 2.f * utils::math::vec2f::dot(b, b);
		const float qd = 3.f * (utils::math::vec2f::dot(b, c) + utils::math::vec2f::dot(d, a));
		const float qe = utils::math::vec2f::dot(c, c) + 2.f * utils::math::vec2f::dot(d, b);
		const float qf = utils::math::vec2f::dot(d, c);

		closest_precalculated_information_t closest;

		const auto quintic_roots = solveQuintic(qa, qb, qc, qd, qe, qf);
		return quintic_roots;
		}

	template <ends::ab ends>
	utils_gpu_available constexpr float closest_t(const utils::math::vec2f& point, const shape::concepts::bezier auto& shape) noexcept
		{
		const auto p0{shape.vertices[0]};
		const auto p1{shape.vertices[1]};
		const auto p2{shape.vertices[2]};
		const auto p3{shape.vertices[3]};
		const auto ts{solved_quintic(point, p0, p1, p2, p3)};

		//The curve is a disguised quadratic, which utterly breaks algorithms for cubic curves. 
		//So let's treat it as a quadratic
		if (ts.size == 0)
			{
			const auto quadratic{shape.revert_quadratic_elevated_to_cubic()};
			return quadratic.sdf(point).template closest_t<ends>();
			}

		float closest_distance{utils::math::constants::finf};
		float closest_t       {utils::math::constants::fnan};

		for (size_t i = 0; i < ts.size; i++)
			{
			const float candidate_t{ends::clamp_t<ends>(ts.data[i])};
			const auto at_proxy{shape.at(candidate_t)};
			const auto candidate_point{at_proxy.point()};
			const float candidate_distance(utils::math::vec2f::distance2(candidate_point, point));
			if (candidate_distance < closest_distance)
				{
				closest_distance = candidate_distance;
				closest_t        = candidate_t       ;
				}
			}

		return closest_t;
		}
	



















	//int solve_cubic(utils::math::vec3f coeffs, utils::math::vec3f& r) 
	//	{
	//	const float a{coeffs[2]};
	//	const float b{coeffs[1]};
	//	const float c{coeffs[0]};
	//
	//	const float p {b - a * a / 3.f};
	//	const float q {a * (2.f * a * a - 9.f * b) / 27.f + c};
	//	const float p3{p * p * p};
	//	const float d {q * q + 4.f * p3 / 27.f};
	//	const float offset = -a / 3.f;
	//
	//	if (d >= 0.0) 
	//		{ // Single solution
	//		float z = std::sqrt(d);
	//		float u = (-q + z) / 2.f;
	//		float v = (-q - z) / 2.f;
	//		u = sign(u) * std::pow(std::abs(u), 1.f / 3.f);
	//		v = sign(v) * std::pow(std::abs(v), 1.f / 3.f);
	//		r[0] = offset + u + v;
	//
	//		//Single newton iteration to account for cancellation
	//		float f = ((r[0] + a) * r[0] + b) * r[0] + c;
	//		float f1 = (3.f * r[0] + 2.f * a) * r[0] + b;
	//
	//		r[0] -= f / f1;
	//
	//		return 1;
	//		}
	//	const float u{sqrt(-p / 3.f)};
	//	const float v{acos(-sqrt(-27.f / p3) * q / 2.f) / 3.f};
	//	const float m{std::cos(v)};
	//	const float n{std::sin(v) * 1.732050808f};
	//
	//	//Single newton iteration to account for cancellation
	//	//(once for every root)
	//	r[0] = offset + u * (m + m);
	//	r[1] = offset - u * (n + m);
	//	r[2] = offset + u * (n - m);
	//
	//	utils::math::vec3f f {((r + a) * r + b) * r + c};
	//	utils::math::vec3f f1{(r * 3.f + a * 2.f) * r + b};
	//
	//	r -= f / f1;
	//
	//	return 3;
	//	}


	//utils_gpu_available constexpr bool intersects_for_closed_shape_pieces(const utils::math::vec2f& point, const shape::concepts::bezier auto& shape) noexcept
	//	{
	//	shape.
	//
	//	const auto uv{point};
	//	const auto p0{shape.vertices[0]};
	//	const auto p1{shape.vertices[1]};
	//	const auto p2{shape.vertices[2]};
	//	const auto p3{shape.vertices[3]};
	//
	//	const float cu{(-p0.y() + 3.f * p1.y() - 3.f * p2.y() + p3.y())};
	//	const float qu{(3.f * p0.y() - 6.f * p1.y() + 3.f * p2.y())};
	//	const float li{(-3.f * p0.y() + 3.f * p1.y())};
	//	const float co{p0.y() - uv.y()};
	//
	//	utils::math::vec3f roots = utils::math::vec3f{1e38, 1e38, 1e38};
	//	const int n_roots{solve_cubic(utils::math::vec3f{co / cu, li / cu, qu / cu}, roots)};
	//
	//	int n_ints = 0;
	//
	//	for (int i = 0; i < 3; i++) 
	//		{
	//		if (i < n_roots) 
	//			{
	//			if (roots[i] >= 0. && roots[i] <= 1.) 
	//				{
	//				float x_pos = -p0.x + 3. * p1.x - 3. * p2.x + p3.x;
	//				x_pos = x_pos * roots[i] + 3. * p0.x - 6. * p1.x + 3. * p2.x;
	//				x_pos = x_pos * roots[i] + -3. * p0.x + 3. * p1.x;
	//				x_pos = x_pos * roots[i] + p0.x;
	//
	//				if (x_pos < uv.x) 
	//					{
	//					n_ints++;
	//					}
	//				}
	//			}
	//		}
	//
	//	vec2 tang1 = p0.xy - p1.xy;
	//	vec2 tang2 = p2.xy - p3.xy;
	//
	//	vec2 nor1 = vec2(tang1.y, -tang1.x);
	//	vec2 nor2 = vec2(tang2.y, -tang2.x);
	//
	//	if (p0.y < p1.y)
	//		{
	//		if ((uv.y <= p0.y) && (dot(uv - p0.xy, nor1) < 0.))
	//			{
	//			n_ints++;
	//			}
	//		}
	//	else
	//		{
	//		if (!(uv.y <= p0.y) && !(dot(uv - p0.xy, nor1) < 0.))
	//			{
	//			n_ints++;
	//			}
	//		}
	//
	//	if (p2.y < p3.y)
	//		{
	//		if (!(uv.y <= p3.y) && dot(uv - p3.xy, nor2) < 0.)
	//			{
	//			n_ints++;
	//			}
	//		}
	//	else
	//		{
	//		if ((uv.y <= p3.y) && !(dot(uv - p3.xy, nor2) < 0.))
	//			{
	//			n_ints++;
	//			}
	//		}
	//
	//	if (n_ints == 0 || n_ints == 2 || n_ints == 4)
	//		{
	//		return false;
	//		}
	//	else
	//		{
	//		return true;
	//		}
	//	}



	}